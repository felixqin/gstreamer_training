set(EXECUTABLE_OUTPUT_PATH  "${OUTDIR}/bin")

include_directories(
        ${TOPDIR}/include/gstreamer-1.0
        ${TOPDIR}/include/glib-2.0
        ${TOPDIR}/lib/glib-2.0/include
)

link_directories(
        ${TOPDIR}/lib
)

add_executable(my_appsrc my_appsrc.c appsrc_context.cpp)

target_link_libraries(my_appsrc
        gstrtspserver-1.0
        gstreamer-1.0 gobject-2.0 glib-2.0
        )
