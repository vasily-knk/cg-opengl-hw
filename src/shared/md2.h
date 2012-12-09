#pragma once

#include "resources.h"

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

    private:
        void cleanup();

    private:
        typedef GLuint handle;
        vector<gl_buffer> frames_;
        gl_buffer texcoords_;
        gl_buffer indices_;

        size_t n_verts_;
        size_t n_texcoords_;
        size_t n_indices_;
    };

}

  
    