#include "stdafx.h"
#include "obj_model.h"

namespace cg_homework
{

obj_model::obj_model()
{

}

void obj_model::load(std::istream &stream)
{
    vector<vec3_t> verts;
    vector<int> indices;

    string str;
    while (std::getline(stream, str))
    {
        std::stringstream ss(str);
        string cmd;

        ss >> cmd;

        const size_t pos = size_t(ss.tellg());
        string substr(str.substr(pos + 1));
        if (cmd == "v")
            parse_vertex(substr, verts);
        else if (cmd == "f")
            parse_face(substr, indices);
    }

    verts_ = gl_make<gl_buffer>();
    glBindBuffer(GL_ARRAY_BUFFER, verts_->id());
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(vec3_t), &verts.at(0), GL_STATIC_DRAW);

    n_indices_ = indices.size();
    indices_ = gl_make<gl_buffer>();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_->id());
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices.at(0), GL_STATIC_DRAW);

}

void obj_model::parse_vertex(const string &str, vector<vec3_t> &dst)
{
    vec3_t vert;
    std::stringstream ss(str);
    ss >> vert.x >> vert.y >> vert.z;
    dst.push_back(vert);
}

namespace 
{
    inline bool is_slash(const char c)
    {
        return (c == '/');
    }
}

void obj_model::parse_face(const string &str, vector<int> &dst)
{
    vector<string> blocks;
    boost::split(blocks, str, boost::is_space(), boost::token_compress_on);

    BOOST_FOREACH(const string &block, blocks)
    {
        vector<string> numbers;
        boost::split(numbers, block, is_slash);

        int vert = boost::lexical_cast<int>(numbers.front());
        //face.verts.push_back(vert);
        dst.push_back(vert - 1);
    }
}

void obj_model::draw() const
{
    glBindBuffer(GL_ARRAY_BUFFER, verts_->id());
    glVertexPointer(3, GL_FLOAT, 0, NULL);
    //glBindBuffer(GL_ARRAY_BUFFER, texcoords_->id());
    //glTexCoordPointer(2, GL_FLOAT, 0, NULL);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_->id());
    glDrawElements(GL_TRIANGLES, n_indices_, GL_UNSIGNED_INT, NULL);
}

} // namespace cg_homework
