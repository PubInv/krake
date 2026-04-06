#ifndef _MOCK_PRINT_H
#define _MOCK_PRINT_H

#include <stdlib.h>
#include <cstdint>

class MockPrint
{
public:
    virtual std::size_t write(uint8_t) = 0;
    std::size_t print(char character)
    {
        return this->write((uint8_t)character);
    }
};

#endif