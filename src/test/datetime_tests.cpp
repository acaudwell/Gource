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
#include "../main.h"
#include "../gource_settings.h"

#include <stdlib.h>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( parse_date_time_tests )
{
    time_t timestamp;

    BOOST_CHECK_EQUAL(putenv((char*)"TZ=UTC"), 0);
    BOOST_CHECK(SDLAppSettings::parseDateTime("2021-11-01", timestamp));
    BOOST_CHECK_EQUAL(timestamp, 1635724800);

    BOOST_CHECK_EQUAL(putenv((char*)"TZ=Pacific/Auckland"), 0);
    BOOST_CHECK(SDLAppSettings::parseDateTime("2021-11-01", timestamp));
    BOOST_CHECK_EQUAL(timestamp, 1635678000);

    BOOST_CHECK(SDLAppSettings::parseDateTime("2021-11-01Z", timestamp));
    BOOST_CHECK_EQUAL(timestamp, 1635724800);

    BOOST_CHECK(SDLAppSettings::parseDateTime("2021-11-01+0", timestamp));
    BOOST_CHECK_EQUAL(timestamp, 1635724800);

    BOOST_CHECK(SDLAppSettings::parseDateTime("2021-11-01+13", timestamp));
    BOOST_CHECK_EQUAL(timestamp, 1635678000);
    
    BOOST_CHECK(SDLAppSettings::parseDateTime("2021-11-01 +13", timestamp));
    BOOST_CHECK_EQUAL(timestamp, 1635678000);

    BOOST_CHECK(SDLAppSettings::parseDateTime("2021-11-01+5:30", timestamp));
    BOOST_CHECK_EQUAL(timestamp, 1635705000);

    BOOST_CHECK(SDLAppSettings::parseDateTime("2021-11-01 12:01", timestamp));
    BOOST_CHECK_EQUAL(timestamp, 1635721260);

    BOOST_CHECK(SDLAppSettings::parseDateTime("2021-11-01 12:01Z", timestamp));
    BOOST_CHECK_EQUAL(timestamp, 1635768060);

    BOOST_CHECK(SDLAppSettings::parseDateTime("2021-11-01 12:01+0", timestamp));
    BOOST_CHECK_EQUAL(timestamp, 1635768060);

    BOOST_CHECK(SDLAppSettings::parseDateTime("2021-11-01 12:01+13", timestamp));
    BOOST_CHECK_EQUAL(timestamp, 1635721260);

    BOOST_CHECK(SDLAppSettings::parseDateTime("2021-11-01 12:01 +13", timestamp));
    BOOST_CHECK_EQUAL(timestamp, 1635721260);

    BOOST_CHECK(SDLAppSettings::parseDateTime("2021-11-01 12:01+5:30", timestamp));
    BOOST_CHECK_EQUAL(timestamp, 1635748260);

    BOOST_CHECK(SDLAppSettings::parseDateTime("2021-11-01 12:01:59", timestamp));
    BOOST_CHECK_EQUAL(timestamp, 1635721319);

    BOOST_CHECK(SDLAppSettings::parseDateTime("2021-11-01T12:01:59.123", timestamp));
    BOOST_CHECK_EQUAL(timestamp, 1635721319);

    BOOST_CHECK(SDLAppSettings::parseDateTime("2021-11-01 12:01:59Z", timestamp));
    BOOST_CHECK_EQUAL(timestamp, 1635768119);
    
    BOOST_CHECK(SDLAppSettings::parseDateTime("2021-11-01 12:01:59+0", timestamp));
    BOOST_CHECK_EQUAL(timestamp, 1635768119);

    BOOST_CHECK(SDLAppSettings::parseDateTime("2021-11-01 12:01:59+13", timestamp));
    BOOST_CHECK_EQUAL(timestamp, 1635721319);

    BOOST_CHECK(SDLAppSettings::parseDateTime("2021-11-01 12:01:59 +13", timestamp));
    BOOST_CHECK_EQUAL(timestamp, 1635721319);

    BOOST_CHECK(SDLAppSettings::parseDateTime("2021-11-01 12:01:59+5:30", timestamp));
    BOOST_CHECK_EQUAL(timestamp, 1635748319);

    BOOST_CHECK(SDLAppSettings::parseDateTime("2021-11-01T12:01:59", timestamp));
    BOOST_CHECK_EQUAL(timestamp, 1635721319);

    BOOST_CHECK(SDLAppSettings::parseDateTime("2021-11-01T12:01:59.123", timestamp));
    BOOST_CHECK_EQUAL(timestamp, 1635721319);

    BOOST_CHECK(SDLAppSettings::parseDateTime("2021-11-01T12:01:59Z", timestamp));
    BOOST_CHECK_EQUAL(timestamp, 1635768119);
    
    BOOST_CHECK(SDLAppSettings::parseDateTime("2021-11-01T12:01:59+0", timestamp));
    BOOST_CHECK_EQUAL(timestamp, 1635768119);

    BOOST_CHECK(SDLAppSettings::parseDateTime("2021-11-01T12:01:59+13", timestamp));
    BOOST_CHECK_EQUAL(timestamp, 1635721319);

    BOOST_CHECK(SDLAppSettings::parseDateTime("2021-11-01T12:01:59+13", timestamp));
    BOOST_CHECK_EQUAL(timestamp, 1635721319);

    BOOST_CHECK(SDLAppSettings::parseDateTime("2021-11-01T12:01:59+5:30", timestamp));
    BOOST_CHECK_EQUAL(timestamp, 1635748319);

    BOOST_CHECK(SDLAppSettings::parseDateTime("2021-11-01T12:01:59.123+5:30", timestamp));
    BOOST_CHECK_EQUAL(timestamp, 1635748319);
}
