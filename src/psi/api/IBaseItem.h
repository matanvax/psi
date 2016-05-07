/*
 * IBaseItem.h
 *
 *  Created on: Apr 26, 2016
 *      Author: ballance
 */

#ifndef SRC_PROGRAMMATIC_IBASEITEM_H_
#define SRC_PROGRAMMATIC_IBASEITEM_H_

namespace psi_api {

	/**
	 * Class: IBaseItem
	 *
	 * Common base API to PSI objects
	 */
	class IBaseItem {

	public:

		enum ItemType {
			TypeAction,
			TypeComponent,
			TypeConstraint,
			TypeField,
			TypeImport,
			TypeExec,
			TypeExtendAction,
			TypeExtendComponent,
			TypeExtendStruct,
			TypeModel,
			TypePackage,
			TypeScalar,
			TypeStruct
		};

		public:
			virtual ~IBaseItem() { }

			/**
			 * Method: getType()
			 * Returns the type of this object.
			 */
			virtual ItemType getType() const = 0;

	};


}




#endif /* SRC_PROGRAMMATIC_IBASEITEM_H_ */
