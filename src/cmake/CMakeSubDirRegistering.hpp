#pragma once

#include <boost/filesystem/path.hpp>

namespace proj2cmake
{
namespace cmake
{
   
class CMakeSubDirRegistering
{
private:
   std::ostream& mOs;

public:
   CMakeSubDirRegistering(std::ostream& os)
    : mOs(os)
   {}

   void operator()(const boost::filesystem::path& subDir);
};

}
}
