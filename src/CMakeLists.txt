set(EXECUTABLE_OUTPUT_PATH  "${OUTDIR}/bin")

include_directories(
    ${TOPDIR}/include/gstreamer-1.0
    ${TOPDIR}/include/glib-2.0
    ${TOPDIR}/lib/glib-2.0/include
)

link_directories(
    ${TOPDIR}/lib
)

add_executable(test_readme test_readme.c)

target_link_libraries(test_readme
    gstrtspserver-1.0
    gstreamer-1.0 gobject-2.0 glib-2.0
    )

