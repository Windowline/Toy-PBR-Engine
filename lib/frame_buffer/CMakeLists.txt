add_library(frame_buffer STATIC
        include/FrameBufferObject.hpp
        src/FrameBufferObject.cpp
)

target_include_directories(frame_buffer
PUBLIC
        ../frame_buffer/include
        ../math
        ../util
)

target_link_libraries(frame_buffer
        PUBLIC
        glad::glad
        math
        util
)

