#include "VCXParser.hpp"
#include <iostream>
#include <cerrno>
using namespace proj2cmake::vcx;

namespace
{
inline std::string pathToNative(std::string&& in)
{
   for(auto& c : in)
      if(c == '\\')
         c = '/';
   return in;
}

inline void toUpper(char& c)
{
   if(c >= 'a' && c <= 'z')
      c += 'A' - 'a';
}

inline std::string toUpper(std::string&& in)
{
   for(auto& c : in)
      toUpper(c);
   return in;
}
}

fs::path SolutionParser::basePath() const
{
   return mSlnFile.parent_path();
}

SolutionParser::SolutionParser(fs::path slnFile)
   : mSlnFile(std::move(slnFile))
{}

Solution SolutionParser::operator()()
{
   Solution res;

   res.basePath = basePath();
   res.name = mSlnFile.stem().string();

   auto infos = parseSolution(mSlnFile);
   for(auto&& projInfo : infos) {
	   try {
		   res.projects[projInfo] = parseProject(projInfo);
	   }
	   catch (std::runtime_error e) {
		   std::cerr << e.what() << std::endl;
	   }
   }


   for(auto&& proj : res.projects)
   {
      for(auto&& refProj : proj.second.referencedProjects)
      {
         auto itr = res.projects.find(refProj);
         if(itr == res.projects.end())
            std::cerr << "Warning: Project " << refProj.guid << " is referenced but can not be found" << std::endl;
         else
            refProj.name = itr->first.name;
      }
   }

   return res;
}

Project SolutionParser::parseProject(const ProjectInfo& projInfo)
{
   Project res;

   auto projFilePath = projInfo.projectFile;
   if(!projFilePath.is_absolute())
      projFilePath = basePath() / projFilePath;

   boost::property_tree::ptree pt;
   std::cout << "Parsing file: " << projFilePath << std::endl;
   if(fs::exists(projFilePath) == false)
      throw std::runtime_error("Project file '"+ projFilePath.string() + "' not found!");
   boost::property_tree::xml_parser::read_xml(projFilePath.string(), pt, 0, std::locale());

   std::string type;

   auto project = pt.get_child("Project");
   for(auto&& pp : project)
   {
      if(type.empty() && pp.first == "PropertyGroup")
      {
         type = pp.second.get<std::string>("ConfigurationType", "");
         continue;
      }
      else if(pp.first != "ItemGroup")
         continue;

      for(auto&& ip : pp.second)
      {
         if(ip.first == "ClCompile")
         {
            auto f = ip.second.get<std::string>("<xmlattr>.Include");
            f = pathToNative(std::move(f));
            res.compileFiles.push_back(f);
         }
         else if(ip.first == "ClInclude")
         {
            auto f = ip.second.get<std::string>("<xmlattr>.Include");
            f = pathToNative(std::move(f));
            res.includeFiles.push_back(f);
         }
         else if(ip.first == "ProjectReference")
         {
            ProjectInfo pInfo;
            auto f = ip.second.get<std::string>("<xmlattr>.Include");
            pInfo.projectFile = pathToNative(std::move(f));
            pInfo.guid = toUpper(ip.second.get<std::string>("Project"));
            res.referencedProjects.push_back(pInfo);
         }
      }
   }

   if (type == "Application")
	   res.type = ConfigurationType::Application;
   else if (type == "DynamicLibrary")
	   res.type = ConfigurationType::DynamicLibrary;
   else if (type == "StaticLibrary")
	   res.type = ConfigurationType::StaticLibrary;
   else if (type == "Utility")
	   res.type = ConfigurationType::Utility;
   else if (type == "Makefile")
	   res.type = ConfigurationType::Makefile;
   else
      throw std::runtime_error("Project '" + projInfo.name + "' has an invalid ConfigurationType ('" + type + "')");

   std::cout << "  Type: " << type << ", CompileFiles: " << res.compileFiles.size() << ", IncludeFiles: " << res.includeFiles.size() << ", ProjectReferences: " << res.referencedProjects.size() << std::endl;

   std::sort(begin(res.compileFiles), end(res.compileFiles));
   std::sort(begin(res.includeFiles), end(res.includeFiles));
   std::sort(begin(res.referencedProjects), end(res.referencedProjects));

   return res;
}

std::vector<ProjectInfo> SolutionParser::parseSolution(std::istream& is)
{
   std::vector<ProjectInfo> res;

   while(is.good())
   {
      std::string line;

      std::getline(is, line);

      if(boost::starts_with(line, "Project("))
      {
         ProjectInfo sol;

         auto pos = line.find(" = ");
         if(pos == std::string::npos)
            throw std::runtime_error("Invalid solution file!");

         auto val = line.substr(pos+4);
         auto end = val.find('\"');
         sol.name = val.substr(0, end);

         val = val.substr(end+1);
         val = val.substr(val.find('\"')+1);
         end = val.find('\"');
         auto projFile = pathToNative(val.substr(0, end));
         if(boost::iends_with(projFile, ".vcxproj") == false)
            continue;
         sol.projectFile = projFile;

         val = val.substr(end+1);
         val = val.substr(val.find('\"')+1);
         sol.guid = toUpper(val.substr(0, val.find('\"')));

         res.push_back(sol);
      }
   }

   return res;
}

std::vector<ProjectInfo> SolutionParser::parseSolution(const fs::path& solutionFile)
{
   std::fstream is(solutionFile.native(), std::fstream::in);
   if (!is.is_open())
	   throw std::runtime_error("Error opening file: " + std::string(strerror(errno)));
   return parseSolution(is);
}
