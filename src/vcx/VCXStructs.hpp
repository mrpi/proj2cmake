#pragma once

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <memory>
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
   Application,
   Utility,
   Makefile
};

struct ProjectInfo
{
   std::string guid;
   std::string name;
   fs::path projectFile;
   ProjectInfo() = default;

   ProjectInfo(const ProjectInfo&) = default;

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
    Project() {
        bMultiThreaded = false;
		bMultiThreadedDebug = false;
        spCompileFiles = std::make_shared<std::vector<fs::path>>();
		spIncludeFiles = std::make_shared<std::vector<fs::path>>();
        spAdditionalIncludeDirs = std::make_shared<std::vector<fs::path>>();
        spAdditionalLinkDirs = std::make_shared<std::vector<fs::path>>();
		spReferencedProjects = std::make_shared<std::vector<ProjectInfo>>();
   }
   ConfigurationType type;

   bool bMultiThreaded;
   bool bMultiThreadedDebug;
   std::shared_ptr<std::vector<fs::path>> spCompileFiles;
   std::shared_ptr<std::vector<fs::path>> spIncludeFiles;
   std::shared_ptr<std::vector<fs::path>> spAdditionalIncludeDirs;
   std::shared_ptr<std::vector<fs::path>> spAdditionalLinkDirs;
   std::shared_ptr<std::vector<ProjectInfo>> spReferencedProjects;
};


struct Solution
{
  fs::path basePath;
  std::string name;
  std::map<ProjectInfo, Project> projects;
};

using ProjectPair = std::pair<const ProjectInfo, Project>;
using ProjectsPaths = std::map<fs::path, std::vector<std::shared_ptr<ProjectPair>>>;
};
}
