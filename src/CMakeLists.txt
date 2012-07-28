########### next target ###############

SET(svmtool_LIB_SRCS
  swindow.cc
  weight.cc
  dict.cc
  tagger.cc
  common.cc
  er.cc
  mapping.cc
  api.cc
  reader.cc
)

add_library(svmtool SHARED ${svmtool_LIB_SRCS})

target_link_libraries(svmtool)

set_target_properties(svmtool PROPERTIES VERSION 1.1.6 SOVERSION 1)
install(TARGETS svmtool DESTINATION lib)

########### next target ###############

SET(SVMTagger_SRCS
  SVMTagger.cc
)

add_executable(SVMTagger ${SVMTagger_SRCS})

target_link_libraries(SVMTagger svmtool)

install(TARGETS SVMTagger DESTINATION bin)

########### next target ###############

SET(SVMTeval_SRCS
  SVMTeval.cc
)

add_executable(SVMTeval ${SVMTeval_SRCS})

target_link_libraries(SVMTeval svmtool)

install(TARGETS SVMTeval DESTINATION bin)

########### next target ###############

SET(SVMTlearn_SRCS
SVMTlearn.cc
learner.cc
)

add_executable(SVMTlearn ${SVMTlearn_SRCS})

target_link_libraries(SVMTlearn svmtool)

install(TARGETS SVMTlearn DESTINATION bin)

########### install files ###############
install(FILES
  tagger.h
  common.h
  weight.h
  list.h
  hash.h
  swindow.h
  dict.h
  er.h
  reader.h
  nodo.h
DESTINATION  include/svmtool)
