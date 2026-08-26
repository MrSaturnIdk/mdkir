/// Test for the list interaction

#include "helpers/list.hpp"

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

int main() {
    std::int64_t time {std::chrono::steady_clock::now().time_since_epoch().count()};
    char buffer[32] {};
    std::snprintf(buffer, sizeof(buffer), "testList%lld.log", static_cast<long long>(time));
    helpers::list::internal::COMMAND_LIST = std::filesystem::path {SYSCONFDIR}/buffer;
    // This will force create an empty file
    if (helpers::list::internal::changeList({}, true) == -1) {
        return 2;
    }

    helpers::list::CommandList tempList {
        "burger",
        "h",
        "foo",
        "bar",
        "hello world",
        "burger"
    };
    /// Does it add and error on invalids?
    for (const std::string& item : tempList) {
        if (helpers::list::addToList(item, true) == -1) {
            return 2;
        }
    }

    auto list {helpers::list::getList(true)};
    if (!list) {
        return 2;
    }

    std::printf("List of commands:\n");
    for (const std::string& item : *list) {
        std::printf("%s\n", item.c_str());
    }

    helpers::list::CommandList tempListTwo {
        "foo",
        "bar"
    };
    for (const std::string& item : tempListTwo) {
        if (helpers::list::removeFromList(item, true) == -1) {
            return 2;
        }
    }

    auto listTwo {helpers::list::getList(true)};
    if (!listTwo) {
        return 2;
    }

    std::printf("List of commands:\n");
    for (const std::string& item : *listTwo) {
        std::printf("%s\n", item.c_str());
    }
    return 0;
}
