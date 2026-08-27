/// Small helper for handling the list and symlinks (header)

#ifndef HELPERS_LIST_HPP
#define HELPERS_LIST_HPP

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace helpers::list {

using CommandList = std::vector<std::string>;

extern bool verbose;

namespace internal {
    extern std::filesystem::path COMMAND_LIST;
    int changeList(const CommandList& newList);
}

std::optional<CommandList> getList();
int listIncludes(const std::string& item);
int addToList(const std::string& item);
int removeFromList(const std::string& item);

}

#endif
