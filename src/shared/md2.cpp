// md2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "md2.h"

namespace cg_homework
{
    md2_model::md2_model()
        : texcoords_(0)
        , indices_(0)
    {

    }

    md2_model::~md2_model()
    {
        cleanup();
    }

    void md2_model::cleanup()
    {
        if (!frames_.empty())
            glDeleteBuffers(frames_.size(), &(frames_[0]));

        if (texcoords_ != 0)
            glDeleteBuffers(1, &texcoords_);

        if (indices_ != 0)
            glDeleteBuffers(1, &indices_);

        frames_.clear();
        texcoords_ = 0;
        indices_ = 0;
    }

    size_t md2_model::num_frames() const
    {
        return frames_.size();
    }

    void md2_model::draw(int frame) const
    {
        glBindBuffer(GL_ARRAY_BUFFER, frames_.at(frame));
        glVertexPointer(3, GL_FLOAT, 0, NULL);
        glBindBuffer(GL_ARRAY_BUFFER, texcoords_);
        glTexCoordPointer(2, GL_FLOAT, 0, NULL);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_);
        glDrawElements(GL_TRIANGLES, n_indices_, GL_UNSIGNED_INT, NULL);
    }

    namespace
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

        template<typename T>
        T read_struct(istream &src)
        {
            T t;
            src.read(reinterpret_cast<char*>(&t), sizeof(T));
            return t;
        }
    }

    bool md2_model::load(istream &src)
    {
        struct vertex
        {
            float x, y, z;
        };
        
        struct texcoord
        {
            float u, v;
        };

        typedef vector<vertex> frame_buffer_t;

        const auto header = read_struct<header_t>(src);
        vector<set<int> > mappings(header.num_xyz);

        vector<frame_buffer_t> frames_buffer;
        vector<texcoord> texcoords_buffer;
        vector<int> indices_buffer;

        auto load_indices = [&]() 
        {
            src.seekg(header.ofs_tris);
            for (int n = 0; n < header.num_tris; ++n)
            {
                const auto tri_desc = read_struct<triangle_t>(src);
                for (int i = 0; i < 3; ++i)
                {
                    auto &m = mappings[tri_desc.index_xyz[i]];
                    m.insert(tri_desc.index_st[i]);
                    indices_buffer[(n + 1) * 3 - (i + 1)] = tri_desc.index_st[i];
                }
            }
        };

        auto load_frames = [&]()
        {
            src.seekg(header.ofs_frames);
            for (int n_frame = 0; n_frame < header.num_frames; ++n_frame)
            {
                const auto frame_desc = read_struct<frame_t>(src);
                frame_buffer_t &frame = frames_buffer[n_frame];

                for (int n_vertex = 0; n_vertex < header.num_xyz; ++n_vertex)
                {
                    const auto vertex_desc = read_struct<vertex_t>(src);

                    for (auto it = mappings[n_vertex].begin(); it != mappings[n_vertex].end(); ++it)
                    //BOOST_FOREACH(int st, mappings[n_vertex])
                    {
                        const auto &st = *it;
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
                const auto texcoord_desc = read_struct<texcoord_t>(src);
                texcoords_buffer[n_texcoord].u = float(texcoord_desc.s) / header.skinwidth;
                texcoords_buffer[n_texcoord].v = float(texcoord_desc.t) / header.skinheight;
            }
        };

        const frame_buffer_t f(header.num_st);
        frames_buffer.resize(header.num_frames, f);
        indices_buffer.resize(header.num_tris * 3);
        texcoords_buffer.resize(header.num_st);

        load_indices();
        load_frames();
        load_texcoords();

        frames_.resize(frames_buffer.size());
        glGenBuffers(frames_.size(), &(frames_[0]));

        for (size_t i = 0; i < frames_.size(); ++i)
        {
            n_verts_ = frames_buffer[i].size();
            auto verts_ptr = &(frames_buffer[i][0]);
            glBindBuffer(GL_ARRAY_BUFFER, frames_[i]);
            glBufferData(GL_ARRAY_BUFFER, n_verts_ * sizeof(vertex), verts_ptr, GL_STATIC_DRAW);
        }

        n_texcoords_ = texcoords_buffer.size();
        auto texcoords_ptr = &(texcoords_buffer[0]);
        glGenBuffers(1, &texcoords_);
        glBindBuffer(GL_ARRAY_BUFFER, texcoords_);
        glBufferData(GL_ARRAY_BUFFER, n_texcoords_ * sizeof(texcoord), texcoords_ptr, GL_STATIC_DRAW);

        n_indices_ = indices_buffer.size();
        auto indices_ptr = &(indices_buffer[0]);
        glGenBuffers(1, &indices_);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, n_indices_ * sizeof(int), indices_ptr, GL_STATIC_DRAW);

        return true;
    }

} // namespace cg_homework
