#include <kinc/graphics4/graphics.h>
#include <kinc/graphics4/indexbuffer.h>
#include <kinc/graphics4/pipeline.h>
#include <kinc/graphics4/shader.h>
#include <kinc/graphics4/vertexbuffer.h>
#include <kinc/io/filereader.h>
#include <kinc/system.h>

#include <assert.h>
#include <stdlib.h>

static kinc_g4_shader_t vertex_shader;
static kinc_g4_shader_t fragment_shader;
static kinc_g4_pipeline_t pipeline;
static kinc_g4_vertex_buffer_t vertices;
static kinc_g4_vertex_buffer_t vertices_inst;
static kinc_g4_index_buffer_t indices;

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

	kinc_g4_set_pipeline(&pipeline);
	kinc_g4_vertex_buffer_t *buffers[] = {&vertices, &vertices_inst};
	kinc_g4_set_vertex_buffers(buffers, 2);
	kinc_g4_set_index_buffer(&indices);
	kinc_g4_draw_indexed_vertices_instanced(4);

	kinc_g4_end(0);
	kinc_g4_swap_buffers();
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
	kinc_g4_vertex_structure_t structure_inst;
	kinc_g4_vertex_structure_init(&structure_inst);
	kinc_g4_vertex_structure_add(&structure_inst, "off", KINC_G4_VERTEX_DATA_F32_3X);
	structure_inst.instanced = true;
	kinc_g4_pipeline_init(&pipeline);
	pipeline.vertex_shader = &vertex_shader;
	pipeline.fragment_shader = &fragment_shader;
	pipeline.input_layout[0] = &structure;
	pipeline.input_layout[1] = &structure_inst;
	pipeline.input_layout[2] = NULL;
	kinc_g4_pipeline_compile(&pipeline);

	kinc_g4_vertex_buffer_init(&vertices, 3, &structure, KINC_G4_USAGE_STATIC, 0);
	{
		float *v = kinc_g4_vertex_buffer_lock_all(&vertices);
		int i = 0;

		v[i++] = -0.5;
		v[i++] = -0.5;
		v[i++] = 0.0;

		v[i++] = 0.5;
		v[i++] = -0.5;
		v[i++] = 0.0;

		v[i++] = 0.0;
		v[i++] = 0.5;
		v[i++] = 0.0;

		kinc_g4_vertex_buffer_unlock_all(&vertices);
	}

	kinc_g4_vertex_buffer_init(&vertices_inst, 4, &structure_inst, KINC_G4_USAGE_STATIC, 1);
	{
		float *v = kinc_g4_vertex_buffer_lock_all(&vertices_inst);
		int i = 0;

		v[i++] = -0.5;
		v[i++] = -0.5;
		v[i++] = 0.0;

		v[i++] = 0.5;
		v[i++] = -0.5;
		v[i++] = 0.0;

		v[i++] = -0.5;
		v[i++] = 0.5;
		v[i++] = 0.0;

		v[i++] = 0.5;
		v[i++] = 0.5;
		v[i++] = 0.0;

		kinc_g4_vertex_buffer_unlock_all(&vertices_inst);
	}

	kinc_g4_index_buffer_init(&indices, 3, KINC_G4_INDEX_BUFFER_FORMAT_16BIT, KINC_G4_USAGE_STATIC);
	{
		uint16_t *i = (uint16_t *)kinc_g4_index_buffer_lock_all(&indices);
		i[0] = 0;
		i[1] = 1;
		i[2] = 2;
		kinc_g4_index_buffer_unlock_all(&indices);
	}

	kinc_start();

	return 0;
}
