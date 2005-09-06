// $Id: MCEffTree.cpp,v 1.1.1.1 2005-09-06 07:06:43 pkoppenb Exp $
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


void MCEffTree::addMCEffParticle( int recble, int rected, 
                                  MCParticle* mcParticle ) {

  MCEffParticle effPart;
  
  
  effPart.setRecble(recble);
  effPart.setRected(rected);
  effPart.setMcStable(mcParticle);
  
  m_allStables.push_back(effPart);
  
}


//=============================================================================
