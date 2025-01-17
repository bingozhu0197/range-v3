/// \file
// Range v3 library
//
//  Copyright Andrey Diduh 2019
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGE_V3_DETAIL_BIND_BACK_HPP
#define RANGE_V3_DETAIL_BIND_BACK_HPP

#include <tuple>

#include <meta/meta.hpp>

#include <range/v3/range_fwd.hpp>

#include <range/v3/functional/invoke.hpp>
#include <range/v3/utility/tuple_algorithm.hpp>

namespace ranges
{
    // bind_back like std::bind_front has no special treatment for nested
    // bind-expressions or reference_wrappers; there is no need to wrap
    // Callables with ranges::protect.
    namespace detail
    {
        template<typename Fn, typename... Args>
        struct bind_back_fn_
        {
            std::tuple<Fn, Args...> fn_args_;

            template<typename... CallArgs>
                constexpr auto operator()(CallArgs &&... cargs) &&
                noexcept(is_nothrow_invocable_v<Fn, CallArgs..., Args...>)
                    -> invoke_result_t<Fn, CallArgs..., Args...>
            {
                return tuple_apply(
                    [&](auto && fn, auto &&... args) -> decltype(auto) {
                        return invoke((decltype(fn))fn,
                                      (CallArgs &&) cargs...,
                                      (decltype(args))args...);
                    },
                    (std::tuple<Fn, Args...> &&) fn_args_);
            }

            /// \overload
            template<typename... CallArgs>
                constexpr auto operator()(CallArgs &&... cargs) &
                noexcept(is_nothrow_invocable_v<Fn &, CallArgs..., Args &...>)
                    -> invoke_result_t<Fn &, CallArgs..., Args &...>
            {
                return tuple_apply(
                    [&](auto & fn, auto &... args) -> decltype(auto) {
                        return invoke(fn, (CallArgs &&) cargs..., args...);
                    },
                    fn_args_);
            }

            /// \overload
            template<typename... CallArgs>
            constexpr auto operator()(CallArgs &&... cargs) const
                noexcept(is_nothrow_invocable_v<Fn const &, CallArgs..., Args const &...>)
                    -> invoke_result_t<Fn const &, CallArgs..., Args const &...>
            {
                return tuple_apply(
                    [&](auto & fn, auto &... args) -> decltype(auto) {
                        return invoke(fn, (CallArgs &&) cargs..., args...);
                    },
                    fn_args_);
            }
        };

        template<typename Fn, typename... Args>
        using bind_back_fn = bind_back_fn_<decay_t<Fn>, decay_t<Args>...>;
    } // namespace detail

    struct bind_back_fn
    {
        template<typename Fn, typename Arg1, typename... Args>
        constexpr auto operator()(Fn && fn, Arg1 && arg1, Args &&... args) const
            -> detail::bind_back_fn<Fn, Arg1, Args...>
        {
#if defined(__GNUC__) && !defined(__clang__) && __GNUC__ <= 5
            using detail::decay_t;
            using T = std::tuple<decay_t<Fn>, decay_t<Arg1>, decay_t<Args>...>;
            return {T{(Fn &&) fn, (Arg1 &&) arg1, (Args &&) args...}};
#else
            return {{(Fn &&) fn, (Arg1 &&) arg1, (Args &&) args...}};
#endif
        }
    };

    /// \ingroup group-utility
    /// \sa `bind_back_fn`
    RANGES_INLINE_VARIABLE(bind_back_fn, bind_back)

} // namespace ranges

#endif // RANGE_V3_DETAIL_BIND_BACK_HPP
