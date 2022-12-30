#include <kinc/graphics4/graphics.h>
#include <kinc/graphics4/indexbuffer.h>
#include <kinc/graphics4/pipeline.h>
#include <kinc/graphics4/shader.h>
#include <kinc/graphics4/texturearray.h>
#include <kinc/graphics4/vertexbuffer.h>
#include <kinc/io/filereader.h>
#include <kinc/system.h>

#include <assert.h>

static kinc_g4_shader_t vertexShader;
static kinc_g4_shader_t fragmentShader;
static kinc_g4_pipeline_t pipeline;
static kinc_g4_vertex_buffer_t vertices;
static kinc_g4_index_buffer_t indices;
static kinc_g4_texture_array_t texture;
static kinc_g4_texture_unit_t texunit;
static kinc_g4_constant_location_t offset;

#define HEAP_SIZE 1024 * 1024
static uint8_t *heap = NULL;
static size_t heap_top = 0;

static void *allocate(size_t size) {
	size_t old_top = heap_top;
	heap_top += size;
	assert(heap_top <= HEAP_SIZE);
	return &heap[old_top];
}

static void update(void) {
	kinc_g4_begin(0);
	kinc_g4_clear(KINC_G4_CLEAR_COLOR, 0, 0.0f, 0);

	kinc_g4_set_pipeline(&pipeline);
	kinc_matrix3x3_t matrix = kinc_matrix3x3_rotation_z((float)kinc_time());
	kinc_g4_set_matrix3(offset, &matrix);
	kinc_g4_set_vertex_buffer(&vertices);
	kinc_g4_set_index_buffer(&indices);
	kinc_g4_set_texture_array(texunit, &texture);
	kinc_g4_draw_indexed_vertices();

	kinc_g4_end(0);
	kinc_g4_swap_buffers();
}

int kickstart(int argc, char **argv) {
	kinc_init("TextureTest", 1024, 768, NULL, NULL);
	kinc_set_update_callback(update);

	heap = (uint8_t *)malloc(HEAP_SIZE);
	assert(heap != NULL);

	kinc_image_t textures[2];
	{
		void *image_mem = allocate(250 * 250 * 4);
		kinc_image_init_from_file(&textures[0], image_mem, "parrot.png");
	}

	{
		void *image_mem = allocate(250 * 250 * 4);
		kinc_image_init_from_file(&textures[1], image_mem, "parrot2.png");
	}

	kinc_g4_texture_array_init(&texture, textures, 2);

	kinc_image_destroy(&textures[0]);
	kinc_image_destroy(&textures[1]);

	{
		kinc_file_reader_t reader;
		kinc_file_reader_open(&reader, "texture.vert", KINC_FILE_TYPE_ASSET);
		size_t size = kinc_file_reader_size(&reader);
		uint8_t *data = allocate(size);
		kinc_file_reader_read(&reader, data, size);
		kinc_file_reader_close(&reader);

		kinc_g4_shader_init(&vertexShader, data, size, KINC_G4_SHADER_TYPE_VERTEX);
	}

	{
		kinc_file_reader_t reader;
		kinc_file_reader_open(&reader, "texture.frag", KINC_FILE_TYPE_ASSET);
		size_t size = kinc_file_reader_size(&reader);
		uint8_t *data = allocate(size);
		kinc_file_reader_read(&reader, data, size);
		kinc_file_reader_close(&reader);

		kinc_g4_shader_init(&fragmentShader, data, size, KINC_G4_SHADER_TYPE_FRAGMENT);
	}

	kinc_g4_vertex_structure_t structure;
	kinc_g4_vertex_structure_init(&structure);
	kinc_g4_vertex_structure_add(&structure, "pos", KINC_G4_VERTEX_DATA_FLOAT3);
	kinc_g4_vertex_structure_add(&structure, "tex", KINC_G4_VERTEX_DATA_FLOAT2);
	kinc_g4_pipeline_init(&pipeline);
	pipeline.input_layout[0] = &structure;
	pipeline.input_layout[1] = NULL;
	pipeline.vertex_shader = &vertexShader;
	pipeline.fragment_shader = &fragmentShader;
	kinc_g4_pipeline_compile(&pipeline);

	texunit = kinc_g4_pipeline_get_texture_unit(&pipeline, "texsampler");
	offset = kinc_g4_pipeline_get_constant_location(&pipeline, "mvp");

	kinc_g4_vertex_buffer_init(&vertices, 3, &structure, KINC_G4_USAGE_STATIC, 0);
	float *v = kinc_g4_vertex_buffer_lock_all(&vertices);
	v[0] = -1.0f;
	v[1] = -1.0f;
	v[2] = 0.5f;
	v[3] = 0.0f;
	v[4] = 1.0f;
	v[5] = 1.0f;
	v[6] = -1.0f;
	v[7] = 0.5f;
	v[8] = 1.0f;
	v[9] = 1.0f;
	v[10] = -1.0f;
	v[11] = 1.0f;
	v[12] = 0.5f;
	v[13] = 0.0f;
	v[14] = 0.0f;
	kinc_g4_vertex_buffer_unlock_all(&vertices);

	kinc_g4_index_buffer_init(&indices, 3, KINC_G4_INDEX_BUFFER_FORMAT_32BIT, KINC_G4_USAGE_STATIC);
	int *i = kinc_g4_index_buffer_lock(&indices);
	i[0] = 0;
	i[1] = 1;
	i[2] = 2;
	kinc_g4_index_buffer_unlock(&indices);

	kinc_start();

	return 0;
}
