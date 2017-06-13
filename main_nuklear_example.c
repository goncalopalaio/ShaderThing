#include <stdio.h>

#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>


#define STB_IMAGE_IMPLEMENTATION
#include "external_stb_image.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#include "external_nuklear.h"
#include "external_nuklear_glfw_gl3.h"

#include "g_shaders.h"
#include "g_textures.h"
#include "g_models.h"

#define WINDOW_W 400
#define WINDOW_H 400
#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

void glfw_window_close_callback(GLFWwindow * window);
void glfw_window_size_callback(GLFWwindow * window, int width, int height);
void glfw_cursor_pos_callback(GLFWwindow * window, double mx, double my);
void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void glfw_mouse_button_callback(GLFWwindow * window, int button, int action, int mods);
void glfw_error_callback(int error, const char* description);


typedef struct Keys {
	int change_slide;
	int right;
	int left;
	int up;
	int down;
}Keys;

Keys key_state;


int main(int argc, char const *argv[]) {
	printf("\n\n\nLaunching ShaderThing\nPress any key to change image.\nEsc or Q to quit.\n\n\n");


	// Create window
    GLFWwindow* window;

    // Set error callback early
    glfwSetErrorCallback(glfw_error_callback);
    if(!glfwInit()) {
        printf("glfwInit Failed\n");
    } else {
        printf("glfwInit Success\n");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(WINDOW_W, WINDOW_H, "ShaderThing", NULL, NULL);
    if(!window) {
        printf("glfwCreateWindow Failed\n");
        glfwTerminate();
        return 1;
    }else {
    	printf("glfwCreateWindow Success\n");
    }
	
	printf("Setting callbacks\n");
    glfwSetWindowCloseCallback(window, glfw_window_close_callback);
    glfwSetWindowSizeCallback(window, glfw_window_size_callback);
    glfwSetKeyCallback(window, glfw_key_callback);
    glfwSetMouseButtonCallback(window, glfw_mouse_button_callback);
    glfwSetCursorPosCallback(window, glfw_cursor_pos_callback);
    
    
    printf("Setting context\n");
    glfwMakeContextCurrent(window);

    // @note is this required?
	//if (glewInit() != GLEW_OK) {
	//    printf("Failed to setup GLEW\n");
	//    return 1;
	//}





	////

    float aspect = WINDOW_W / (float)WINDOW_H;
    

	GLuint program = load_shader_single_file("shaders/shader.glsl", "position", "color", "uv");

	if (!program){
		glfwSetWindowShouldClose(window, GL_TRUE);
		printf("Error loading shader file\n");
		return 1;
	}

	// Set initial key states
	key_state.up = 0;
	key_state.down = 0;
	key_state.left = 0;
	key_state.right = 0;
	key_state.change_slide = 0;

	// Load textures

	int number_loaded_textures = 4;
	GLuint textures[number_loaded_textures];
	load_gl_texture("images/0.png", &textures[0]);
	load_gl_texture("images/1.png", &textures[1]);
	load_gl_texture("images/2.png", &textures[2]);
	load_gl_texture("images/test.png", &textures[3]);



    glUseProgram(program);
    GLuint uniform_loc_frame_count = glGetUniformLocation(program, "frame_time");
	GLuint uniform_loc_transition = glGetUniformLocation(program, "transition");
  	GLuint uniform_loc_tex0 = glGetUniformLocation(program, "tex0");

  	// Coordinates are defined so they fill the viewport (no camera is setup)
	// Upload data
  	int data_n_vertices = 6;
	GLfloat data_vertices[] = {
		-1,	-1,	0,
		 1,	-1,	0,
		-1,	 1,	0,
		-1,	 1,	0,
		 1,	-1,	0,
		 1,	 1,	0
	};

	GLfloat data_normals[] = {
		0,0,0,
		0,1,0,
		1,1,0,
		1,1,1,
		0,0,1,
		1,1,1
	};

	GLfloat data_tex_coords[] = {
		0,0,
		1,0,
		0,-1,
		0,-1,
		1,0,
		1,-1
	};

	GLuint vao;
	load_gl_data_to_vao(&vao, data_n_vertices, data_vertices, 3, data_normals, 3, data_tex_coords, 2);


	// Setup nuklear
    struct nk_context* ctx = nk_glfw3_init(window, NK_GLFW3_INSTALL_CALLBACKS);

	struct nk_font_atlas *atlas;
	nk_glfw3_font_stash_begin(&atlas);
	struct nk_font *font = nk_font_atlas_add_from_file(atlas, "fonts/kenvector_future_thin.ttf", 13, 0);
	nk_glfw3_font_stash_end();
	static struct nk_color background = {130, 50, 50, 255};
	//set_style(ctx, THEME_WHITE);
    nk_init_default(ctx, &font->handle);






	float frame_time = 0;
	float frame_time_speed = 0.01;
	int current_index = 0;
	int frame_time_since_last_slide = -1;

  	glEnable(GL_DEPTH_TEST);
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
    while(!glfwWindowShouldClose(window)) {
    	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    	if (key_state.change_slide) {
    		if ((frame_time - frame_time_since_last_slide) > 0.1) {
    			current_index = (current_index+1) % number_loaded_textures;
    			frame_time_since_last_slide = frame_time;
    		}
    		
    		// Force key state (probably should be done elsewhere)
    		key_state.change_slide = 0;
    	}

    	glUseProgram(program);
    	glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[current_index]);
		glUniform1i(uniform_loc_tex0, 0);
		glUniform1f(uniform_loc_frame_count, frame_time);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, data_n_vertices);
		glUseProgram(0);


        nk_glfw3_new_frame();

        /* GUI */
        if (nk_begin(ctx, "Demo", nk_rect(50, 50, 230, 250),
            NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
            NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
        {
            enum {EASY, HARD};
            static int op = EASY;
            
            nk_layout_row_static(ctx, 30, 80, 1);
            if (nk_button_label(ctx, "button"))
                fprintf(stdout, "button pressed\n");

            nk_layout_row_dynamic(ctx, 30, 2);
            if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
            if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;

            nk_layout_row_dynamic(ctx, 25, 1);
            nk_property_float(ctx, "Frame time speed:", 0, &frame_time_speed, 10, 0.1, 0.1);

            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label(ctx, "background:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            if (nk_combo_begin_color(ctx, background, nk_vec2(nk_widget_width(ctx),400))) {
                nk_layout_row_dynamic(ctx, 120, 1);
                background = nk_color_picker(ctx, background, NK_RGBA);
                nk_layout_row_dynamic(ctx, 25, 1);
                background.r = (nk_byte)nk_propertyi(ctx, "#R:", 0, background.r, 255, 1,1);
                background.g = (nk_byte)nk_propertyi(ctx, "#G:", 0, background.g, 255, 1,1);
                background.b = (nk_byte)nk_propertyi(ctx, "#B:", 0, background.b, 255, 1,1);
                background.a = (nk_byte)nk_propertyi(ctx, "#A:", 0, background.a, 255, 1,1);
                nk_combo_end(ctx);
            }
        }
        nk_end(ctx);

        /* -------------- EXAMPLES ---------------- */
        /*calculator(ctx);*/
        /*overview(ctx);*/
        /*node_editor(ctx);*/
        /* ----------------------------------------- */



        nk_glfw3_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);


		glfwSwapBuffers(window);
        glfwPollEvents();
        frame_time+=(frame_time_speed);
    }

	nk_glfw3_shutdown();    
	glfwMakeContextCurrent(NULL);
    glfwDestroyWindow(window);
	glfwTerminate();

	printf("Slide_Show finished\n");
	return 0;
}

void glfw_window_close_callback(GLFWwindow * window) {

}

void glfw_window_size_callback(GLFWwindow * window, int width, int height) {

}

void glfw_cursor_pos_callback(GLFWwindow * window, double mx, double my) {

}

void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	//int new_state = (action == GLFW_PRESS || action == GLFW_REPEAT) ? 1 : 0;
	int new_state = (action == GLFW_PRESS);
    switch(key) {
        case GLFW_KEY_RIGHT:
            key_state.right = new_state;
        break;
        case GLFW_KEY_LEFT:
            key_state.left = new_state;
        break;
        case GLFW_KEY_UP:
            key_state.up = new_state;
        break;
        case GLFW_KEY_DOWN:
            key_state.down = new_state;
        break;
    }
    key_state.change_slide = new_state;
    
	if(key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

void glfw_mouse_button_callback(GLFWwindow * window, int button, int action, int mods) {

}

void glfw_error_callback(int error, const char* description) {
	printf("glfw_error_callback: %d -> %s\n", error, description );
}
