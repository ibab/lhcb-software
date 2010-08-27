// $Id: Particle2VertexBase.cpp,v 1.1 2010/08/19 15:09:43 jpalac Exp $
// Include files
// local
#include "Kernel/Particle2VertexBase.h"

// anonymous namespace to hide local symbols:
namespace 
{
   
  // ===========================================================================
  /** @var CLID_Candidate 
   *  Class ID definition 
   */
  static const CLID CLID_Particle2VertexBase = 881;
  
  // ==========================================================================
}
// ============================================================================
// Class ID 
// ============================================================================
const CLID& DaVinci::Map::Particle2VertexBase::classID() 
{
  return CLID_Particle2VertexBase;
}
//-----------------------------------------------------------------------------
// Implementation file for class : Particle2LHCbIDVectorMap
//
// 2010-08-18 : Juan Palacios
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DaVinci::Map::Particle2VertexBase::Particle2VertexBase(  ) 
  :
  DataObject(),
  m_map()
{

}
//=============================================================================
// Destructor
//=============================================================================
DaVinci::Map::Particle2VertexBase::~Particle2VertexBase() {
  m_map.clear();
}
//=============================================================================
