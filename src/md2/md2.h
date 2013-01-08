#pragma once

#include "../shared/resources.h"

namespace cg_homework
{
    class md2_model
    {
    public:
        md2_model();
        ~md2_model();

        bool load(istream &src);
        inline size_t num_frames() const;
        void draw(int frame) const;

    public:
        struct anim
        {
            string name;
            int begin, end;
        };

        const vector<anim> &animations() const { return anims_; }

    private:
        void cleanup();
        void load_animations(vector<string> &names);

    private:
        typedef GLuint handle;
        vector<gl_buffer_ptr> frames_;
        gl_buffer_ptr texcoords_;
        gl_buffer_ptr indices_;

        size_t n_verts_;
        size_t n_texcoords_;
        size_t n_indices_;

        vector<anim> anims_;
    };

}

  
    