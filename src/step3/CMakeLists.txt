set(EXECUTABLE_OUTPUT_PATH  "${OUTDIR}/bin")

include_directories(
        ${TOPDIR}/include/gstreamer-1.0
        ${TOPDIR}/include/glib-2.0
        ${TOPDIR}/lib/glib-2.0/include
)

link_directories(
        ${TOPDIR}/lib
)

aux_source_directory(./ SRC_FILES)

add_executable(raw_rtsp2 ${SRC_FILES})

target_link_libraries(raw_rtsp2
        gstrtspserver-1.0 gstapp-1.0
        gstreamer-1.0 gobject-2.0 glib-2.0
        )
