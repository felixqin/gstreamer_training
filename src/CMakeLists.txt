

file(COPY
        ${TOPDIR}/bin/libgstrtspserver-1.0-0.dll
        ${TOPDIR}/bin/libgstapp-1.0-0.dll
        ${TOPDIR}/bin/libgstrtsp-1.0-0.dll
        ${TOPDIR}/bin/libgstsdp-1.0-0.dll
        ${TOPDIR}/bin/libgstrtp-1.0-0.dll
        ${TOPDIR}/bin/libgstnet-1.0-0.dll
        ${TOPDIR}/bin/libgstaudio-1.0-0.dll
        ${TOPDIR}/bin/libgstvideo-1.0-0.dll
        ${TOPDIR}/bin/libgsttag-1.0-0.dll
        ${TOPDIR}/bin/libgstpbutils-1.0-0.dll
        ${TOPDIR}/bin/libgstreamer-1.0-0.dll
        ${TOPDIR}/bin/libgstbase-1.0-0.dll
        ${TOPDIR}/bin/libgobject-2.0-0.dll
        ${TOPDIR}/bin/libglib-2.0-0.dll
        ${TOPDIR}/bin/libgmodule-2.0-0.dll
        ${TOPDIR}/bin/libgio-2.0-0.dll
        ${TOPDIR}/bin/libx264-148.dll
        ${TOPDIR}/bin/liborc-0.4-0.dll
        ${TOPDIR}/bin/libz.dll
        ${TOPDIR}/bin/libffi-7.dll
        ${TOPDIR}/bin/libintl-8.dll
        DESTINATION
        ${OUTDIR}/bin
        )

file(COPY
        ${TOPDIR}/lib/gstreamer-1.0/libgstapp.dll
        ${TOPDIR}/lib/gstreamer-1.0/libgstcoreelements.dll
        ${TOPDIR}/lib/gstreamer-1.0/libgstrtp.dll
        ${TOPDIR}/lib/gstreamer-1.0/libgstrtpmanager.dll
        ${TOPDIR}/lib/gstreamer-1.0/libgstudp.dll
        ${TOPDIR}/lib/gstreamer-1.0/libgstvideoconvert.dll
        ${TOPDIR}/lib/gstreamer-1.0/libgstvideotestsrc.dll
        ${TOPDIR}/lib/gstreamer-1.0/libgstx264.dll
        DESTINATION
        ${OUTDIR}/lib/gstreamer-1.0
        )


add_subdirectory(step0)
add_subdirectory(step1)
add_subdirectory(step2)
add_subdirectory(step3)

