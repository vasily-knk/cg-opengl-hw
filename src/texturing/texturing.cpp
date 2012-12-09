// texturing.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../shared/resources.h"
#include "../shared/md2.h"
//#include "../envir.h"
#include "../shared/scene.h"
#include "../shared/rotator.h"

namespace cg_homework
{
struct file_not_found_exception
    : std::runtime_error
{
    file_not_found_exception(const string &filename)
        : std::runtime_error(string("File not found: ") + filename)
    {}
};

class md2_scene : public scene
{
public:
    md2_scene(const string &md2_filename, const string &texture_filename);

    void init();
    void draw(); 
    void update(float elapsed_seconds); 

private:
    void load_model();
    void load_texture();

private:
    string md2_filename_;
    string texture_filename_;
    md2_model model_;
    gl_texture texture_;
    
    float frame_;
    int anim_begin_, anim_end_;
};


md2_scene::md2_scene(const string &md2_filename, const string &texture_filename)
    : md2_filename_     (md2_filename)
    , texture_filename_ (texture_filename)
    , frame_(0.0f)
    , anim_begin_(0)
    , anim_end_  (0)
{

}


void md2_scene::update(float el_time)
{
    frame_ += el_time * 12.0f;
    while(frame_ > anim_end_ - anim_begin_)
        frame_ -= anim_end_ - anim_begin_;
}

void md2_scene::draw()
{
    glClearColor(0, 0, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, texture_->id());
    model_.draw(anim_begin_ + int(frame_));
}

void md2_scene::init()
{
    glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
    glClearColor(0.0f, 0.0f, 1.0f, 0.5f);				// Black Background
    glClearDepth(1.0f);									// Depth Buffer Setup
    glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
    glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);

    glEnableClientState(GL_VERTEX_ARRAY);						// Enable Vertex Arrays
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);						// Enable Vertex Arrays
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    anim_begin_ = 73;
    anim_end_ = 85;

    load_model();
    load_texture();
}

void resize(int width, int height)
{
    (void)width;
    (void)height;
}

void md2_scene::load_model()
{
    ifstream file(md2_filename_.c_str(), ios_base::in | ios_base::binary);
    if (!file.is_open())
        throw file_not_found_exception(md2_filename_);

    model_.load(file);
}

void md2_scene::load_texture()
{
    GLuint texture_id = SOIL_load_OGL_texture
        (texture_filename_.c_str(),
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS);

    if (texture_id == 0)
        throw file_not_found_exception(texture_filename_);

    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    texture_ = use_gl_texture(texture_id);
}

void glut_init(scene &s);
void glut_main_loop();

}


int main(int argc, char **argv)
{
    using namespace cg_homework;

    md2_scene scene("infantry.md2", "infantry.tga");
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

