#pragma once

#include <string>
#include <vector>
#include <map>
#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace proj2cmake
{
namespace vcx
{

namespace fs = boost::filesystem;

enum class ConfigurationType
{
   StaticLibrary,
   DynamicLibrary,
   Application
};

struct ProjectInfo
{
   std::string guid;
   std::string name;
   fs::path projectFile;
   
   bool operator<(const ProjectInfo& other) const
   {
      return guid < other.guid;
   }
   
   bool operator==(const ProjectInfo& other) const
   {
      return guid == other.guid;
   }
};

struct Project
{
   ConfigurationType type;

   std::vector<fs::path> compileFiles;
   std::vector<fs::path> includeFiles;
   std::vector<ProjectInfo> referencedProjects;
};

using ProjectPair = std::pair<const vcx::ProjectInfo, vcx::Project>;

struct Solution
{
  fs::path basePath;
  std::map<ProjectInfo, Project> projects;
};

}
}
