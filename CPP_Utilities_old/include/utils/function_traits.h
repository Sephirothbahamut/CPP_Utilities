//    Copyright (c) 2019 Will Wray https://keybase.io/willwray
//
//   Distributed under the Boost Software License, Version 1.0.
//          (http://www.boost.org/LICENSE_1_0.txt)
//
//   Repo: https://github.com/willwray/function_traits

#include <type_traits>

/*
  "function_traits.hpp": function signature, cvref and noexcept traits
   ^^^^^^^^^^^^^^^^^^^
       Type trait: A compile-time template-based interface to
                     query or modify the properties of types.

   This header provides traits for C++ function types:

     - Function signature:  R(P...) or R(P..., ...)
         - Return type      R
         - Parameter types    P...
         - Presence of variadic parameter pack ... 'varargs'
           ...C-style trailing elipsis: 2 combinations

     - Function cvref qualifiers       12 combinations total:
         - cv:  const and/or volatile   4 combinations
         - ref: lvalue & or rvalue &&  x3 combinations

     - Function exception specification:
        - noexcept(bool): true|false    2 combinations

   Function 'signature' here refers to return type R and parameters P...
   (with optional C-style varargs but without qualifiers and noexcept).
   The function_signature trait extracts just this R(P...[,...]) part:

     function_signature_t<F> // type alias yielding a function type =
                             // F with cvref quals and noexcept removed
     function_signature<F>   // class with public type alias member
                             //   type = function_signature_t<F>
   Other 'signature' traits:

     function_return_type<F> // class with member type alias for R
     function_arg_types<F>   // a typelist of F's arg types P...

   This library follows std trait conventions as appropriate:

     '_t' suffix for result type   (the trait is a type alias template)
     '_v' suffix for result value  (the trait is a variable template)
     no suffix for a result class (the trait is a class template
                                    with 'type' or 'value' member)

 ** All traits prefixed with 'function_' are *only* well defined for  **
 ** function types; instantiating with non-function type is an error. **

 Predicate traits
 ================
   Two 'top level' predicates classify function types among all types

     is_function<T>       // ltl:: equivalent of std::is_function
     is_free_function<T>  // Is T a valid type for a free function?

   These traits always evaluate true or false (for the _v variant;
   the trait class inherits from std::true_type | std::false_type).

   * 'Free' function types are function types without cvref qualifiers.
   * Function types with cvref qualifiers are 'abominable' - see docs.

   Nine function_trait predicates test for; const, volatile, cv, cvref,
   reference, reference_lvalue, reference_rvalue, noexcept and variadic.

   There are two versions:
                is_function_*<T> // empty class for non-function type T
                function_is_*<F> // compile error for non-function type
   I.e.:
   The function_is_*<F> traits are for Fs known to be of function type.

   The is_function_*<T> traits are 'SFINAE-friendly' predicates that
    - inherit from std::bool_constant<*> for function type T or
    - inherit from an empty class for non-function type T
   (there is no 'is_function_*_v' variant - use 'function_is_*_v').

   For example, here are bad and good definitions of is_free_function_v:

    template <typename T>
    inline constexpr bool is_free_function_v

    (1) = ltl::is_function_v<T> && !ltl::function_is_cvref_v<T>; // BAD

    (2) = std::conjunction_v< ltl::is_function<T>,           // CORRECT
                std::negation<ltl::is_function_cvref<T>> >;

    (3) = [] { if constexpr (ltl::is_function_v<T>)     // ALSO CORRECT
                  return !ltl::function_is_cvref_v<T>;
               return false; }();

   Instantiation of function_is_cvref<T> fails for non-function type T
   so it must be 'guarded' e.g. by logic traits (2) or constexpr-if (3)
   (note - ltl::is_function avoids redundant work of std::is_function).

 Modifying traits
 ================
   Conventional 'add' and 'remove' traits modify their named trait:

     function_add_noexcept_t<F> // add noexcept specifier
     function_remove_cvref_t<F> // remove all cv and ref qualifiers
                                // leaving signature and exception spec

   Unconventionally, this library also provides transforming / mutating
   'set' traits that take extra template arguments, e.g.:

     function_set_noexcept_t<F,B>  // set noexcept(B) for bool const B
     function_set_signature_t<F,S> // set S as the function signature
                                   // keeping cvref-nx of function F

   Reference qualifiers are represented by an enum type ref_qual:

     - null_ref_v    no reference qualifier, same as ref_qual{}
     - lval_ref_v    lvalue reference qualifier: &
     - rval_ref_v    rvalue reference qualifier: &&

   Addition of ref_qual values gives the same reference-collapsed result
   as compounding references on ordinary object types.

   A pair of variable templates give the reference type of T or F:

     reference_v<T>          // ordinary type top-level ref_qual value
     function_reference_v<F> // function type ref_qual value

   The 'set_reference' traits then allow to copy between function types:

     // copy the reference qualifiers from G to F (with no collapse)
     function_set_reference<F, function_reference_v<G>>

     function_set_reference<F, lval_ref_v>   // set ref qual to &
     function_set_reference_lvalue<F>        // set ref qual to &

     function_set_reference<F, null_ref_v>   // set ref qual to none
     function_remove_reference<F>            // set ref qual to none

     function_add_reference<F, rval_ref_v>   // does reference collapse
                                             // (so F& + && = F&)

   A trait is provided to copy all cvref qualifiers, otherwise verbose:

     function_set_cvref_as_t<F,G> // copy cvref quals of G to F
*/

