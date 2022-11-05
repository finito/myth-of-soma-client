// Generate code for string conversion of Enum using Boost.Preprocessor library.
// Taken from http://stackoverflow.com/questions/5093460/how-to-convert-an-enum-type-variable-to-a-string/5094430#5094430
#include <boost/preprocessor.hpp>

#define X_DEFINE_ENUM_WITH_STRING_CONVERSIONS_TOSTRING_CASE(r, data, elem) \
    case elem : return BOOST_PP_STRINGIZE(elem);

#define DEFINE_ENUM_WITH_STRING_CONVERSIONS(name, enumerators)             \
	enum name {                                                            \
	BOOST_PP_SEQ_ENUM(enumerators)                                         \
	};                                                                     \
	                                                                       \
	inline const char* ToString(name v)                                    \
	{                                                                      \
	switch (v)                                                             \
		{                                                                  \
		BOOST_PP_SEQ_FOR_EACH(                                             \
		X_DEFINE_ENUM_WITH_STRING_CONVERSIONS_TOSTRING_CASE,               \
		name,                                                              \
		enumerators                                                        \
		)                                                                  \
	default: return "[Unknown " BOOST_PP_STRINGIZE(name) "]";              \
		}                                                                  \
	}