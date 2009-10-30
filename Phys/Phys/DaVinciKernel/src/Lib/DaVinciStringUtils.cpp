// $Id: DaVinciStringUtils.cpp,v 1.1 2009-10-30 15:23:25 jpalac Exp $
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
    
  } // namespace StringUtils
  
} // namespace DaVinci
//=============================================================================
