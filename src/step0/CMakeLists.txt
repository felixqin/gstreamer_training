set(EXECUTABLE_OUTPUT_PATH  "${OUTDIR}/bin")

include_directories(
    ${TOPDIR}/include/gstreamer-1.0
    ${TOPDIR}/include/glib-2.0
    ${TOPDIR}/lib/glib-2.0/include
)

link_directories(
    ${TOPDIR}/lib
)

add_executable(basic-tutorial-8 basic-tutorial-8.c)

target_link_libraries(basic-tutorial-8
        gstaudio-1.0
        gstreamer-1.0 gobject-2.0 glib-2.0
        )

add_executable(test-readme test-readme.c)

target_link_libraries(test-readme
        gstrtspserver-1.0
        gstreamer-1.0 gobject-2.0 glib-2.0
        )

add_executable(test-appsrc test-appsrc.c)

target_link_libraries(test-appsrc
        gstrtspserver-1.0
        gstreamer-1.0 gobject-2.0 glib-2.0
        )

