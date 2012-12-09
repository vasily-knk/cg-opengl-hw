#include "stdafx.h"
#include "scene.h"

namespace cg_homework
{

namespace
{
    scene *glut_scene = NULL;

    void glut_draw() 
    { 
        glut_scene->draw(); 
        glutSwapBuffers();
    }

    void glut_update()
    {
        static ptime last_time = microsec_clock::local_time();
        const ptime new_time = microsec_clock::local_time();
        const float el_time = float((new_time - last_time).total_milliseconds()) * 0.001f;
        last_time = new_time;

        glut_scene->update(el_time);
        glutPostRedisplay();
    }
    
    void glut_resize(int width, int height) { glut_scene->resize(width, height); }
    void glut_mouse_move(int x, int y)      { glut_scene->mouse_move(x, y); }
    void glut_mouse_button(int button, int state, int x, int y) { glut_scene->mouse_button(button, state, x, y); }
    void glut_mouse_wheel(int wheel, int dir, int x, int y) { glut_scene->mouse_button(wheel, dir, x, y); }

}

void glut_init(scene &s)
{
    int argc = 0;
    char *args[] = {""};
    glutInit(&argc, args);
    
    glut_scene = &s;
    glutCreateWindow("CG homework");

    glutDisplayFunc(glut_draw);
    glutIdleFunc   (glut_update);
    glutReshapeFunc(glut_resize);
    glutMouseFunc  (glut_mouse_button);
    glutMotionFunc (glut_mouse_move);    
    //glutMouseWheelFunc(glut_mouse_wheel);    
}

void glut_main_loop()
{
    glutMainLoop();
}

}