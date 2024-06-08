#include <linux/input-event-codes.h>
#include <linux/input.h>

#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <format>
#include <iostream>
#include <iterator>
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

#include "errors.h"
#include <capsremap_config.h>

namespace po = boost::program_options;

using namespace std::chrono_literals;

constexpr auto DEFAULT_DELAY = 20ms;

enum class KeyState {
    None,
    CapsHeld,
};

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

// NOLINTNEXTLINE(*-avoid-c-arrays): argv has to to be a C array here
std::vector<std::string_view> transform_args(int argc, const char *argv[]) {
    auto argv_char_span = std::span(argv, static_cast<unsigned long>(argc));

    std::vector<std::string_view> argv_string_view{};
    std::transform(argv_char_span.begin(),
                   argv_char_span.end(),
                   std::back_inserter(argv_string_view),
                   [](const char *arg) { return std::string_view(arg); });

    return argv_string_view;
}

int main(int argc, const char *argv[]) {
    auto arguments = transform_args(argc, argv);

    po::options_description options("options");

    int input_delay_ms = 0;

    // clang-format off
    options.add_options()
        ("delay,d", po::value<int>(&input_delay_ms)->default_value(DEFAULT_DELAY.count()), "capslock key delay in milliseconds")
        ("help,h", "display help message")
        ("version,v", "print the program's version then exit");
    // clang-format on

    po::variables_map option_variables;

    try {
        po::store(po::parse_command_line(argc, argv, options), option_variables);
        po::notify(option_variables);
    } catch (po::unknown_option& e) {
        std::cerr << program_usage(arguments[0]);
        return EINVAL;
    }

    if (option_variables.count("help") > 0U) {
        std::cout << help_menu(arguments[0], options);
        return EXIT_SUCCESS;
    }

    if (option_variables.count("version") > 0U) {
        std::cout << std::format("{} version {}\n", PROJECT_NAME, PROJECT_VERSION);
        return EXIT_SUCCESS;
    }

    if (argc > 1 && option_variables.count("delay") == 0U) {
        std::cerr << program_usage(arguments[0]);
        return EINVAL;
    }

    if (input_delay_ms < 0L) {
        std::cerr << "input delay is negative\n";
        return EINVAL;
    }

    const std::chrono::nanoseconds delay = std::chrono::milliseconds(input_delay_ms);

    try {
        disable_stdio_buffering();
    } catch (const capsremap::errors::SystemException& e) {
        std::cerr << std::format("failed to disable stdio buffering: {}\n", e.what());
        return e.code().value();
    }

    struct input_event event = {};
    try {
        while (fread(&event, sizeof(event), 1, stdin) == 1) {
            // key events
            if (event.type == EV_KEY) {
                if (event.code == KEY_RIGHTALT) { // right alt
                    event.code = KEY_CAPSLOCK;
                } else if (event.code == KEY_CAPSLOCK) { // capslock
                    event.code = KEY_LEFTCTRL;
                }
            }

            // Write out the modified event
            write_event(&event);
        }
    } catch (capsremap::errors::SystemException& e) {
        std::cerr << std::format("failed to write event: {}\n", e.what());
    }
}
