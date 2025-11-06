#pragma once
#include "TypeName.hpp"
#include <type_traits>

#ifndef uuid846698ec_828e_4b71_b1db_3d7b75d70a8e
#define uuid846698ec_828e_4b71_b1db_3d7b75d70a8e

template<bool is_const = false>
class AnyRefGeneric {
public:

    using Ptr = std::conditional_t<is_const, const void *, void *>;
    using TypeHash = std::size_t;

        ///Create empty AnyRef
    constexpr AnyRefGeneric():_ref(nullptr), _type(type_name_hash<std::nullptr_t>) {}

    ///Create AnyRef from a Type (must not be AnyRef or ConstAnyRef)
    template<typename T>
    requires(!std::is_same_v<std::decay_t<T>, AnyRefGeneric<true> > && !std::is_same_v<std::decay_t<T>, AnyRefGeneric<false> >)
    constexpr AnyRefGeneric(T &val):_ref(&val),_type(type_name_hash<std::remove_cvref_t<T> >) {}

    constexpr AnyRefGeneric(const AnyRefGeneric<false> &other) requires (is_const) 
        :_ref(other._ref), _type(other._type) {}

    ///Copy and move
    constexpr AnyRefGeneric(const AnyRefGeneric &other) = default;
    ///Move constructor
    constexpr AnyRefGeneric(AnyRefGeneric &&other) = default;
    ///Copy and move assignment
    constexpr AnyRefGeneric &operator=(const AnyRefGeneric &other) = default;
    ///Move assignment
    constexpr AnyRefGeneric &operator=(AnyRefGeneric &&other) = default;
    ///Check whether AnyRef contains a value
    constexpr explicit operator bool () const {return _ref != nullptr;}
    ///Check whether AnyRef holds a value of type T
    template<typename T>
    constexpr friend bool holds_alternative(const AnyRefGeneric &inst) {
        return inst._type == type_name_hash<std::remove_cvref_t<T> >;
    }
    ///Get the value of type T 
    template<typename T>
    constexpr friend auto get(const AnyRefGeneric &inst) 
        -> std::add_lvalue_reference_t<std::conditional_t<is_const, std::add_const_t<T>, T> > {
            using PtrRet = std::add_pointer_t<std::conditional_t<is_const, std::add_const_t<T>, T> >;
        return *static_cast<PtrRet>(inst._ref);
    }

protected:
    Ptr _ref;
    TypeHash _type;
};


using AnyRef = AnyRefGeneric<false>;
using AnyRefConst = AnyRefGeneric<true>; 

#endif