// test_nan_negative.cpp
// Negative test: verifies that TEST_ASSERT_NEAR correctly rejects NaN values.
// This executable is expected to exit with code 1.
// CTest registers it with WILL_FAIL TRUE so it is counted as a PASS.
//
// Design rationale: the NaN-rejection guard added in Phase 17 prevents silent
// false-positive passes when a numerical result is non-finite. This test
// provides live evidence that the guard fires correctly in Release mode.

#include "TestAssert.hpp"
#include <iostream>
#include <cmath>

int main() {
    std::cout << "[NAN NEGATIVE TEST] Verifying that TEST_ASSERT_NEAR rejects NaN.\n";
    std::cout << "[NAN NEGATIVE TEST] The next line will trigger the non-finite guard.\n";
    std::cout << "[NAN NEGATIVE TEST] Exit code 1 is EXPECTED and is the success condition.\n";
    std::cout.flush();

    double nan_val = std::numeric_limits<double>::quiet_NaN();
    double expected = 0.0;

    // This MUST trigger [TEST FAILURE] and exit(1) due to the isfinite guard.
    TEST_ASSERT_NEAR(nan_val, expected, 1e-6);

    // If we reach here, the NaN guard is broken — this is the true failure.
    std::cerr << "[NAN NEGATIVE TEST] BROKEN: NaN silently passed TEST_ASSERT_NEAR!\n";
    return 0; // exit(0) means the guard failed — CTest's WILL_FAIL will flag this as FAIL
}
