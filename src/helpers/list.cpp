/**
 * Small helper for handling the list (source)
 *
 * -1 = I/O fail
 * 0 = Success
 * 1 = Also success, for usage in an if statement, otherwise bad input
 */

#include "helpers/list.hpp"

#include <unistd.h>

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <optional>
#include <fstream>
#include <string>
#include <vector>

namespace helpers::list {

using CommandList = std::vector<std::string>;

bool verbose {};

namespace internal {
    std::filesystem::path COMMAND_LIST {std::filesystem::path {SYSCONFDIR}/"list"};
    int changeList(const CommandList& newList) {
        std::setvbuf(stdout, nullptr, _IONBF, BUFSIZ);
        if (verbose) {
            std::printf("Opening list\n");
        }
        std::ofstream list {COMMAND_LIST};
        if (!list.is_open()) {
            std::fprintf(stderr,
                isatty(STDERR_FILENO)
                    ? "\033[1;31mError:\033[0m Failed to open command list\n"
                    : "Error: Failed to open command list\n"
            );
            return -1;
        }

        if (verbose) {
            std::printf("Writing to list\n");
        }
        for (const std::string& item : newList) {
            list << item << '\n';
        }
        return 0;
        std::setvbuf(stdout, nullptr, isatty(STDOUT_FILENO) ? _IOLBF : _IOFBF, BUFSIZ);
    }
}

std::optional<CommandList> getList() {
    std::setvbuf(stdout, nullptr, _IONBF, BUFSIZ);
    if (verbose) {
        std::printf("Opening list\n");
    }
    std::ifstream list {internal::COMMAND_LIST};
    if (!list.is_open()) {
        std::fprintf(stderr,
            isatty(STDERR_FILENO)
                ? "\033[1;31mError:\033[0m Failed to open command list\n"
                : "Error: Failed to open command list\n"
        );
        return std::nullopt;
    }
    if (verbose) {
        std::printf("Getting contents of list\n");
    }

    std::string item {};
    CommandList items {};
    while (list >> item) {
        items.push_back(item);
    }
    return items;
    std::setvbuf(stdout, nullptr, isatty(STDOUT_FILENO) ? _IOLBF : _IOFBF, BUFSIZ);
}

int listIncludes(const std::string& item) {
    std::setvbuf(stdout, nullptr, _IONBF, BUFSIZ);
    auto list {getList()};
    if (!list) {
        return -1;
    }

    if (verbose) {
        std::printf("Checking if list contains \"%s\"\n", item.c_str());
    }
    if (std::find(list->begin(), list->end(), item) != list->end()) {
        return 1;
    }
    return 0;
    std::setvbuf(stdout, nullptr, isatty(STDOUT_FILENO) ? _IOLBF : _IOFBF, BUFSIZ);
}
int addToList(const std::string& item) {
    std::setvbuf(stdout, nullptr, _IONBF, BUFSIZ);
    if (std::find(item.begin(), item.end(), ' ') != item.end()) {
        std::fprintf(stderr,
            isatty(STDERR_FILENO)
                ? "\033[1;31mError:\033[0m Item \"%s\" contains spaces\n"
                : "Error: Item \"%s\" contains spaces\n",
            item.c_str()
        );
        return 1;
    }
    int listIncludesItem {listIncludes(item)};
    if (listIncludesItem == -1) {
        return -1;
    }
    if (listIncludesItem == 1) {
        std::fprintf(stderr,
            isatty(STDERR_FILENO)
                ? "\033[1;31mError:\033[0m List already includes \"%s\"\n"
                : "Error: List already includes \"%s\"\n",
            item.c_str()
        );
        return 1;
    }

    auto list {getList()};
    if (!list) {
        return -1;
    }
    list->push_back(item);

    if (internal::changeList(*list) != 0) {
        return -1;
    }
    if (verbose) {
        std::printf("Item \"%s\" added successfully\n", item.c_str());
    }
    return 0;
    std::setvbuf(stdout, nullptr, isatty(STDOUT_FILENO) ? _IOLBF : _IOFBF, BUFSIZ);
}
int removeFromList(const std::string& item) {
    std::setvbuf(stdout, nullptr, _IONBF, BUFSIZ);
    int listIncludesItem {listIncludes(item)};
    if (listIncludesItem == -1) {
        return -1;
    }
    if (listIncludesItem == 0) {
        std::fprintf(stderr,
            isatty(STDERR_FILENO)
                ? "\033[1;31mError:\033[0m List does not contain \"%s\"\n"
                : "Error: List does not contain \"%s\"\n",
            item.c_str()
        );
        return 1;
    }

    auto list {getList()};
    if (!list) {
        return -1;
    }
    list->erase(std::remove(list->begin(), list->end(), item), list->end());

    if (internal::changeList(*list) != 0) {
        return -1;
    }
    if (verbose) {
        std::printf("Item \"%s\" removed successfully\n", item.c_str());
    }
    return 0;
    std::setvbuf(stdout, nullptr, isatty(STDOUT_FILENO) ? _IOLBF : _IOFBF, BUFSIZ);
}

}
