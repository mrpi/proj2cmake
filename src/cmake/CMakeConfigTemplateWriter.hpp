#pragma once

#include <string>
#include <iostream>

#include "../vcx/VCXStructs.hpp"

namespace proj2cmake
{
namespace cmake
{

class ConfigTemplateWriter
{
private:
   vcx::Solution mSolution;
   
public:
   ConfigTemplateWriter(vcx::Solution solution)
    : mSolution(std::move(solution))
   {}

   void operator()(std::ostream& os);
};

}
}
