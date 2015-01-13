#include "CMakeSrcWriter.hpp"

#include "CMakeMsc.hpp"

using namespace proj2cmake;

void cmake::SourcesListWriter::operator()(std::ostream& os)
{
   os << "SET(" << cmake::tokenize(mProject.first.name) << "_SRC" << std::endl;
   for(auto&& f : mProject.second.compileFiles)
   {
      os << "    " << f << std::endl;
   }
   os << "   )" << std::endl;
}
