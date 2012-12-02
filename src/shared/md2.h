#pragma once

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

    const vector<frame_t>  &frames   () const {return frames_   ;}
    const vector<texcoord> &texcoords() const {return texcoords_;}
    const vector<int>      &indices  () const {return indices_  ;}

private:    
    vector<frame_t> frames_;
    vector<texcoord> texcoords_;
    vector<int> indices_;

};
    
    


}