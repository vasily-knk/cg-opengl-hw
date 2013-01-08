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

        typedef shared_ptr<gl_resource<G> > ptr;
        
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

    typedef gl_resource<gl_buffer_gen > gl_buffer;
    typedef gl_resource<gl_texture_gen> gl_texture;
    typedef gl_resource<gl_program_gen> gl_program;

    typedef gl_resource<gl_shader_gen<GL_VERTEX_SHADER  > > gl_vertex_shader;
    typedef gl_resource<gl_shader_gen<GL_FRAGMENT_SHADER> > gl_fragment_shader;
    
    typedef shared_ptr<gl_buffer > gl_buffer_ptr;
    typedef shared_ptr<gl_texture> gl_texture_ptr;

    typedef shared_ptr<gl_vertex_shader  > gl_vertex_shader_ptr;
    typedef shared_ptr<gl_fragment_shader> gl_fragment_shader_ptr;
    typedef shared_ptr<gl_program> gl_program_ptr;

    template<typename T>
    shared_ptr<T> gl_make()
    {
        return make_shared<T>();
    }

    template<typename T>
    shared_ptr<T> gl_use(GLuint id)
    {
        return make_shared<T>(id);
    }
}

