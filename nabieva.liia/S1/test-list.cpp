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

BOOST_AUTO_TEST_CASE(test_push_front)
{
    List< int > list;

    list.push_front(10);
    BOOST_CHECK(!list.empty());
    BOOST_CHECK_EQUAL(list.front(), 10);
    BOOST_CHECK_EQUAL(list.back(), 10);

    list.push_front(20);
    list.push_front(30);
    BOOST_CHECK_EQUAL(list.front(), 30);
    BOOST_CHECK_EQUAL(list.back(), 10);
    BOOST_CHECK_EQUAL(getSize(list), 3u);
}

BOOST_AUTO_TEST_CASE(test_push_back)
{
    List< int > list;

    list.push_back(1);
    list.push_back(2);
    list.push_back(3);

    BOOST_CHECK_EQUAL(list.front(), 1);
    BOOST_CHECK_EQUAL(list.back(), 3);
    BOOST_CHECK_EQUAL(getSize(list), 3u);
}

BOOST_AUTO_TEST_CASE(test_pop_front)
{
    List< int > list;
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);

    list.pop_front();
    BOOST_CHECK_EQUAL(list.front(), 2);
    BOOST_CHECK_EQUAL(list.back(), 3);
    BOOST_CHECK_EQUAL(getSize(list), 2u);

    list.pop_front();
    list.pop_front();
    BOOST_CHECK(list.empty());
    BOOST_CHECK(list.begin() == list.end());
}

BOOST_AUTO_TEST_CASE(test_pop_back)
{
    List< int > list;
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);

    list.pop_back();
    BOOST_CHECK_EQUAL(list.front(), 1);
    BOOST_CHECK_EQUAL(list.back(), 2);
    BOOST_CHECK_EQUAL(getSize(list), 2u);

    list.pop_back();
    BOOST_CHECK_EQUAL(list.front(), 1);
    BOOST_CHECK_EQUAL(list.back(), 1);

    list.pop_back();
    BOOST_CHECK(list.empty());
}

BOOST_AUTO_TEST_CASE(test_pop_empty_does_not_throw)
{
    List< int > list;

    BOOST_CHECK_NO_THROW(list.pop_front());
    BOOST_CHECK_NO_THROW(list.pop_back());
    BOOST_CHECK(list.empty());
}

BOOST_AUTO_TEST_CASE(test_clear)
{
    List< int > list;
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);

    list.clear();

    BOOST_CHECK(list.empty());
    BOOST_CHECK_EQUAL(getSize(list), 0u);
    BOOST_CHECK_THROW(list.front(), std::logic_error);
    BOOST_CHECK_THROW(list.back(), std::logic_error);
}


BOOST_AUTO_TEST_SUITE_END()