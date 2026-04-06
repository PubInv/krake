#ifndef _MOCK_PRINT_H
#define _MOCK_PRINT_H

#include <stdlib.h>

class MockPrint
{
public:
    std::size_t print(char)
    {
        return 0;
    }
};

#endif