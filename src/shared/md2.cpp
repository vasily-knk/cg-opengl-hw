// md2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "md2.h"

namespace md2
{

namespace file
{
#pragma pack(push)
#pragma pack(1)

typedef struct
{
    int     ident;              // magic number. must be equal to "IDP2"
    int     version;            // md2 version. must be equal to 8

    int     skinwidth;          // width of the texture
    int     skinheight;         // height of the texture
    int     framesize;          // size of one frame in bytes

    int     num_skins;          // number of textures
    int     num_xyz;            // number of vertices
    int     num_st;             // number of texture coordinates
    int     num_tris;           // number of triangles
    int     num_glcmds;         // number of opengl commands
    int     num_frames;         // total number of frames

    int     ofs_skins;          // offset to skin names (64 bytes each)
    int     ofs_st;             // offset to s-t texture coordinates
    int     ofs_tris;           // offset to triangles
    int     ofs_frames;         // offset to frame data
    int     ofs_glcmds;         // offset to opengl commands
    int     ofs_end;            // offset to end of file
} header_t;

typedef float vec3_t[3];

// vertex
typedef struct
{
    unsigned char   v[3];                // compressed vertex (x, y, z) coordinates
    unsigned char   lightnormalindex;    // index to a normal vector for the lighting
} vertex_t;

typedef struct
{
    short    s;
    short    t;
} texcoord_t;

// frame
typedef struct
{
    float       scale[3];       // scale values
    float       translate[3];   // translation vector
    char        name[16];       // frame name
    //vertex_t    verts[1];       // first vertex of this frame
} frame_t;

typedef struct
{
    short   index_xyz[3];    // indexes to triangle's vertices
    short   index_st[3];     // indexes to vertices' texture coorinates
} triangle_t;

#pragma pack(pop)

}

template<typename T>
T read_struct(istream &src)
{
    T t;
    src.read(reinterpret_cast<char*>(&t), sizeof(T));
    return t;
}
    
void model::load_md2(istream &src)
{
    const auto header = read_struct<file::header_t>(src);
    vector<vector<int> > mappings(header.num_xyz);
    
    auto load_indices = [&]() 
    {
        src.seekg(header.ofs_tris);
        for (int n = 0; n < header.num_tris; ++n)
        {
            const auto tri_desc = read_struct<file::triangle_t>(src);
            for (int i = 0; i < 3; ++i)
            {
                mappings[tri_desc.index_xyz[i]].push_back(tri_desc.index_st[i]);
                indices_[n * 3 + 0] = tri_desc.index_st[i];
            }
        }
    };

    auto load_frames = [&]()
    {
        src.seekg(header.ofs_frames);
        for (int n_frame = 0; n_frame < header.num_frames; ++n_frame)
        {
            const auto frame_desc = read_struct<file::frame_t>(src);
            frame_t &frame = frames_[n_frame];

            for (int n_vertex = 0; n_vertex < header.num_xyz; ++n_vertex)
            {
                const auto vertex_desc = read_struct<file::vertex_t>(src);

                BOOST_FOREACH(int st, mappings[n_vertex])
                {
                    frame[st].x = float(vertex_desc.v[0]) * frame_desc.scale[0] + frame_desc.translate[0];
                    frame[st].y = float(vertex_desc.v[1]) * frame_desc.scale[1] + frame_desc.translate[1];
                    frame[st].z = float(vertex_desc.v[2]) * frame_desc.scale[2] + frame_desc.translate[2];
                }
            }
        }
    };

    auto load_texcoords = [&]()
    {
        src.seekg(header.ofs_st);
        for (int n_texcoord = 0; n_texcoord < header.num_st; ++n_texcoord)
        {
            const auto texcoord_desc = read_struct<file::texcoord_t>(src);
            texcoords_[n_texcoord].u = float(texcoord_desc.s) / header.skinwidth;
            texcoords_[n_texcoord].v = float(texcoord_desc.t) / header.skinheight;
        }
    };

    const frame_t f(header.num_st);
    frames_.resize(header.num_frames, f);
    indices_.resize(header.num_tris * 3);
    texcoords_.resize(header.num_st);

    load_frames();
    load_indices();
    load_texcoords();
}

} // namespace md2

