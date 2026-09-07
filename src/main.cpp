/**
 * Both config and insult
 */

#include "ansicolors.h"
#include "helpers/list.hpp"

#include <unistd.h>

#include <cstdio>
#include <filesystem>
#include <string>
#include <system_error>
#include <unordered_map>
#include <vector>

/**
 * 1 = Bad input
 * 2 = I/O fail
 * 3 = Filesystem fail
 */
int main(int argc, char* argv[]) {
    std::string callName {std::filesystem::path {argv[0]}.filename().string()};
    if (callName != CALL_NAME) {
        std::printf(isatty(STDOUT_FILENO)
            ? "That's \033[1mnot" ANSI_RESET " how it's spelled\n"
            : "That's not how it's spelled\n"
        );
        return 0;
    }

    if (argc == 1 || (argc == 2 && std::string {argv[1]} == "--verbose")) {
        std::fprintf(stderr,
            isatty(STDERR_FILENO)
                ? ANSI_BOLD ANSI_RED "Error:" ANSI_RESET " No commands given\n"
                  "See \"%s --help\" for details\n"
                : "Error: No commanda given\n"
                  "See \"%s --help\" for details\n",
                argv[0]
            );
        return 1;
    }

    bool verbose {};
    std::string expectedArgumentParameter {};
    std::unordered_map<std::string, int> argumentLookupTable {
        /// Basic commands
        {"create", 1},
        {"delete", 2},
        {"list", 3},
        /// Misc
        {"--verbose", 4},
        {"--help", 5},
        {"--version", 6}
    };
    std::unordered_map<std::string, int> argumentParameterLookupTable {
        {"create", 1},
        {"delete", 2}
    };
    for (int i {1}; i < argc; ++i) {
        if (argumentParameterLookupTable[expectedArgumentParameter] == 0) {
            int lookup {argumentLookupTable[argv[i]]};
            switch (lookup) {
                /// Same order as in argumentLookupTable
                case 1: {
                    expectedArgumentParameter = "create";
                    break;
                }
                case 2: {
                    expectedArgumentParameter = "delete";
                    break;
                }
                case 3: {
                    auto list {helpers::list::getList()};
                    if (!list) {
                        return 2;
                    }

                    std::printf("List of commands:\n");
                    for (const std::string& item : *list) {
                        std::printf("%s\n", item.c_str());
                    }
                    return 0;
                }

                case 4: {
                    verbose = true;
                    helpers::list::verbose = true;
                    break;
                }
                case 5: {
                    std::printf("%s",
                        "Usage: mdkir [options] <commands>\n"
                        "The Unix utility that insults you upon misspelling a command\n"
                        "These are system wide by the way\n\n"
                        "List of options:\n"
                        "  --verbose  Makes it verbose\n\n"
                        "List of commands:\n"
                        "Interact with the command list\n"
                        "  create     Create a command to insult you\n"
                        "  delete     Delete a command so now it won't insult you\n"
                        "  list       List the commands that insult you\n"
                        "Miscellaneous\n"
                        "  --help     Prints this screen\n"
                        "  --version  Prints version\n"
                    );
                    return 0;
                }
                case 6: {
                    std::printf("%s",
                        "mdkir " PROJECT_VER "\n\n"
                        "Copyright © 2026 MrSaturnIdk\n"
                        "Licensed under the AGPL-3.0-or-later\n\n"
                        "Source code hosted at <" HOMEPAGE ">\n"
                    );
                    return 0;
                }
                default: {
                    std::fprintf(stderr, "%s%s%s%s%s",
                        isatty(STDERR_FILENO)
                            ? ANSI_BOLD ANSI_RED "Error:" ANSI_RESET " Unrecognized argument \""
                            : "Error: Unrecognized argument \"",
                        argv[i], "\"\n"
                        "See \"", argv[0], " --help\" for details\n"
                    );
                    return 1;
                }
            }
        } else {
            int lookup {argumentParameterLookupTable[expectedArgumentParameter]};
            switch (lookup) {
                /// In the same order as argumentParameterLookupTable, similar to argumentLookupTable
                case 1: {
                    if (NEEDS_SUDO && getuid() != 0) {
                        std::fprintf(stderr,
                            isatty(STDERR_FILENO)
                                ? ANSI_BOLD ANSI_RED "Error:" ANSI_RESET " No permissions\n"
                                : "Error: No permissions\n"
                        );
                        return 1;
                    }

                    int status {helpers::list::addToList(argv[i])};
                    switch (status) {
                        case -1: {
                            return 2;
                        }
                        case 1: {
                            return 1;
                        }
                    }

                    if (verbose) {
                        std::printf("Making symlink %s/%s\n", BINDIR, argv[i]);
                        std::fflush(stdout);
                    }
                    std::error_code ec {};
                    std::filesystem::create_symlink(
                        BINDIR "/" CALL_NAME,
                        std::filesystem::path {BINDIR}/argv[i],
                        ec
                    );
                    if (ec) {
                        std::fprintf(stderr,
                            isatty(STDERR_FILENO)
                                ? ANSI_BOLD ANSI_RED "Error:" ANSI_RESET " Couldn't make symlink %s/%s\n"
                                  "Associated message: \"%s\" (code %d)\n"
                                : "Error: Couldn't make symlink %s/%s\n"
                                  "Associated message: \"%s\" (code %d)\n",
                            BINDIR,
                            argv[i],
                            ec.message().c_str(),
                            ec.value()
                        );
                        return 3;
                    }
                    if (verbose) {
                        std::printf("Symlink made\n");
                        std::fflush(stdout);
                    }
                    std::printf("Command \"%s\" added successfully\n", argv[i]);

                    expectedArgumentParameter = "";
                    return 0;
                }
                case 2: {
                    if (NEEDS_SUDO && getuid() != 0) {
                        std::fprintf(stderr,
                            isatty(STDERR_FILENO)
                                ? ANSI_BOLD ANSI_RED "Error:" ANSI_RESET " No permissions\n"
                                : "Error: No permissions\n"
                        );
                        return 1;
                    }

                    int status {helpers::list::removeFromList(argv[i])};
                    switch (status) {
                        case -1: {
                            return 2;
                        }
                        case 1: {
                            return 1;
                        }
                    }

                    if (verbose) {
                        std::printf("Removing symlink %s/%s\n", BINDIR, argv[i]);
                        std::fflush(stdout);
                    }
                    std::error_code ec {};
                    std::filesystem::remove(
                        std::filesystem::path {BINDIR}/argv[i],
                        ec
                    );
                    if (ec) {
                        std::fprintf(stderr,
                            isatty(STDERR_FILENO)
                                ? ANSI_BOLD ANSI_RED "Error:" ANSI_RESET " Couldn't remove symlink %s/%s\n"
                                  "Associated message: \"%s\" (code %d)\n"
                                : "Error: Couldn't remove symlink %s/%s\n"
                                  "Associated message: \"%s\" (code %d)\n",
                            BINDIR,
                            argv[i],
                            ec.message().c_str(),
                            ec.value()
                        );
                        return 3;
                    }
                    if (verbose) {
                        std::printf("Symlink removed\n");
                        std::fflush(stdout);
                    }
                    std::printf("Command \"%s\" removed successfully\n", argv[i]);

                    expectedArgumentParameter = "";
                    return 0;
                }
            }
        }
    }
    std::fprintf(stderr,
        isatty(STDERR_FILENO)
            ? ANSI_BOLD ANSI_RED "Error:" ANSI_RESET " Parameter for argument \"%s\" not given\n"
              "See \"%s --help\" for details\n"
            : "Error: Parameter for argument \"%s\" not given\n"
              "See \"%s --help\" for details\n",
        argv[argc - 1],
        argv[0]
    );
    return 1;
}
