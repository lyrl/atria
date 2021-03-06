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

#include <atria/xform/config.hpp>
#include <atria/xform/transduce.hpp>
#if ABL_STATEFUL_INTO
#include <atria/xform/reducing/output_rf.hpp>
#else
#include <atria/xform/reducing/emplace_back_rf.hpp>
#endif

namespace atria {
namespace xform {

/*!
 * Similar to clojure.core/into$4
 */
#if ABL_STATEFUL_INTO

template <typename CollectionT,
          typename XformT,
          typename ...InputRangeTs>
auto into(CollectionT&& col, XformT&& xform, InputRangeTs&& ...ranges)
  -> CollectionT&&
{
  transduce(
    std::forward<XformT>(xform),
    output_rf,
    std::back_inserter(col),
    std::forward<InputRangeTs>(ranges)...);
  return std::forward<CollectionT>(col);
}

#else

template <typename CollectionT,
          typename XformT,
          typename ...InputRangeTs>
auto into(CollectionT&& col, XformT&& xform, InputRangeTs&& ...ranges)
  -> estd::decay_t<CollectionT>
{
  return transduce(
    std::forward<XformT>(xform),
    emplace_back_rf,
    std::forward<CollectionT>(col),
    std::forward<InputRangeTs>(ranges)...);
}

#endif

} // namespace xform
} // namespace atria
