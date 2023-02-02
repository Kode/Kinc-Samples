#include <kinc/graphics4/graphics.h>
#include <kinc/io/filereader.h>
#include <kinc/system.h>

#include <assert.h>
#include <stdlib.h>

#define HEAP_SIZE 1024 * 1024
static uint8_t *heap = NULL;
static size_t heap_top = 0;

static void *allocate(size_t size) {
	size_t old_top = heap_top;
	heap_top += size;
	assert(heap_top <= HEAP_SIZE);
	return &heap[old_top];
}

static void update(void *data) {
	kinc_g4_begin(0);
	kinc_g4_clear(KINC_G4_CLEAR_COLOR, 0, 0.0f, 0);
	kinc_g4_end(0);
	kinc_g4_swap_buffers();

	kinc_g4_scissor(0, 0, 0, 0);
	kinc_g4_disable_scissor();
	kinc_g4_constant_location_t location;
	kinc_g4_set_matrix4(location, NULL);
	kinc_g4_set_matrix3(location, NULL);
	kinc_g4_set_bool(location, false);
	kinc_g4_set_floats(location, NULL, 0);
	kinc_g4_set_float4(location, 0, 0, 0, 0);
	kinc_g4_set_float3(location, 0, 0, 0);
	kinc_g4_set_float2(location, 0, 0);
	kinc_g4_set_float(location, 0);
	kinc_g4_set_ints(location, NULL, 0);
	kinc_g4_set_int4(location, 0, 0, 0, 0);
	kinc_g4_set_int3(location, 0, 0, 0);
	kinc_g4_set_int2(location, 0, 0);
	kinc_g4_set_int(location, 0);
	kinc_g4_texture_unit_t unit;
	kinc_g4_set_texture_compare_mode(unit, false);
	kinc_g4_set_cubemap_compare_mode(unit, false);
	kinc_g4_set_texture_addressing(unit, KINC_G4_TEXTURE_DIRECTION_U, KINC_G4_TEXTURE_ADDRESSING_REPEAT);
	kinc_g4_set_texture_mipmap_filter(unit, KINC_G4_MIPMAP_FILTER_LINEAR);
	kinc_g4_set_texture_minification_filter(unit, KINC_G4_TEXTURE_FILTER_LINEAR);
	kinc_g4_set_texture_magnification_filter(unit, KINC_G4_TEXTURE_FILTER_LINEAR);
}

int kickstart(int argc, char **argv) {
	kinc_init("Example", 1024, 768, NULL, NULL);
	kinc_set_update_callback(update, NULL);

	heap = (uint8_t *)malloc(HEAP_SIZE);
	assert(heap != NULL);

	kinc_start();

	return 0;
}
