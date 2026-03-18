// Copyright Citra Emulator Project / Azahar Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include <catch2/catch_test_macros.hpp>

#include "common/settings.h"

TEST_CASE("Settings RestoreGlobalState resets async GPU overrides", "[common][settings]") {
    Settings::values.async_gpu.SetGlobal(false);
    Settings::values.async_gpu = false;

    REQUIRE_FALSE(Settings::values.async_gpu.UsingGlobal());
    REQUIRE_FALSE(Settings::values.async_gpu.GetValue());

    Settings::RestoreGlobalState(false);

    REQUIRE(Settings::values.async_gpu.UsingGlobal());
    REQUIRE(Settings::values.async_gpu.GetValue());
}
