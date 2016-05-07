/*
 * IModel.h
 *
 *  Created on: Apr 26, 2016
 *      Author: ballance
 */

#ifndef SRC_PROGRAMMATIC_IMODEL_H_
#define SRC_PROGRAMMATIC_IMODEL_H_
#include <vector>
#include <string>

#include "api/IAction.h"
#include "api/IBaseItem.h"
#include "api/IBinaryExpr.h"
#include "api/IComponent.h"
#include "api/IConstraint.h"
#include "api/IConstraintBlock.h"
#include "api/IConstraintExpr.h"
#include "api/IConstraintIf.h"
#include "api/IField.h"
#include "api/ILiteral.h"
#include "api/IPackage.h"
#include "api/IScalarType.h"
#include "api/IScopeItem.h"
#include "api/IStruct.h"

namespace psi_api {

	class IModel : public IScopeItem {
		public:

			virtual ~IModel() { }

			virtual IPackage *getGlobalPackage() = 0;

			virtual IPackage *findPackage(const std::string &name, bool create=false) = 0;

			/**
			 * Data Types
			 */

			/**
			 * Creates a scalar type. The msb and lsb parameters are ignored for types
			 * other than Int and Bit
			 */
			virtual IScalarType *mkScalarType(
					IScalarType::ScalarType t,
					uint32_t				msb,
					uint32_t				lsb) = 0;

			/**
			 * Action
			 */
			virtual IAction *mkAction(const std::string &name, IAction *super_type) = 0;

			/**
			 * Create a new component type
			 */
			virtual IComponent *mkComponent(const std::string &name) = 0;

			/**
			 * Create a new struct type
			 */
			virtual IStruct *mkStruct(
					const std::string 		&name,
					IStruct::StructType		t,
					IStruct 				*super_type) = 0;

			/**
			 * Create a field for use in declaring the contents of an
			 * action or struct data type
			 */
			virtual IField *mkField(
					const std::string		&name,
					IBaseItem				*field_type,
					IField::FieldAttr		attr=IField::FieldAttr_None) = 0;

			virtual IBinaryExpr *mkBinExpr(
					IExpr 					*lhs,
					IBinaryExpr::BinOpType	op,
					IExpr 					*rhs) = 0;

			virtual ILiteral *mkIntLiteral(int64_t v) = 0;

			virtual ILiteral *mkBitLiteral(uint64_t v) = 0;

			virtual ILiteral *mkBoolLiteral(bool v) = 0;

			virtual ILiteral *mkStringLiteral(const std::string &v) = 0;

			virtual IConstraintBlock *mkConstraintBlock(const std::string &name) = 0;

			virtual IConstraintExpr *mkConstraintExpr(IExpr *expr) = 0;

			virtual IConstraintIf *mkConstraintIf(
					IExpr 			*cond,
					IConstraint 	*true_c,
					IConstraint 	*false_c) = 0;

	};

}



#endif /* SRC_PROGRAMMATIC_IMODEL_H_ */
