#ifndef PROTO_READ_HPP_INCLUDED
#define PROTO_READ_HPP_INCLUDED

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
struct Source
{
    __awaitable__ read( void* p, std::size_t n );
};
*/

using task_type = boost::capy::task<void>;

template<class Source, class T>
    requires std::is_fundamental_v<T>
auto proto_read( Source& source, T& v )
{
    return source.read( &v, sizeof(v) );
}

// forward declarations

template<class Source, class Tr, class Al>
task_type proto_read( Source& source, std::basic_string<char, Tr, Al>& v );

template<class Source, class T, class A>
task_type proto_read( Source& source, std::vector<T, A>& v );

template<class Source, class A>
task_type proto_read( Source& source, std::vector<unsigned char, A>& v );

template<class Source, class T>
    requires (boost::describe::has_describe_members<T>::value && !std::is_union_v<T>)
task_type proto_read( Source& source, T& v );

// string

template<class Source, class Tr, class Al>
task_type proto_read( Source& source, std::basic_string<char, Tr, Al>& v )
{
    std::size_t n = 0;
    co_await proto_read( source, n );

    v.resize( n );
    co_await source.read( v.data(), v.size() );
}

// vector

template<class Source, class T, class A>
task_type proto_read( Source& source, std::vector<T, A>& v )
{
    std::size_t n = 0;
    co_await proto_read( source, n );

    v.resize( n );

    for( auto& x: v )
    {
        co_await proto_read( source, x );
    }
}

template<class Source, class A>
task_type proto_read( Source& source, std::vector<unsigned char, A>& v )
{
    std::size_t n = 0;
    co_await proto_read( source, n );

    v.resize( n );
    co_await source.read( v.data(), v.size() );
}

// described class

template<class Source, class T>
    requires (boost::describe::has_describe_members<T>::value && !std::is_union_v<T>)
task_type proto_read( Source& source, T& v )
{
    using D = boost::describe::describe_members<T, boost::describe::mod_any_access | boost::describe::mod_inherited>;

    co_await for_each_await( D(), [&](auto d) {

        return proto_read( source, v.*d.pointer );

    });
}

#endif // #ifndef PROTO_READ_HPP_INCLUDED
