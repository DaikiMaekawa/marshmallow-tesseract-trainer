project(MarshmallowTesseractTrainer)

find_package(Qt4 REQUIRED)
set(QT_USE_QTMAIN true)
set(QT_USE_QTGUI true)
set(QT_USE_QTNETWORK true)
include(${QT_USE_FILE})

include_directories(
    ${QT_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
    ${CMAKE_BINARY_DIR}
)

link_directories(
    ${QT_LIBRARY_DIR}
)

set(TesseractTrainer_SRCS
    main.cpp
    TesseractTrainer.cpp
)

set(TesseractTrainer_HDRS
    TesseractTrainer.h
)

set(TesseractTrainer_UIS
    TesseractTrainer.ui
)

set(TesseractTrainer_MOC_HDRS
    TesseractTrainer.h
)

QT4_ADD_RESOURCES(TesseractTrainer_RC_SRCS TesseractTrainer.qrc)
QT4_WRAP_UI(TesseractTrainer_UIS_H ${TesseractTrainer_UIS})
QT4_WRAP_CPP(TesseractTrainer_MOC_SRCS ${TesseractTrainer_MOC_HDRS})
if(WIN32)
    set_source_files_properties(${TesseractTrainer_SRCS} ${TesseractTrainer_MOC_SRCS} PROPERTIES COMPILE_FLAGS "/W1 /wd4819 /Yc${PCH_HEADER} /FI${PCH_HEADER} /D_WIN32_WINNT=0x0501")
endif(WIN32)

add_executable(MarshmallowTesseractTrainer WIN32
    ${TesseractTrainer_SRCS}
    ${TesseractTrainer_HDRS}
    ${TesseractTrainer_MOC_SRCS}
    ${TesseractTrainer_UIS_H}
    ${TesseractTrainer_RC_SRCS}
)

target_link_libraries(MarshmallowTesseractTrainer
    ${QT_LIBRARIES}
)

