// $Id: DaVinciStringUtils.cpp,v 1.4 2010-06-22 07:18:19 jpalac Exp $
// Include files 



// local
#include "Kernel/DaVinciStringUtils.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DaVinciStringUtils
//
// 2009-10-30 : Juan PALACIOS
//-----------------------------------------------------------------------------

namespace DaVinci 
{
  namespace StringUtils 
  {
    //=========================================================================
    void stripParticleName(std::string& name)
    {
      if ( 0 == name.find("anti-") ) { name.erase(0,5); }
      std::string::size_type pos = name.find_first_of("+-~") ;
      if ( std::string::npos == pos ) { return; }
      stripParticleName( name.erase( pos , 1 ) ) ;  
    }
    //=========================================================================
    void expandLocations( std::vector<std::string>::iterator begin,
                          std::vector<std::string>::iterator end,
                          const std::string& prefix                )
    {

      for (std::vector<std::string>::iterator loc = begin; loc!=end; ++loc) {
        if ( (*loc).find("/") == std::string::npos ) {
          *loc = prefix + "/" + *loc;
        }

      }
    }
    //=========================================================================
    void expandLocation( std::string& location,
                         const std::string& prefix) 
    {
      if ( location.find("/") == std::string::npos ) {
          location = prefix + "/" + location;
      }
    }
    //=========================================================================
    void removeEnding(std::string& a, const std::string& ending)
    {
      std::string::size_type pos = a.rfind(ending);
      if ( pos != std::string::npos ) {
        a = std::string(a, 0, pos);
      }
    }
    
  } // namespace StringUtils
  
} // namespace DaVinci
//=============================================================================
