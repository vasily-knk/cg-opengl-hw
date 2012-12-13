#pragma once

#include "../shared/resources.h"

namespace cg_homework
{

class obj_model
{
public:
    obj_model();
    explicit obj_model(std::istream &stream);

public:
    void load(std::istream &stream);
    void draw() const;

private:
    struct vec3_t
    {
        float x, y, z;
    };

    void parse_vertex(const string &str, vector<vec3_t> &dst);
    void parse_face(const string &str, vector<int> &dst);

private:
    gl_buffer verts_, indices_;
    size_t n_indices_;
};

} // namespace cg_homework
