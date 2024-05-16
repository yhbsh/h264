#include <stdio.h>

#include <GLFW/glfw3.h>
#include <OpenGL/gl.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>

#include "util.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "[USAGE]: ./main <input.mp4>\n");
        return 1;
    }

    int ret;

    /* State */
    GLFWwindow      *w          = NULL;
    AVFormatContext *fmt_ctx    = NULL;
    const AVCodec   *vcodec     = NULL;
    const AVCodec   *acodec     = NULL;
    int              vsi        = -1;
    int              asi        = -1;
    AVStream        *vs         = NULL;
    AVStream        *as         = NULL;
    AVCodecContext  *vcodec_ctx = NULL;
    AVCodecContext  *acodec_ctx = NULL;
    AVFrame         *frame      = NULL;
    AVPacket        *packet     = NULL;
    uint8_t         *buffer     = NULL;

    /* Window */
    ret = glfwInit();
    w   = glfwCreateWindow(800, 600, "W", NULL, NULL);
    glfwMakeContextCurrent(w);

    /* OpenGL */
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* FFmpeg */
    ret        = avformat_open_input(&fmt_ctx, argv[1], NULL, NULL);
    ret        = avformat_find_stream_info(fmt_ctx, NULL);
    vsi        = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, 1, 0, &vcodec, 0);
    asi        = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_AUDIO, 0, 0, &acodec, 0);
    vs         = fmt_ctx->streams[vsi];
    as         = fmt_ctx->streams[asi];
    vcodec_ctx = avcodec_alloc_context3(vcodec);
    acodec_ctx = avcodec_alloc_context3(acodec);
    ret        = avcodec_parameters_to_context(vcodec_ctx, vs->codecpar);
    ret        = avcodec_parameters_to_context(acodec_ctx, as->codecpar);
    ret        = avcodec_open2(vcodec_ctx, vcodec, NULL);
    ret        = avcodec_open2(acodec_ctx, acodec, NULL);
    frame      = av_frame_alloc();
    packet     = av_packet_alloc();
    buffer     = malloc(1024 * 1024 * 10);

    /* Render Loop */
    while (!glfwWindowShouldClose(w)) {
        ret = av_read_frame(fmt_ctx, packet);
        if (ret < 0) {
            if (ret == AVERROR_EOF) break;
            fprintf(stderr, "[ERROR]: av_read_frame(fmt_ctx, pkt): %s\n", av_err2str(ret));
            return 1;
        }

        if (packet->stream_index == vsi) {
            ret = avcodec_send_packet(vcodec_ctx, packet);
            while (ret >= 0) {
                ret = avcodec_receive_frame(vcodec_ctx, frame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) break;

                if (ret < 0) {
                    fprintf(stderr, "[ERROR]: avcodec_receive_frame(vcodec_ctx, frm): %s\n", av_err2str(ret));
                    return 1;
                }

                convert_yuv420p_rgb(frame->data[0], frame->data[1], frame->data[2], buffer, frame->width, frame->height);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame->width, frame->height, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glEnable(GL_TEXTURE_2D);
                glBegin(GL_QUADS);
                glTexCoord2f(1.0, 1.0);
                glVertex2f(-1.0, -1.0);
                glTexCoord2f(0.0, 1.0);
                glVertex2f(1.0, -1.0);
                glTexCoord2f(0.0, 0.0);
                glVertex2f(1.0, 1.0);
                glTexCoord2f(1.0, 0.0);
                glVertex2f(-1.0, 1.0);
                glEnd();
                glDisable(GL_TEXTURE_2D);

                glfwSwapBuffers(w);
                glfwPollEvents();

                dump_avframe(vs, frame);
            }
        }

        if (packet->stream_index == asi) {
            ret = avcodec_send_packet(acodec_ctx, packet);
            while (ret >= 0) {
                ret = avcodec_receive_frame(acodec_ctx, frame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) break;

                if (ret < 0) {
                    fprintf(stderr, "[ERROR]: avcodec_receive_frame(acodec_ctx, frm): %s\n", av_err2str(ret));
                    return 1;
                }

                dump_avframe(as, frame);
            }
        }

        av_usleep((double) 1e6 / (double) 1000.0);
        av_packet_unref(packet);
    }

    avformat_close_input(&fmt_ctx);
    avcodec_free_context(&vcodec_ctx);
    avcodec_free_context(&acodec_ctx);
    av_frame_free(&frame);
    av_packet_free(&packet);

    glfwDestroyWindow(w);
    glfwTerminate();

    return 0;
}
