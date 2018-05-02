#include <stdio.h>


extern "C" int rtsp_server_main (int argc, char *argv[]);

int main(int argc, char* argv[])
{
    printf("hello\n");
    char gst_arg0[] = "";
    char gst_arg1[] = "--gst-debug-level=3";
    char* gst_argv[] = {gst_arg0, gst_arg1};
    int gst_argc = sizeof(gst_argv) / sizeof(gst_argv[0]);
    rtsp_server_main(gst_argc, gst_argv);
    return 0;
}
