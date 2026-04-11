#ifndef _MOCK_PRINTABLE_H
#define _MOCK_PRINTABLE_H

#include <stdlib.h>

class MockPrint;

class MockPrintable
{
public:
    virtual ~MockPrintable() {}
    virtual std::size_t printTo(MockPrint &p) const = 0;
};

#endif
