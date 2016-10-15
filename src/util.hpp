#pragma once

#define FAIL(msg)                  \
    endwin();                      \
    std::cerr << msg << std::endl; \
    std::exit(1);
