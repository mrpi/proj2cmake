#pragma once

#include "VCXStructs.hpp"

namespace proj2cmake
{
namespace vcx
{

class SolutionParser
{
private:
   fs::path mSlnFile;
   
   fs::path basePath() const;
   
public:
   SolutionParser(fs::path slnFile);
   
   Solution operator()();
   
   Project parseProject(const ProjectInfo& projInfo);
   
   std::vector<ProjectInfo> parseSolution(std::istream& is);
   
   std::vector<ProjectInfo> parseSolution(const fs::path& solutionFile);   
};
   
   
}
}
