
include_directories(
    ${TOPDIR}/include/gstreamer-1.0
    ${TOPDIR}/include/glib-2.0
    ${TOPDIR}/lib/glib-2.0/include
)

add_executable(test_readme test_readme.c)
