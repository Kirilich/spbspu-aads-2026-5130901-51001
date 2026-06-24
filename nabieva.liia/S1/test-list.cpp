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

BOOST_AUTO_TEST_CASE(test_iterator_order)
{
    List< int > list;
    list.push_back(10);
    list.push_back(20);
    list.push_back(30);

    LIter< int > it = list.begin();
    BOOST_REQUIRE(it != list.end());
    BOOST_CHECK_EQUAL(*it, 10);
    ++it;
    BOOST_REQUIRE(it != list.end());
    BOOST_CHECK_EQUAL(*it, 20);
    ++it;
    BOOST_REQUIRE(it != list.end());
    BOOST_CHECK_EQUAL(*it, 30);
    ++it;
    BOOST_CHECK(it == list.end());
}

BOOST_AUTO_TEST_CASE(test_iterator_postincrement)
{
    List< int > list;
    list.push_back(5);
    list.push_back(6);

    LIter< int > it = list.begin();
    LIter< int > old = it++;

    BOOST_CHECK_EQUAL(*old, 5);
    BOOST_CHECK_EQUAL(*it, 6);
}

BOOST_AUTO_TEST_CASE(test_const_iterator_order)
{
    List< int > list;
    list.push_back(7);
    list.push_back(8);

    const List< int >& constList = list;
    LCIter< int > it = constList.cbegin();

    BOOST_REQUIRE(it != constList.cend());
    BOOST_CHECK_EQUAL(*it, 7);
    ++it;
    BOOST_REQUIRE(it != constList.cend());
    BOOST_CHECK_EQUAL(*it, 8);
    ++it;
    BOOST_CHECK(it == constList.cend());
}

BOOST_AUTO_TEST_CASE(test_dereference_end_iterator_throws)
{
    List< int > list;

    BOOST_CHECK_THROW(*list.end(), std::logic_error);
    BOOST_CHECK_THROW(*list.cend(), std::logic_error);
}

BOOST_AUTO_TEST_CASE(test_insert_after_middle)
{
    List< int > list;
    list.push_back(1);
    list.push_back(3);

    LIter< int > pos = list.begin();
    list.insert_after(pos, 2);

    LIter< int > it = list.begin();
    BOOST_CHECK_EQUAL(*it, 1);
    ++it;
    BOOST_CHECK_EQUAL(*it, 2);
    ++it;
    BOOST_CHECK_EQUAL(*it, 3);
    ++it;
    BOOST_CHECK(it == list.end());
}

BOOST_AUTO_TEST_CASE(test_insert_after_end_appends)
{
    List< int > list;
    list.push_back(1);
    list.push_back(2);

    LIter< int > inserted = list.insert_after(list.end(), 3);

    BOOST_CHECK_EQUAL(*inserted, 3);
    BOOST_CHECK_EQUAL(list.back(), 3);
    BOOST_CHECK_EQUAL(getSize(list), 3u);
}

BOOST_AUTO_TEST_CASE(test_insert_after_end_on_empty)
{
    List< int > list;

    LIter< int > inserted = list.insert_after(list.end(), 42);

    BOOST_CHECK(!list.empty());
    BOOST_CHECK_EQUAL(*inserted, 42);
    BOOST_CHECK_EQUAL(list.front(), 42);
    BOOST_CHECK_EQUAL(list.back(), 42);
}

BOOST_AUTO_TEST_CASE(test_copy_constructor_makes_independent_copy)
{
    List< int > original;
    original.push_back(1);
    original.push_back(2);

    List< int > copy(original);
    original.push_front(0);
    copy.push_back(3);

    BOOST_CHECK_EQUAL(original.front(), 0);
    BOOST_CHECK_EQUAL(original.back(), 2);
    BOOST_CHECK_EQUAL(getSize(original), 3u);

    BOOST_CHECK_EQUAL(copy.front(), 1);
    BOOST_CHECK_EQUAL(copy.back(), 3);
    BOOST_CHECK_EQUAL(getSize(copy), 3u);
}

BOOST_AUTO_TEST_CASE(test_copy_assignment_makes_independent_copy)
{
    List< int > original;
    original.push_back(4);
    original.push_back(5);

    List< int > copy;
    copy.push_back(100);
    copy = original;

    original.pop_front();

    BOOST_CHECK_EQUAL(copy.front(), 4);
    BOOST_CHECK_EQUAL(copy.back(), 5);
    BOOST_CHECK_EQUAL(getSize(copy), 2u);
    BOOST_CHECK_EQUAL(original.front(), 5);
}

BOOST_AUTO_TEST_CASE(test_self_copy_assignment)
{
    List< int > list;
    list.push_back(1);
    list.push_back(2);

    list = list;

    BOOST_CHECK_EQUAL(list.front(), 1);
    BOOST_CHECK_EQUAL(list.back(), 2);
    BOOST_CHECK_EQUAL(getSize(list), 2u);
}

BOOST_AUTO_TEST_CASE(test_move_constructor)
{
    List< int > original;
    original.push_back(9);
    original.push_back(10);

    List< int > moved(std::move(original));

    BOOST_CHECK(original.empty());
    BOOST_CHECK_EQUAL(moved.front(), 9);
    BOOST_CHECK_EQUAL(moved.back(), 10);
    BOOST_CHECK_EQUAL(getSize(moved), 2u);
}

BOOST_AUTO_TEST_CASE(test_move_assignment)
{
    List< int > original;
    original.push_back(11);
    original.push_back(12);

    List< int > moved;
    moved.push_back(100);
    moved = std::move(original);

    BOOST_CHECK(original.empty());
    BOOST_CHECK_EQUAL(moved.front(), 11);
    BOOST_CHECK_EQUAL(moved.back(), 12);
    BOOST_CHECK_EQUAL(getSize(moved), 2u);
}

BOOST_AUTO_TEST_CASE(test_swap)
{
    List< int > first;
    first.push_back(1);
    first.push_back(2);

    List< int > second;
    second.push_back(3);

    first.swap(second);

    BOOST_CHECK_EQUAL(first.front(), 3);
    BOOST_CHECK_EQUAL(first.back(), 3);
    BOOST_CHECK_EQUAL(getSize(first), 1u);

    BOOST_CHECK_EQUAL(second.front(), 1);
    BOOST_CHECK_EQUAL(second.back(), 2);
    BOOST_CHECK_EQUAL(getSize(second), 2u);
}

BOOST_AUTO_TEST_CASE(test_list_with_strings)
{
    List< std::string > list;
    list.push_back("first");
    list.push_back("second");

    BOOST_CHECK_EQUAL(list.front(), "first");
    BOOST_CHECK_EQUAL(list.back(), "second");
}

BOOST_AUTO_TEST_SUITE_END()