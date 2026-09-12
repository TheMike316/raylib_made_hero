#include <cstdint>
#include <cstdio>
#include <cstring>
#include <raylib.h>
#include <sys/mman.h>

// static sucks as a keyword
#define local_persist static
#define global_variable static
#define internal static

struct ray_offscreen_buffer {
    void *memory;
    int mem_size;
    int width;
    int height;
    int pitch;
    int bytes_per_pixel;
};

struct ray_window_dimensions {
    int width;
    int height;
};

// these are global for now; will change in the future
global_variable ray_offscreen_buffer global_backbuffer = {};

internal ray_window_dimensions
get_window_dimensions() {
    // TODO get render width does not seem to return actual window stuff
    const int width = GetScreenWidth();
    // const int width = GetRenderWidth();
    const int height = GetScreenHeight();
    // const int height = GetRenderHeight();
    return {width, height};
}

internal void
RenderWeirdGradient(ray_offscreen_buffer buffer, int x_offset, int y_offset) {
    // todo decide whether to pass by ref or val

    uint8_t *row = (uint8_t *) buffer.memory;
    for (int y = 0; y < buffer.height; ++y) {
        uint32_t *pixel = (uint32_t *)row;
        for (int x = 0; x < buffer.width; ++x) {

            const uint8_t blue = x + x_offset;
            const uint8_t green = y + y_offset;
            constexpr uint8_t alpha = 0xff;

            *pixel++ = alpha << 24 | blue << 16 | green << 8;

        }
        row += buffer.pitch;
    }
}

internal void
RayResizeDIBSection(ray_offscreen_buffer *buffer, int width, int height) {
    // TODO bulletproof

    if (buffer->memory) {
        // TODO handle errors
        munmap(buffer->memory, buffer->mem_size);
    }

    buffer->width = width;
    buffer->height = height;

    buffer->bytes_per_pixel = 4; // TODO maybe constant
    buffer->pitch = buffer->width * buffer->bytes_per_pixel;

    buffer->mem_size = buffer->width * buffer->height * buffer->bytes_per_pixel;
    // TODO handle errors
    buffer->memory = mmap(nullptr, buffer->mem_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    memset(buffer->memory, 0, buffer->mem_size);
    if (buffer->memory == MAP_FAILED) {
        perror("mmap");
    }
}

internal void
RayDisplayBufferInWindow(ray_offscreen_buffer buffer, int width, int height) {
    const Image raw_image = {
        .data = buffer.memory,
        .width = width,
        .height = height,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
    };

    // TODO can i avoid loading and unloadig textures every single frame?
    const Texture2D tex = LoadTextureFromImage(raw_image);

    DrawTexture(tex, 0, 0, WHITE);
}

int main() {

    InitWindow(1200, 720, "Handmade Hero");

    RayResizeDIBSection(&global_backbuffer, 1200, 720);

    int x_offset = 0;
    int y_offset = 0;
    while (!WindowShouldClose()) {

        RenderWeirdGradient(global_backbuffer, x_offset, y_offset);
        ray_window_dimensions dimensions = get_window_dimensions();

        BeginDrawing();
        // TODO stretch
        // RayDisplayBufferInWindow(global_backbuffer, 1200, 720);
        RayDisplayBufferInWindow(global_backbuffer, dimensions.width, dimensions.height);
        EndDrawing();

        ++x_offset;
        y_offset += 2;
    }
    return 0;
}
