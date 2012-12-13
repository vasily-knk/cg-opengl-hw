#include "stdafx.h"
#include "rotator.h"

namespace cg_homework
{

rotator::rotator(scene &s)
    : scene_(&s)
    , dragging_(false)
    , angle_x_(0.0f)
    , angle_y_(0.0f)
    , dist_(-3.0f)
{}

void rotator::resize(int width, int height)
{
    glViewport(0, 0, width, height);				    // Reset The Current Viewport

    scene_->resize(width, height);
}

void rotator::update(float elapsed_seconds)
{
    glm::mat4 matrix(1.0f);
    matrix = glm::translate(matrix, glm::vec3(0, 0, dist_));
    matrix = glm::rotate(matrix, angle_x_, glm::vec3(1, 0, 0));
    matrix = glm::rotate(matrix, angle_y_, glm::vec3(0, 1, 0));
    scene_->update_modelview(matrix);

    glm::mat4 matrix_inv(1.0f);
    matrix_inv = glm::rotate(matrix_inv, -angle_y_, glm::vec3(0, 1, 0));
    matrix_inv = glm::rotate(matrix_inv, -angle_x_, glm::vec3(1, 0, 0));
    matrix_inv = glm::translate(matrix_inv, glm::vec3(0, 0, -dist_));
    scene_->update_modelview_inv(matrix_inv);
    scene_->update(elapsed_seconds);
}

void rotator::draw()
{
    //glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
    //glLoadIdentity();									// Reset The Modelview Matrix}
    //glTranslatef(0.0f, 0.0f, dist_);
    //glRotatef(-90.0f, 1, 0, 0);
    //glRotatef(angle_x_, 1, 0, 0);
    //glRotatef(angle_y_, 0, 1, 0);

    scene_->draw();
}

void rotator::mouse_move(int x, int y)
{
    if (dragging_)
    {
        const float deltay = float(y - drag_y_);
        angle_x_ += deltay * 0.5f;

        const float deltax = float(x - drag_x_);
        angle_y_ += deltax * 0.5f;

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
    else if (button == 3)
    {
        dist_ += 0.1f;
    }
    else if (button == 4)
    {
        dist_ -= 0.1f;
    }
    scene_->mouse_button(button, state, x, y);
}

void rotator::mouse_wheel(int wheel, int direction, int x, int y)
{
    dist_ += direction * 0.1f;
    scene_->mouse_wheel(wheel, direction, x, y);
}

void rotator::keypress(unsigned char key, int x, int y)
{
    scene_->keypress(key, x, y);
}

}



