#include "TestAssert.hpp"
#include <iostream>
#include "core/CoreVersion.hpp"

int main() {
    std::cout << "[SMOKE TEST] Initializing Workbench Core Smoke Test...\n";

    std::string ver = Workbench::Core::getVersionString();
    std::string name = Workbench::Core::getProjectName();

    TEST_ASSERT(!ver.empty());
    TEST_ASSERT(!name.empty());
    TEST_ASSERT(name.find("PASS Science Works") != std::string::npos);

    std::cout << "[SMOKE TEST] Version: " << ver << "\n";
    std::cout << "[SMOKE TEST] Project: " << name << "\n";
    std::cout << "[SMOKE TEST] Core Smoke Test Passed Successfully.\n";

    return 0;
}
