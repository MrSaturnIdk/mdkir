/// Small helper for handling the list and symlinks (header)

#ifndef HELPERS_LIST_HPP
#define HELPERS_LIST_HPP

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace helpers::list {

using CommandList = std::vector<std::string>;

namespace internal {
    extern std::filesystem::path COMMAND_LIST; 
    int changeList(const CommandList& newList, bool verbose);
}

std::optional<CommandList> getList(bool verbose);
int listIncludes(const std::string& item, bool verbose);
int addToList(const std::string& item, bool verbose); 
int removeFromList(const std::string& item, bool verbose);

}

#endif
