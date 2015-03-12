// boost
#include "boost/filesystem/path.hpp"
#include "boost/filesystem.hpp"

// GaudiKernel
#include <GaudiKernel/System.h>

// local
#include "Utils.h"


std::string resolveEnv(const std::string& in)
{
  /* cp'd directly from $L0MUONROOT/src/component/SubstituteEnvVarInPath.cpp */
  boost::filesystem::path path(in);
  boost::filesystem::path::iterator i = path.begin();
  boost::filesystem::path out;
  while (i != path.end()) 
  {
    if ( *(i->c_str())=='$' ) 
    {
      std::string x = System::getEnv( i->c_str()+1 );
      out /= x.empty() ? *i : x ;
    }
    else
    {
      out /= *i;
    }
    ++i;
  }
  return out.string();
}
