#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <x265.h>

int main() {
    int width      = 1920;
    int height     = 1080;
    int frameCount = 10;

    x265_param *param = x265_param_alloc();
    x265_param_default(param);
    param->bRepeatHeaders = 1;             // Write headers again with each keyframe
    param->internalCsp    = X265_CSP_I420; // Chroma subsampling
    param->sourceWidth    = width;
    param->sourceHeight   = height;
    param->fpsNum         = 25;
    param->fpsDenom       = 1;

    x265_encoder *encoder = x265_encoder_open(param);
    if (!encoder) {
        fprintf(stderr, "Failed to open encoder!\n");
        return 1;
    }

    x265_picture *picture = x265_picture_alloc();
    x265_picture_init(param, picture);
    picture->planes[0] = malloc(width * height);
    picture->planes[1] = malloc(width * height / 4);
    picture->planes[2] = malloc(width * height / 4);
    picture->stride[0] = width;
    picture->stride[1] = width / 2;
    picture->stride[2] = width / 2;

    memset(picture->planes[0], 0, width * height);
    memset(picture->planes[1], 128, width * height / 4);
    memset(picture->planes[2], 128, width * height / 4);

    for (int i = 0; i < frameCount; i++) {
        x265_nal *nals       = NULL;
        uint32_t  nal_count  = 0;
        int       frame_size = x265_encoder_encode(encoder, &nals, &nal_count, picture, NULL);

        if (frame_size < 0) {
            fprintf(stderr, "Failed to encode frame\n");
            return 1;
        } else if (frame_size) {
            printf("Encoded frame %d, size %d bytes\n", i + 1, frame_size);
        }
    }

    return 0;
}
