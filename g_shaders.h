
char* _load_file(char const* path) {
    char* buffer = 0;
    long length = 0;
    FILE * f = fopen (path, "rb");

    if (f) {
      fseek (f, 0, SEEK_END);
      length = ftell (f);
      fseek (f, 0, SEEK_SET);
      buffer = (char*)malloc ((length+1)*sizeof(char));
      if (buffer) {
        fread (buffer, sizeof(char), length, f);
      }
      fclose (f);
    } else {
      return NULL;
    }

    buffer[length] = '\0';

    return buffer;
}


GLuint _compile_shader(GLenum type, const char *src) {
    GLuint shader;
    GLint compiled;
    shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if(!compiled) {
        GLint infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
        GLchar strInfoLog[infoLogLength];
        glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);
        printf("\n_compile_shader:\n!!!\nCompilation error in shader %s\n\n", strInfoLog);
        glDeleteShader(shader);
        return 0;
    }
    printf("_compile_shader: Success!\n");
    return shader;
}

int _compile_shader_program(const char* str_vert_shader,
                           const char* str_frag_shader,
                           const char* attrib_name_0,
                           const char* attrib_name_1,
                           const char* attrib_name_2) {
    GLuint vert_shader;
    GLuint frag_shader;
    GLuint prog_object;

    vert_shader = _compile_shader(GL_VERTEX_SHADER, str_vert_shader);
    if(vert_shader == 0) {
        printf("Error compiling Vertex shader\n");
        return 1;
    }

    frag_shader = _compile_shader(GL_FRAGMENT_SHADER, str_frag_shader);
    if(frag_shader == 0) {
        printf("Error compiling Fragment shader\n");
        return 1;
    }

    printf("Creating shader program\n");

    prog_object = glCreateProgram();
    glAttachShader(prog_object, vert_shader);
    glAttachShader(prog_object, frag_shader);

    if (attrib_name_0 != NULL) {
        printf("Binding attrib0 %s \n", attrib_name_0);
        glBindAttribLocation(prog_object, 0, attrib_name_0);
    }

    if (attrib_name_1 != NULL) {
        printf("Binding attrib1 %s \n", attrib_name_1);
        glBindAttribLocation(prog_object, 1, attrib_name_1);
    }

    if (attrib_name_2 != NULL) {
        printf("Binding attrib2 %s \n", attrib_name_2);
        glBindAttribLocation(prog_object, 2, attrib_name_2);
    }

    printf("Linking shader program\n");
    glLinkProgram(prog_object);

    return prog_object;
}

GLuint load_shader_single_file(char* file_name, char* attrib_name_0, char* attrib_name_1, char* attrib_name_2) {

  char* buf = _load_file(file_name);
  if (!buf){
    return 0;
  }

  char* start_frag_shader = strstr(buf, "#FRAG");
  char* frag_shader = start_frag_shader + strlen("#FRAG");
  *start_frag_shader = '\0';
  char* vert_shader = buf;
  
  //printf("From file: VERT_SHADER: %s\n", vert_shader);
  //printf("From file: FRAG_SHADER: %s\n", frag_shader);

  GLuint program = _compile_shader_program(vert_shader, frag_shader, attrib_name_0, attrib_name_1, attrib_name_2);
  free(buf);
  return program;
  

}
