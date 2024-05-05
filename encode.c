#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <x264.h>

static int WIDTH = 640;
static int HEIGHT = 480;



void encode_fractal_noise_vertex(int frame, x264_picture_t pic_in);
void encode_polar_coordinate_color_cycling(int frame, x264_picture_t pic_in);
void encode_swirling_vortex(int frame, x264_picture_t pic_in);
void encode_fractal_noise_vertex2(int frame, x264_picture_t pic_in);

int main() {
    int fps = 5;
    int num_frames = 200;

    x264_t* encoder;
    x264_picture_t pic_in, pic_out;
    x264_param_t param;
    FILE* h264_file;

    x264_param_default_preset(&param, "veryfast", "zerolatency");
    param.i_csp = X264_CSP_I420;
    param.i_width = WIDTH;
    param.i_height = HEIGHT;
    param.i_fps_num = fps;
    param.i_fps_den = 1;
    param.i_keyint_max = fps;
    param.b_repeat_headers = 1;
    x264_param_apply_profile(&param, "baseline");

    encoder = x264_encoder_open(&param);
    h264_file = fopen("video.h264", "wb");

    x264_picture_alloc(&pic_in, X264_CSP_I420, WIDTH, HEIGHT);
    pic_in.i_type = X264_TYPE_AUTO;

    for (int i = 0; i < num_frames; i++) {
        //encode_fractal_noise_vertex(i, pic_in);
        //encode_swirling_vortex(i, pic_in);
        //encode_polar_coordinate_color_cycling(i, pic_in);
        encode_fractal_noise_vertex2(i, pic_in);

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

void encode_polar_coordinate_color_cycling(int frame, x264_picture_t pic_in) {
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                double angle = atan2(y - HEIGHT / 2, x - WIDTH / 2);
                double radius = sqrt(pow(x - WIDTH / 2, 2) + pow(y - HEIGHT / 2, 2));
                double intensity = 128 + 127 * sin(radius * 0.05 + angle * 3 + frame * 0.1);
                pic_in.img.plane[0][x + y * WIDTH] = (int)intensity;
            }
            if (y < HEIGHT / 2) {
                uint8_t* u_plane = pic_in.img.plane[1] + y * pic_in.img.i_stride[1];
                uint8_t* v_plane = pic_in.img.plane[2] + y * pic_in.img.i_stride[2];
                for (int x = 0; x < WIDTH / 2; x++) {
                    double angle = atan2(y - HEIGHT / 4, x - WIDTH / 4);
                    double radius = sqrt(pow(x - WIDTH / 4, 2) + pow(y - HEIGHT / 4, 2));
                    u_plane[x] = 128 + (int)(127 * sin(radius * 0.1 + frame * 0.05));
                    v_plane[x] = 128 + (int)(127 * cos(angle * 5 + frame * 0.03));
                }
            }
        }
}

void encode_fractal_noise_vertex(int frame, x264_picture_t pic_in) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            double angle = atan2(y - HEIGHT / 2, x - WIDTH / 2);
            double radius = sqrt(pow(x - WIDTH / 2, 2) + pow(y - HEIGHT / 2, 2));
            double phase = frame * 0.02;
            double frequency = 0.1 * (radius + 1);
            double intensity = 128 + 127 * sin(frequency * radius + 5 * angle + phase);
            intensity += 127 * sin(3 * frequency * (radius * 0.5) - 3 * angle + phase);
            intensity = intensity / 2;
            pic_in.img.plane[0][x + y * WIDTH] = (int)intensity;
        }
        if (y < HEIGHT / 2) {
            uint8_t* u_plane = pic_in.img.plane[1] + y * pic_in.img.i_stride[1];
            uint8_t* v_plane = pic_in.img.plane[2] + y * pic_in.img.i_stride[2];
            for (int x = 0; x < WIDTH / 2; x++) {
                double angle = atan2(y - HEIGHT / 4, x - WIDTH / 4);
                double radius = sqrt(pow(x - WIDTH / 4, 2) + pow(y - HEIGHT / 4, 2));
                double phase = frame * 0.03;
                double frequency = 0.15 * (radius + 1); // Higher frequency for color
                u_plane[x] = 128 + (int)(127 * cos(frequency * radius - 4 * angle + phase));
                v_plane[x] = 128 + (int)(127 * cos(frequency * radius + 4 * angle + phase));
            }
        }
    }
}

void encode_swirling_vortex(int frame, x264_picture_t pic_in) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            double angle = atan2(y - HEIGHT / 2, x - WIDTH / 2);
            double radius = sqrt(pow(x - WIDTH / 2, 2) + pow(y - HEIGHT / 2, 2));
            double wave = (radius * 0.05) + (frame * 0.15);
            double swirl = angle + wave;
            double intensity = 128 + 127 * sin(swirl * 2);
            pic_in.img.plane[0][x + y * WIDTH] = (int)intensity;
        }
        if (y < HEIGHT / 2) {
            uint8_t* u_plane = pic_in.img.plane[1] + y * pic_in.img.i_stride[1];
            uint8_t* v_plane = pic_in.img.plane[2] + y * pic_in.img.i_stride[2];
            for (int x = 0; x < WIDTH / 2; x++) {
                double angle = atan2(y - HEIGHT / 4, x - WIDTH / 4);
                double radius = sqrt(pow(x - WIDTH / 4, 2) + pow(y - HEIGHT / 4, 2));
                double wave = (radius * 0.1) + (frame * 0.1);
                double swirl = angle + wave;
                u_plane[x] = 128 + (int)(127 * sin(swirl * 3));
                v_plane[x] = 128 + (int)(127 * cos(swirl * 4));
            }
        }
    }
}


void encode_fractal_noise_vertex2(int frame, x264_picture_t pic_in) {
    const int max_radius_effect = 3;
    const double frequency_base = 0.04;
    const double phase_shift = frame * 0.02;
    const double color_shift = frame * 0.03;

     double (^modulate_intensity) (double, double, double) = ^double (double radius, double angle, double phase) {
        return 128 + 127 * sin(radius * frequency_base + max_radius_effect * angle + phase);
    };

    double (^complex_wave) (int, int, double) = ^(int x, int y, double phase_shift) {
        double local_radius = sqrt(x * x + y * y);
        double local_angle = atan2(y, x);
        return modulate_intensity(local_radius, local_angle, phase_shift);
    };

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int cx = x - WIDTH / 2;
            int cy = y - HEIGHT / 2;
            pic_in.img.plane[0][x + y * WIDTH] = (int)complex_wave(cx, cy, phase_shift);
        }
    }

    for (int y = 0; y < HEIGHT / 2; y++) {
        uint8_t* u_plane = pic_in.img.plane[1] + y * pic_in.img.i_stride[1];
        uint8_t* v_plane = pic_in.img.plane[2] + y * pic_in.img.i_stride[2];
        for (int x = 0; x < WIDTH / 2; x++) {
            int cx = x - WIDTH / 4;
            int cy = y - HEIGHT / 4;
            double u_wave = complex_wave(cx, cy, color_shift);
            double v_wave = complex_wave(-cx, -cy, -color_shift);
            u_plane[x] = 128 + (int)(127 * cos(u_wave));
            v_plane[x] = 128 + (int)(127 * sin(v_wave));
        }
    }
}
