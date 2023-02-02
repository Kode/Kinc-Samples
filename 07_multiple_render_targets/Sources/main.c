#include <kinc/graphics1/graphics.h>
#include <kinc/graphics4/graphics.h>
#include <kinc/graphics4/indexbuffer.h>
#include <kinc/graphics4/pipeline.h>
#include <kinc/graphics4/shader.h>
#include <kinc/graphics4/vertexbuffer.h>
#include <kinc/graphics4/rendertarget.h>
#include <kinc/io/filereader.h>
#include <kinc/system.h>

#include <assert.h>
#include <stdlib.h>

static kinc_g4_shader_t vertex_shader;
static kinc_g4_shader_t fragment_shader;
static kinc_g4_pipeline_t pipeline;
static kinc_g4_vertex_buffer_t vertices;
static kinc_g4_index_buffer_t indices;
static kinc_g4_render_target_t target0;
static kinc_g4_render_target_t target1;
static kinc_g4_render_target_t target2;
static kinc_g4_render_target_t target3;
extern uint32_t *kinc_internal_g1_image;
static uint8_t pixels0[512 * 384 * 4];
static uint8_t pixels1[512 * 384 * 4];
static uint8_t pixels2[512 * 384 * 4];
static uint8_t pixels3[512 * 384 * 4];

#define HEAP_SIZE 1024 * 1024
static uint8_t *heap = NULL;
static size_t heap_top = 0;

static void *allocate(size_t size) {
	size_t old_top = heap_top;
	heap_top += size;
	assert(heap_top <= HEAP_SIZE);
	return &heap[old_top];
}

static void draw_image(uint8_t *pixels, int x, int y) {
	uint8_t *g1image = (uint8_t *)kinc_internal_g1_image;
	for (int i = 0; i < 512 * 384; ++i) {
		int xpos = i % 512 + x;
		int ypos = i / 512 + y;
		// if (kinc_g4_render_targets_inverted_y()) {}
		int j = ypos * 1024 + xpos;
		g1image[j * 4] = pixels[i * 4];
		g1image[j * 4 + 1] = pixels[i * 4 + 1];
		g1image[j * 4 + 2] = pixels[i * 4 + 2];
		g1image[j * 4 + 3] = pixels[i * 4 + 3];
	}
}

static void update(void *data) {
	kinc_g4_render_target_t *targets[] = { &target0, &target1, &target2, &target3 };
	kinc_g4_set_render_targets(targets, 4);
	kinc_g4_clear(KINC_G4_CLEAR_COLOR, 0, 0.0f, 0);
	kinc_g4_set_pipeline(&pipeline);
	kinc_g4_set_vertex_buffer(&vertices);
	kinc_g4_set_index_buffer(&indices);
	kinc_g4_draw_indexed_vertices();
	kinc_g4_restore_render_target();

	kinc_g4_render_target_get_pixels(&target0, pixels0);
	kinc_g4_render_target_get_pixels(&target1, pixels1);
	kinc_g4_render_target_get_pixels(&target2, pixels2);
	kinc_g4_render_target_get_pixels(&target3, pixels3);

	kinc_g1_begin();
	draw_image(pixels0, 0, 0);
	draw_image(pixels1, 512, 0);
	draw_image(pixels2, 0, 384);
	draw_image(pixels3, 512, 384);
	kinc_g1_end();
}

static void load_shader(const char *filename, kinc_g4_shader_t *shader, kinc_g4_shader_type_t shader_type) {
	kinc_file_reader_t file;
	kinc_file_reader_open(&file, filename, KINC_FILE_TYPE_ASSET);
	size_t data_size = kinc_file_reader_size(&file);
	uint8_t *data = allocate(data_size);
	kinc_file_reader_read(&file, data, data_size);
	kinc_file_reader_close(&file);
	kinc_g4_shader_init(shader, data, data_size, shader_type);
}

int kickstart(int argc, char **argv) {
	kinc_init("Example", 1024, 768, NULL, NULL);
	kinc_set_update_callback(update, NULL);

	heap = (uint8_t *)malloc(HEAP_SIZE);
	assert(heap != NULL);

	load_shader("shader.vert", &vertex_shader, KINC_G4_SHADER_TYPE_VERTEX);
	load_shader("shader.frag", &fragment_shader, KINC_G4_SHADER_TYPE_FRAGMENT);

	kinc_g4_vertex_structure_t structure;
	kinc_g4_vertex_structure_init(&structure);
	kinc_g4_vertex_structure_add(&structure, "pos", KINC_G4_VERTEX_DATA_F32_3X);
	kinc_g4_pipeline_init(&pipeline);
	pipeline.vertex_shader = &vertex_shader;
	pipeline.fragment_shader = &fragment_shader;
	pipeline.input_layout[0] = &structure;
	pipeline.input_layout[1] = NULL;
	pipeline.color_attachment_count = 4;
	pipeline.color_attachment[0] = KINC_G4_RENDER_TARGET_FORMAT_32BIT;
	pipeline.color_attachment[1] = KINC_G4_RENDER_TARGET_FORMAT_32BIT;
	pipeline.color_attachment[2] = KINC_G4_RENDER_TARGET_FORMAT_32BIT;
	pipeline.color_attachment[3] = KINC_G4_RENDER_TARGET_FORMAT_32BIT;
	kinc_g4_pipeline_compile(&pipeline);

	kinc_g4_render_target_init(&target0, 512, 384, KINC_G4_RENDER_TARGET_FORMAT_32BIT, 0, 0);
	kinc_g4_render_target_init(&target1, 512, 384, KINC_G4_RENDER_TARGET_FORMAT_32BIT, 0, 0);
	kinc_g4_render_target_init(&target2, 512, 384, KINC_G4_RENDER_TARGET_FORMAT_32BIT, 0, 0);
	kinc_g4_render_target_init(&target3, 512, 384, KINC_G4_RENDER_TARGET_FORMAT_32BIT, 0, 0);
	kinc_g1_init(1024, 768);

	kinc_g4_vertex_buffer_init(&vertices, 3, &structure, KINC_G4_USAGE_STATIC, 0);
	{
		float *v = kinc_g4_vertex_buffer_lock_all(&vertices);
		int i = 0;

		v[i++] = -1.0;
		v[i++] = -1.0;
		v[i++] = 0.0;

		v[i++] = 1.0;
		v[i++] = -1.0;
		v[i++] = 0.0;

		v[i++] = 0.0;
		v[i++] = 1.0;
		v[i++] = 0.0;

		kinc_g4_vertex_buffer_unlock_all(&vertices);
	}

	kinc_g4_index_buffer_init(&indices, 3, KINC_G4_INDEX_BUFFER_FORMAT_16BIT, KINC_G4_USAGE_STATIC);
	{
		uint16_t *i = (uint16_t *)kinc_g4_index_buffer_lock(&indices);
		i[0] = 0;
		i[1] = 1;
		i[2] = 2;
		kinc_g4_index_buffer_unlock(&indices);
	}

	kinc_start();

	return 0;
}
