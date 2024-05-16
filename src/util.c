#include "util.h"

void dump_avframe(AVStream *stream, const AVFrame *frame) {
    const char        *frame_type  = "Unknown";
    const char        *format_name = NULL;
    const char        *codec_name;
    int                width, height, num, den, sample_rate;
    int64_t            bitrate, pts, dts;
    int                is_key_frame, pict_type;
    AVChannelLayout   *channel_layout;
    AVDictionaryEntry *tag = NULL;

    switch (stream->codecpar->codec_type) {
        case AVMEDIA_TYPE_VIDEO:
            frame_type       = "Video";
            format_name      = av_get_pix_fmt_name(frame->format);
            codec_name       = avcodec_get_name(stream->codecpar->codec_id);
            bitrate          = stream->codecpar->bit_rate;
            width            = stream->codecpar->width;
            height           = stream->codecpar->height;
            num              = stream->avg_frame_rate.num;
            den              = stream->avg_frame_rate.den;
            pts              = frame->pts;
            dts              = frame->pkt_dts;
            int is_key_frame = (frame->flags & AV_FRAME_FLAG_KEY) ? 1 : 0;
            pict_type        = av_get_picture_type_char(frame->pict_type);
            printf("Video Frame: %s - Codec: %s, Bitrate: %lld, Resolution: "
                   "%dx%d, "
                   "Frame rate: %d/%d, ",
                   format_name ? format_name : "Unknown",
                   codec_name,
                   bitrate,
                   width,
                   height,
                   num,
                   den);

            printf("PTS: %lld, DTS: %lld, Key Frame: %d, Picture Type: %c\n", pts, dts, is_key_frame, pict_type);
            break;
        case AVMEDIA_TYPE_AUDIO:
            frame_type     = "Audio";
            format_name    = av_get_sample_fmt_name(frame->format);
            codec_name     = avcodec_get_name(stream->codecpar->codec_id);
            bitrate        = stream->codecpar->bit_rate;
            sample_rate    = stream->codecpar->sample_rate;
            channel_layout = &stream->codecpar->ch_layout;
            pts            = frame->pts;
            dts            = frame->pkt_dts;
            printf("Audio Frame: %s - Codec: %s, Bitrate: %lld, Sample rate: %d, "
                   "Channel count: %d, ",
                   format_name ? format_name : "Unknown",
                   codec_name,
                   bitrate,
                   sample_rate,
                   channel_layout->nb_channels);

            printf("PTS: %lld, DTS: %lld\n", pts, dts);
            break;
        default:
            format_name = "N/A";
            printf("%s Frame: %s\n", frame_type, format_name);
            printf("Details: No additional info available\n");
            break;
    }
}

void convert_yuv420p_rgb(uint8_t *y_plane, uint8_t *u_plane, uint8_t *v_plane, uint8_t *buffer, int width, int height) {
    int r, g, b, y, u, v;
    int uvIndex, rgbIndex;

    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            y       = y_plane[j * width + i];
            uvIndex = (j / 2) * (width / 2) + (i / 2);
            u       = u_plane[uvIndex] - 128;
            v       = v_plane[uvIndex] - 128;

            r = y + (1.370705 * v);
            g = y - (0.698001 * v) - (0.337633 * u);
            b = y + (1.732446 * u);

            rgbIndex             = (j * width + i) * 3;
            buffer[rgbIndex + 0] = (r < 0 ? 0 : (r > 255 ? 255 : r));
            buffer[rgbIndex + 1] = (g < 0 ? 0 : (g > 255 ? 255 : g));
            buffer[rgbIndex + 2] = (b < 0 ? 0 : (b > 255 ? 255 : b));
        }
    }
}
