/**
 * Both config and insult
 */

#include "ansicolors.h"
#include "array.h"
#include "list.h"

#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* PROGRAM_NAME = "";
int STDERR_TTY = 0;
int verbose = 0;

/**
 * 1 = Bad input
 * 2 = I/O fail
 * 3 = Filesystem fail
 * 4 = Memory fail
 */
int main(int argc, char* argv[]) {
    PROGRAM_NAME = argv[0];
    STDERR_TTY = isatty(STDERR_FILENO);

    const char* start = strrchr(PROGRAM_NAME, '/');
    const char* callName = PROGRAM_NAME;
    if (start) {
        callName = start + 1;
    }
    if (strcmp(callName, CALL_NAME)) {
        printf("That's %snot%s how it's spelled\n",
            STDERR_TTY ? ANSI_BOLD : "",
            STDERR_TTY ? ANSI_RESET : ""
        );
        return 0;
    }

    if (argc == 1 || argc == 2 && !strcmp(argv[1], "--verbose")) {
        fprintf(stderr, "%s: %serror:%s %sno commands given%s\n",
            PROGRAM_NAME,
            STDERR_TTY ? ANSI_BOLD ANSI_RED : "",
            STDERR_TTY ? ANSI_RESET : "",
            STDERR_TTY ? ANSI_BOLD : "",
            STDERR_TTY ? ANSI_RESET : ""
        );
        fprintf(stderr, "%s: %snote:%s %ssee '%s --help' for details%s\n",
            PROGRAM_NAME,
            STDERR_TTY ? ANSI_BOLD ANSI_GRAY : "",
            STDERR_TTY ? ANSI_RESET : "",
            STDERR_TTY ? ANSI_BOLD : "",
            PROGRAM_NAME,
            STDERR_TTY ? ANSI_RESET : ""
        );
        return 1;
    }

    const char* expectedArgumentParameter = "";
    const char* argumentLookupTable[] = {
        /// Basic commands
        "create",
        "delete",
        "list",
        /// Misc
        "--verbose",
        "--help",
        "--version"
    };
    const char* argumentParameterLookupTable[] = {
        "create",
        "delete"
    };
    for (int i = 1; i < argc; ++i) {
        int parameterLookup = findIndexOfString(
            argumentParameterLookupTable,
            sizeof(argumentParameterLookupTable) / sizeof(argumentParameterLookupTable[0]),
            expectedArgumentParameter
        );
        if (parameterLookup == -1) {
            int lookup = findIndexOfString(
                argumentLookupTable,
                sizeof(argumentLookupTable) / sizeof(argumentLookupTable[0]),
                argv[i]
            );
            switch (lookup) {
                /// Same order as in argumentLookupTable
                case 0: {
                    expectedArgumentParameter = "create";
                    break;
                }
                case 1: {
                    expectedArgumentParameter = "delete";
                    break;
                }
                case 2: {
                    CommandList list = getList();
                    if (!list.list) {
                        switch (list.len) {
                            case -1: {
                                return 2;
                            }
                            case -2: {
                                return 4;
                            }
                        }
                    }

                    printf("List of commands:\n");
                    for (int i = 0; i < list.len; ++i) {
                        printf("%s\n", list.list[i]);
                    }
                    free(list.allocated);
                    free(list.list);
                    return 0;
                }

                case 3: {
                    verbose = 1;
                    break;
                }
                case 4: {
                    printf(
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
                case 5: {
                    printf(
                        "mdkir " PROJECT_VER "\n\n"
                        "Copyright (c) 2026 MrSaturnIdk\n"
                        "Licensed under the AGPL-3.0-or-later\n\n"
                        "Source code hosted at <" HOMEPAGE ">\n"
                    );
                    return 0;
                }
                default: {
                    fprintf(stderr, "%s: %serror:%s %sunrecognized argument '%s'%s\n",
                        PROGRAM_NAME,
                        STDERR_TTY ? ANSI_BOLD ANSI_RED : "",
                        STDERR_TTY ? ANSI_RESET : "",
                        STDERR_TTY ? ANSI_BOLD : "",
                        argv[i],
                        STDERR_TTY ? ANSI_RESET : ""
                    );
                    fprintf(stderr, "%s: %snote:%s %ssee '%s --help' for details%s\n",
                        PROGRAM_NAME,
                        STDERR_TTY ? ANSI_BOLD ANSI_GRAY : "",
                        STDERR_TTY ? ANSI_RESET : "",
                        STDERR_TTY ? ANSI_BOLD : "",
                        PROGRAM_NAME,
                        STDERR_TTY ? ANSI_RESET : ""
                    );
                    return 1;
                }
            }
        } else {
            int lookup = parameterLookup;
            switch (lookup) {
                /// In the same order as argumentParameterLookupTable, similar to argumentLookupTable
                case 0: {
                    if (NEEDS_SUDO && geteuid()) {
                        fprintf(stderr,"%s: %serror:%s %sno permissions%s\n",
                            PROGRAM_NAME,
                            STDERR_TTY ? ANSI_BOLD ANSI_RED : "",
                            STDERR_TTY ? ANSI_RESET : "",
                            STDERR_TTY ? ANSI_BOLD : "",
                            STDERR_TTY ? ANSI_RESET : ""
                        );
                        return 1;
                    }

                    int status = addToList(argv[i]);
                    switch (status) {
                        case -1: {
                            return 2;
                        }
                        case -2: {
                            return 4;
                        }
                        case 1: {
                            return 1;
                        }
                    }

                    if (verbose) {
                        printf("Making symlink %s/%s\n", BINDIR, argv[i]);
                        fflush(stdout);
                    }

                    char linkPath[4096] = BINDIR "/";
                    strncat(linkPath, argv[i], sizeof(linkPath) - strlen(argv[i]) - 1);
                    if (symlink(BINDIR "/" CALL_NAME, linkPath)) {
                        fprintf(stderr, "%s: %serror:%s %scouldn't link %s to %s%s\n",
                            PROGRAM_NAME,
                            STDERR_TTY ? ANSI_BOLD ANSI_RED : "",
                            STDERR_TTY ? ANSI_RESET : "",
                            STDERR_TTY ? ANSI_BOLD : "",
                            BINDIR "/" CALL_NAME,
                            linkPath,
                            STDERR_TTY ? ANSI_RESET : ""
                        );
                        return 3;
                    }
                    if (verbose) {
                        printf("Symlink made\n");
                        fflush(stdout);
                    }
                    printf("Command '%s' added successfully\n", argv[i]);

                    return 0;
                }
                case 1: {
                    if (NEEDS_SUDO && geteuid()) {
                        fprintf(stderr,"%s: %serror:%s %sno permissions%s\n",
                            PROGRAM_NAME,
                            STDERR_TTY ? ANSI_BOLD ANSI_RED : "",
                            STDERR_TTY ? ANSI_RESET : "",
                            STDERR_TTY ? ANSI_BOLD : "",
                            STDERR_TTY ? ANSI_RESET : ""
                        );
                        return 1;
                    }

                    int status = removeFromList(argv[i]);
                    switch (status) {
                        case -1: {
                            return 2;
                        }
                        case -2: {
                            return 4;
                        }
                        case 1: {
                            return 1;
                        }
                    }

                    if (verbose) {
                        printf("Removing symlink %s/%s\n", BINDIR, argv[i]);
                        fflush(stdout);
                    }
                    char linkPath[4096] = BINDIR "/";
                    strncat(linkPath, argv[i], sizeof(linkPath) - strlen(argv[i]) - 1);
                    if (unlink(linkPath)) {
                        fprintf(stderr, "%s: %serror:%s %scouldn't remove symlink %s%s\n",
                            PROGRAM_NAME,
                            STDERR_TTY ? ANSI_BOLD ANSI_RED : "",
                            STDERR_TTY ? ANSI_RESET : "",
                            STDERR_TTY ? ANSI_BOLD : "",
                            linkPath,
                            STDERR_TTY ? ANSI_RESET : ""
                        );
                        return 3;
                    }
                    if (verbose) {
                        printf("Symlink removed\n");
                        fflush(stdout);
                    }
                    printf("Command '%s' removed successfully\n", argv[i]);

                    return 0;
                }
            }
        }
    }
    fprintf(stderr, "%s: %serror:%s %sno parameter given for argument '%s'%s\n",
        PROGRAM_NAME,
        STDERR_TTY ? ANSI_BOLD ANSI_RED : "",
        STDERR_TTY ? ANSI_RESET : "",
        STDERR_TTY ? ANSI_BOLD : "",
        argv[argc - 1],
        STDERR_TTY ? ANSI_RESET : ""
    );
    fprintf(stderr, "%s: %snote:%s %ssee '%s --help' for details%s\n",
        PROGRAM_NAME,
        STDERR_TTY ? ANSI_BOLD ANSI_GRAY : "",
        STDERR_TTY ? ANSI_RESET : "",
        STDERR_TTY ? ANSI_BOLD : "",
        PROGRAM_NAME,
        STDERR_TTY ? ANSI_RESET : ""
    );
    return 1;
}
