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

file(COPY
    ${TOPDIR}/bin/libgstrtspserver-1.0-0.dll
    ${TOPDIR}/bin/libgstapp-1.0-0.dll
    ${TOPDIR}/bin/libgstrtsp-1.0-0.dll
    ${TOPDIR}/bin/libgstsdp-1.0-0.dll
    ${TOPDIR}/bin/libgstrtp-1.0-0.dll
    ${TOPDIR}/bin/libgstnet-1.0-0.dll
    ${TOPDIR}/bin/libgstreamer-1.0-0.dll
    ${TOPDIR}/bin/libgstbase-1.0-0.dll
    ${TOPDIR}/bin/libgobject-2.0-0.dll
    ${TOPDIR}/bin/libglib-2.0-0.dll
    ${TOPDIR}/bin/libgmodule-2.0-0.dll
    ${TOPDIR}/bin/libgio-2.0-0.dll
    ${TOPDIR}/bin/libz.dll
    ${TOPDIR}/bin/libffi-7.dll
    ${TOPDIR}/bin/libintl-8.dll
    DESTINATION
    ${OUTDIR}/bin
    )

