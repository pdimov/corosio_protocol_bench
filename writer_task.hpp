#ifndef WRITER_TASK_HPP_INCLUDED
#define WRITER_TASK_HPP_INCLUDED

// Copyright 2026 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include "element.hpp"
#include "proto_write.hpp"
#include "task_type.hpp"
#include <boost/container_hash/hash.hpp>
#include <vector>
#include <cstdio>

template<class Sink> task_type writer_task( Sink sink, std::vector<element> const& v )
{
    co_await proto_write( sink, v );

    std::size_t hash = boost::hash_value( v );
    co_await proto_write( sink, hash );

    co_await sink.flush();

    std::printf( "  writer_task: n=%zu hash=%zx\n", v.size(), hash );
}

#endif // #ifndef WRITER_TASK_HPP_INCLUDED
