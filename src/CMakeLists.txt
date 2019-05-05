
# Make library for image retargeting
add_library (image_retargeter image_retargeter.cc)
target_include_directories (image_retargeter PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${OpenCV_INCLUDE_DIRS})

# Make demo executable
add_executable (image_retargeter_demo demo.cc)
target_include_directories (image_retargeter_demo PUBLIC ${OpenCV_INCLUDE_DIRS})
target_link_libraries (image_retargeter_demo LINK_PUBLIC image_retargeter ${OpenCV_LIBS})
