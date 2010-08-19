// $Id: Particle2LHCbIDVectorMap.cpp,v 1.1 2010-08-19 15:09:43 jpalac Exp $
// Include files 



// local
#include "MicroDST/Particle2LHCbIDVectorMap.h"

// anonymous namespace to hide local symbols:
namespace 
{
   
  // ===========================================================================
  /** @var CLID_Candidate 
   *  Class ID definition 
   */
  static const CLID CLID_Particle2LHCbIDVectorMap = 880;
  
  // ==========================================================================
}
// ============================================================================
// Class ID 
// ============================================================================
const CLID& Particle2LHCbIDVectorMap::classID() 
{
  return CLID_Particle2LHCbIDVectorMap;
}
//-----------------------------------------------------------------------------
// Implementation file for class : Particle2LHCbIDVectorMap
//
// 2010-08-18 : Juan Palacios
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Particle2LHCbIDVectorMap::Particle2LHCbIDVectorMap(  ) 
  :
  DataObject(),
  _Base()
{

}
//=============================================================================
// Destructor
//=============================================================================
Particle2LHCbIDVectorMap::~Particle2LHCbIDVectorMap() {
  //
} 

//=============================================================================
