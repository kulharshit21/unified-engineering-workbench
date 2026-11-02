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
        double _ta = static_cast<double>(actual); \
        double _te = static_cast<double>(expected); \
        if (!std::isfinite(_ta) || !std::isfinite(_te)) { \
            std::cerr << "\n[TEST FAILURE] Non-finite value in TEST_ASSERT_NEAR: (" #actual " vs " #expected ")\n" \
                      << "  Actual:   " << _ta << "\n" \
                      << "  Expected: " << _te << "\n" \
                      << "  (NaN or Inf values always fail the nearness check)\n" \
                      << "  File:     " << __FILE__ << "\n" \
                      << "  Line:     " << __LINE__ << "\n" << std::endl; \
            std::exit(1); \
        } \
        double diff = std::abs(_ta - _te); \
        if (diff > (tol)) { \
            std::cerr << "\n[TEST FAILURE] Numerical tolerance exceeded: (" #actual " vs " #expected ")\n" \
                      << "  Actual:   " << _ta << "\n" \
                      << "  Expected: " << _te << "\n" \
                      << "  Diff:     " << diff << "\n" \
                      << "  Tol:      " << (tol) << "\n" \
                      << "  File:     " << __FILE__ << "\n" \
                      << "  Line:     " << __LINE__ << "\n" << std::endl; \
            std::exit(1); \
        } \
    } while (0)
