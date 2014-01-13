#include "stdafx.h"
#include "../shared/resources.h"
#include "../shared/scene.h"
#include "../shared/rotator.h"
#include "../shared/stuff.h"
#include "obj_model.h"

namespace cg_homework
{
    class frustum_scene : public scene
    {
    public:
        frustum_scene();

        void init();
        void draw(); 
        void resize(int width, int height);
        void update_modelview(const glm::mat4 &matrix);
        void update_modelview_inv(const glm::mat4 &matrix);
        void keypress(unsigned char key, int x, int y);

    private:
        void load_model();
        void load_cube();
        void draw_cube();

    private:
        obj_model model_;
        gl_buffer_ptr cube_verts_, cube_indices_;

        glm::mat4 modelview_, modelview_inv_, clipped_projection_, usual_projection_; 
        glm::mat4 cube_modelview_inv_, cube_projection_inv_;
        glm::vec4 plane_, tplane_;
        float aspect_;
        bool draw_frustum_;
    };

    frustum_scene::frustum_scene()
        : plane_(0.0f, 1.0f, 0.0f, 0.0f)
        , draw_frustum_(false)
    {

    }

    void frustum_scene::draw()
    {
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(glm::value_ptr(clipped_projection_));
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(glm::value_ptr(modelview_));

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        glColor3f(1.0f, 0.0f, 0.0f);
        model_.draw();
        glCullFace(GL_BACK);
        glColor3f(0.0f, 0.0f, 1.0f);
        model_.draw();

        if (draw_frustum_)
        {
            glMatrixMode(GL_PROJECTION);
            glLoadMatrixf(glm::value_ptr(usual_projection_));
            glMatrixMode(GL_MODELVIEW);
            const glm::mat4 matrix = modelview_ * cube_modelview_inv_ * cube_projection_inv_;
            glLoadMatrixf(glm::value_ptr(matrix));

            glColor3f(1.0f, 1.0f, 1.0f);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDisable(GL_CULL_FACE);
            draw_cube();
        }
    }

    void frustum_scene::init()
    {
        glShadeModel(GL_SMOOTH);							
        glClearColor(0.0f, 0.0f, 1.0f, 0.5f);				
        glClearDepth(1.0f);									
        glEnable(GL_DEPTH_TEST);							
        glDepthFunc(GL_LEQUAL);								
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	
        //glEnable(GL_CULL_FACE);
        //glEnable(GL_TEXTURE_2D);

        glEnableClientState(GL_VERTEX_ARRAY);				

        load_model();
        load_cube();
    }

    void frustum_scene::load_model()
    {
        const string filename = "model.obj";
        ifstream file(filename.c_str(), ios_base::in | ios_base::binary);
        if (!file.is_open())
            throw file_not_found_exception(filename);

        model_.load(file);
    }


    void frustum_scene::load_cube()
    {
        static const float verts[] = {
            -1.0, -1.0,  1.0,
            1.0, -1.0,  1.0,
            -1.0,  1.0,  1.0,
            1.0,  1.0,  1.0,
            -1.0, -1.0, -1.0,
            1.0, -1.0, -1.0,
            -1.0,  1.0, -1.0,
            1.0,  1.0, -1.0,
        };

        static const int indices[] = {
            0, 1, 2, 3, 7, 1, 5, 4, 7, 6, 2, 4, 0, 1
        };

        cube_verts_ = gl_make<gl_buffer>();
        glBindBuffer(GL_ARRAY_BUFFER, cube_verts_->id());
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

        cube_indices_ = gl_make<gl_buffer>();
        glBindBuffer(GL_ARRAY_BUFFER, cube_indices_->id());
        glBufferData(GL_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        
    }

    void frustum_scene::draw_cube()
    {
        glBindBuffer(GL_ARRAY_BUFFER, cube_verts_->id());
        glVertexPointer(3, GL_FLOAT, 0, NULL);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_indices_->id());
        glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_INT, NULL);
    }

    void frustum_scene::resize(int width, int height)
    {
        aspect_ = float(width) / height;
    }

    void frustum_scene::update_modelview(const glm::mat4 &matrix)
    {
        modelview_ = matrix;
    }

    void frustum_scene::update_modelview_inv(const glm::mat4 &matrix)
    {
        modelview_inv_ = matrix;

        const float n = 0.1f;
        const float f = 100.0f;
        const float fovy = 45.0f;
        const glm::mat4 m = glm::perspective(fovy, aspect_, n, f);

        const float range = tan(glm::radians(fovy / 2.0f)) * n;
        const float lr = range * aspect_;
        const float tb = range;

        // cutting plane in cam space
        tplane_ = glm::transpose(modelview_inv_) * plane_;
        const float sx = tplane_.x > 0 ? 1.0f : -1.0f;
        const float sy = tplane_.y > 0 ? 1.0f : -1.0f;

        const glm::vec4 q(sx * lr / n, sy * tb / n, -1.0f, 1.0f / f);

        const float a = 2 * glm::dot(glm::row(m, 3), q) / glm::dot(tplane_, q);
        const glm::vec4 m2 = tplane_ * a - glm::row(m, 3);

        clipped_projection_ = m;
        usual_projection_ = m;
        clipped_projection_ = glm::row(clipped_projection_, 2, m2);
    }

    void frustum_scene::keypress(unsigned char key, int, int)
    {
        if (key == ' ')             
        {
            cube_modelview_inv_ = modelview_inv_;
            cube_projection_inv_ = glm::inverse(clipped_projection_);
            draw_frustum_ = true;
        }
    }

    void glut_init(scene &s);
    void glut_main_loop();

}

int main(int argc, char **argv)
{
    using namespace cg_homework;

    frustum_scene scene;
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

