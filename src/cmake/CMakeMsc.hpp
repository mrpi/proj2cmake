#pragma once

#include <string>

#include "../vcx/VCXStructs.hpp"

namespace proj2cmake
{
namespace cmake
{

const char* cmakeTypeCaption(vcx::ConfigurationType type);

std::string cmakeStartType(const std::string& name, vcx::ConfigurationType type);
   
std::string tokenize(const std::string& name);

}
}
