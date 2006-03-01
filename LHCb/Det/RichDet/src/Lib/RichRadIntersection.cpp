// $Id: RichRadIntersection.cpp,v 1.1 2006-03-01 14:53:02 papanest Exp $
// Include files 



// local
#include "RichDet/RichRadIntersection.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRadIntersection
//
// 2006-02-28 : Antonis Papanestis
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichRadIntersection::RichRadIntersection(  ) :
  m_radiator ( 0 )
{

}

//=========================================================================
//  
//=========================================================================
RichRadIntersection::RichRadIntersection (Gaudi::XYZPoint entryPoint,
                                          Gaudi::XYZPoint exitPoint,
                                          const DeRichRadiator* rad ) :
  m_entryPoint ( entryPoint ),
  m_exitPoint  ( exitPoint ),
  m_radiator   ( rad )

{
}
//=============================================================================
// Destructor
//=============================================================================
RichRadIntersection::~RichRadIntersection() {} 

//=============================================================================
