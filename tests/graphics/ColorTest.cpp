/*
 * Copyright 2011-2012 Arx Libertatis Team (see the AUTHORS file)
 *
 * This file is part of Arx Libertatis.
 *
 * Arx Libertatis is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Arx Libertatis is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Arx Libertatis.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ColorTest.h"

#include <cppunit/TestAssert.h>

#include "tests/math/AssertionTraits.h"

void ColorTest::ColorTypeConversionTests()
{
	Color3f white(1.f, 1.f, 1.f);

	ColorBGRA result = white.toBGR(255);

	CPPUNIT_ASSERT_EQUAL(0xFFFFFFFF, result);
	
	CPPUNIT_ASSERT_EQUAL(Color::fromRGBA(0xFF000000), Color(0, 0, 0, 255));
	CPPUNIT_ASSERT_EQUAL(Color::fromRGBA(0x00FF0000), Color(0, 0, 255, 0));
	CPPUNIT_ASSERT_EQUAL(Color::fromRGBA(0x0000FF00), Color(0, 255, 0, 0));
	CPPUNIT_ASSERT_EQUAL(Color::fromRGBA(0x000000FF), Color(255, 0, 0, 0));
	
	CPPUNIT_ASSERT_EQUAL(Color::fromBGRA(0xFF000000), Color(0, 0, 0, 255));
	CPPUNIT_ASSERT_EQUAL(Color::fromBGRA(0x00FF0000), Color(255, 0, 0, 0));
	CPPUNIT_ASSERT_EQUAL(Color::fromBGRA(0x0000FF00), Color(0, 255, 0, 0));
	CPPUNIT_ASSERT_EQUAL(Color::fromBGRA(0x000000FF), Color(0, 0, 255, 0));
	CPPUNIT_ASSERT_EQUAL(Color::fromBGRA(0xFFa8d0df), Color(168, 208, 223, 255));
}
