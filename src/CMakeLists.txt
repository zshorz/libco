set(LIBCO_SRCS
    co_ctx.cpp
    co_ctx_swap.S)

add_library(colib ${LIBCO_SRCS})
#target_link_libraries(colib)

file(GLOB HEADERS *.h)
