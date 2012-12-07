#pragma once


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
        vector<handle> frames_;
        handle texcoords_;
        handle indices_;

        size_t n_verts_;
        size_t n_texcoords_;
        size_t n_indices_;
    };

}

namespace md2
{

class model
{
public:
    struct vertex
    {
        float x, y, z;
    };
    struct texcoord
    {
        float u, v;
    };
    typedef vector<vertex> frame_t;

public:
    void load_md2(istream &src);

    const vector<frame_t>  &frames   () const {return frames_buffer   ;}
    const vector<texcoord> &texcoords() const {return texcoords_buffer;}
    const vector<int>      &indices  () const {return indices_buffer  ;}

private:    
    vector<frame_t> frames_buffer;
    vector<texcoord> texcoords_buffer;
    vector<int> indices_buffer;

};
    
    


}