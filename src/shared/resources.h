#pragma once

namespace cg_homework
{

    template<typename G>
    class gl_resource
        : boost::noncopyable
    {
    public:
        explicit gl_resource(GLuint id)
            : id_(id) {}
        gl_resource () { id_ = G::gen(); }
        ~gl_resource() { G::del(id_);    }

        GLuint id() const { return id_; }
        
    private:
        GLuint id_;
    };

    struct gl_buffer_gen
    {
        static GLuint gen() 
        {
            GLuint res;
            glGenBuffers(1, &res);
            return res;
        }
        static void del(GLuint id)
        {
            glDeleteBuffers(1, &id);
        }
    };

    struct gl_texture_gen
    {
        static GLuint gen() 
        {
            GLuint res;
            glGenTextures(1, &res);
            return res;
        }
        static void del(GLuint id)
        {
            glDeleteTextures(1, &id);
        }
    };

    struct gl_program_gen
    {
        static GLuint gen() { return glCreateProgram(); }
        static void del(GLuint id) { return glDeleteProgram(id); }
    };

    template<GLenum TYPE>
    struct gl_shader_gen
    {
        static GLuint gen() { return glCreateShader(TYPE); }
        static void del(GLuint id) { return glDeleteShader(id); }
    };

    typedef gl_resource<gl_buffer_gen > gl_buffer_keeper;
    typedef gl_resource<gl_texture_gen> gl_texture_keeper;
    typedef gl_resource<gl_program_gen> gl_program_keeper;

    typedef gl_resource<gl_shader_gen<GL_VERTEX_SHADER  > > gl_vertex_shader_keeper;
    typedef gl_resource<gl_shader_gen<GL_FRAGMENT_SHADER> > gl_fragment_shader_keeper;
    
    typedef shared_ptr<gl_buffer_keeper > gl_buffer_ptr;
    typedef shared_ptr<gl_texture_keeper> gl_texture_ptr;

    typedef shared_ptr<gl_vertex_shader_keeper  > gl_vertex_shader_ptr;
    typedef shared_ptr<gl_fragment_shader_keeper> gl_fragment_shader_ptr;
    typedef shared_ptr<gl_program_keeper> gl_program_ptr;

    inline gl_buffer_ptr  make_gl_buffer ()                 { return make_shared<gl_buffer_keeper>();         }
    inline gl_buffer_ptr  use_gl_buffer  (GLuint id)        { return make_shared<gl_buffer_keeper>(id);       }
    inline gl_texture_ptr make_gl_texture()                 { return make_shared<gl_texture_keeper>();        }
    inline gl_texture_ptr use_gl_texture (GLuint id)        { return make_shared<gl_texture_keeper>(id);      }
    inline gl_vertex_shader_ptr   make_gl_vertex_shader()   { return make_shared<gl_vertex_shader_keeper>();  }
    inline gl_fragment_shader_ptr make_gl_fragment_shader() { return make_shared<gl_fragment_shader_keeper>();}
    inline gl_program_ptr make_gl_program()                 { return make_shared<gl_program_keeper>();        }
    inline gl_program_ptr use_gl_program (GLuint id)        { return make_shared<gl_program_keeper>(id);      }
}

