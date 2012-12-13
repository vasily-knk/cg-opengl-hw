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
        gl_buffer cube_verts_, cube_indices_;

        glm::mat4 modelview_, modelview_inv_, projection_; 
        glm::mat4 cube_modelview_inv_, cube_projection_inv_;
        glm::vec4 plane_, tplane_;
        float aspect_;
    };

    frustum_scene::frustum_scene()
        : plane_(0.0f, 1.0f, 0.0f, 0.0f)
    {

    }

    void frustum_scene::draw()
    {
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(glm::value_ptr(projection_));

        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(glm::value_ptr(modelview_));

        glClearColor(0, 0, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glDisable(GL_CULL_FACE);
        model_.draw();

        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(glm::value_ptr(projection_));

        glMatrixMode(GL_MODELVIEW);
        const glm::mat4 matrix = modelview_ * cube_modelview_inv_ * cube_projection_inv_;
        glLoadMatrixf(glm::value_ptr(matrix));
        draw_cube();
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
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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
        float verts[] = 
        {
            -1, -1, -1,
            -1, -1,  1,
            -1,  1, -1,
            -1,  1,  1,
             1, -1, -1,
             1, -1,  1,
             1, -1,  1,
             1,  1,  1
        };

        int indices[] = 
        {
            0, 2, 3, 3, 1, 0, // -x
            7, 6, 4, 4, 5, 7, // +x
            0, 1, 5, 5, 4, 1, // -y
            7, 3, 2, 2, 6, 7, // +y
            0, 4, 6, 6, 2, 0, // -z
            1, 3, 7, 7, 5, 1  // +z
        };

        cube_verts_ = make_gl_buffer();
        glBindBuffer(GL_ARRAY_BUFFER, cube_verts_->id());
        glBufferData(GL_ARRAY_BUFFER, 12 * 8, verts, GL_STATIC_DRAW);

        cube_indices_ = make_gl_buffer();
        glBindBuffer(GL_ARRAY_BUFFER, cube_indices_->id());
        glBufferData(GL_ARRAY_BUFFER, 12 * 12, indices, GL_STATIC_DRAW);
        
    }

    void frustum_scene::draw_cube()
    {
        glBindBuffer(GL_ARRAY_BUFFER, cube_verts_->id());
        glVertexPointer(3, GL_FLOAT, 0, NULL);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_indices_->id());
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);
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

        projection_ = m;
        projection_ = glm::row(projection_, 2, m2);
    }

    void frustum_scene::keypress(unsigned char key, int, int)
    {
        if (key == ' ')             
        {
            cube_modelview_inv_ = modelview_inv_;
            cube_projection_inv_ = glm::inverse(projection_);
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

