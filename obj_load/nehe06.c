
#include <kos.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glkos.h>
#include <png/png.h>
#include <tinyobjloader/tinyobj_loader_c.h>

/* Simple GL example to demonstrate loading an OBJ model with textures */

GLfloat xrot = 30.0f;   /* X Rotation - fixed angle */
GLfloat yrot = 45.0f;   /* Y Rotation - fixed angle */
GLfloat zrot = 0.0f;    /* Z Rotation - fixed angle */
GLfloat zoom = -5.0f;   /* Distance from camera */

/* Model data structures */
typedef struct {
    tinyobj_attrib_t attrib;
    tinyobj_shape_t* shapes;
    size_t num_shapes;
    tinyobj_material_t* materials;
    size_t num_materials;
    GLuint* texture_ids; /* OpenGL texture IDs for each material */
} ObjModel;

ObjModel model;

void default_file_reader(void *ctx, const char *filename, int is_mtl, 
                        const char *obj_filename, char **buf, size_t *len) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        *buf = NULL;
        *len = 0;
        return;
    }
    
    fseek(fp, 0, SEEK_END);
    *len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    *buf = (char*)malloc(*len + 1);
    if (!*buf) {
        fclose(fp);
        *len = 0;
        return;
    }
    
    fread(*buf, 1, *len, fp);
    (*buf)[*len] = '\0';  // Null-terminate the buffer
    fclose(fp);
}

/* Function to get directory path from filename */
void get_directory(const char* filepath, char* directory, size_t size) {
    strncpy(directory, filepath, size);
    directory[size-1] = '\0';
    
    char* last_slash = strrchr(directory, '/');
    if (last_slash) {
        *(last_slash + 1) = '\0';
    } else {
        directory[0] = '\0';
    }
}

GLuint load_png_texture(const char* filename) {
    kos_img_t img;
    GLuint texture_id = 0;
    
    printf("Loading texture: %s\n", filename);
    
    /* Load PNG file */
    if (png_to_img(filename, PNG_NO_ALPHA, &img) < 0) {
        printf("Failed to load PNG: %s\n", filename);
        return 0;
    }
    
    /* Generate OpenGL texture */
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    
    /* Set texture parameters */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    /* Upload texture data */
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.w, img.h, 0, 
                 GL_RGB, GL_UNSIGNED_SHORT_5_6_5, img.data);
    
    /* Free image data manually */
    if (img.data) {
        free(img.data);
    }
    
    printf("Loaded texture %s with ID %d\n", filename, texture_id);
    return texture_id;
}

void load_material_textures(const char* obj_filename) {
    char directory[256];
    char texture_path[512];
    
    if (model.num_materials == 0) {
        printf("No materials to load textures for\n");
        return;
    }
    
    /* Get directory of OBJ file */
    get_directory(obj_filename, directory, sizeof(directory));
    
    /* Allocate texture ID array */
    model.texture_ids = (GLuint*)calloc(model.num_materials, sizeof(GLuint));
    
    /* Load texture for each material */
    for (size_t i = 0; i < model.num_materials; i++) {
        if (model.materials[i].diffuse_texname && strlen(model.materials[i].diffuse_texname) > 0) {
            /* Build full texture path */
            snprintf(texture_path, sizeof(texture_path), "%s%s", 
                    directory, model.materials[i].diffuse_texname);
            
            /* Load texture */
            model.texture_ids[i] = load_png_texture(texture_path);
        } else {
            model.texture_ids[i] = 0; /* No texture */
        }
    }
}

void load_obj(const char* filename) {
    printf("Starting to load OBJ file: %s\n", filename);
    
    FILE* f = fopen(filename, "r");
    if (!f) {
        printf("ERROR: Cannot open file %s\n", filename);
        return;
    }
    fclose(f);
    printf("File exists and can be opened\n");
    
    unsigned int flags = TINYOBJ_FLAG_TRIANGULATE;
    tinyobj_attrib_init(&model.attrib);
    
    int ret = tinyobj_parse_obj(&model.attrib, &model.shapes, &model.num_shapes, 
                              &model.materials, &model.num_materials,
                              filename, default_file_reader, NULL, flags);
    
    if (ret != TINYOBJ_SUCCESS) {
        printf("Failed to load OBJ file: %s (error code %d)\n", filename, ret);
        return;
    }
    
    printf("OBJ loaded: %s\n", filename);
    printf("# of vertices: %d\n", (int)(model.attrib.num_vertices));
    printf("# of normals: %d\n", (int)(model.attrib.num_normals));
    printf("# of texcoords: %d\n", (int)(model.attrib.num_texcoords));
    printf("# of faces: %d\n", (int)(model.attrib.num_faces));
    printf("# of face_num_verts: %d\n", (int)(model.attrib.num_face_num_verts));
    printf("# of shapes: %d\n", (int)model.num_shapes);
    printf("# of materials: %d\n", (int)model.num_materials);
    
    /* Load textures */
    load_material_textures(filename);
}

