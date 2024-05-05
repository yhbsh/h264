#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <x264.h>

int main() {
    int width = 640;
    int height = 480;
    int fps = 5;
    int num_frames = 200;

    x264_t* encoder;
    x264_picture_t pic_in, pic_out;
    x264_param_t param;
    FILE* h264_file;

    x264_param_default_preset(&param, "veryfast", "zerolatency");
    param.i_csp = X264_CSP_I420;
    param.i_width = width;
    param.i_height = height;
    param.i_fps_num = fps;
    param.i_fps_den = 1;
    param.i_keyint_max = fps;
    param.b_repeat_headers = 1;
    x264_param_apply_profile(&param, "baseline");

    encoder = x264_encoder_open(&param);
    h264_file = fopen("video.h264", "wb");

    x264_picture_alloc(&pic_in, X264_CSP_I420, width, height);
    pic_in.i_type = X264_TYPE_AUTO;

    for (int i = 0; i < num_frames; i++) {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                double angle = atan2(y - height / 2, x - width / 2);
                double radius = sqrt(pow(x - width / 2, 2) + pow(y - height / 2, 2));
                double intensity = 128 + 127 * sin(radius * 0.05 + angle * 3 + i * 0.1);
                pic_in.img.plane[0][x + y * width] = (int)intensity;
            }
            if (y < height / 2) {
                uint8_t* u_plane = pic_in.img.plane[1] + y * pic_in.img.i_stride[1];
                uint8_t* v_plane = pic_in.img.plane[2] + y * pic_in.img.i_stride[2];
                for (int x = 0; x < width / 2; x++) {
                    double angle = atan2(y - height / 4, x - width / 4);
                    double radius = sqrt(pow(x - width / 4, 2) + pow(y - height / 4, 2));
                    u_plane[x] = 128 + (int)(127 * sin(radius * 0.1 + i * 0.05));
                    v_plane[x] = 128 + (int)(127 * cos(angle * 5 + i * 0.03));
                }
            }
        }
    
        x264_nal_t* nals;
        int i_nal;
        x264_encoder_encode(encoder, &nals, &i_nal, &pic_in, &pic_out);
        for (int j = 0; j < i_nal; j++) {
            fwrite(nals[j].p_payload, 1, nals[j].i_payload, h264_file);
        }
    }

    while (x264_encoder_delayed_frames(encoder)) {
        x264_nal_t* nals;
        int i_nal;
        x264_encoder_encode(encoder, &nals, &i_nal, NULL, &pic_out);
        for (int j = 0; j < i_nal; j++) {
            fwrite(nals[j].p_payload, 1, nals[j].i_payload, h264_file);
        }
    }

    x264_picture_clean(&pic_in);
    x264_encoder_close(encoder);
    fclose(h264_file);

    return 0;
}
