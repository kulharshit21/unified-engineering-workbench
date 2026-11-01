#include <cassert>
#include <iostream>
#include "core/CoreVersion.hpp"

int main() {
    std::cout << "[SMOKE TEST] Initializing Workbench Core Smoke Test...\n";

    std::string ver = Workbench::Core::getVersionString();
    std::string name = Workbench::Core::getProjectName();

    assert(!ver.empty());
    assert(!name.empty());
    assert(name.find("PASS Science Works") != std::string::npos);

    std::cout << "[SMOKE TEST] Version: " << ver << "\n";
    std::cout << "[SMOKE TEST] Project: " << name << "\n";
    std::cout << "[SMOKE TEST] Core Smoke Test Passed Successfully.\n";

    return 0;
}
