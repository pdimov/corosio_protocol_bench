// Copyright 2026 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/corosio/io_context.hpp>
#include <boost/corosio/tcp_socket.hpp>
#include <boost/corosio/test/socket_pair.hpp>
#include <boost/capy/ex/run_async.hpp>
#include <boost/capy/task.hpp>
#include <boost/capy/read.hpp>
#include <boost/capy/write.hpp>
#include <boost/capy/buffers.hpp>

namespace corosio = boost::corosio;
namespace capy = boost::capy;

capy::task<void> writer_task( corosio::tcp_socket sock )
{
    unsigned char const data[ 4 ] = {};
    co_await capy::write( sock, capy::const_buffer{ data, sizeof(data) } );
}

capy::task<void> reader_task( corosio::tcp_socket sock )
{
    unsigned char data[ 4 ] = {};
    co_await capy::read( sock, capy::mutable_buffer{ data, sizeof(data) } );
}

int main()
{
    corosio::io_context ioc;

    auto [rs, ws] = corosio::test::make_socket_pair( ioc );

    capy::run_async( ioc.get_executor() )( writer_task( std::move(ws) ) );
    capy::run_async( ioc.get_executor() )( reader_task( std::move(rs) ) );

    ioc.run();
}
