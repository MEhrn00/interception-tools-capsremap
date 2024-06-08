#ifndef CAPSREMAP_ERRORS_H
#define CAPSREMAP_ERRORS_H

#include <system_error>

namespace capsremap::errors {

class SystemException: public std::system_error {
public:
    explicit SystemException(std::error_code code): std::system_error(code) {}

    SystemException(int underyling_error, const std::error_category& ecat)
        : std::system_error(underyling_error, ecat) {}
};

inline SystemException errno_or_value(int value) {
    return {errno != 0 ? errno : value, std::generic_category()};
}

}; // namespace capsremap::errors

#endif // CAPSREMAP_ERRORS_H
