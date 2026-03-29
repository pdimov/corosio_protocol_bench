// Copyright 2026 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include "proto_read.hpp"
#include "proto_write.hpp"
#include "simple_socket_source.hpp"
#include "simple_socket_sink.hpp"
#include "buffered_socket_source.hpp"
#include "buffered_socket_sink.hpp"
#include "immediate_socket_source.hpp"
#include "immediate_socket_sink.hpp"
#include "reader_task.hpp"
#include "writer_task.hpp"
#include <boost/corosio/io_context.hpp>
#include <boost/corosio/test/socket_pair.hpp>
#include <boost/capy/ex/run_async.hpp>
#include <boost/current_function.hpp>
#include <chrono>
#include <cstdio>

namespace corosio = boost::corosio;
namespace capy = boost::capy;

template<class Source, class Sink> void bench( std::vector<element> const& v )
{
    corosio::io_context ioc;

    auto [rs, ws] = corosio::test::make_socket_pair( ioc );

    std::printf( "-- %s:\n", BOOST_CURRENT_FUNCTION );

    auto t1 = std::chrono::steady_clock::now();

    capy::run_async( ioc.get_executor() )( writer_task( Sink( std::move(ws) ), v ) );
    capy::run_async( ioc.get_executor() )( reader_task( Source( std::move(rs) ) ) );

    ioc.run();

    auto t2 = std::chrono::steady_clock::now();

    using namespace std::chrono_literals;

    std::printf( "-- %lld ms\n\n", static_cast<long long>( ( t2 - t1 ) / 1ms ) );
}

int main()
{
    int const N = 100'000;

    std::vector<element> v( N );

    for( int j = 0; j < N; ++j )
    {
        v[ j ].index_ = j;
        v[ j ].key_ = "key" + std::to_string( j );
        v[ j ].value_.resize( 4, { j * 1.0f, j * 2.0f, j * 3.0f } );
    }

    bench<simple_socket_source, simple_socket_sink>( v );
    bench<buffered_socket_source, buffered_socket_sink>( v );
    bench<immediate_socket_source, immediate_socket_sink>( v );
}
