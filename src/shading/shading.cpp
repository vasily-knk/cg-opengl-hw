#include "stdafx.h"
#include "../shared/resources.h"
#include "../shared/scene.h"
#include "../shared/rotator.h"
#include "../shared/stuff.h"

namespace cg_homework
{
    class shading_scene : public scene
    {
    public:
        shading_scene();

        void init();
        void draw(); 
        void resize(int width, int height);
        void update_modelview(const glm::mat4 &matrix);

    private:
        void load_sphere_verts(float radius, int segments);
        void load_sphere_indices(float radius, int segments);

        void draw_sphere();

    private:
        struct vertex
        {
            float x, y, z;
        };

        struct texcoord
        {
            float u, v;
        };

    private:
        gl_buffer_ptr verts_, texcoords_, indices_;
        size_t n_verts_, n_indices_;
        gl_texture_ptr texture_;

        glm::mat4 modelview_, projection_;
    };


    shading_scene::shading_scene()
    {

    }


    void shading_scene::draw()
    {
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(glm::value_ptr(projection_));


        glClearColor(0, 0, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        const int xs = 3, ys = 2;

        for (GLint x = 0; x < xs; ++x)
        {
            for (GLint y = 0; y < ys; ++y)
            {
                const float x_ofs = float(x) - (float(xs) / 2 - 0.5f);
                const float y_ofs = float(y) - (float(ys) / 2 - 0.5f);
                const glm::mat4 matrix = glm::translate(modelview_, glm::vec3(x_ofs * 2.5f, y_ofs * 2.5f, 0));

                glMatrixMode(GL_MODELVIEW);
                glLoadMatrixf(glm::value_ptr(matrix));
                draw_sphere();
            }
        }

    }

    void shading_scene::draw_sphere()
    {
        glBindBuffer(GL_ARRAY_BUFFER, verts_->id());
        glVertexPointer(3, GL_FLOAT, 0, NULL);
        glBindBuffer(GL_ARRAY_BUFFER, texcoords_->id());
        glTexCoordPointer(2, GL_FLOAT, 0, NULL);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_->id());
        glDrawElements(GL_TRIANGLES, n_indices_, GL_UNSIGNED_INT, NULL);
    }

    void shading_scene::init()
    {
        glShadeModel(GL_SMOOTH);							
        glClearColor(0.0f, 0.0f, 1.0f, 0.5f);				
        glClearDepth(1.0f);									
        glEnable(GL_DEPTH_TEST);							
        glDepthFunc(GL_LEQUAL);								
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	
        glEnable(GL_CULL_FACE);

        glEnableClientState(GL_VERTEX_ARRAY);				
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);		
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        const float radius = 1.0f;
        const int segments = 32;

        load_sphere_verts(radius, segments);
        load_sphere_indices(radius, segments);
    }

    void shading_scene::resize(int width, int height)
    {
        projection_ = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
    }

    void shading_scene::update_modelview(const glm::mat4 &matrix)
    {
        modelview_ = matrix;
    }

    void shading_scene::load_sphere_verts(float radius, int segments)
    {
        const float PI = 3.1415926f;
        const float deltax = PI / segments;
        const float deltay = PI / segments;
        const float deltau = 1.0f / (segments * 2);
        const float deltav = 1.0f / segments;


        vector<vertex> verts;
        vector<texcoord> texcoords;

        for (int y = 0; y <= segments; ++y)
        {
            const float angley = -PI / 2 + y * deltay;

            vertex vert;
            vert.y = sin(angley) * radius;
            const float projection = cos(angley) * radius;

            texcoord tc;
            tc.v = 1.0f - y * deltav;

            for (int x = 0; x < segments * 2 + 1; ++x)
            {
                const float anglex = x * deltax;

                vert.x = sin(anglex) * projection;
                vert.z = cos(anglex) * projection;
                verts.push_back(vert);

                tc.u = x * deltau;
                texcoords.push_back(tc);
            }
        }

        n_verts_ = verts.size();
        verts_ = gl_make<gl_buffer>();
        glBindBuffer(GL_ARRAY_BUFFER, verts_->id());
        glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(vertex), &verts.at(0), GL_STATIC_DRAW);

        texcoords_ = gl_make<gl_buffer>();
        glBindBuffer(GL_ARRAY_BUFFER, texcoords_->id());
        glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof(vertex), &texcoords.at(0), GL_STATIC_DRAW);
    }

    void shading_scene::load_sphere_indices(float radius, int segments)
    {
        vector<int> indices;
        const int x_segments = segments * 2 + 1;

        for (int y = 1; y < segments + 1; ++y)
        {
            const int pos = x_segments * y;
            for (int x = 0; x < x_segments; ++x)
            {
                const int x1 = (x + 1) % x_segments;
                indices.push_back(pos + x);
                indices.push_back(pos + x  - x_segments);
                indices.push_back(pos + x1 - x_segments);

                indices.push_back(pos + x1 - x_segments);
                indices.push_back(pos + x1);
                indices.push_back(pos + x);
            }
        }

        n_indices_ = indices.size();
        indices_ = gl_make<gl_buffer>();
        glBindBuffer(GL_ARRAY_BUFFER, indices_->id());
        glBufferData(GL_ARRAY_BUFFER, indices.size() * 4, &indices.at(0), GL_STATIC_DRAW);
    }

    void glut_init(scene &s);
    void glut_main_loop();

}

int main(int argc, char **argv)
{
    using namespace cg_homework;

    shading_scene scene;
    rotator r(scene);

    glut_init(r);
    glewInit();

    try
    {
        scene.init();
    }
    catch (const file_not_found_exception &e)
    {
        std::cerr << e.what() << endl;
        return -1;
    }

    glut_main_loop();
    return 0;
}

