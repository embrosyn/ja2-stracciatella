include(ExternalProject)

externalproject_add(boost-internal
    URL "${CMAKE_CURRENT_SOURCE_DIR}/boost_1_61_0.tar.bz2"
    INSTALL_DIR "${DEPENDENCIES_INSTALL_DIR}"
    CONFIGURE_COMMAND <SOURCE_DIR>/bootstrap.sh --prefix=<INSTALL_DIR> --with-libraries=filesystem
    BUILD_COMMAND <SOURCE_DIR>/b2
    INSTALL_COMMAND <SOURCE_DIR>/b2 install
    BUILD_IN_SOURCE 1
)
