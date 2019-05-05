
# Make library for image retargeting
add_library (seam_carving seam_carving.cc)
target_include_directories (seam_carving PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${OpenCV_INCLUDE_DIRS})

# Make demo executable
add_executable (seam_carving_demo demo.cc)
target_include_directories (seam_carving_demo PUBLIC ${OpenCV_INCLUDE_DIRS})
target_link_libraries (seam_carving_demo LINK_PUBLIC seam_carving ${OpenCV_LIBS})
