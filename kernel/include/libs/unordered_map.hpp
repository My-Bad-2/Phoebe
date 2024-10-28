// <unordered_map> -*- C++ -*-

// Copyright (C) 2007-2024 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file include/unordered_map
 *  This is a Standard C++ Library header.
 */

#ifndef _GLIBCXX_UNORDERED_MAP
#define _GLIBCXX_UNORDERED_MAP 1

#ifdef _GLIBCXX_SYSHDR
	#pragma GCC system_header
#endif

#include <initializer_list>
#include <bits/unordered_map.h>
#include <bits/range_access.h>
#include <bits/erase_if.h>

#ifdef _GLIBCXX_DEBUG
	#include <debug/unordered_map>
#endif

#define __glibcxx_want_allocator_traits_is_always_equal
#define __glibcxx_want_erase_if
#define __glibcxx_want_generic_unordered_lookup
#define __glibcxx_want_node_extract
#define __glibcxx_want_nonmember_container_access
#define __glibcxx_want_unordered_map_try_emplace
#define __glibcxx_want_tuple_like
#include <bits/version.h>

#if __cplusplus > 201703L
namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION
template<typename _Key, typename _Tp, typename _Hash, typename _CPred, typename _Alloc,
		 typename _Predicate>
inline typename unordered_map<_Key, _Tp, _Hash, _CPred, _Alloc>::size_type
	erase_if(unordered_map<_Key, _Tp, _Hash, _CPred, _Alloc>& __cont, _Predicate __pred)
{
	_GLIBCXX_STD_C::unordered_map<_Key, _Tp, _Hash, _CPred, _Alloc>& __ucont = __cont;
	return __detail::__erase_nodes_if(__cont, __ucont, __pred);
}

template<typename _Key, typename _Tp, typename _Hash, typename _CPred, typename _Alloc,
		 typename _Predicate>
inline typename unordered_multimap<_Key, _Tp, _Hash, _CPred, _Alloc>::size_type
	erase_if(unordered_multimap<_Key, _Tp, _Hash, _CPred, _Alloc>& __cont, _Predicate __pred)
{
	_GLIBCXX_STD_C::unordered_multimap<_Key, _Tp, _Hash, _CPred, _Alloc>& __ucont = __cont;
	return __detail::__erase_nodes_if(__cont, __ucont, __pred);
}
_GLIBCXX_END_NAMESPACE_VERSION
} // namespace std _GLIBCXX_VISIBILITY(default)
#endif // C++20

#endif // _GLIBCXX_UNORDERED_MAP