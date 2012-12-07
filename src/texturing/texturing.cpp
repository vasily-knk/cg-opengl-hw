// texturing.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../shared/md2.h"

namespace cg_homework
{

DWORD last_time = timeGetTime();
GLuint texture_id_;
md2_model model;
float angle = 0;
const int anim_begin = 73, anim_end = 85;
float frame = 0;
int iframe = 0;

void update()
{
    const DWORD time = timeGetTime();
    const float el_time = (time - last_time) * 0.001f;
    last_time = time;

    glMatrixMode(GL_MODELVIEW);
    //glRotatef(float(el_time * 10.0), 0, 1, 0);
    glutPostRedisplay();
    frame += el_time * 12.0f;
    iframe = anim_begin + int(frame) % (anim_end - anim_begin);
}

void draw()
{
    glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
    glLoadIdentity();									// Reset The Modelview Matrix}
    glTranslatef(0.0f, 0.0f, -100.0f);
    glRotatef(-90.0f, 1, 0, 0);
    glRotatef(angle, 0, 0, 1);

    glClearColor(0, 0, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glColor3f(1.0f, 1.0f, 0);

    glBindTexture(GL_TEXTURE_2D, texture_id_);

    model.draw(iframe);

    glutSwapBuffers();
}

void init()
{
    glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
    glClearColor(0.0f, 0.0f, 1.0f, 0.5f);				// Black Background
    glClearDepth(1.0f);									// Depth Buffer Setup
    glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
    glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);

    glEnableClientState(GL_VERTEX_ARRAY);						// Enable Vertex Arrays
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);						// Enable Vertex Arrays
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void resize(int width, int height)
{
    glViewport(0, 0, width, height);				    // Reset The Current Viewport

    glMatrixMode(GL_PROJECTION);					    // Select The Projection Matrix
    glLoadIdentity();									// Reset The Projection Matrix

    // Calculate The Aspect Ratio Of The Window
    gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 1000.0f);
}

bool dragging = false;
int drag_x = 0, drag_y = 0;
void mouse_button(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        dragging = (state == GLUT_DOWN);
        if (dragging)
        {
            drag_x = x;
            drag_y = y;
        }
    }
}

void mouse_move(int x, int y)
{
    if (dragging)
    {
        const float deltax = float(x - drag_x);
        angle += deltax * 0.5f;

        drag_x = x;
        drag_y = y;
    }
}

bool load_model()
{
    ifstream file("infantry.md2", ios_base::in | ios_base::binary);
    if (!file.is_open())
        return false;

    return model.load(file);
}

bool load_texture()
{
    texture_id_ = SOIL_load_OGL_texture
        ("infantry.tga",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS);

    if (texture_id_ == 0)
        return false;

    glBindTexture(GL_TEXTURE_2D, texture_id_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return true;
}

}

int main(int argc, char **argv)
{
    using namespace cg_homework;

    glutInit(&argc, argv);
    glutCreateWindow("Hello!");
    glutDisplayFunc(draw);
    glutIdleFunc(update);
    glutReshapeFunc(resize);
    glutMouseFunc(mouse_button);
    glutMotionFunc(mouse_move);
    
    glewInit();
    init();

    if (!load_model() || !load_texture())
        return -1;

    glutMainLoop();
    return 0;
}

