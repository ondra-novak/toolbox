#pragma once
#ifndef uuid386bf845_968d_409a_945b_c7076e13d6b7
#define uuid386bf845_968d_409a_945b_c7076e13d6b7

///@file FunctionView.hpp
/**
Creates a reference (view) to function, callable or lambda without copying and allocating data

Keep on mind, that view is valid only when original callable is valid. Especially when
lambda is passed directly as argument you need to ensure, that lambda is destroyed after this
object, for example when lambda is used as argument to a function call

@code

void do_call(FunctionView<void(int)> fn, int i) {
    fn(i);
}

do_call([&](int){....}, 42);
@endcode

*/


#include <type_traits>
#include <utility>

template<typename Fn> class FunctionView;

namespace _details {

template<bool nx, typename RetVal, typename ... Args>
class FunctionViewImpl {
public:

    using CallFnPtr = RetVal (*)(const void *context, Args && ...)
                                                             noexcept (nx);


    RetVal operator()(Args ... args) const noexcept(nx){
        return _callptr(_context, std::forward<Args>(args) ...);
    }

    template<typename Fn>
    requires(std::is_invocable_r_v<RetVal, Fn, Args...> )
    constexpr FunctionViewImpl(Fn &&fn) {
        using PFn = std::add_pointer_t<std::remove_reference_t<Fn> >;
        using CPFn = std::add_pointer_t<std::add_const_t<std::remove_reference_t<Fn> > >;
        _context = &fn;
        _callptr = [](const void *ctx, Args && ...  args) {
            PFn fptr = const_cast<PFn>(static_cast<CPFn>(ctx));
            return (*fptr)(std::forward<Args>(args)...);
        };
    }

protected:

    CallFnPtr _callptr;
    const void *_context;
};

}

template< class R, class... Args >
class FunctionView<R(Args...)>:
   public _details::FunctionViewImpl<false, R, Args...> {
    using _details::FunctionViewImpl<false, R, Args...>::FunctionViewImpl;
};


template< class R, class... Args >
class FunctionView<R(Args...) noexcept>:
   public _details::FunctionViewImpl<true, R, Args...> {
    using _details::FunctionViewImpl<true, R, Args...>::FunctionViewImpl;
};

#endif