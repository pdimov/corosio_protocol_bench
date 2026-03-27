#ifndef PROTO_WRITE_HPP_INCLUDED
#define PROTO_WRITE_HPP_INCLUDED

// Copyright 2026 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include "for_each_await.hpp"
#include <boost/capy/task.hpp>
#include <boost/describe/members.hpp>
#include <vector>
#include <string>
#include <cstddef>
#include <type_traits>

/*
struct Sink
{
    __awaitable__ write( void const* p, std::size_t n );
    __awaitable__ flush();
};
*/

using task_type = boost::capy::task<void>;

template<class Sink, class T>
    requires std::is_fundamental_v<T>
auto proto_write( Sink& sink, T const& v )
{
    return sink.write( &v, sizeof(v) );
}

// forward declarations

template<class Sink, class Tr, class Al>
task_type proto_write( Sink& sink, std::basic_string<char, Tr, Al> const& v );

template<class Sink, class T, class A>
task_type proto_write( Sink& sink, std::vector<T, A> const& v );

template<class Sink, class A>
task_type proto_write( Sink& sink, std::vector<unsigned char, A> const& v );

template<class Sink, class T>
    requires (boost::describe::has_describe_members<T>::value && !std::is_union_v<T>)
task_type proto_write( Sink& sink, T const& v );

// string

template<class Sink, class Tr, class Al>
task_type proto_write( Sink& sink, std::basic_string<char, Tr, Al> const& v )
{
    std::size_t n = v.size();
    co_await proto_write( sink, n );

    co_await sink.write( v.data(), v.size() );
}

// vector

template<class Sink, class T, class A>
task_type proto_write( Sink& sink, std::vector<T, A> const& v )
{
    std::size_t n = v.size();
    co_await proto_write( sink, n );

    for( auto& x: v )
    {
        co_await proto_write( sink, x );
    }
}

template<class Sink, class A>
task_type proto_write( Sink& sink, std::vector<unsigned char, A> const& v )
{
    std::size_t n = v.size();
    co_await proto_write( sink, n );

    co_await sink.write( v.data(), v.size() );
}

// described class

template<class Sink, class T>
    requires (boost::describe::has_describe_members<T>::value && !std::is_union_v<T>)
task_type proto_write( Sink& sink, T const& v )
{
    using D = boost::describe::describe_members<T, boost::describe::mod_any_access | boost::describe::mod_inherited>;

    co_await for_each_await( D(), [&](auto d) {

        return proto_write( sink, v.*d.pointer );

    });
}

#endif // #ifndef PROTO_WRITE_HPP_INCLUDED
