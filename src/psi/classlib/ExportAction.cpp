/*
 * ExportAction.cpp
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
 *  Created on: May 10, 2016
 *      Author: ballance
 */

#include "ExportAction.h"

namespace psi {

ExportAction::ExportAction(
		Type				*p,
		Action				*t_ref) : Type(Type::TypeExportAction, p) {
	setTypeData(t_ref);
}

ExportAction::ExportAction(
		Type 				*p,
		Action				*t_ref,
		const ExprList		&exp_params) :
	Type(Type::TypeExportAction, p), m_exp_params(exp_params) {
	setTypeData(t_ref);
}

ExportAction::~ExportAction() {
	// TODO Auto-generated destructor stub
}

}
