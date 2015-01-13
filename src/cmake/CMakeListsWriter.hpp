#pragma once

#include <string>
#include <iostream>

#include "../vcx/VCXStructs.hpp"

namespace proj2cmake
{
namespace cmake
{

class ListsWriter
{
private:
   vcx::ProjectPair mProject;
   
public:
   ListsWriter(vcx::ProjectPair project)
    : mProject(std::move(project))
   {}

   void operator()(std::ostream& os);
};

}
}
