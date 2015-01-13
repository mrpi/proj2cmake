#include "CMakeSubDirRegistering.hpp"

using namespace proj2cmake;

void cmake::CMakeSubDirRegistering::operator()(const boost::filesystem::path& subDir)
{
   mOs << "ADD_SUBDIRECTORY(" << subDir << ")" << std::endl;
}
