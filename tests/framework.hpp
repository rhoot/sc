//---
// Copyright (c) 2016 Johan Sköld
// License: https://opensource.org/licenses/ISC
//---

#pragma once

#include <string>

#include <catch.hpp>

#define DESCRIBE(...)   TEST_CASE(__VA_ARGS__)
#define IT(desc)        SECTION(std::string("      It: ") + desc, "")

