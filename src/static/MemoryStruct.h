/*
 * MemoryStruct.h
 *
 *  Created on: Apr 23, 2016
 *      Author: ballance
 */

#ifndef MEMORYSTRUCT_H_
#define MEMORYSTRUCT_H_

#include "Struct.h"
#include "TypeRegistry.h"

namespace psi {

class MemoryStruct: public Struct {

	public:
		MemoryStruct(
				const std::string 		&name,
				IConstructorContext 	*p,
				Struct					&super_type=Struct::None);

		MemoryStruct(
				const std::string 		&name,
				IConstructorContext 	*p,
				const std::string		&super_type);

		virtual ~MemoryStruct();
};

} /* namespace psi */

#endif /* MEMORYSTRUCT_H_ */