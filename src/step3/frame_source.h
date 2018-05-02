#ifndef __FRAME_SOURCE_H__
#define __FRAME_SOURCE_H__

#include <gst/gstbuffer.h>

#ifdef __cplusplus
extern "C" {
#endif


void* create_frame_source();
void destroy_frame_source(void*);
void frame_source_start(void*);
void frame_source_stop(void*);
GstBuffer* frame_source_get_frame(void*);


#ifdef __cplusplus
};
#endif

#endif // __FRAME_SOURCE_H__
