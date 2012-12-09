#pragma once

namespace cg_homework
{

struct scene
{
    virtual void init() {};
    virtual void resize(int /*width*/, int /*height*/) {}
    virtual void draw() {};
    virtual void update(float /*elapsed_seconds*/) {}

    virtual void mouse_move(int /*x*/, int /*y*/) {}
    virtual void mouse_button(int /*button*/, int /*state*/, int /*x*/, int /*y*/) {}
    virtual void mouse_wheel(int /*wheel*/, int /*direction*/, int /*x*/, int /*y*/) {}

    virtual void update_modelview(const glm::mat4 &/*matrix*/) {}
    //virtual void update_projection(const glm::mat4 &/*matrix*/) {}
};

} // namespace cg_homework
