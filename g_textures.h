
void load_gl_texture(char* file, GLuint* tex) {
	unsigned char * data;
    int width = 256;
    int height = 256;
    int bitdepth = 4;
    GLuint texture;
    data = stbi_load(file, &width, &height, &bitdepth, 0);
    // Create one OpenGL texture
    glGenTextures(1, tex);

    glBindTexture(GL_TEXTURE_2D, *tex);

    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


    free(data);
}
