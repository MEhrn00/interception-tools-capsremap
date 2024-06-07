#include <linux/input-event-codes.h>
#include <linux/input.h>

#include <algorithm>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <format>
#include <iostream>
#include <iterator>
#include <optional>
#include <span>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

#include <boost/program_options/errors.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp> // IWYU pragma: keep
#include <boost/program_options/variables_map.hpp>
// IWYU pragma: no_include <boost/program_options/detail/parsers.hpp>

#include "config.h"
#include "errors.h"

namespace po = boost::program_options;

std::string program_usage(const std::string_view program) {
    return std::format("usage: {} [-h | -v]\n", program);
}

std::string help_menu(const std::string_view program, po::options_description& opts) {
    std::stringstream opts_message;
    opts_message << opts;

    return std::format("{}\n{}\n{}\n",
                       std::format("{} - {}\n", PROJECT_NAME, PROJECT_DESCRIPTION),
                       program_usage(program),
                       opts_message.str());
}

std::optional<struct input_event> read_event() {
    struct input_event event = {};

    if (std::fread(&event, sizeof(event), 1, stdin) != 1) {
        const int error_value = ferror(stdin);
        if (error_value != 0) {
            throw capsremap::errors::SystemException(error_value,
                                                     std::generic_category());
        }

        return std::nullopt;
    }

    return event;
}

void write_event(struct input_event *event) {
    if (std::fwrite(event, sizeof(struct input_event), 1, stdout) != 1) {
        throw capsremap::errors::SystemException(std::ferror(stdout),
                                                 std::generic_category());
    }
}

void disable_stdio_buffering() {
    int res = 0;
    if ((res = std::setvbuf(stdin, nullptr, _IONBF, 0)); res != 0) {
        throw capsremap::errors::errno_or_value(res);
    }

    if ((res = setvbuf(stdout, nullptr, _IONBF, 0)); res != 0) {
        throw capsremap::errors::errno_or_value(res);
    }
}

int main(int argc, const char *argv[]) {
    auto argvsp = std::span(argv, static_cast<unsigned long>(argc));

    std::vector<std::string_view> argv_sv{};
    std::transform(argvsp.begin(),
                   argvsp.end(),
                   std::back_inserter(argv_sv),
                   [](const char *arg) { return std::string_view(arg); });

    po::options_description options("options");

    // clang-format off
    options.add_options()
        ("help,h", "display help message")
        ("version,v", "print the program's version then exit");
    // clang-format on

    po::variables_map option_variables;

    try {
        po::store(po::parse_command_line(argc, argv, options), option_variables);
        po::notify(option_variables);
    } catch (po::unknown_option& e) {
        std::cerr << program_usage(argv_sv[0]);
        return EINVAL;
    }

    if (option_variables.count("help") > 0U) {
        std::cout << help_menu(argv_sv[0], options);
        return EXIT_SUCCESS;
    }

    if (option_variables.count("version") > 0U) {
        std::cout << std::format("{} v{}\n", PROJECT_NAME, PROJECT_VERSION);
        return EXIT_SUCCESS;
    }

    if (argc > 1) {
        std::cerr << program_usage(argv_sv[0]);
        return EINVAL;
    }

    try {
        disable_stdio_buffering();
    } catch (const std::system_error& e) {
        std::cerr << std::format("failed to disable stdio buffering: {}\n", e.what());
        return e.code().value();
    }

    for (;;) {
        std::optional<struct input_event> event = {};
        try {
            if ((event = read_event()); event) {
                struct input_event input = event.value();
                if (input.type == EV_MSC && input.code == MSC_SCAN) {
                    continue;
                }

                if (input.type != EV_KEY && input.type != EV_REL
                    && input.type != EV_ABS) {
                    write_event(&input);
                    continue;
                }
            }

        } catch (capsremap::errors::SystemException& e) {
            std::cerr << std::format("failed to process event: {}\n", e.what());
            return e.code().value();
        }
    }
}
