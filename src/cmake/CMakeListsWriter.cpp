#include "CMakeListsWriter.hpp"

#include "CMakeMsc.hpp"

using namespace proj2cmake;

void cmake::ListsWriter::operator()(std::ostream& os)
{
   os << "SET(" << cmake::tokenize(mProject.first.name) << "_SRC" << std::endl;
   for(auto&& f : mProject.second.compileFiles)
   {
      os << "    " << f << std::endl;
   }
   os << "   )" << std::endl;
   
   os << std::endl;
   
   os << "SET(" << cmake::tokenize(mProject.first.name) << "_DEPS" << std::endl;
   for(auto&& proc : mProject.second.referencedProjects)
   {
      os << "    " << cmake::tokenize(proc.name) << std::endl;
   }
   os << "   )" << std::endl;
}
