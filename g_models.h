

void load_gl_data_to_vao(GLuint* vao, int point_count, float* vertices, int number_elements_per_vert, float* normals, int number_elements_per_normal, float* texture_coords, int number_elements_per_texcoord) {
	// note: does not free data arrays

	glGenVertexArrays(1, vao);
    glBindVertexArray(*vao);
    
	{
		printf("Uploading vertices\n");
		int n_elements = number_elements_per_vert;
	    GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, n_elements * point_count * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, n_elements, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);
    }

	{
		printf("Uploading normals\n");
		int n_elements = number_elements_per_normal;
		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, n_elements * (point_count) * sizeof(GLfloat), normals, GL_STATIC_DRAW);
		glVertexAttribPointer(1, n_elements, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(1);
	}
	
	{
		printf("Uploading texture_coords\n");
		int n_elements = number_elements_per_texcoord;
		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, number_elements_per_texcoord * (point_count) * sizeof(GLfloat), texture_coords, GL_STATIC_DRAW);
		glVertexAttribPointer(2, n_elements, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(2);
	}

	printf("load_gl_data_to_vao: finished\n");
}
