#pragma once

#include <string>

#include "../vcx/VCXStructs.hpp"

namespace proj2cmake
{
namespace cmake
{

std::string cmakeStartType(const std::string& name, vcx::ConfigurationType type);
   
std::string tokenize(const std::string& name);

}
}
