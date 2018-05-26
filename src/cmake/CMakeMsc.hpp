#pragma once

#include <string>

#include "../vcx/VCXStructs.hpp"

namespace fs = boost::filesystem;

namespace proj2cmake
{
namespace cmake
{

const char* cmakeTypeCaption(vcx::ConfigurationType type);

std::string cmakeStartType(const std::string& name, vcx::ConfigurationType type);
   
std::string tokenize(const std::string& name);

void writeGeneratedNote(std::ostream& os);

void writeProject(std::ofstream& os, const vcx::Solution& solution, const vcx::ProjectsPaths& dirToProj);

void writeSolution(const vcx::Solution& solution);

}
}
