#ifndef SPLAY_ASSERT_H
#define SPLAY_ASSERT_H

#include <iostream>
#include <sstream>

void _assert(const char *msg, const char *file, int line) {
    throw std::runtime_error((std::stringstream() << "Assert failed: '" << std::string(msg) << "' in file '"
        << std::string(file) << "' at line " << line << ".").str());
}

#define assert(EXPR) (void)((EXPR) || (_assert(#EXPR, __FILE_NAME__, __LINE__), 0))

#endif // SPLAY_ASSERT_H
