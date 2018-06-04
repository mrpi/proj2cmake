#include "vcx/VCXParser.hpp"
#include "vcx/VCXStructs.hpp"
#include "cmake/CMakeMsc.hpp"
#include <map>
#include <iostream>

using namespace proj2cmake;


int main(int argc, char** argv)
{
   namespace fs = boost::filesystem;

   auto procName = argv[0];

   if(argc < 2)
   {
      std::cerr << "Usage: " << procName << " <SolutionFile>" << std::endl;
      return 1;
   }

   auto solutionFile = argv[1];

   if(boost::iends_with(solutionFile, ".sln") == false)
   {
      std::cerr << "The first parameter has to be a Visual Studio solution file (*.sln)" << std::endl;
      return 1;
   }

   vcx::SolutionParser parser(solutionFile);

   try {
       cmake::writeSolution(parser());
   }
   catch (std::runtime_error e) {
	   std::cerr << e.what() << std::endl;
	   return 1;
   }

   return 0;
}
