// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3

// HACKHACKHACK silence false positive:
//   error: ‘<anonymous>.ranges::istream_view<int>::cursor::rng_’ may be used uninitialized in this function
// triggered on line 39.
#if defined(__GNUC__) && !defined(__clang__) && __GNUC__ >= 7
#include <range/v3/detail/config.hpp>
RANGES_DIAGNOSTIC_IGNORE("-Wmaybe-uninitialized")
#endif

#include <string>
#include <sstream>
#include <vector>
#include <range/v3/core.hpp>
#include <range/v3/view/istream.hpp>
#include <range/v3/functional/reference_wrapper.hpp>
#include <range/v3/utility/copy.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/replace_if.hpp>
#include <range/v3/view/common.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

int main()
{
    using namespace ranges;

    std::string str{"1 2 3 4 5 6 7 8 9 1 2 3 4 5 6 7 8 9 1 2 3 4 5 6 7 8 9 "};
    std::stringstream sin{str};

    auto && rng = istream<int>(sin) | view::replace_if([](int i){return i == 1; }, 42);
    has_type<int const &>(*begin(rng));
    models<ViewConcept>(aux::copy(rng));
    models_not<SizedViewConcept>(aux::copy(rng));
    models_not<CommonViewConcept>(aux::copy(rng));
    models<InputIteratorConcept>(begin(rng));
    models_not<ForwardIteratorConcept>(begin(rng));

    auto && tmp = rng | view::common;
    has_type<int const &>(*begin(tmp));
    models<CommonViewConcept>(aux::copy(tmp));
    models_not<SizedViewConcept>(aux::copy(tmp));
    models<InputIteratorConcept>(begin(tmp));
    models_not<ForwardIteratorConcept>(begin(tmp));
    std::vector<int> actual{begin(tmp), end(tmp)};
    ::check_equal(actual, {42, 2, 3, 4, 5, 6, 7, 8, 9, 42, 2, 3, 4, 5, 6, 7, 8, 9, 42, 2, 3, 4, 5, 6, 7, 8, 9});

    std::vector<int> vi{1,2,3,4,5,6,7,8,9};
    auto &&rng2 = vi | view::replace_if([](int i){return i == 5;}, 42);
    CPP_assert(Same<range_value_t<decltype(rng2)>, int>);
    has_type<int const &>(*begin(rng2));
    has_type<int const &>(iter_move(begin(rng2)));
    models<ViewConcept>(aux::copy(rng2));
    models<SizedViewConcept>(aux::copy(rng2));
    models<CommonViewConcept>(aux::copy(rng2));
    models<RandomAccessIteratorConcept>(begin(rng2));
    ::check_equal(rng2, {1,2,3,4,42,6,7,8,9});

    int forty_two = 42;
    auto &&rng3 = vi | view::replace_if([](int i){return i == 5;}, ref(forty_two));
    CPP_assert(Same<range_value_t<decltype(rng3)>, int>);
    has_type<int &>(*begin(rng3));
    has_type<int const &>(iter_move(begin(rng3)));
    models<ViewConcept>(aux::copy(rng3));
    models<SizedViewConcept>(aux::copy(rng3));
    models<CommonViewConcept>(aux::copy(rng3));
    models<RandomAccessIteratorConcept>(begin(rng3));
    ::check_equal(rng3, {1,2,3,4,42,6,7,8,9});

    auto &&rng4 = view::ints | view::replace_if([](int i){return i == 5;},42) | view::take(10);
    CPP_assert(Same<range_value_t<decltype(rng4)>, int>);
    has_type<int>(*begin(rng4));
    has_type<int>(iter_move(begin(rng4)));
    models<ViewConcept>(aux::copy(rng4));
    models<SizedViewConcept>(aux::copy(rng4));
    models_not<CommonViewConcept>(aux::copy(rng4));
    models<RandomAccessIteratorConcept>(begin(rng4));
    ::check_equal(rng4, {0,1,2,3,4,42,6,7,8,9});

    // Check with a mutable predicate
    int rgi[] = {0,1,2,3,4,5,6,7,8,9};
    bool flag = false;
    auto mutable_only = view::replace_if(rgi, [flag](int) mutable { return flag = !flag;}, 42);
    ::check_equal(mutable_only, {42,1,42,3,42,5,42,7,42,9});
    CPP_assert(View<decltype(mutable_only)>);
    CPP_assert(!View<decltype(mutable_only) const>);

    {
        int const some_ints[] = {1,2,3,4,5,6,7,8,9,1,2,3,4,5,6,7,8,9,1,2,3,4,5,6,7,8,9};

        auto rng = debug_input_view<int const>{some_ints} |
            view::replace_if([](int i){ return i == 1; }, 42);
        ::check_equal(rng, {42,2,3,4,5,6,7,8,9,42,2,3,4,5,6,7,8,9,42,2,3,4,5,6,7,8,9});
    }

    return test_result();
}
