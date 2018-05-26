#include "CMakeMsc.hpp"
#include "CMakeListsWriter.hpp"
#include "CMakeConfigTemplateWriter.hpp"
#include "CMakeSubDirRegistering.hpp"

using namespace proj2cmake;

const char* cmake::cmakeTypeCaption(vcx::ConfigurationType type)
{
   switch(type)
   {
   case vcx::ConfigurationType::Application:
      return "APP";
   case vcx::ConfigurationType::StaticLibrary:
      return "STATIC_LIB";
   case vcx::ConfigurationType::DynamicLibrary:
      return "SHARED_LIB";
   case vcx::ConfigurationType::Utility:
      return "UTILITY";
   case vcx::ConfigurationType::Makefile:
      return "MAKEFILE";
   }

   throw std::logic_error("Unhandled config type! (cmake::cmakeTypeCaption)");
}

std::string cmake::cmakeStartType(const std::string& name, vcx::ConfigurationType type)
{
   std::string res;
   if(type == vcx::ConfigurationType::Application)
      res = "ADD_EXECUTABLE(";
   else
      res = "ADD_LIBRARY(";

   res += cmake::tokenize(name);

   if(type == vcx::ConfigurationType::StaticLibrary)
      res += " STATIC";
   else if(type == vcx::ConfigurationType::DynamicLibrary)
      res += " SHARED";

   return res;
}

std::string cmake::tokenize(const std::string& name)
{
   std::string res;

   for(char c : name)
   {
      switch(c)
      {
      case ' ':
         res += '_';
         break;
      case '(':
      case ')':
         break;
      default:
         res += c;
         break;
      }
   }

   return res;
}


void cmake::writeGeneratedNote(std::ostream& os)
{
    os << "#" << std::endl;
    os << "# This file was genared by proj2cmake and will be overwritten on it's next run!" << std::endl;
    os << "# Please put all configurations in the cmake_conf/*.cmake files." << std::endl;
    os << "#" << std::endl;
    os << std::endl;
}

void cmake::writeProject(std::ofstream& os, const vcx::Solution& solution, const vcx::ProjectsPaths& dirToProj)
{
    os << "PROJECT(" << solution.name << ")" << std::endl;
    os << std::endl;
    os << "set(CMAKE_INCLUDE_CURRENT_DIR ON)" << std::endl;
    os << std::endl;
    fs::path confFile = "cmake_conf";
    confFile /= (solution.name + ".cmake");

    //os << "IF(EXISTS \"${" << solution.name << "_SOURCE_DIR}/cmake_conf/" << solution.name << ".cmake\")" << std::endl;
    os << "INCLUDE(\"${" << solution.name << "_SOURCE_DIR}/cmake_conf/" << solution.name << ".cmake\")" << std::endl;
    //os << "ENDIF()" << std::endl;
    os << std::endl;

    auto fullConfPath = solution.basePath / confFile;
    if (fs::exists(fullConfPath) == false)
    {
        fs::create_directories(fullConfPath.parent_path());
        cmake::ConfigTemplateWriter writer(solution);
        std::ofstream os(fullConfPath.native());
        writer(os);
    }

    cmake::CMakeSubDirRegistering subDirRegister(os);
    for (auto&& subDir : dirToProj)
    {
        if (solution.basePath == subDir.first)
            continue;

        subDirRegister(subDir.second[0]->first.projectFile.parent_path());
    }
    os << std::endl;
}

void cmake::writeSolution(const vcx::Solution& solution)
{

    vcx::ProjectsPaths dirToProj;

    for (auto p : solution.projects)
    {
        auto&& pInfo = p.first;
        auto&& project = p.second;

        if (project.spCompileFiles->empty())
            continue;

        auto cmakeSrcFile = solution.basePath / pInfo.projectFile;
        cmakeSrcFile.replace_extension(".cmake");

        cmake::ListsWriter writer(p);

        std::ofstream os(cmakeSrcFile.native());
        cmake::writeGeneratedNote(os);
        writer(os);
        dirToProj[cmakeSrcFile.parent_path()].push_back(std::make_shared<vcx::ProjectPair>(p));
    }

    bool hasProject = false;

    for (auto p : dirToProj)
    {
        auto f = p.first / "CMakeLists.txt";
        std::ofstream os(f.native());
        cmake::writeGeneratedNote(os);

        os << "cmake_minimum_required(VERSION 2.8)" << std::endl;
        os << std::endl;
        if (p.first == solution.basePath)
        {
            cmake::writeProject(os, solution, dirToProj);
            hasProject = true;
        }

        for (auto&& pr : p.second)
        {
            auto&& pInfo = pr->first;
            auto&& project = pr->second;

            os << std::endl;

            auto cmakeSrcFile = solution.basePath / pInfo.projectFile;
            cmakeSrcFile.replace_extension(".cmake");
            std::cout << "Generating cmake " << cmakeSrcFile.string() << std::endl;

            os << "INCLUDE(\"" + cmakeSrcFile.filename().string() + "\")" << std::endl;
            os << std::endl;
            if (project.bMultiThreaded)
                os << "set(CMAKE_CXX_FLAGS_RELEASE \"/MT\")" << std::endl;
            if (project.bMultiThreadedDebug)
                os << "set(CMAKE_CXX_FLAGS_DEBUG \"/MTd\")" << std::endl;
            os << std::endl;

            os << "TARGET_INCLUDE_DIRECTORIES(" << cmake::tokenize(pInfo.name) << std::endl;
            os << "            ${" << cmake::tokenize(pInfo.name) << "_ADDITIONAL_INCLUDE_DIRS})" << std::endl;
            os << std::endl;

            os << "LINK_DIRECTORIES(" << cmake::tokenize(pInfo.name) << std::endl;
            os << "            ${" << cmake::tokenize(pInfo.name) << "_ADDITIONAL_LINK_DIRS})" << std::endl;
            os << std::endl;

            os << cmake::cmakeStartType(pInfo.name, project.type) << std::endl;
            os << "            ${" << cmake::tokenize(pInfo.name) << "_SRC})" << std::endl;
            os << std::endl;
            os << "TARGET_LINK_LIBRARIES(" << cmake::tokenize(pInfo.name) << std::endl;
            os << "            ${" << cmake::tokenize(pInfo.name) << "_DEPS}" << std::endl;
            os << "            ${" << cmake::tokenize(pInfo.name) << "_ADDITIONAL_DEPS}" << std::endl;
            os << "            ${SOLUTION_" << cmake::cmakeTypeCaption(project.type) << "_DEPS}" << std::endl;
            os << "            ${SOLUTION_GENERAL_DEPS})" << std::endl;
            os << std::endl;
        }
    }

    if (!hasProject)
    {
        auto f = solution.basePath / "CMakeLists.txt";
        std::ofstream os(f.native());
        cmake::writeProject(os, solution, dirToProj);
    }

    std::cout << dirToProj.size() << " projects have been converted" << std::endl;
}