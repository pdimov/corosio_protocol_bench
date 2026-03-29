// Copyright 2026 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include "proto_read.hpp"
#include "proto_write.hpp"
#include "simple_socket_source.hpp"
#include "simple_socket_sink.hpp"
#include "buffered_socket_source.hpp"
#include "buffered_socket_sink.hpp"
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

template<class Source, class Sink> void bench()
{
    corosio::io_context ioc;

    auto [rs, ws] = corosio::test::make_socket_pair( ioc );

    std::printf( "-- %s:\n", BOOST_CURRENT_FUNCTION );

    auto t1 = std::chrono::steady_clock::now();

    capy::run_async( ioc.get_executor() )( writer_task( Sink( std::move(ws) ) ) );
    capy::run_async( ioc.get_executor() )( reader_task( Source( std::move(rs) ) ) );

    ioc.run();

    auto t2 = std::chrono::steady_clock::now();

    using namespace std::chrono_literals;

    std::printf( "-- %lld ms\n\n", static_cast<long long>( ( t2 - t1 ) / 1ms ) );
}

int main()
{
    bench<simple_socket_source, simple_socket_sink>();
    bench<buffered_socket_source, buffered_socket_sink>();
}
