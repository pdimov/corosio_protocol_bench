#ifndef READER_TASK_HPP_INCLUDED
#define READER_TASK_HPP_INCLUDED

// Copyright 2026 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include "element.hpp"
#include "proto_read.hpp"
#include "task_type.hpp"
#include <boost/container_hash/hash.hpp>
#include <vector>
#include <cstdio>

template<class Source> task_type reader_task( Source source )
{
    std::vector<element> v;

    co_await proto_read( source, v );

    std::size_t h1 = boost::hash_value( v );

    std::size_t h2 = 0;
    co_await proto_read( source, h2 );

    std::printf( "  reader_task: n=%zu computed=%zx received=%zx %s\n", v.size(), h1, h2, ( h1 == h2? "": "***FAIL***") );

    source.shutdown();
}

#endif // #ifndef READER_TASK_HPP_INCLUDED
