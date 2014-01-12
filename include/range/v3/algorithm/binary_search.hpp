//  Copyright Neil Groves 2009.
//  Copyright Eric Niebler 2013
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_ALGORITHM_BINARY_SEARCH_HPP
#define RANGES_V3_ALGORITHM_BINARY_SEARCH_HPP

#include <utility>
#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/algorithm/lower_bound.hpp>
#include <range/v3/utility/bindable.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/functional.hpp>

namespace ranges
{
    inline namespace v3
    {
        namespace detail
        {
            template<typename ForwardIterator, typename EndForwardIterator, typename Value,
                typename BinaryPredicate = ranges::less>
            bool
            binary_search(ForwardIterator begin, EndForwardIterator end, Value const& val,
                BinaryPredicate pred = BinaryPredicate{})
            {
                begin = detail::lower_bound(std::move(begin), end, val, std::ref(pred));
                return begin != end && !pred(val, *begin);
            }
        }

        struct binary_searcher : bindable<binary_searcher>
        {
            /// \brief function template \c binary_searcher::operator()
            ///
            /// range-based version of the \c binary_search std algorithm
            ///
            /// \pre \c ForwardRange is a model of the ForwardRange concept
            /// \pre \c BinaryPredicate is a model of the BinaryPredicate concept
            template<typename ForwardRange, typename Value>
            static bool invoke(binary_searcher, ForwardRange && rng, Value const & val)
            {
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange>());
                return detail::binary_search(ranges::begin(rng), ranges::end(rng), val);
            }

            /// \overload
            template<typename ForwardRange, typename Value, typename BinaryPredicate>
            static bool invoke(binary_searcher, ForwardRange && rng, Value const & val,
                BinaryPredicate pred)
            {
                CONCEPT_ASSERT(ranges::ForwardRange<ForwardRange>());
                CONCEPT_ASSERT(ranges::BinaryPredicate<invokable_t<BinaryPredicate>,
                                                       range_reference_t<ForwardRange>,
                                                       range_reference_t<ForwardRange>>());
                return detail::binary_search(ranges::begin(rng), ranges::end(rng), val,
                    ranges::make_invokable(std::move(pred)));
            }
        };

        RANGES_CONSTEXPR binary_searcher binary_search {};

    } // namespace v3

} // namespace ranges

#endif // include guard
