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
        void update_modelview_inv(const glm::mat4 &matrix);
        void update(float elapsed_seconds);
        void keypress(unsigned char /*key*/, int /*x*/, int /*y*/);

    private:
        void load_sphere_verts(float radius, int segments);
        void load_sphere_indices(float radius, int segments);

        void draw_sphere();
        void apply_parameters(size_t mode_id); 

    private:
       /* struct vertex
        {
            float x, y, z;
        };*/
        typedef glm::vec3 vertex;

        struct texcoord
        {
            float u, v;
        };

    private:
        gl_buffer_ptr verts_, normals_, texcoords_, indices_;
        size_t n_verts_, n_indices_;
        gl_texture_ptr texture_;
        
        struct mode_t
        {
            mode_t(const string &name, gl_program_ptr prog)
                : name(name)
                , program(prog)
            {
                modelview_id     = glGetUniformLocation(program->id(), "modelview");
                modelview_inv_id = glGetUniformLocation(program->id(), "modelview_inv");
                projection_id    = glGetUniformLocation(program->id(), "projection");
                light_pos_id     = glGetUniformLocation(program->id(), "light_pos");
                power_id         = glGetUniformLocation(program->id(), "power");
                diffuse_id       = glGetUniformLocation(program->id(), "diffuse_color");
                specular_id      = glGetUniformLocation(program->id(), "specular_color");
                ambient_id       = glGetUniformLocation(program->id(), "ambient_factor");
                atten_id         = glGetUniformLocation(program->id(), "atten_factor");
            }

            string name;
            gl_program_ptr program;
            GLuint modelview_id, modelview_inv_id, projection_id;
            GLuint light_pos_id;
            GLuint power_id;
            GLuint diffuse_id, specular_id, ambient_id;
            GLuint atten_id;
        };

        vector<mode_t> modes_;
        size_t mode_;
        
        glm::mat4 modelview_, modelview_inv_, projection_;
        float light_angle_, light_radius_;
    };


    shading_scene::shading_scene()
        : light_angle_(0.0f)
        , light_radius_(5.0f)
        , mode_(0)
    {

    }


    void shading_scene::apply_parameters(size_t mode_id)
    {
        const mode_t &mode = modes_.at(mode_id);

        glm::vec4 light_pos(light_radius_ * cos(light_angle_), 0.0f, light_radius_ * sin(light_angle_), 1.0f);
        light_pos = modelview_ * light_pos;
        glUseProgram(mode.program->id());
        glUniformMatrix4fv(mode.projection_id, 1, GL_FALSE, glm::value_ptr(projection_));
        glUniform4fv(mode.light_pos_id, 1, glm::value_ptr(light_pos));

        const float power = 4.;
        const float atten_factor = 0.3;
        const glm::vec4 diffuse(1, 0, 0, 1);
        const glm::vec4 specular(0.5, 0.5, 0.5, 0.5);
        const float ambient_factor = 0.2;

        glUniform1f(mode.power_id, 4.);
        glUniform4fv(mode.diffuse_id, 1, glm::value_ptr(diffuse));
        glUniform4fv(mode.specular_id, 1, glm::value_ptr(specular));
        glUniform1f(mode.atten_id, atten_factor);
        glUniform1f(mode.ambient_id, ambient_factor);
    }
    
    void shading_scene::draw()
    {
        glClearColor(0, 0, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const int xs = 3, ys = 2;
        apply_parameters(mode_);

        for (GLint x = 0; x < xs; ++x)
        {
            for (GLint y = 0; y < ys; ++y)
            {

                const float x_ofs = float(x) - (float(xs) / 2 - 0.5f);
                const float y_ofs = float(y) - (float(ys) / 2 - 0.5f);
                const glm::mat4 matrix = glm::translate(modelview_, glm::vec3(x_ofs * 2.5f, y_ofs * 2.5f, 0));
                const glm::mat4 inv = glm::transpose(glm::inverse(matrix));

                glUniformMatrix4fv(modes_.at(mode_).modelview_id, 1, GL_FALSE, glm::value_ptr(matrix));
                // translation doesn't affect normals, so using modelview_inv_
                glUniformMatrix4fv(modes_.at(mode_).modelview_inv_id, 1, GL_FALSE, glm::value_ptr(modelview_inv_));
                draw_sphere();
            }
        }

    }

    void shading_scene::draw_sphere()
    {
        glBindBuffer(GL_ARRAY_BUFFER, verts_->id());
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glBindBuffer(GL_ARRAY_BUFFER, normals_->id());
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_->id());
        glDrawElements(GL_TRIANGLES, n_indices_, GL_UNSIGNED_INT, NULL);
    }

    GLuint LoadShaders(const char *vertex_file_path, const char *fragment_file_path);
     
    void shading_scene::init()
    {
        glShadeModel(GL_SMOOTH);							
        glClearColor(0.0f, 0.0f, 1.0f, 0.5f);				
        glClearDepth(1.0f);									
        glEnable(GL_DEPTH_TEST);							
        glDepthFunc(GL_LEQUAL);								
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	
        glEnable(GL_CULL_FACE);

        //glEnableClientState(GL_VERTEX_ARRAY);				
        //glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        //glEnableVertexAttribArray(0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        modes_.push_back(mode_t("phong", gl_use<gl_program>(LoadShaders("vertex.glsl", "fragment_phong.glsl"))));
        modes_.push_back(mode_t("blinn", gl_use<gl_program>(LoadShaders("vertex.glsl", "fragment_blinn.glsl"))));
        
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

    void shading_scene::update_modelview_inv(const glm::mat4 &matrix)
    {
        modelview_inv_ = glm::transpose(matrix);
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
        vector<glm::vec3> normals;

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

                normals.push_back(glm::normalize(vert));
            }
        }

        n_verts_ = verts.size();
        verts_ = gl_make<gl_buffer>();
        glBindBuffer(GL_ARRAY_BUFFER, verts_->id());
        glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(vertex), &verts.at(0), GL_STATIC_DRAW);

        texcoords_ = gl_make<gl_buffer>();
        glBindBuffer(GL_ARRAY_BUFFER, texcoords_->id());
        glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof(texcoord), &texcoords.at(0), GL_STATIC_DRAW);

        normals_ = gl_make<gl_buffer>();
        glBindBuffer(GL_ARRAY_BUFFER, normals_->id());
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals.at(0), GL_STATIC_DRAW);
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

    void shading_scene::update(float elapsed_seconds)
    {
        const float PI = 3.14159f;
        const float light_speed = PI / 5.0f;

        light_angle_ += light_speed * elapsed_seconds;
        
        while (light_angle_ > PI * 2.0f)
            light_angle_ -= PI * 2.0f;
    }

    void shading_scene::keypress(unsigned char key, int /*x*/, int /*y*/)
    {
        const float radius_speed = 0.1f;

        switch(key)
        {
        case ' ':
            mode_ = (mode_ + 1) % modes_.size();
            cout << "Mode: " << modes_.at(mode_).name << endl;
            break;
        case '-':
        case '=':
        case '+':
            if (key == '-')
                light_radius_ = std::max(light_radius_ - radius_speed, 3.0f);
            else
                light_radius_ = light_radius_ + radius_speed;
            cout << "light distance: " << light_radius_ << endl;
            break;
        }
      
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

