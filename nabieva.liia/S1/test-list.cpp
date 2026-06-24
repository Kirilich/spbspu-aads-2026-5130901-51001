#include <boost/test/unit_test.hpp>
#include <sstream>
#include <iostream>
#include "list.hpp"

using namespace nabieva;

BOOST_AUTO_TEST_SUITE(list_tests)

BOOST_AUTO_TEST_CASE(test_empty_list)
{
    List< int > list;

    BOOST_CHECK(list.empty());
    BOOST_CHECK(list.begin() == list.end());
    BOOST_CHECK(list.cbegin() == list.cend());
    BOOST_CHECK_EQUAL(getSize(list), 0u);
}

BOOST_AUTO_TEST_CASE(test_front_back_throw_on_empty)
{
    List< int > list;

    BOOST_CHECK_THROW(list.front(), std::logic_error);
    BOOST_CHECK_THROW(list.back(), std::logic_error);
}

BOOST_AUTO_TEST_SUITE_END()