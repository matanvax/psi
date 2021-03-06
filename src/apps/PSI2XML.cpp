/*
 * PSI2XML.cpp
 *
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 *  Created on: May 3, 2016
 *      Author: ballance
 */

#include "PSI2XML.h"
#include <stdio.h>

namespace psi {
namespace apps {

PSI2XML::PSI2XML() : m_ind_incr(4) {
	// TODO Auto-generated constructor stub

}

PSI2XML::~PSI2XML() {
	// TODO Auto-generated destructor stub
}

const std::string &PSI2XML::traverse(IModel *model) {
	m_content.clear();

	println("<model>");
	inc_indent();

	IPackage *pkg = model->getGlobalPackage();
	process_pkg(pkg);

	std::vector<IBaseItem *>::const_iterator it=model->getItems().begin();

	for (; it!=model->getItems().end(); it++) {
		IBaseItem *i = *it;

		if (i->getType() == IBaseItem::TypePackage) {
			IPackage *pkg = static_cast<IPackage *>(i);
			process_pkg(pkg);
		} else if (i->getType() == IBaseItem::TypeComponent) {
			process_component(static_cast<IComponent *>(i));
		} else {
			// Really shouldn't be anything else in the global scope
		}
	}

	dec_indent();
	println("</model>");


	return m_content;
}

void PSI2XML::process_pkg(IPackage *pkg) {
	if (pkg->getName() == "") {
		println("<package>");
	} else {
		println("<package name=\"" + pkg->getName() + "\">");
	}

	inc_indent();

	std::vector<IBaseItem *>::const_iterator it=pkg->getItems().begin();

	for (; it!=pkg->getItems().end(); it++) {
		IBaseItem *i = *it;

		switch (i->getType()) {
			case IBaseItem::TypeAction:
				break;

			case IBaseItem::TypeStruct:
				process_struct(static_cast<IStruct *>(i));
				break;
		}
	}

	dec_indent();

	println("</package>");
}

void PSI2XML::process_action(IAction *a) {
	IAction *super_a = a->getSuperType();

	std::string tag = "<action name=\"" + a->getName() + "\"";

	if (super_a) {
		tag += " super=\"" + type2string(super_a) + "\"";
	}

	tag += ">";
	println(tag);

	inc_indent();
	process_body(a->getItems());

	if (a->getGraph()) {
		process_graph(a->getGraph());
	}
	dec_indent();

	println("</action>");
}

void PSI2XML::process_struct(IStruct *str) {
	IStruct *super_s = 0; // TODO: super-type
	std::string tag = "<struct name=\"" + str->getName() + "\"";

	// TODO: handle super-type
	if (str->getSuperType()) {
		tag += " super=\"" + type2string(str->getSuperType()) + "\"";
	}

	switch (str->getStructType()) {
	case IStruct::Memory: tag += " type=\"memory\""; break;
	case IStruct::Resource: tag += " type=\"resource\""; break;
	case IStruct::Stream: tag += " type=\"stream\""; break;
	case IStruct::State: tag += " type=\"state\""; break;
	}

	tag += ">";
	println(tag);

	inc_indent();
	process_body(str->getItems());
	dec_indent();

	println("</struct>");
}

void PSI2XML::process_bind(IBind *b) {
	println("<bind>");
	inc_indent();

	for (std::vector<IBaseItem *>::const_iterator it=b->getTargets().begin();
			it!=b->getTargets().end(); it++) {
		// TODO:
	}

	dec_indent();
	println("</bind>");
}

void PSI2XML::process_body(const std::vector<IBaseItem *> &items) {
	std::vector<IBaseItem *>::const_iterator it = items.begin();

	for (; it!=items.end(); it++) {
		IBaseItem *i = *it;

		switch (i->getType()) {
		case IBaseItem::TypeBind:
			process_bind(static_cast<IBind *>(i));
			break;
		case IBaseItem::TypeConstraint:
			process_constraint_block(static_cast<IConstraintBlock *>(i));
			break;

		case IBaseItem::TypeField:
			process_field(static_cast<IField *>(i));
			break;

		default:
			fprintf(stdout, "Error: Unknown body item %d\n", i->getType());
		}
	}

}

void PSI2XML::process_component(IComponent *c) {

	println(std::string("<component name=\"") + c->getName() + "\">");
	inc_indent();

	process_comp_pkg_body(c->getItems());

	dec_indent();
	println("</component>");
}

void PSI2XML::process_comp_pkg_body(const std::vector<IBaseItem *> &items) {
	std::vector<IBaseItem *>::const_iterator it=items.begin();

	for (; it!=items.end(); it++) {
		switch ((*it)->getType()) {
			case IBaseItem::TypeAction:
				process_action(static_cast<IAction *>(*it));
				break;

			case IBaseItem::TypeStruct:
				process_struct(static_cast<IStruct *>(*it));
				break;
		}
	}

}

void PSI2XML::process_constraint(IConstraint *c) {
	switch (c->getConstraintType()) {
	case IConstraint::ConstraintType_Block:
		fprintf(stdout, "  block\n");
		break;

	case IConstraint::ConstraintType_Expr:
		process_expr(static_cast<IConstraintExpr *>(c)->getExpr());
		break;

	case IConstraint::ConstraintType_If: {
		IConstraintIf *c_if = static_cast<IConstraintIf *>(c);

		println("<if>");
		inc_indent();
		println("<cond>");
		inc_indent();
		process_expr(c_if->getCond());
		dec_indent();
		println("</cond>");

		println("<isTrue>");
		inc_indent();
		process_constraint(c_if->getTrue());
		dec_indent();
		println("</isTrue>");

		if (c_if->getFalse()) {
			println("<isFalse>");
			inc_indent();
			process_constraint(c_if->getFalse());
			dec_indent();
			println("</isFalse>");
		}

		dec_indent();
		println("</if>");

		fprintf(stdout, "  if\n");
		} break;
	}
}

void PSI2XML::process_constraint_block(IConstraintBlock *block) {
	std::vector<IConstraint *>::const_iterator it = block->getConstraints().begin();

	if (block->getName() == "") {
		println("<constraint>");
	} else {
		println("<constraint name=\"" + block->getName() + "\">");
	}

	inc_indent();

	for (; it!=block->getConstraints().end(); it++) {
		IConstraint *c = *it;
		process_constraint(c);
	}

	dec_indent();

	println("</constraint>");
}

void PSI2XML::process_expr(IExpr *e) {
	if (!e) {
		fprintf(stdout, "Error: null expression\n");
		return;
	}
	switch (e->getType()) {
		case IExpr::ExprType_BinOp: {
			IBinaryExpr *be = static_cast<IBinaryExpr *>(e);
			std::string op = "<unknown>";
			switch (be->getBinOpType()) {
			case IBinaryExpr::BinOp_EqEq: op = "EqEq"; break;
			case IBinaryExpr::BinOp_NotEq: op = "NotEq"; break;
			case IBinaryExpr::BinOp_GE: op = "GE"; break;
			case IBinaryExpr::BinOp_GT: op = "GT"; break;
			case IBinaryExpr::BinOp_LE: op = "LE"; break;
			case IBinaryExpr::BinOp_LT: op = "LT"; break;
			case IBinaryExpr::BinOp_And: op = "And"; break;
			case IBinaryExpr::BinOp_AndAnd: op = "AndAnd"; break;
			case IBinaryExpr::BinOp_Or: op = "Or"; break;
			case IBinaryExpr::BinOp_OrOr: op = "OrOr"; break;
			case IBinaryExpr::BinOp_Minus: op = "Minus"; break;
			case IBinaryExpr::BinOp_Plus: op = "Plus"; break;
			case IBinaryExpr::BinOp_Multiply: op = "Mul"; break;
			case IBinaryExpr::BinOp_Divide: op = "Div"; break;
			case IBinaryExpr::BinOp_Mod: op = "Mod"; break;
			case IBinaryExpr::BinOp_ArrayRef: op = "ArrRef"; break;
			}
			println("<binexp op=\"" + op + "\">");
			inc_indent();
			process_expr(be->getLHS());
			process_expr(be->getRHS());
			dec_indent();
			println("</binexp>");
			} break;

		case IExpr::ExprType_Literal: {
			ILiteral *l = static_cast<ILiteral *>(e);
			std::string tag;
			char val[64];

			switch (l->getLiteralType()) {
			case ILiteral::LiteralBit: {
				sprintf(val, "0x%llx", (long long)l->getBit());
				tag = "<literal type=\"bit\" value=\"";
				tag += val;
				tag += "\"/>";
			} break;
			case ILiteral::LiteralInt: {
				sprintf(val, "0x%llx", (long long)l->getInt());
				tag = "<literal type=\"int\" value=\"";
				tag += val;
				tag += "\"/>";
			} break;
			case ILiteral::LiteralBool: {
				tag = "<literal type=\"bool\" value=\"";
				if (l->getBool()) {
					tag += "true";
				} else {
					tag += "false";
				}
				tag += "\"/>";
			} break;
			default:
				tag = "<unknown/>";
			}

			println(tag);

			} break;

		case IExpr::ExprType_FieldRef: {
			const std::vector<IField *> &fields =
					static_cast<IFieldRef *>(e)->getFieldPath();

			for (uint32_t i=0; i<fields.size(); i++) {
				IField *field = fields.at(i);

				if (i+1<fields.size()) {
					println(std::string("<fieldref name=\"" + field->getName() + "\">"));
					inc_indent();
				} else {
					println(std::string("<fieldref name=\"" + field->getName() + "\"/>"));
				}
			}
			for (uint32_t i=0; fields.size()>0 && i<fields.size()-1; i++) {
				dec_indent();
				println("</fieldref>");
			}
			} break;
	}
}

void PSI2XML::process_field(IField *f) {
	char msb_s[64], lsb_s[64];
	std::string tag = "<field name=\"" + f->getName() + "\"";

	switch (f->getAttr()) {
	case IField::FieldAttr_Rand:
		tag += " type=\"rand\"";
		break;
	case IField::FieldAttr_Input:
		tag += " type=\"input\"";
		break;
	case IField::FieldAttr_Output:
		tag += " type=\"output\"";
		break;
	case IField::FieldAttr_Lock:
		tag += " type=\"lock\"";
		break;
	case IField::FieldAttr_Share:
		tag += " type=\"share\"";
		break;
	case IField::FieldAttr_Pool:
		tag += " type=\"pool\"";
		break;
	}

	tag += ">";
	println(tag);

	inc_indent();

	IBaseItem *dt_i = f->getDataType();

	if (dt_i) {
		if (dt_i->getType() == IBaseItem::TypeScalar) {
			IScalarType *st = static_cast<IScalarType *>(dt_i);
			std::string tname = "unknown-scalar";
			sprintf(msb_s, "%d", st->getMSB());
			sprintf(lsb_s, "%d", st->getLSB());
			bool has_bitwidth = false;

			if (st->getScalarType() == IScalarType::ScalarType_Bit) {
				tname = "bit";
				has_bitwidth = true;
			} else if (st->getScalarType() == IScalarType::ScalarType_Int) {
				tname = "int";
				has_bitwidth = true;
			} else if (st->getScalarType() == IScalarType::ScalarType_Bool) {
				tname = "bool";
				has_bitwidth = false;
			}

			if (has_bitwidth) {
				println(std::string("<") + tname + " msb=\"" +
						msb_s + "\" lsb=\"" + lsb_s + "\"/>");
			} else {
				println(std::string("<") + tname + "/>");
			}
		} else if (dt_i->getType() == IBaseItem::TypeAction) {
			println(std::string("<action type=\"") + type2string(dt_i) + "\"/>");
		} else if (dt_i->getType() == IBaseItem::TypeStruct) {
			println(std::string("<struct type=\"") + type2string(dt_i) + "\"/>");
		} else {
			println("<unknown/>");
		}
	} else {
		println("<null/>");
	}
	dec_indent();

	println("</field>");
}

void PSI2XML::process_graph(IGraphStmt *graph) {
	std::vector<IGraphStmt *>::const_iterator it;

	println("<graph>");
	inc_indent();

	process_graph_stmt(graph);

	dec_indent();
	println("</graph>");
}

void PSI2XML::process_graph_stmt(IGraphStmt *stmt) {
	switch (stmt->getStmtType()) {
	case IGraphStmt::GraphStmt_Block: {
		println("<block>");
		inc_indent();
		process_graph_block_stmt(static_cast<IGraphBlockStmt *>(stmt));
		dec_indent();
		println("</block>");
	} break;

	case IGraphStmt::GraphStmt_IfElse: {
		fprintf(stdout, "TODO: GraphStmt_IfElse\n");
	} break;

	case IGraphStmt::GraphStmt_Parallel: {
		println("<parallel>");
		inc_indent();

		process_graph_block_stmt(static_cast<IGraphBlockStmt *>(stmt));

		dec_indent();
		println("</parallel>");
	} break;

	case IGraphStmt::GraphStmt_Schedule: {
		println("<schedule>");
		inc_indent();

		process_graph_block_stmt(static_cast<IGraphBlockStmt *>(stmt));

		dec_indent();
		println("</schedule>");
	} break;

	case IGraphStmt::GraphStmt_Select: {
		println("<select>");
		inc_indent();

		process_graph_block_stmt(static_cast<IGraphBlockStmt *>(stmt));

		dec_indent();
		println("</select>");
	} break;

	case IGraphStmt::GraphStmt_Repeat: {
		IGraphRepeatStmt *r = static_cast<IGraphRepeatStmt *>(stmt);

		std::string tag = "<repeat type=\"";
		switch (r->getRepeatType()) {
			case IGraphRepeatStmt::RepeatType_Forever: tag += "forever"; break;
			case IGraphRepeatStmt::RepeatType_While: tag += "while"; break;
			case IGraphRepeatStmt::RepeatType_Until: tag += "until"; break;
		}
		tag += "\">";
		println(tag);
		inc_indent();

		// TODO: handle non-forever versions
		process_graph_stmt(r->getBody());

		dec_indent();
		println("</repeat>");
	} break;

	case IGraphStmt::GraphStmt_Traverse: {
		IGraphTraverseStmt *t = static_cast<IGraphTraverseStmt *>(stmt);

		std::string tag = "<traverse action=\"";
		tag += path2string(t->getAction());

		if (t->getWith()) {
			tag += "\">";
		} else {
			tag += "\"/>";
		}

		println(tag);

		if (t->getWith()) {
			println("<with>");
			// TODO: elaborate with constraint
			println("</with>");

			println("</traverse>");
		}
	} break;

	default: fprintf(stdout, "TODO: handle graph stmt %d\n", stmt->getStmtType());

	}
}

void PSI2XML::process_graph_block_stmt(IGraphBlockStmt *block) {
	std::vector<IGraphStmt *>::const_iterator it;

	for (it=block->getStmts().begin(); it!=block->getStmts().end(); it++) {
		process_graph_stmt(*it);
	}
}

std::string PSI2XML::type2string(IBaseItem *it) {
	std::string ret;

	while (it) {
		INamedItem *ni = toNamedItem(it);

		if (ni) {
			if (ret.size() > 0) {
				ret.insert(0, "::");
			}
			ret.insert(0, ni->getName());
		} else {
			break;
		}

		it = it->getParent();
	}

	return ret;
}

std::string PSI2XML::path2string(IFieldRef *f) {
	std::string ret;
	std::vector<IField *>::const_iterator it;

	for (it=f->getFieldPath().begin(); it!=f->getFieldPath().end(); ) {
		IField *field = *(it);
		ret += field->getName();
		it++;
		if (it != f->getFieldPath().end()) {
			ret += ".";
		}
	}

	return ret;
}

void PSI2XML::println(const std::string &str) {
	m_content.append(m_ind);
	m_content.append(str);
	m_content.append("\n");
}

void PSI2XML::inc_indent() {
	for (int i=0; i<m_ind_incr; i++) {
		m_ind.append(" ");
	}
}

void PSI2XML::dec_indent() {
	if (m_ind.size() > m_ind_incr) {
		m_ind.resize(m_ind.size()-m_ind_incr);
	} else {
		m_ind.clear();
	}
}

INamedItem *PSI2XML::toNamedItem(IBaseItem *it) {
	switch (it->getType()) {
	case IBaseItem::TypeAction: return static_cast<IAction *>(it);
	case IBaseItem::TypeComponent: return static_cast<IComponent *>(it);
	case IBaseItem::TypeField: return static_cast<IField *>(it);
	case IBaseItem::TypeStruct: return static_cast<IStruct *>(it);
	}

	return 0;
}

}
}
