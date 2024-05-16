#include <stdint.h>
#include <stdio.h>
#include <x264.h>

int main() {
    int width = 640, height = 480;

    // Configure the encoder
    x264_param_t param;
    x264_param_default_preset(&param, "veryfast", "zerolatency");
    param.i_threads            = 12;
    param.i_width              = width;
    param.i_height             = height;
    param.i_fps_num            = 25;
    param.i_fps_den            = 1;
    param.i_keyint_max         = 25;
    param.b_intra_refresh      = 1;
    param.rc.i_rc_method       = X264_RC_CRF;
    param.rc.f_rf_constant     = 25;
    param.rc.f_rf_constant_max = 35;
    param.i_sps_id             = 7;

    // Apply profile
    x264_param_apply_profile(&param, "baseline");

    // Open encoder
    x264_t *encoder = x264_encoder_open(&param);
    if (!encoder) {
        fprintf(stderr, "Failed to open encoder!\n");
        return -1;
    }

    // Frame
    x264_picture_t pic_in, pic_out;
    x264_picture_alloc(&pic_in, X264_CSP_I420, width, height);

    // Fill Y, Cb, and Cr planes
    for (int i = 0; i < 10; ++i) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                pic_in.img.plane[0][y * pic_in.img.i_stride[0] + x] = (x + y + i * 3) % 256; // Y plane
                if (x % 2 == 0 && y % 2 == 0) {
                    int uv_index                  = (y / 2) * pic_in.img.i_stride[1] + (x / 2);
                    pic_in.img.plane[1][uv_index] = (128 + y + i * 2) % 256; // Cb plane
                    pic_in.img.plane[2][uv_index] = (128 + x + i * 2) % 256; // Cr plane
                }
            }
        }

        // Encode
        x264_nal_t *nals;
        int         i_nals;
        int         frame_size = x264_encoder_encode(encoder, &nals, &i_nals, &pic_in, &pic_out);

        if (frame_size < 0) {
            fprintf(stderr, "Failed to encode frame\n");
            return -1;
        } else if (frame_size) {
            printf("Encoded frame %d, size %d bytes\n", i, frame_size);
        }
    }

    // Cleanup
    x264_picture_clean(&pic_in);
    x264_encoder_close(encoder);

    return 0;
}
