#pragma once

namespace cg_homework
{

    template<typename G>
    class gl_resource_keeper
        : boost::noncopyable
    {
    public:
        explicit gl_resource_keeper(GLuint id)
            : id_(id) {}
        gl_resource_keeper () { G::gen(1, &id_); }
        ~gl_resource_keeper() { G::del(1, &id_); }

        GLuint id() const { return id_; }

    private:
        GLuint id_;
    };

    struct gl_buffer_gen
    {
        static void gen(size_t n, GLuint *dst) { glGenBuffers   (n, dst); }
        static void del(size_t n, GLuint *dst) { glDeleteBuffers(n, dst); }
    };

    struct gl_texture_gen
    {
        static void gen(size_t n, GLuint *dst) { glGenTextures   (n, dst); }
        static void del(size_t n, GLuint *dst) { glDeleteTextures(n, dst); }
    };

    typedef gl_resource_keeper<gl_buffer_gen > gl_buffer_keeper;
    typedef gl_resource_keeper<gl_texture_gen> gl_texture_keeper;
    
    typedef shared_ptr<gl_buffer_keeper > gl_buffer;
    typedef shared_ptr<gl_texture_keeper> gl_texture;

    inline gl_buffer  make_gl_buffer ()          { return make_shared<gl_buffer_keeper>();    }
    inline gl_buffer  use_gl_buffer  (GLuint id) { return make_shared<gl_buffer_keeper>(id);  }
    inline gl_texture make_gl_texture()          { return make_shared<gl_texture_keeper>();   }
    inline gl_texture use_gl_texture (GLuint id) { return make_shared<gl_texture_keeper>(id); }
}

