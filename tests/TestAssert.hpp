#pragma once

#include <iostream>
#include <cstdlib>
#include <string>
#include <cmath>

// Reliable, active test verification macro that ALWAYS runs in both Debug and Release modes
// (Never disabled by NDEBUG or /DNDEBUG).
#define TEST_ASSERT(expr) \
    do { \
        if (!(expr)) { \
            std::cerr << "\n[TEST FAILURE] Assertion failed: (" #expr ")\n" \
                      << "  File: " << __FILE__ << "\n" \
                      << "  Line: " << __LINE__ << "\n" \
                      << "  Function: " << __func__ << "\n" << std::endl; \
            std::exit(1); \
        } \
    } while (0)

#define TEST_ASSERT_MSG(expr, msg) \
    do { \
        if (!(expr)) { \
            std::cerr << "\n[TEST FAILURE] Assertion failed: (" #expr ")\n" \
                      << "  Message: " << (msg) << "\n" \
                      << "  File: " << __FILE__ << "\n" \
                      << "  Line: " << __LINE__ << "\n" \
                      << "  Function: " << __func__ << "\n" << std::endl; \
            std::exit(1); \
        } \
    } while (0)

#define TEST_ASSERT_NEAR(actual, expected, tol) \
    do { \
        double diff = std::abs(static_cast<double>(actual) - static_cast<double>(expected)); \
        if (diff > (tol)) { \
            std::cerr << "\n[TEST FAILURE] Numerical tolerance exceeded: (" #actual " vs " #expected ")\n" \
                      << "  Actual:   " << (actual) << "\n" \
                      << "  Expected: " << (expected) << "\n" \
                      << "  Diff:     " << diff << "\n" \
                      << "  Tol:      " << (tol) << "\n" \
                      << "  File:     " << __FILE__ << "\n" \
                      << "  Line:     " << __LINE__ << "\n" << std::endl; \
            std::exit(1); \
        } \
    } while (0)
