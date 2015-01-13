#include "vcx/VCXParser.hpp"

#include "cmake/CMakeSrcWriter.hpp"
#include "cmake/CMakeMsc.hpp"
#include "cmake/CMakeSubDirRegistering.hpp"

#include <map>

using namespace proj2cmake;

std::string ProcName;

void writeGeneratedNote(std::ostream& os)
{
   os << "#" << std::endl;
   os << "# This file was genared by " << ProcName << " and will be overwritten on it's next run!" << std::endl;
   os << "# Please put all configurations in the *_conf.cmake files." << std::endl;
   os << "#" << std::endl;
   os << std::endl;
}


int main(int argc, char** argv)
{
   namespace fs = boost::filesystem;
   
   ProcName = argv[0];

   if(argc < 2)
   {
      std::cerr << "Usage: " << ProcName << " <SolutionFile>" << std::endl;
      return 1;
   }
   
   auto solutionFile = argv[1];
   
   if(boost::iends_with(solutionFile, ".sln") == false)
   {
      std::cerr << "The first parameter has to be a Visual Studio solution file (*.sln)" << std::endl;
      return 1;
   }
      
   vcx::SolutionParser parser(solutionFile);
   auto solution = parser();
   
   std::map<fs::path, std::vector<vcx::ProjectPair*>> dirToProj;
   
   for(auto&& p : solution.projects)
   {
      auto&& pInfo = p.first;
      auto&& project = p.second;
      
      if(project.compileFiles.empty())
         continue;
      
      auto cmakeSrcFile = solution.basePath / pInfo.projectFile;
      cmakeSrcFile.replace_extension(".cmake");
      cmake::SourcesListWriter writer(p);

      std::ofstream os(cmakeSrcFile.native());
      writeGeneratedNote(os);
      writer(os);
      
      dirToProj[cmakeSrcFile.parent_path()].push_back(&p);
   }

   for(auto&& p : dirToProj)
   {
      auto f = p.first / "CMakeLists.txt";
      std::ofstream os(f.native());
      writeGeneratedNote(os);

      os << "cmake_minimum_required(VERSION 2.8)" << std::endl;
      os << std::endl;
      
      if(p.first == solution.basePath)
      {
         os << "PROJECT(" << solution.name << ")" << std::endl;
         os << std::endl;
         os << "IF(EXISTS \"" << solution.name << "_conf.cmake\")" << std::endl;
         os << "   INCLUDE(\"" << solution.name << "_conf.cmake\")" << std::endl;
         os << "ENDIF()" << std::endl;
         os << std::endl;
         cmake::CMakeSubDirRegistering subDirRegister(os);
         for(auto&& subDir : dirToProj)
         {
            if(p.first == subDir.first)
               continue;
            
            subDirRegister(subDir.second[0]->first.projectFile.parent_path());
         }
         os << std::endl;
      }
      
      for(auto&& pr : p.second)
      {
         auto&& pInfo = pr->first;
         auto&& project = pr->second;
      
         os << std::endl;
         
         auto cmakeSrcFile = solution.basePath / pInfo.projectFile;
         cmakeSrcFile.replace_extension(".cmake");
         
         os << "INCLUDE(\"" + cmakeSrcFile.filename().string() + "\")" << std::endl;
         os << std::endl;
         os << cmake::cmakeStartType(pInfo.name, project.type) << std::endl;
         os << "            ${" << cmake::tokenize(pInfo.name) << "_SRC})" << std::endl;
         os << std::endl;
      }
   }
   
   return 0;
}
