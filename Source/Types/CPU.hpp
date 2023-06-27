#ifndef TYPES_CPU_HPP
#define TYPES_CPU_HPP

#include <Config.hpp>

union OperationCode {
    struct {
        uint8_t group : 2;
        uint8_t mode : 3;
        uint8_t inst : 3;
    };

    struct {
        uint8_t : 5;
        uint8_t test : 1;
        uint8_t flag : 2;
    };

    uint8_t _raw;
};

#endif // TYPES_CPU_HPP