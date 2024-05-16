#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
#include <libavutil/time.h>

void dump_avframe(AVStream *stream, const AVFrame *frame);
void convert_yuv420p_rgb(uint8_t *y_plane, uint8_t *u_plane, uint8_t *v_plane, uint8_t *buffer, int width, int height);
