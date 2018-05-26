#include "VCXParser.hpp"
#include <iostream>
#include <cerrno>
using namespace proj2cmake::vcx;

namespace
{
std::string pathToNative(std::string& in)
{
   for(auto& c : in)
      if(c == '\\')
         c = '/';
   return in;
}

void toUpper(char& c)
{
   if(c >= 'a' && c <= 'z')
      c += 'A' - 'a';
}

std::string toUpper(std::string& in)
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
   : mSlnFile(slnFile)
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
       catch (std::exception e) {
           std::cerr << "ERROR: " << e.what() << std::endl;
       }
   }


   for(auto&& proj : res.projects)
   {
      for(auto&& refProj : *proj.second.spReferencedProjects)
      {
         auto itr = res.projects.find(refProj);
         if(itr == res.projects.end())
            std::cerr << "WARNING: Project " << refProj.guid << " is referenced but can not be found" << std::endl;
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

      if (pp.first == "ItemDefinitionGroup") 
      {
          for (auto&& ip : pp.second)
          {
              if (ip.first == "ClCompile")
              {
                  for (auto&& jp : ip.second)
                  {
                      if (jp.first == "AdditionalIncludeDirectories")
                      {
                          auto sAddtionalIncludeDirs = jp.second.data();
                          auto i = 0;
                          auto pos = sAddtionalIncludeDirs.find(';');
                          while (pos != std::string::npos)
                          {
                              auto dir = sAddtionalIncludeDirs.substr(i, pos - i);
                              i = ++pos;
                              pos = sAddtionalIncludeDirs.find(';', pos);
                              dir = pathToNative(dir);
                              if (std::find(res.spAdditionalIncludeDirs->begin(),
                                  res.spAdditionalIncludeDirs->end(), dir) ==
                                  res.spAdditionalIncludeDirs->end())
                                  res.spAdditionalIncludeDirs->push_back(dir);
                          }
                      }
                      else if (jp.first == "RuntimeLibrary")
                      {
                          auto sRuntimeLibrary = jp.second.data();
                          if (sRuntimeLibrary == "MultiThreaded")
                              res.bMultiThreaded = true;
                          else if (sRuntimeLibrary == "MultiThreadedDebug")
                              res.bMultiThreadedDebug = true;
                      }
                  }
              }
              else if (ip.first == "Link")
              {
                  for (auto&& jp : ip.second)
                  {
                      if (jp.first == "AdditionalLibraryDirectories")
                      {
                          auto sAddtionalLinkDirs = jp.second.data();
                          auto i = 0;
                          auto pos = sAddtionalLinkDirs.find(';');
                          while (pos != std::string::npos)
                          {
                              auto dir = sAddtionalLinkDirs.substr(i, pos - i);
                              i = ++pos;
                              pos = sAddtionalLinkDirs.find(';', pos);
                              dir = pathToNative(dir);
                              if (std::find(res.spAdditionalLinkDirs->begin(),
                                  res.spAdditionalLinkDirs->end(), dir) ==
                                  res.spAdditionalLinkDirs->end())
                                  res.spAdditionalLinkDirs->push_back(dir);
                          }
                      }
                      
                  }
              }
          }
      }

      else if(pp.first == "ItemGroup")
      {
          for (auto&& ip : pp.second)
          {
              if (ip.first == "ClCompile")
              {
                  auto f = ip.second.get<std::string>("<xmlattr>.Include");
                  res.spCompileFiles->push_back(pathToNative(f));
              }
              else if (ip.first == "ClInclude")
              {
                  auto f = ip.second.get<std::string>("<xmlattr>.Include");
                  res.spIncludeFiles->push_back(pathToNative(f));
              }
              else if (ip.first == "ProjectReference")
              {
                  ProjectInfo pInfo;
                  auto f = ip.second.get<std::string>("<xmlattr>.Include");
                  pInfo.projectFile = pathToNative(f);
                  pInfo.guid = toUpper(ip.second.get<std::string>("Project"));
                  res.spReferencedProjects->push_back(pInfo);
              }
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

   std::cout << "  Type: " << type << ", CompileFiles: " << res.spCompileFiles->size() << ", IncludeFiles: " << res.spIncludeFiles->size() << ", ProjectReferences: " << res.spReferencedProjects->size() << std::endl;

   std::sort(begin(*res.spCompileFiles), end(*res.spCompileFiles));
   std::sort(begin(*res.spIncludeFiles), end(*res.spIncludeFiles));
   std::sort(begin(*res.spReferencedProjects), end(*res.spReferencedProjects));

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
