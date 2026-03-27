#ifndef WRITER_TASK_HPP_INCLUDED
#define WRITER_TASK_HPP_INCLUDED

// Copyright 2026 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include "element.hpp"
#include "proto_write.hpp"
#include <boost/container_hash/hash.hpp>
#include <vector>
#include <cstdio>

template<class Sink> boost::capy::task<void> writer_task( Sink sink )
{
    std::vector<element> v;

    int const N = 200'000;

    v.resize( N );

    for( int j = 0; j < N; ++j )
    {
        v[ j ].key_ = "";
        v[ j ].value_ = j;
        v[ j ].padding_.resize( 12, 0xAA );
    }

    co_await proto_write( sink, v );

    std::size_t hash = boost::hash_value( v );
    co_await proto_write( sink, hash );

    co_await sink.flush();

    std::printf( "  writer_task: n=%d hash=%zx\n", N, hash );

    sink.shutdown();
}

#endif // #ifndef WRITER_TASK_HPP_INCLUDED
