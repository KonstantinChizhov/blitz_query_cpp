#pragma once

//////////////////////////////////////////////////////////////////////////
// DECLARE_ENUM_OPERATIONS is used to declare enum bitwise operations
// to use enum type as a type safe flags
//////////////////////////////////////////////////////////////////////////

#define DECLARE_ENUM_OPERATIONS(ENUM_NAME)                               \
inline ENUM_NAME                                                         \
operator|(ENUM_NAME left, ENUM_NAME right)                               \
{ return ENUM_NAME(static_cast<int>(left) | static_cast<int>(right)); }  \
                                                                         \
inline ENUM_NAME                                                         \
operator&(ENUM_NAME left, ENUM_NAME right)                               \
{ return ENUM_NAME(static_cast<int>(left) & static_cast<int>(right)); }  \
                                                                         \
inline ENUM_NAME                                                         \
operator^(ENUM_NAME left, ENUM_NAME right)                               \
{ return ENUM_NAME(static_cast<int>(left) ^ static_cast<int>(right)); }  \
                                                                         \
inline ENUM_NAME                                                         \
operator~(ENUM_NAME left)                                                \
{ return ENUM_NAME(~static_cast<int>(left)); }                           \
                                                                         \
inline const ENUM_NAME&                                                  \
operator|=(ENUM_NAME& left, ENUM_NAME right)                             \
{ return left = left | right; }                                          \
                                                                         \
inline const ENUM_NAME&                                                  \
operator&=(ENUM_NAME& left, ENUM_NAME right)                             \
{ return left = left & right; }                                          \
                                                                         \
inline const ENUM_NAME&                                                  \
operator^=(ENUM_NAME& left, ENUM_NAME right)                             \
{ return left = left ^ right; }                                          \


#define DECLARE_ENUM_OPERATIONS_TAMPLATE(ENUM_NAME, TEMPLATE)            \
TEMPLATE                                                                 \
inline ENUM_NAME                                                         \
operator|(ENUM_NAME left, ENUM_NAME right)                               \
{ return ENUM_NAME(static_cast<int>(left) | static_cast<int>(right)); }  \
                                                                         \
TEMPLATE                                                                 \
inline ENUM_NAME                                                         \
operator&(ENUM_NAME left, ENUM_NAME right)                               \
{ return ENUM_NAME(static_cast<int>(left) & static_cast<int>(right)); }  \
                                                                         \
TEMPLATE                                                                 \
inline ENUM_NAME                                                         \
operator^(ENUM_NAME left, ENUM_NAME right)                               \
{ return ENUM_NAME(static_cast<int>(left) ^ static_cast<int>(right)); }  \
                                                                         \
TEMPLATE                                                                 \
inline ENUM_NAME                                                         \
operator~(ENUM_NAME left)                                                \
{ return ENUM_NAME(~static_cast<int>(left)); }                           \
                                                                         \
TEMPLATE                                                                 \
inline const ENUM_NAME&                                                  \
operator|=(ENUM_NAME& left, ENUM_NAME right)                             \
{ return left = left | right; }                                          \
                                                                         \
TEMPLATE                                                                 \
inline const ENUM_NAME&                                                  \
operator&=(ENUM_NAME& left, ENUM_NAME right)                             \
{ return left = left & right; }                                          \
                                                                         \
TEMPLATE                                                                 \
inline const ENUM_NAME&                                                  \
operator^=(ENUM_NAME& left, ENUM_NAME right)                             \
{ return left = left ^ right; }

namespace blitz_query_cpp
{
	template<class Enum>
	bool has_all_flags(Enum value, Enum flags)
	{
		return (static_cast<unsigned>(value) & static_cast<unsigned>(flags)) == static_cast<unsigned>(flags);
	}
	
	template<class Enum>
	bool has_any_flag(Enum value, Enum flags)
	{
		return (static_cast<unsigned>(value) & static_cast<unsigned>(flags)) != 0;
	}
}
