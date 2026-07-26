
#ifndef IC4_PROPID_H_INC_
#define IC4_PROPID_H_INC_

#pragma once

namespace ic4
{
	namespace PropId
	{
		/**
		 * @brief Typed property identifier for integer properties.
		 */
		struct PropIdInteger
		{
#ifndef IC4_DOXYGEN_INVISIBLE
			const char* prop_name;
#endif
		};

		/**
		 * @brief Typed property identifier for float properties.
		 */
		struct PropIdFloat
		{
#ifndef IC4_DOXYGEN_INVISIBLE
			const char* prop_name;
#endif
		};

		/**
		 * @brief Typed property identifier for command properties.
		 */
		struct PropIdCommand
		{
#ifndef IC4_DOXYGEN_INVISIBLE
			const char* prop_name;
#endif
		};

		/**
		 * @brief Typed property identifier for enumeration properties.
		 */
		struct PropIdEnumeration
		{
#ifndef IC4_DOXYGEN_INVISIBLE
			const char* prop_name;
#endif
		};

		/**
		 * @brief Typed property identifier for boolean properties.
		 */
		struct PropIdBoolean
		{
#ifndef IC4_DOXYGEN_INVISIBLE
			const char* prop_name;
#endif
		};

		/**
		 * @brief Typed property identifier for string properties.
		 */
		struct PropIdString
		{
#ifndef IC4_DOXYGEN_INVISIBLE
			const char* prop_name;
#endif
		};

		/**
		 * @brief Typed property identifier for register properties.
		 */
		struct PropIdRegister
		{
#ifndef IC4_DOXYGEN_INVISIBLE
			const char* prop_name;
#endif
		};
	}
}

#endif // IC4_PROPID_H_INC_