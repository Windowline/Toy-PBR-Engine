add_library(stb_image_loader STATIC
    include/ImageLoader.hpp
    src/ImageLoader.cpp
)

find_package(Stb REQUIRED)
target_include_directories(stb_image_loader
        PRIVATE
        include
        ${Stb_INCLUDE_DIR}
)


#target_link_libraries(img_loader
#PRIVATE
#    stb_image
#)
