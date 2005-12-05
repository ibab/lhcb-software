// $Id: MCEffTree.cpp,v 1.2 2005-12-05 16:50:59 cattanem Exp $
// Include files 



// local
#include "Event/MCEffTree.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCEffTree
//
// 2004-02-10 : Christian Jacoby
//-----------------------------------------------------------------------------

//=============================================================================
// implementation for addMCEffParticle method
//=============================================================================

/// std destructor
// MCEffTree:: ~MCEffTree()
//   {
//     while( m_allStables.size() > 0 ) {
//       MCEffParticle* ipart = m_allStables.back();
//       m_allStables.pop_back();
//       delete ipart;
//     }
//   }


void LHCb::MCEffTree::addMCEffParticle( int recble, int rected, 
                                        LHCb::MCParticle* mcParticle ) {

  MCEffParticle effPart;
  
  
  effPart.setRecble(recble);
  effPart.setRected(rected);
  effPart.setMcStable(mcParticle);
  
  m_allStables.push_back(effPart);
  
}


//=============================================================================
