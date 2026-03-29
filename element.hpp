#ifndef ELEMENT_HPP_INCLUDED
#define ELEMENT_HPP_INCLUDED

// Copyright 2026 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/describe/class.hpp>
#include <string>
#include <vector>

struct point
{
    float x, y, z;
};

BOOST_DESCRIBE_STRUCT(point, (), (x, y, z))

struct element
{
    int index_;
    std::string key_;
    std::vector<point> value_;
};

BOOST_DESCRIBE_STRUCT(element, (), (index_, key_, value_))

#endif // #ifndef ELEMENT_HPP_INCLUDED