#if !defined(__cpp_noexcept_function_type)
#error function_traits requires c++17 support for noexcept function types \
(MSVC: /Zc:noexceptTypes- must not be used)
#endif

// GCC and Clang deduce noexcept via partial specialization
// MSVC doesn't deduce yet (early 2019 V 15.9.4 Preview 1.0)
#if defined(__GNUC__)
#   define NOEXCEPT_DEDUCED
#endif

// Fallback macro switch for lack of noexcept deduction
#if defined(NOEXCEPT_DEDUCED)
#   define NOEXCEPT_ND(NON, ...) __VA_ARGS__
#else
#   define NOEXCEPT_ND(NON, ...) NON
#endif

// Test noexcept deduction - compile fail if deduction fails
#if defined(NOEXCEPT_DEDUCED)
namespace test {
    constexpr void voidfn();
    // GCC appears to need R introduced & deduced in order to deduce X
    template <typename R, bool X>
    constexpr auto noexcept_deduction(R() noexcept(X))->std::true_type;
    constexpr auto noexcept_deduction(...)->std::false_type;
    static_assert(decltype(noexcept_deduction(voidfn))(),
        "NOEXCEPT_DEDUCED flag is set but deduction fails");
    } // namespace test
#endif

namespace ltl
    {
    // function_traits<F>
    // class template: a collection of member traits for function type F
    //                 or an incomplete type for non-function type F
    template <typename F> class function_traits;
    // Note: the 24 (or 48) partial specializations for function_traits<F>
    // are generated by macro-expansion.

    // A default type-list type for returning function parameter types
    template <typename...> struct arg_types;

    // ref_qual: a value to represent a reference qualifier
    //   null_ref_v    no reference qualifier
    //   rval_ref_v    rvalue reference qualifier: &&
    //   lval_ref_v    lvalue reference qualifier: &
    enum ref_qual { null_ref_v, rval_ref_v, lval_ref_v = 3 };

    // ref_qual operator+( ref_qual, ref_qual)
    // 'adds' reference qualifiers with reference collapse
    constexpr ref_qual operator+(ref_qual a, ref_qual b)
        {
        return static_cast<ref_qual>(a | b);
        }

    // reference_v<T> is a ref_qual value inidicating whether type T is
    // lvalue-reference, rvalue-reference or not a reference.
    template <typename T>
    inline constexpr
        ref_qual
        reference_v = std::is_lvalue_reference_v<T> ? lval_ref_v
        : std::is_rvalue_reference_v<T> ? rval_ref_v : null_ref_v;

    // function_reference_v<F> is a ref_qual value of the reference qualifier on a
    // function type - it is well defined only for function type arguments.
    template <typename F>
    inline constexpr
        ref_qual
        function_reference_v =
        typename function_traits<F>::is_reference_lvalue() ? lval_ref_v
        : typename function_traits<F>::is_reference_rvalue() ? rval_ref_v : null_ref_v;


    namespace impl
        {
        // function_cvref_nx<setter, c, v, ref, nx>
        // Convenience collection of type aliases for cvref & noexcept properties.
        // Injected setter template set_cvref_nx is used for template set_* aliases.
        template <template <bool, bool, ref_qual, bool> typename set_cvref_nx,
            bool c, bool v, ref_qual ref, bool nx>
            struct function_cvref_nx
            {
            using is_const = std::bool_constant<c>;
            using is_volatile = std::bool_constant<v>;
            using is_reference_lvalue = std::bool_constant<ref == lval_ref_v>;
            using is_reference_rvalue = std::bool_constant<ref == rval_ref_v>;
            using is_noexcept = std::bool_constant<nx>;

            using is_cv = std::bool_constant<c || v>;  // Note: const OR volatile
            using is_reference = std::bool_constant<ref != null_ref_v>;
            using is_cvref = std::bool_constant<c || v || ref != null_ref_v>;

            template <bool C> using set_const_t = set_cvref_nx<C, v, ref, nx>;
            template <bool V> using set_volatile_t = set_cvref_nx<c, V, ref, nx>;
            template <bool C, bool V> using set_cv_t = set_cvref_nx<C, V, ref, nx>;
            template <ref_qual R> using set_reference_t = set_cvref_nx<c, v, R, nx>;
            template <bool C, bool V, ref_qual R = null_ref_v>
            using set_cvref_t = set_cvref_nx<C, V, R, nx>;
            template <bool NX> using set_noexcept_t = set_cvref_nx<c, v, ref, NX>;

            template <bool C> using set_const = function_traits<set_const_t<C>>;
            template <bool V> using set_volatile = function_traits<set_volatile_t<V>>;
            template <bool C, bool V> using set_cv = function_traits<set_cv_t<C, V>>;
            template <ref_qual R>
            using set_reference = function_traits<set_reference_t<R>>;
            template <bool C, bool V, ref_qual R = null_ref_v>
            using set_cvref = function_traits<set_cvref_t<C, V, R>>;
            template <bool NX> using set_noexcept = function_traits<set_noexcept_t<NX>>;
            };

        // function_base<F>:
        // Base class template for function_traits<F> holding F's 'signature'
        //   R(P...)     - return type R, parameter types P..., or
        //   R(P...,...) - with a trailing variadic parameter pack ...
        // but not including any function cvref qualifiers or noexcept specifier.
        template <typename F> class function_base;
        // A macro definition is used to expand non-variadic and variadic signatures.
        // Clang warns when a variadic signature omits the comma; R(P... ...), so
        // __VA_ARGS__ = ,... includes the leading comma, present as needed
        // (C++20's __VA_OPT__(,...) is another way to expand with leading comma).
        // MSVC doesn't handle empty variadic macro, so add a BOGUS macro parameter.

        // function_base<F> specialisations for non-variadic and variadic signatures
#define FUNCTION_BASE(BOGUS,...) \
template <typename R, typename... P>                                   \
class function_base<R(P...__VA_ARGS__)>                                \
{                                                                      \
 public:                                                               \
  using return_type_t = R;                                             \
  using signature_t = R(P...__VA_ARGS__);                              \
  using is_variadic = std::bool_constant<bool(#__VA_ARGS__[0])>;       \
  template <template <typename...> typename T=arg_types>               \
  using arg_types = T<P...>;                                           \
  template <typename T> struct id { using type = T; };\
  template <bool c, bool v, ref_qual r, bool nx>\
  static constexpr auto set_cvref_noexcept()\
  {\
    if constexpr (r == null_ref_v) {\
      if constexpr(!v) {\
        if constexpr(!c)\
          return id<R(P...__VA_ARGS__) noexcept(nx)>{};\
        else\
          return id<R(P...__VA_ARGS__) const noexcept(nx)>{};\
      } else {\
        if constexpr(!c)\
          return id<R(P...__VA_ARGS__) volatile noexcept(nx)>{};\
        else\
          return id<R(P...__VA_ARGS__) const volatile noexcept(nx)>{};\
      }\
    } else if constexpr (r == lval_ref_v) {\
      if constexpr(!v) {\
        if constexpr(!c)\
          return id<R(P...__VA_ARGS__) & noexcept(nx)>{};\
        else\
          return id<R(P...__VA_ARGS__) const & noexcept(nx)>{};\
      } else {\
        if constexpr(!c)\
          return id<R(P...__VA_ARGS__) volatile & noexcept(nx)>{};\
        else\
          return id<R(P...__VA_ARGS__) const volatile & noexcept(nx)>{};\
      }\
    } else {\
      if constexpr(!v) {\
        if constexpr(!c)\
          return id<R(P...__VA_ARGS__) && noexcept(nx)>{};\
        else\
          return id<R(P...__VA_ARGS__) const && noexcept(nx)>{};\
      } else {\
        if constexpr(!c)\
          return id<R(P...__VA_ARGS__) volatile && noexcept(nx)>{};\
        else\
          return id<R(P...__VA_ARGS__) const volatile && noexcept(nx)>{};\
      }\
    }\
  }\
  template <bool c, bool v, ref_qual r, bool nx>\
  using set_cvref_noexcept_t = typename decltype(\
        set_cvref_noexcept<c,v,r,nx>())::type;\
} // Macro end ////////////////////////////////////////////////////////////////

        FUNCTION_BASE(, );
        FUNCTION_BASE(, , ...); // leading comma forwarded via macro varargs
#undef FUNCTION_BASE

        } // namespace impl

        // function_traits<F> specializations for 24 cvref varargs combinations
        //                          or for 48 cvref varargs noexcept combinations
#define CV_REF(CV,REF,NX,...) \
template <typename R, typename... P NOEXCEPT_ND(,,bool X)>                   \
class function_traits<R(P...__VA_ARGS__) CV REF noexcept(NOEXCEPT_ND(NX,X))> \
    : public impl::function_base<R(P...__VA_ARGS__)>,                        \
      public impl::function_cvref_nx<                                        \
          impl::function_base<R(P...__VA_ARGS__)>::                          \
          template set_cvref_noexcept_t,                                     \
          std::is_const_v<int CV>, std::is_volatile_v<int CV>,               \
          reference_v<int REF>, NOEXCEPT_ND(NX,X)>                           \
{                                                                            \
  template <typename> struct set_signature;                                  \
  template <typename r, typename... p> struct set_signature<r(p...)> {       \
    using type = r(p...) CV REF noexcept(NOEXCEPT_ND(NX,X)); };              \
  template <typename r, typename... p> struct set_signature<r(p..., ...)> {  \
    using type = r(p..., ...) CV REF noexcept(NOEXCEPT_ND(NX,X)); };         \
public:                                                                      \
  using type = R(P...__VA_ARGS__) CV REF noexcept(NOEXCEPT_ND(NX,X));        \
  using remove_cvref_t = R(P...__VA_ARGS__) noexcept(NOEXCEPT_ND(NX,X));     \
  template <typename r> using set_return_type_t =                            \
      r(P...__VA_ARGS__) CV REF noexcept(NOEXCEPT_ND(NX,X));                 \
  template <bool V> using set_variadic_t = std::conditional_t<V,             \
      R(P..., ...) CV REF noexcept(NOEXCEPT_ND(NX,X)),                       \
      R(P...) CV REF noexcept(NOEXCEPT_ND(NX, X))>;                          \
  template <typename B>                                                      \
  using set_signature_t = typename set_signature<B>::type;                   \
};

// CV_REF_QUALIFIERS(...)
// X-macro list to expand the 12 cv-ref combos, and
//   pass through X; optional true|false noexcept(bool) specification, and ...
//   ... varargs to pass through C/C++ varargs (inluding a leading comma)
#define CV_REF_QUALIFIERS(X, ...)           \
  CV_REF(, , X, __VA_ARGS__)                \
  CV_REF(, &, X, __VA_ARGS__)               \
  CV_REF(, &&, X, __VA_ARGS__)              \
  CV_REF(const, , X, __VA_ARGS__)           \
  CV_REF(const, &, X, __VA_ARGS__)          \
  CV_REF(const, &&, X, __VA_ARGS__)         \
  CV_REF(volatile, , X, __VA_ARGS__)        \
  CV_REF(volatile, &, X, __VA_ARGS__)       \
  CV_REF(volatile, &&, X, __VA_ARGS__)      \
  CV_REF(const volatile, , X, __VA_ARGS__)  \
  CV_REF(const volatile, &, X, __VA_ARGS__) \
  CV_REF(const volatile, &&, X, __VA_ARGS__)

// X-macro list to expand all 24 or 48 variadic,cv,ref,[noexcept] combos
#if defined(NOEXCEPT_DEDUCED)
    CV_REF_QUALIFIERS(, )
        CV_REF_QUALIFIERS(, , ...) // leading comma for variadic match
#else
    CV_REF_QUALIFIERS(true, )
        CV_REF_QUALIFIERS(true, , ...) // leading comma for variadic match
        CV_REF_QUALIFIERS(false, )
        CV_REF_QUALIFIERS(false, , ...) // leading comma for variadic match
#endif
#undef CV_REF
#undef CV_REF_QUALIFIERS
#undef NOEXCEPT_DEDUCED
#undef NOEXCEPT_ND


// Implementation detail for is_function
// (this SFINAE impl can be removed in C++20 - see comment below).
namespace impl
        {
        template <typename T, typename = decltype(sizeof(int))>
        inline constexpr bool is_function_v = false;

        template <typename T>
        inline constexpr bool is_function_v<T, decltype(sizeof(function_traits<T>))>
            = true;
        } // namespace impl

        // ltl::is_function is equivalent to std::is_function
        // Using this definition saves redundant instantiation of std::is_function
    template <typename T> struct is_function
        : std::bool_constant<impl::is_function_v<T>> {};
    template <typename T>
    inline constexpr bool is_function_v = impl::is_function_v<T>;
    // = requires {sizeof(function_traits<T>);};
// Note: SFINAE impl can be removed with C++20 concepts as above ^^^

    namespace impl
        {
        // is_function_*<T> traits derive from a predicate_base class that is either
        //    bool_constant<P<T>> of function predicate P for function type T, or
        //    empty_base for non-function type T

        struct empty_base {};

        // pred_base<P,F>(), for function predicate P, returns (default constructed)
        //  empty_base  for non-function type F, or
        //  P<F>        for function type F (P<F> = std::true_type | std::false_type)
        template <template <typename> typename P, typename F>
        constexpr auto pred_base()
            {
            if constexpr (is_function_v<F>) {
                return P<F>{};
                }
            else {
                return empty_base{};
                }
            }

        template <template <typename> typename P, typename F>
        using predicate_base = decltype(pred_base<P, F>());

        } // namespace impl

        // Predicate traits for c,v,ref,noexcept,variadic properties

        // function_is_* are predicate type trait aliases to true_type / false_type
        //               or compile fail for non-function type argument
    template <typename F> using function_is_const
        = typename function_traits<F>::is_const;

    template <typename F> using function_is_volatile
        = typename function_traits<F>::is_volatile;

    template <typename F> using function_is_cv
        = typename function_traits<F>::is_cv;

    template <typename F> using function_is_reference
        = typename function_traits<F>::is_reference;

    template <typename F> using function_is_reference_lvalue
        = typename function_traits<F>::is_reference_lvalue;

    template <typename F> using function_is_reference_rvalue
        = typename function_traits<F>::is_reference_rvalue;

    template <typename F> using function_is_cvref
        = typename function_traits<F>::is_cvref;

    template <typename F> using function_is_noexcept
        = typename function_traits<F>::is_noexcept;

    template <typename F> using function_is_variadic
        = typename function_traits<F>::is_variadic;

    // function_is_*_v are predicate value traits, equal to true / false
    //               or compile fail for non-function type argument
    template <typename F> inline constexpr bool function_is_const_v =
        function_is_const<F>();

    template <typename F> inline constexpr bool function_is_volatile_v =
        function_is_volatile<F>();

    template <typename F> inline constexpr bool function_is_cv_v =
        function_is_cv<F>();

    template <typename F> inline constexpr bool function_is_reference_v =
        function_is_reference<F>();

    template <typename F> inline constexpr bool function_is_reference_lvalue_v =
        function_is_reference_lvalue<F>();

    template <typename F> inline constexpr bool function_is_reference_rvalue_v =
        function_is_reference_rvalue<F>();

    template <typename F> inline constexpr bool function_is_cvref_v =
        function_is_cvref<F>();

    template <typename F> inline constexpr bool function_is_noexcept_v =
        function_is_noexcept<F>();

    template <typename F> inline constexpr bool function_is_variadic_v =
        function_is_variadic<F>();

    // is_function_* are 'lazy' predicate type traits, safe to call for any type
    //               inherit from true_type / false_type
    //               or empty base for non-function type argument
    template <typename F> struct is_function_const :
        impl::predicate_base<function_is_const, F> {};

    template <typename F> struct is_function_volatile :
        impl::predicate_base<function_is_volatile, F> {};

    template <typename F> struct is_function_cv :
        impl::predicate_base<function_is_cv, F> {};

    template <typename F> struct is_function_reference :
        impl::predicate_base<function_is_reference, F> {};

    template <typename F> struct is_function_reference_lvalue :
        impl::predicate_base<function_is_reference_lvalue, F> {};

    template <typename F> struct is_function_reference_rvalue :
        impl::predicate_base<function_is_reference_rvalue, F> {};

    template <typename F> struct is_function_cvref :
        impl::predicate_base<function_is_cvref, F> {};

    template <typename F> struct is_function_noexcept :
        impl::predicate_base<function_is_noexcept, F> {};

    template <typename F> struct is_function_variadic :
        impl::predicate_base<function_is_variadic, F> {};

    namespace impl
        {
        template <typename T>
        constexpr bool is_free_function()
            {
            if constexpr (is_function_v<T>)
                return !function_is_cvref_v<T>;
            return false;
            }
        } // namespace impl


        // is_free_function_v<T> : checks if type T is a free function type
        //   true if T is a function type without cvref qualifiers
        //   false if T is not a function type or is a cvref qualified function type
    template <typename T>
    inline constexpr bool is_free_function_v = impl::is_free_function<T>();

    template <typename T> struct is_free_function
        : std::bool_constant<is_free_function_v<T>> {};

    // set_const, add_const / remove_const
    template <typename F, bool C>
    using function_set_const = typename function_traits<F>::template set_const<C>;
    template <typename F, bool C>
    using function_set_const_t =
        typename function_traits<F>::template set_const_t<C>;

    template <typename F> using function_add_const = function_set_const<F, true>;
    template <typename F>
    using function_add_const_t = function_set_const_t<F, true>;

    template <typename F>
    using function_remove_const = function_set_const<F, false>;
    template <typename F>
    using function_remove_const_t = function_set_const_t<F, false>;

    // set_volatile, add_volatile / remove_volatile
    template <typename F, bool V>
    using function_set_volatile =
        typename function_traits<F>::template set_volatile<V>;
    template <typename F, bool V>
    using function_set_volatile_t =
        typename function_traits<F>::template set_volatile_t<V>;

    template <typename F>
    using function_add_volatile = function_set_volatile<F, true>;
    template <typename F>
    using function_add_volatile_t = function_set_volatile_t<F, true>;

    template <typename F>
    using function_remove_volatile = function_set_volatile<F, false>;
    template <typename F>
    using function_remove_volatile_t = function_set_volatile_t<F, false>;

    // set_cv, remove_cv (add_cv would add c AND v while is_cv tests c OR v)
    template <typename F, bool C, bool V>
    using function_set_cv = typename function_traits<F>::template set_cv<C, V>;
    template <typename F, bool C, bool V>
    using function_set_cv_t = typename function_traits<F>::template set_cv_t<C, V>;

    template <typename F>
    using function_remove_cv = function_set_cv<F, false, false>;
    template <typename F>
    using function_remove_cv_t = function_set_cv_t<F, false, false>;

    // set_reference, set_reference_lvalue, set_reference_rvalue
    template <typename F, ref_qual R>
    using function_set_reference =
        typename function_traits<F>::template set_reference<R>;
    template <typename F, ref_qual R>
    using function_set_reference_t =
        typename function_traits<F>::template set_reference_t<R>;

    template <typename F>
    using function_set_reference_lvalue = function_set_reference<F, lval_ref_v>;
    template <typename F>
    using function_set_reference_lvalue_t = function_set_reference_t<F, lval_ref_v>;

    template <typename F>
    using function_set_reference_rvalue = function_set_reference<F, rval_ref_v>;
    template <typename F>
    using function_set_reference_rvalue_t = function_set_reference_t<F, rval_ref_v>;

    // add reference does reference-collapsing
    template <typename F, ref_qual R>
    using function_add_reference = function_set_reference<F,
        function_reference_v<F> +R>;
    template <typename F, ref_qual R>
    using function_add_reference_t =
        function_set_reference_t<F, function_reference_v<F> +R>;

    template <typename F>
    using function_remove_reference = function_set_reference<F, null_ref_v>;
    template <typename F>
    using function_remove_reference_t = function_set_reference_t<F, null_ref_v>;

    // set_cvref, set_cvref_as, remove_cvref
    template <typename F, bool C, bool V, ref_qual R = null_ref_v>
    using function_set_cvref =
        typename function_traits<F>::template set_cvref<C, V, R>;
    template <typename F, bool C, bool V, ref_qual R = null_ref_v>
    using function_set_cvref_t =
        typename function_traits<F>::template set_cvref_t<C, V, R>;

    template <typename F, typename S>
    using function_set_cvref_as =
        function_set_cvref<F, function_is_const_v<S>, function_is_volatile_v<S>,
        function_reference_v<S>>;
    template <typename F, typename S>
    using function_set_cvref_as_t =
        function_set_cvref_t<F, function_is_const_v<S>, function_is_volatile_v<S>,
        function_reference_v<S>>;

    template <typename F>
    using function_remove_cvref_t = typename function_traits<F>::remove_cvref_t;
    template <typename F>
    using function_remove_cvref = function_traits<function_remove_cvref_t<F>>;

    // set_noexcept, add_noexcept / remove_noexcept
    template <typename F, bool N>
    using function_set_noexcept =
        typename function_traits<F>::template set_noexcept<N>;
    template <typename F, bool N>
    using function_set_noexcept_t =
        typename function_traits<F>::template set_noexcept_t<N>;

    template <typename F>
    using function_add_noexcept = function_set_noexcept<F, true>;
    template <typename F>
    using function_add_noexcept_t = function_set_noexcept_t<F, true>;

    template <typename F>
    using function_remove_noexcept = function_set_noexcept<F, false>;
    template <typename F>
    using function_remove_noexcept_t = function_set_noexcept_t<F, false>;

    // set_variadic, add_variadic / remove_variadic
    template <typename F, bool A>
    using function_set_variadic_t =
        typename function_traits<F>::template set_variadic_t<A>;
    template <typename F, bool A>
    using function_set_variadic = function_traits<function_set_variadic_t<F, A>>;

    template <typename F>
    using function_add_variadic = function_set_variadic<F, true>;
    template <typename F>
    using function_add_variadic_t = function_set_variadic_t<F, true>;

    template <typename F>
    using function_remove_variadic = function_set_variadic<F, false>;
    template <typename F>
    using function_remove_variadic_t = function_set_variadic_t<F, false>;

    // return_type
    template <typename F>
    using function_return_type_t = typename function_traits<F>::return_type_t;
    template <typename F> struct function_return_type {
        using type = function_return_type_t<F>;
        };

    // set_return_type
    template <typename F, typename T>
    using function_set_return_type_t =
        typename function_traits<F>::template set_return_type_t<T>;
    template <typename F, typename T> using function_set_return_type =
        function_traits<function_set_return_type_t<F, T>>;

    // signature, equivalent to 'remove_cvref_noexcept'
    template <typename F>
    using function_signature_t = typename function_traits<F>::signature_t;
    template <typename F>
    using function_signature = function_traits<function_signature_t<F>>;

    // set_signature
    template <typename F, typename S>
    using function_set_signature_t =
        typename function_traits<F>::template set_signature_t<S>;
    template <typename F, typename S> using function_set_signature =
        function_traits<function_set_signature_t<F, S>>;

    // arg_types
    template <typename F,
        template <typename...> typename T = arg_types>
    using function_arg_types = typename function_traits<F>::template arg_types<T>;

    } // namespace ltl