#include <kinc/graphics4/graphics.h>
#include <kinc/graphics4/indexbuffer.h>
#include <kinc/graphics4/pipeline.h>
#include <kinc/graphics4/shader.h>
#include <kinc/graphics4/vertexbuffer.h>
#include <kinc/io/filereader.h>
#include <kinc/system.h>
#include <kinc/log.h>

#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

static kinc_g4_shader_t vertex_shader;
static kinc_g4_shader_t fragment_shader;
static kinc_g4_pipeline_t pipeline;
static kinc_g4_vertex_buffer_t vertices;
static kinc_g4_index_buffer_t indices;
static kinc_g4_constant_location_t mvp_loc;

#define HEAP_SIZE 1024 * 1024
static uint8_t *heap = NULL;
static size_t heap_top = 0;

static void *allocate(size_t size) {
	size_t old_top = heap_top;
	heap_top += size;
	assert(heap_top <= HEAP_SIZE);
	return &heap[old_top];
}

float vec4_length(kinc_vector3_t a) {
	return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

kinc_vector3_t vec4_normalize(kinc_vector3_t a) {
	float n = vec4_length(a);
	if (n > 0.0) {
		float inv_n = 1.0 / n;
		a.x *= inv_n;
		a.y *= inv_n;
		a.z *= inv_n;
	}
	return a;
}

kinc_vector3_t vec4_sub(kinc_vector3_t a, kinc_vector3_t b) {
	kinc_vector3_t v;
	v.x = a.x - b.x;
	v.y = a.y - b.y;
	v.z = a.z - b.z;
	return v;
}

kinc_vector3_t vec4_cross(kinc_vector3_t a, kinc_vector3_t b) {
	kinc_vector3_t v;
	v.x = a.y * b.z - a.z * b.y;
	v.y = a.z * b.x - a.x * b.z;
	v.z = a.x * b.y - a.y * b.x;
	return v;
}

float vec4_dot(kinc_vector3_t a, kinc_vector3_t b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

kinc_matrix4x4_t matrix4x4_perspective_projection(float fovy, float aspect, float zn, float zf) {
	float uh = 1.0 / tan(fovy / 2);
	float uw = uh / aspect;
	kinc_matrix4x4_t m = {
		uw, 0, 0, 0,
		0, uh, 0, 0,
		0, 0, (zf + zn) / (zn - zf), -1,
		0, 0, 2 * zf * zn / (zn - zf), 0
	};
	return m;
}

kinc_matrix4x4_t matrix4x4_look_at(kinc_vector3_t eye, kinc_vector3_t at, kinc_vector3_t up) {
	kinc_vector3_t zaxis = vec4_normalize(vec4_sub(at, eye));
	kinc_vector3_t xaxis = vec4_normalize(vec4_cross(zaxis, up));
	kinc_vector3_t yaxis = vec4_cross(xaxis, zaxis);
	kinc_matrix4x4_t m = {
		xaxis.x, yaxis.x, -zaxis.x, 0,
		xaxis.y, yaxis.y, -zaxis.y, 0,
		xaxis.z, yaxis.z, -zaxis.z, 0,
		-vec4_dot(xaxis, eye), -vec4_dot(yaxis, eye), vec4_dot(zaxis, eye), 1
	};
	return m;
}

kinc_matrix4x4_t matrix4x4_identity(void) {
	kinc_matrix4x4_t m;
	memset(m.m, 0, sizeof(m.m));
	for (unsigned x = 0; x < 4; ++x) {
		kinc_matrix4x4_set(&m, x, x, 1.0f);
	}
	return m;
}

static void update(void *data) {
	kinc_matrix4x4_t projection = matrix4x4_perspective_projection(45.0, 4.0 / 3.0, 0.1, 100.0);
	kinc_vector3_t v0 = { 4, 3, 3 };
	kinc_vector3_t v1 = { 0, 0, 0 };
	kinc_vector3_t v2 = { 0, 1, 0 };
	kinc_matrix4x4_t view = matrix4x4_look_at(v0, v1, v2);
	kinc_matrix4x4_t model = matrix4x4_identity();
	kinc_matrix4x4_t mvp = matrix4x4_identity();
	mvp = kinc_matrix4x4_multiply(&mvp, &projection);
	mvp = kinc_matrix4x4_multiply(&mvp, &view);
	mvp = kinc_matrix4x4_multiply(&mvp, &model);

	kinc_g4_begin(0);
	kinc_g4_clear(KINC_G4_CLEAR_COLOR, 0xff000044, 0.0f, 0);
	kinc_g4_set_pipeline(&pipeline);
	kinc_g4_set_matrix4(mvp_loc, &mvp);
	kinc_g4_set_vertex_buffer(&vertices);
	kinc_g4_set_index_buffer(&indices);
	kinc_g4_draw_indexed_vertices();

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
	kinc_g4_pipeline_init(&pipeline);
	pipeline.vertex_shader = &vertex_shader;
	pipeline.fragment_shader = &fragment_shader;
	pipeline.input_layout[0] = &structure;
	pipeline.input_layout[1] = NULL;
	kinc_g4_pipeline_compile(&pipeline);

	mvp_loc = kinc_g4_pipeline_get_constant_location(&pipeline, "mvp");

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
