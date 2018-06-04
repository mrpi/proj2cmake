#include "CMakeListsWriter.hpp"

#include "CMakeMsc.hpp"

using namespace proj2cmake;

void cmake::ListsWriter::operator()(std::ostream& os)
{
   os << "SET(" << cmake::tokenize(mProject.first.name) << "_SRC" << std::endl;
   for(auto&& f : *mProject.second.spCompileFiles)
   {
      os << "    " << f << std::endl;
   }
   os << "   )" << std::endl;
   
   os << std::endl;
   
   os << "SET(" << cmake::tokenize(mProject.first.name) << "_DEPS" << std::endl;
   for(auto&& proc : *mProject.second.spReferencedProjects)
   {
      os << "    " << cmake::tokenize(proc.name) << std::endl;
   }
   os << "   )" << std::endl;

   os << "SET(" << cmake::tokenize(mProject.first.name) << "_ADDITIONAL_INCLUDE_DIRS" << std::endl;
   for (auto&& dir : *mProject.second.spAdditionalIncludeDirs)
   {
       os << "    " << dir << std::endl;
   }
   os << "   )" << std::endl;

   os << "SET(" << cmake::tokenize(mProject.first.name) << "_ADDITIONAL_LINK_DIRS" << std::endl;
   for (auto&& dir : *mProject.second.spAdditionalLinkDirs)
   {
       os << "    " << dir << std::endl;
   }
   os << "   )" << std::endl;
}
