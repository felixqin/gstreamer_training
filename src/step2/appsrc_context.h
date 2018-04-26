#ifndef __APPSRC_CONTEXT_H__
#define __APPSRC_CONTEXT_H__

#ifdef __cplusplus
extern "C" {
#endif


void* create_context(GstElement*);
void destroy_context(void*);
void start_feed(GstElement*, guint length, void*);
void stop_feed(GstElement*, void*);

#ifdef __cplusplus
};  // extern "C"
#endif

#endif // __APPSRC_CONTEXT_H__

