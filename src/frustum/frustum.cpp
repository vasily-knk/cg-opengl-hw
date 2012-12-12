#include "stdafx.h"
#include "../shared/resources.h"
#include "../shared/scene.h"
#include "../shared/rotator.h"
#include "../shared/stuff.h"

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


    private:
        gl_buffer verts_, indices_;
        size_t n_verts_, n_indices_;

        glm::mat4 modelview_, projection_;
    };


    frustum_scene::frustum_scene(/*const string &md2_filename, const string &texture_filename*/)
    {

    }


    void frustum_scene::draw()
    {
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(glm::value_ptr(projection_));


        glClearColor(0, 0, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void frustum_scene::init()
    {
        glShadeModel(GL_SMOOTH);							
        glClearColor(0.0f, 0.0f, 1.0f, 0.5f);				
        glClearDepth(1.0f);									
        glEnable(GL_DEPTH_TEST);							
        glDepthFunc(GL_LEQUAL);								
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	
        glEnable(GL_CULL_FACE);
        glEnable(GL_TEXTURE_2D);

        glEnableClientState(GL_VERTEX_ARRAY);				
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    void frustum_scene::resize(int width, int height)
    {
        projection_ = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
    }

    void frustum_scene::update_modelview(const glm::mat4 &matrix)
    {
        modelview_ = matrix;
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

