#pragma once

namespace cg_homework
{

struct file_not_found_exception
    : std::runtime_error
{
    file_not_found_exception(const string &filename)
        : std::runtime_error(string("File not found: ") + filename)
    {}
};

}