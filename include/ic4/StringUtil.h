
#ifndef IC4_STRINGUTIL_H_INC_
#define IC4_STRINGUTIL_H_INC_

#pragma once

#define IC4_C_IN_NAMESPACE
#include "C_Properties.h"

#include <string>
#include <functional>

namespace ic4
{
	namespace detail
	{
		inline std::string string_or_empty(const char* s)
		{
			return s ? s : std::string();
		}
		inline std::string return_string(Error& err, const char* s)
		{
			return updateFromLastErrorReturn(err, s ? s : std::string());
		}
		template<typename TValue>
		inline TValue return_prop_attr(Error& err, c_interface::IC4_PROPERTY* handle, std::function<bool(c_interface::IC4_PROPERTY*, TValue*)> fn)
		{
			TValue value = {};
			if (!fn(handle, &value))
			{
				return detail::updateFromLastErrorReturn(err, value);
			}
			return detail::clearReturn(err, value);
		}
		template<typename TValue>
		inline TValue return_propmap_typed_value(Error& err, c_interface::IC4_PROPERTY_MAP* handle, const char* prop_name, std::function<bool(c_interface::IC4_PROPERTY_MAP*, const char*, TValue*)> fn)
		{
			TValue value = {};
			if (!fn(handle, prop_name, &value))
			{
				return detail::updateFromLastErrorReturn(err, value);
			}
			return detail::clearReturn(err, value);
		}
	}
}

#endif //IC4_STRINGUTIL_H_INC_
