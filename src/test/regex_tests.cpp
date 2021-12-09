/*
    Copyright (C) 2021 Andrew Caudwell (acaudwell@gmail.com)

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version
    3 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "../core/regex.h"

#include <stdlib.h>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( regex_tests )
{
    Regex regex("^[A-Z]+$");
    BOOST_CHECK(regex.isValid());

    BOOST_CHECK(regex.match("ABC"));
    BOOST_CHECK(regex.match("abc") == false);
    BOOST_CHECK(regex.match("ABC123") == false);

    Regex regex_copy = Regex("^[A-Z]+$");
    BOOST_CHECK(regex_copy.isValid());
    BOOST_CHECK(regex_copy.match("ABC"));

    BOOST_CHECK_THROW(Regex("^[A-Z+$"), RegexCompilationException);

    Regex test_regex("^[A-Z+$", true);
    BOOST_CHECK(test_regex.isValid() == false);

    std::vector<std::string> matches;
    Regex capture_regex("([0-9]+)");
    BOOST_CHECK(capture_regex.match("ABC123", &matches));
    BOOST_CHECK_EQUAL(matches.size(), 1);
    BOOST_CHECK_EQUAL(matches[0], "123");
}
