include_directories( ${CMAKE_SOURCE_DIR}/src )
link_directories(${CMAKE_BINARY_DIR}/src)

########### next target ###############

SET(sample_SRCS
  main.cc
)

add_executable(sample ${sample_SRCS})

target_link_libraries(sample svmtool)

# install(TARGETS sample DESTINATION bin)

########### install files ###############
# install(FILES xxx DESTINATION yyy)
