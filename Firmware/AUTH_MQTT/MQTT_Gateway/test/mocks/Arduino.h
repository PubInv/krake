#pragma once
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdint>

// ── String ────────────────────────────────────────────────────────────
class String {
public:
    std::string _s;

    String()                  = default;
    String(const char* s)     : _s(s ? s : "") {}
    String(std::string s)     : _s(std::move(s)) {}
    String(int v)             : _s(std::to_string(v)) {}
    String(unsigned long v)   : _s(std::to_string(v)) {}

    const char* c_str()  const { return _s.c_str(); }
    size_t      length() const { return _s.size(); }
    bool        isEmpty()const { return _s.empty(); }

    bool startsWith(const String& p) const {
        return _s.rfind(p._s, 0) == 0;
    }
    String substring(size_t from) const {
        return String(_s.substr(from));
    }
    void replace(const char* from, const char* to) {
        size_t pos = 0;
        while ((pos = _s.find(from, pos)) != std::string::npos) {
            _s.replace(pos, strlen(from), to);
            pos += strlen(to);
        }
    }

    String  operator+ (const String& o) const { return String(_s + o._s); }
    String& operator+=(const String& o)       { _s += o._s; return *this; }
    bool    operator==(const String& o) const { return _s == o._s; }
    bool    operator!=(const String& o) const { return _s != o._s; }
    bool operator< (const String& o) const { return _s <  o._s; }
    bool operator> (const String& o) const { return _s >  o._s; }
    operator const char*() const { return _s.c_str(); }
};
inline String operator+(const char* lhs, const String& rhs) {
    return String(std::string(lhs) + rhs._s);
}
// ── Serial stub ────────────────────────────────────────────────────────
struct SerialClass {
    void println(const char*)   {}
    void println(const String&) {}
    template<typename... A>
    void printf(const char*, A...) {}
};
static SerialClass Serial;