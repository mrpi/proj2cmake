#include "CMakeMsc.hpp"

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
