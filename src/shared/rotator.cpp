#include "stdafx.h"
#include "rotator.h"

namespace cg_homework
{

rotator::rotator(scene &s)
    : scene_(&s)
    , dragging_(false)
    , angle(0.0f)
{}

void rotator::resize(int width, int height)
{
    glViewport(0, 0, width, height);				    // Reset The Current Viewport

    glMatrixMode(GL_PROJECTION);					    // Select The Projection Matrix
    glLoadIdentity();									// Reset The Projection Matrix

    // Calculate The Aspect Ratio Of The Window
    gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 1000.0f);
    scene_->resize(width, height);
}

void rotator::update(float elapsed_seconds)
{
    scene_->update(elapsed_seconds);
}

void rotator::draw()
{
    glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
    glLoadIdentity();									// Reset The Modelview Matrix}
    glTranslatef(0.0f, 0.0f, -100.0f);
    glRotatef(-90.0f, 1, 0, 0);
    glRotatef(angle, 0, 0, 1);

    scene_->draw();
}

void rotator::mouse_move(int x, int y)
{
    if (dragging_)
    {
        const float deltax = float(x - drag_x_);
        angle += deltax * 0.5f;

        drag_x_ = x;
        drag_y_ = y;
    }
    scene_->mouse_move(x, y);
}

void rotator::mouse_button(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        dragging_ = (state == GLUT_DOWN);
        if (dragging_)
        {
            drag_x_ = x;
            drag_y_ = y;
        }
    }
    scene_->mouse_button(button, state, x, y);
}
}



