#include "stdafx.h"
#include "md2.h"

namespace cg_homework
{
    md2_model::md2_model()
    {

    }

    md2_model::~md2_model()
    {
        cleanup();
    }

    void md2_model::cleanup()
    {
        frames_.clear();
    }

    size_t md2_model::num_frames() const
    {
        return frames_.size();
    }

    void md2_model::draw(int frame) const
    {
        glBindBuffer(GL_ARRAY_BUFFER, frames_[frame]->id());
        glVertexPointer(3, GL_FLOAT, 0, NULL);
        glBindBuffer(GL_ARRAY_BUFFER, texcoords_->id());
        glTexCoordPointer(2, GL_FLOAT, 0, NULL);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_->id());
        glDrawElements(GL_TRIANGLES, n_indices_, GL_UNSIGNED_INT, NULL);
    }


} // namespace cg_homework
