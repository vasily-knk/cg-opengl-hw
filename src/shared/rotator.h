#pragma once

#include "scene.h"

namespace cg_homework
{

class rotator 
    : public scene
{
public:
    rotator(scene &s);

    //void init();
    void resize(int width, int height);
    void draw();
    void update(float elapsed_seconds);

    void mouse_move(int /*x*/, int /*y*/);
    void mouse_button(int /*button*/, int /*state*/, int /*x*/, int /*y*/);

private:
    scene *scene_;

    bool dragging_;
    int drag_x_, drag_y_;
    float angle;
};

}