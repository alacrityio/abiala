#pragma once

#include "map_macro.h"
#include <type_traits>

namespace alaio { namespace reflection {

   template <typename T>
   struct has_for_each_field {
    private:
      struct F {
         template <typename A, typename B>
         void operator()(const A&, const B&);
      };

      template <typename C>
      static char test(decltype(alaio_for_each_field((C*)nullptr, std::declval<F>()))*);

      template <typename C>
      static long test(...);

    public:
      static constexpr bool value = sizeof(test<T>((void*)nullptr)) == sizeof(char);
   };

   template <typename T>
   inline constexpr bool has_for_each_field_v = has_for_each_field<T>::value;

#define ALAIO_REFLECT_MEMBER(STRUCT, FIELD)                                                                            \
   f(#FIELD, [](auto p) -> decltype(&std::decay_t<decltype(*p)>::FIELD) { return &std::decay_t<decltype(*p)>::FIELD; });

#define ALAIO_REFLECT_STRIP_BASEbase
#define ALAIO_REFLECT_BASE(STRUCT, BASE)                                                                               \
   static_assert(std::is_base_of_v<ALAIO_REFLECT_STRIP_BASE##BASE, STRUCT>, #BASE " is not a base class of " #STRUCT); \
   alaio_for_each_field((ALAIO_REFLECT_STRIP_BASE##BASE*)nullptr, f);

#define ALAIO_REFLECT_SIGNATURE(STRUCT, ...)                                                                           \
   [[maybe_unused]] inline constexpr const char* get_type_name(STRUCT*) { return #STRUCT; }                                      \
   template <typename F>                                                                                               \
   constexpr void alaio_for_each_field(STRUCT*, F f)

/**
 * ALAIO_REFLECT(<struct>, <member or base spec>...)
 * Each parameter should be either the keyword 'base' followed by a base class of the struct or
 * an identifier which names a non-static data member of the struct.
 */
#define ALAIO_REFLECT(...)                                                                                             \
   ALAIO_REFLECT_SIGNATURE(__VA_ARGS__) { ALAIO_MAP_REUSE_ARG0(ALAIO_REFLECT_INTERNAL, __VA_ARGS__) }

// Identity the keyword 'base' followed by at least one token
#define ALAIO_REFLECT_SELECT_I(a, b, c, d, ...) ALAIO_REFLECT_##d
#define ALAIO_REFLECT_IS_BASE() ~, ~
#define ALAIO_REFLECT_IS_BASE_TESTbase ~, ALAIO_REFLECT_IS_BASE

#define ALAIO_APPLY(m, x) m x
#define ALAIO_CAT(x, y) x##y
#define ALAIO_REFLECT_INTERNAL(STRUCT, FIELD)                                                                          \
   ALAIO_APPLY(ALAIO_REFLECT_SELECT_I, (ALAIO_CAT(ALAIO_REFLECT_IS_BASE_TEST, FIELD()), MEMBER, BASE, MEMBER))         \
   (STRUCT, FIELD)

}} // namespace alaio::reflection
