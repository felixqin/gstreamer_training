#ifndef __APPSRC_CONTEXT_H__
#define __APPSRC_CONTEXT_H__

#ifdef __cplusplus
extern "C" {
#endif


void* create_context();
void destroy_context(void*);
void start_feed(GstElement*, guint, void*);
void stop_feed(GstElement*, guint, void*);

#ifdef __cplusplus
};  // extern "C"
#endif

#endif // __APPSRC_CONTEXT_H__

