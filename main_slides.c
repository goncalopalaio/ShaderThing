#include <stdio.h>

#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>


#define STB_IMAGE_IMPLEMENTATION
#include "external_stb_image.h"

#include "g_shaders.h"
#include "g_textures.h"
#include "g_models.h"
#include "g_filewatcher.h"

#define WINDOW_W 400
#define WINDOW_H 400

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


	// Setup filewatcher
	start_filewatcher();


	float frame_time = 0;
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
    	if (files_changed_filewatcher){
    		program = load_shader_single_file("shaders/shader.glsl", "position", "color", "uv");
    		files_changed_filewatcher = 0;
    	}
    	glUseProgram(program);
    	glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[current_index]);
		glUniform1i(uniform_loc_tex0, 0);
		glUniform1f(uniform_loc_frame_count, frame_time);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, data_n_vertices);
		glUseProgram(0);


		glfwSwapBuffers(window);
        glfwPollEvents();
        frame_time+=0.001;
    }

    stop_filewatcher();
	
	glfwMakeContextCurrent(NULL);
    glfwDestroyWindow(window);
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