void free_obj_model() {
    /* Free textures */
    if (model.texture_ids) {
        for (size_t i = 0; i < model.num_materials; i++) {
            if (model.texture_ids[i] != 0) {
                glDeleteTextures(1, &model.texture_ids[i]);
            }
        }
        free(model.texture_ids);
    }
    
    /* Free model data */
    tinyobj_attrib_free(&model.attrib);
    tinyobj_shapes_free(model.shapes, model.num_shapes);
    tinyobj_materials_free(model.materials, model.num_materials);
}

void draw_gl(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, zoom);

    glRotatef(xrot, 1.0f, 0.0f, 0.0f);
    glRotatef(yrot, 0.0f, 1.0f, 0.0f);
    glRotatef(zrot, 0.0f, 0.0f, 1.0f);

    /* Enable texturing if we have textures */
    int has_textures = 0;
    if (model.texture_ids) {
        for (size_t i = 0; i < model.num_materials; i++) {
            if (model.texture_ids[i] != 0) {
                has_textures = 1;
                break;
            }
        }
    }
    
    if (has_textures) {
        glEnable(GL_TEXTURE_2D);
    }

    /* Draw the model */
    int current_material = -1;
    size_t face_offset = 0;
    
    for (size_t face_idx = 0; face_idx < model.attrib.num_face_num_verts; face_idx++) {
        /* Get material for this face */
        int material_id = -1;
        if (model.attrib.material_ids) {
            material_id = model.attrib.material_ids[face_idx];
        }
        
        /* Bind texture if material changed */
        if (material_id != current_material) {
            current_material = material_id;
            if (material_id >= 0 && material_id < model.num_materials && model.texture_ids) {
                if (model.texture_ids[material_id] != 0) {
                    glBindTexture(GL_TEXTURE_2D, model.texture_ids[material_id]);
                    glEnable(GL_TEXTURE_2D);
                } else {
                    glDisable(GL_TEXTURE_2D);
                }
            } else {
                glDisable(GL_TEXTURE_2D);
            }
        }
        
        /* Draw the face */
        int num_verts = model.attrib.face_num_verts[face_idx];
        
        glBegin(GL_TRIANGLES);
        for (int v = 0; v < num_verts; v++) {
            tinyobj_vertex_index_t idx = model.attrib.faces[face_offset + v];
            
            /* Texture coordinates */
            if (idx.vt_idx >= 0 && idx.vt_idx < model.attrib.num_texcoords) {
                float u = model.attrib.texcoords[idx.vt_idx * 2];
                float v = model.attrib.texcoords[idx.vt_idx * 2 + 1];
                /* Flip V coordinate - OBJ uses bottom-left origin, OpenGL uses top-left */
                v = 1.0f - v;
                glTexCoord2f(u, v);
            }
            
            /* Normals */
            if (idx.vn_idx >= 0 && idx.vn_idx < model.attrib.num_normals) {
                float nx = model.attrib.normals[idx.vn_idx * 3];
                float ny = model.attrib.normals[idx.vn_idx * 3 + 1];
                float nz = model.attrib.normals[idx.vn_idx * 3 + 2];
                glNormal3f(nx, ny, nz);
            }
            
            /* Vertices */
            if (idx.v_idx >= 0 && idx.v_idx < model.attrib.num_vertices) {
                float x = model.attrib.vertices[idx.v_idx * 3];
                float y = model.attrib.vertices[idx.v_idx * 3 + 1];
                float z = model.attrib.vertices[idx.v_idx * 3 + 2];
                glVertex3f(x, y, z);
            }
        }
        glEnd();
        
        face_offset += num_verts;
    }
    
    if (has_textures) {
        glDisable(GL_TEXTURE_2D);
    }
}

int main(int argc, char **argv) {
    maple_device_t *cont;
    cont_state_t *state;

    printf("nehe06_obj beginning\n");

    /* Get basic stuff initialized */
    glKosInit();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, 640.0f / 480.0f, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    /* Enable lighting for better 3D effect */
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    /* Set up a simple light */
    GLfloat light_position[] = { 1.0f, 1.0f, 1.0f, 0.0f };
    GLfloat light_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);

    /* Load the OBJ file */
    load_obj("/rd/Sonic.obj");

    while(1) {
        cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);

        /* Check key status */
        state = (cont_state_t *)maple_dev_status(cont);

        if(!state) {
            printf("Error reading controller\n");
            break;
        }

        if(state->buttons & CONT_START)
            break;
            
        /* D-pad controls for zoom */
        if(state->buttons & CONT_DPAD_UP) {
            zoom += 0.1f;  /* Zoom in */
        }
        if(state->buttons & CONT_DPAD_DOWN) {
            zoom -= 0.1f;  /* Zoom out */
        }

        /* Draw the GL "scene" */
        draw_gl();

        /* Finish the frame */
        glKosSwapBuffers();
    }

    /* Clean up */
    free_obj_model();

    return 0;
}