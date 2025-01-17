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

#include <utility> // for std::swap on C++14.
#include <map>
#include <set>
#include <list>
#include <sstream>
#include <string>
#include <vector>
#include <range/v3/core.hpp>
#include <range/v3/view/any_view.hpp>
#include <range/v3/view/concat.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/repeat.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/for_each.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/zip.hpp>
#include "../simple_test.hpp"
#include "../test_utils.hpp"

RANGES_DIAGNOSTIC_IGNORE_DEPRECATED_DECLARATIONS

int main()
{
    using namespace ranges;

    // 1-d vector

    auto v = view::ints | view::take(10) | to<std::vector>();
    ::check_equal(v, {0,1,2,3,4,5,6,7,8,9});

    v = view::iota(10) | view::take(10) | view::reverse | to<std::vector>();
    ::check_equal(v, {19,18,17,16,15,14,13,12,11,10});

    // 1-d list

    auto l = view::ints | view::take(10) | to<std::list>();
    ::check_equal(l, {0,1,2,3,4,5,6,7,8,9});

    l = view::iota(10) | view::take(10) | view::reverse | to<std::list>();
    ::check_equal(l, {19,18,17,16,15,14,13,12,11,10});

    // 2-d vector

    auto vv = view::repeat_n(view::ints(0, 8), 10) | to<std::vector<std::vector<int>>>();
    ::check_equal(vv, std::vector<std::vector<int>>(10, {0,1,2,3,4,5,6,7}));

    // issue #556

    {
        std::string s{"abc"};
        any_view<any_view<char, category::random_access>, category::random_access> v1 =
            view::single(s | view::drop(1));
        any_view<any_view<char, category::random_access>, category::random_access> v2 =
            view::single(s | view::drop(2));
        auto v3 = view::concat(v1, v2);

        auto owner1 = v3 | to<std::vector<std::vector<char>>>();
        auto owner2 = v3 | to<std::vector<std::string>>();

        ::check_equal(owner1, std::vector<std::vector<char>>{{'b', 'c'}, {'c'}});
        ::check_equal(owner2, std::vector<std::string>{{"bc"}, {"c"}});
    }

    // map

    auto to_string = [](int i){ std::stringstream str; str << i; return str.str(); };
    auto m = view::zip(view::ints, view::ints | view::transform(to_string)) |
        view::take(5) | to<std::map<int, std::string>>();
    using P = std::pair<int const, std::string>;
    ::check_equal(m, {P{0,"0"}, P{1,"1"}, P{2,"2"}, P{3,"3"}, P{4,"4"}});

    // Another way to say the same thing, but with a range comprehension:
    m = view::for_each(view::ints(0,5), [&](int i) {
            return yield(std::make_pair(i, to_string(i)));
        }) | to<std::map<int, std::string>>();
    ::check_equal(m, {P{0,"0"}, P{1,"1"}, P{2,"2"}, P{3,"3"}, P{4,"4"}});

    // set

    CPP_assert(Range<std::set<int>>);
    CPP_assert(!View<std::set<int>>);
    auto s = view::ints | view::take(10) | to<std::set<int>>();
    ::check_equal(s, {0,1,2,3,4,5,6,7,8,9});

    static_assert(!View<std::initializer_list<int>>, "");

    return ::test_result();
}
