#ifndef __APPSRC_CONTEXT_H__
#define __APPSRC_CONTEXT_H__

#ifdef __cplusplus
extern "C" {
#endif


void* create_context();
void destroy_context(void*);
void need_data (GstElement*, guint, void*);
void enough_data (GstElement*, guint, void*);

#ifdef __cplusplus
};  // extern "C"
#endif

#endif // __APPSRC_CONTEXT_H__

