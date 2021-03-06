//
// Copyright (C) 2014, 2015 Ableton AG, Berlin. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//

/*!
 * @file
 */

#pragma once

#include <atria/xform/transducer_impl.hpp>
#include <atria/xform/state_wrapper.hpp>
#include <atria/xform/with_state.hpp>
#include <atria/estd/tuple_hash.hpp>
#include <atria/prelude/tuplify.hpp>
#include <atria/estd/functional.hpp>
#include <atria/meta/copy_traits.hpp>
#include <unordered_set>

namespace atria {
namespace xform {

namespace detail {

struct distinct_rf_gen
{
  template <typename ReducingFnT>
  struct apply
  {
    ReducingFnT step;

    template <typename ...InputTs>
    using cache_t = std::unordered_set<
        estd::decay_t<decltype(tuplify(std::declval<InputTs>()...))> >;

    template <typename StateT, typename ...InputTs>
    auto operator() (StateT&& s, InputTs&& ...is)
      -> decltype(wrap_state(step(state_unwrap(s), is...), cache_t<InputTs...>{}))
    {
      using result_t   = decltype(wrap_state(step(state_unwrap(s), is...), cache_t<InputTs...>{}));
      using complete_t = decltype(state_complete(s));

      using wrapped_t   = meta::copy_decay_t<StateT, estd::decay_t<result_t>>;
      using unwrapped_t = meta::copy_decay_t<StateT, estd::decay_t<complete_t>>;

      return with_state(
        std::forward<StateT>(s),
        [&] (unwrapped_t&& st) {
          return wrap_state(
            step(state_unwrap(std::forward<unwrapped_t>(st)),
                 std::forward<InputTs>(is)...),
            cache_t<InputTs...>{ tuplify(is...) });
        },
        [&] (wrapped_t st) {
          auto& cache = state_wrapper_data(st);
          auto unique = cache.insert(tuplify(is...)).second;
          return !unique
            ? st
            : wrap_state(
              step(std::move(state_unwrap(st)),
                   std::forward<InputTs>(is)...),
              std::move(cache));
        });
    }
  };
};

} // namespace detail

using distinct_t = transducer_impl<detail::distinct_rf_gen>;

/*!
 * Equivalent to clojure/distinct$0
 */
constexpr distinct_t distinct {};

} // namespace xform
} // namespace atria
