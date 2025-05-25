#include "png_loader.hpp"

#define TAG "png_loader"

extern const uint8_t testpattern_start[] asm("_binary_testpattern_png_start");
extern const uint8_t testpattern_end[]   asm("_binary_testpattern_png_end");


typedef struct {
    const uint8_t *data;
    size_t size;
    size_t offset;
} PngMemoryReader;

void png_mem_read(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
    PngMemoryReader* reader = (PngMemoryReader*)png_get_io_ptr(png_ptr);
    if(reader->offset + byteCountToRead > reader->size) {
        png_error(png_ptr, "Read beyond end of buffer");
    }
    memcpy(outBytes, reader->data + reader->offset, byteCountToRead);
    reader->offset += byteCountToRead;
}

int load_png(const uint8_t *png_data, size_t png_size, png_bytep **out_rows, int *out_width, int *out_height, int *out_channels) {
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(!png_ptr) return -1;

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if(!info_ptr) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return -2;
    }

    if(setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return -3;
    }

    PngMemoryReader reader = {png_data, png_size, 0};
    png_set_read_fn(png_ptr, &reader, png_mem_read);

    png_read_info(png_ptr, info_ptr);

    int width = png_get_image_width(png_ptr, info_ptr);
    int height = png_get_image_height(png_ptr, info_ptr);
    png_byte color_type = png_get_color_type(png_ptr, info_ptr);
    png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    // Convert palette images to RGB
    if(color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);

    // Expand grayscale images to 8 bits
    if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png_ptr);

    // Add alpha channel if transparency present
    if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png_ptr);

    // Convert grayscale to RGB
    if(color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png_ptr);

    // Strip 16-bit to 8-bit per channel
    if(bit_depth == 16)
        png_set_strip_16(png_ptr);

    png_read_update_info(png_ptr, info_ptr);

    int channels = png_get_channels(png_ptr, info_ptr);

    png_bytep *rows = (png_bytep*)malloc(sizeof(png_bytep) * height);
    if(!rows) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return -4;
    }

    for(int y=0; y<height; y++) {
        rows[y] = (png_byte*)malloc(png_get_rowbytes(png_ptr, info_ptr));
        if(!rows[y]) {
            for(int j=0; j<y; j++) free(rows[j]);
            free(rows);
            png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
            return -5;
        }
    }

    png_read_image(png_ptr, rows);

    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

    *out_rows = rows;
    *out_width = width;
    *out_height = height;
    *out_channels = channels;

    return 0;
}


void free_png_rows(png_bytep *rows, int height) {
    for(int y=0; y<height; y++) {
        free(rows[y]);
    }
    free(rows);
}

void testPNG() {
    png_image_led p = png_image_led(testpattern_start,testpattern_end);
    uint8_t r,g,b;
    p.readPixel(2,2,r,g,b);

    ESP_LOGI(TAG,"read pixel values %i %i %i",r,g,b);
}

png_image_led::png_image_led(const uint8_t *png_data_start, const uint8_t *png_data_end) : png_data_start(png_data_start), png_data_end(png_data_end), rows(nullptr), width(0), height(0), channels(0), loaded(0)
{
    png_size = png_data_start - png_data_end;

    int ret = load_png(png_data_start, png_size, &rows, &width, &height, &channels);
    if(ret != 0){
        ESP_LOGE(TAG,"Failed to load PNG %i", ret);
        return;
    }
    loaded = 1;
}

esp_err_t png_image_led::readPixel(uint32_t x, uint32_t y, uint8_t &r, uint8_t &g, uint8_t &b){
    if(!loaded){
        ESP_LOGE(TAG,"Image not loaded!");
        return ESP_FAIL;
    }
    
    if(x >= width || y >= height) {
        ESP_LOGE(TAG,"Pixel coordinate (%i,%i) out of bounds (%i,%i)\n", (int)x, (int)y, (int)width, (int)height);
        return ESP_ERR_INVALID_SIZE;
    }
    
    png_bytep pixel = &(rows[y][x * channels]);

    r = pixel[0];
    g = pixel[1];
    b = pixel[2];

    return ESP_OK;
}

int png_image_led::getWidth(){
    return width;
}

int png_image_led::getHeight(){
    return height;
}

png_image_led::~png_image_led(){
    free_png_rows(rows, height);
}