#ifndef FOR_EACH_AWAIT_HPP_INCLUDED
#define FOR_EACH_AWAIT_HPP_INCLUDED

// Copyright 2026 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include "task_type.hpp"

template<template<class...> class L, class... T, class F> task_type for_each_await( L<T...>, F&& f )
{
    (co_await f( T{} ), ...);
    co_return;
}

#endif // #ifndef FOR_EACH_AWAIT_HPP_INCLUDED
