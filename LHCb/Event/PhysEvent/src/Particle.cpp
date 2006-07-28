// $Id: Particle.cpp,v 1.24 2006-07-28 06:55:48 jpalac Exp $
// Include files 

// STD and STL
#include <iostream> // from Gaudi

// local
#include "Event/Particle.h"

#include "LHCbMath/MatrixManip.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Particle
//                                 derived from class KeyedObjeect
//
// 19/03/2002 : Gloria Corti
// 25/07/2005 : P. Koppenburg
// 28/07/2006 : Juan Palacios
// 
// WARNING: AS OF 28/07/2006 THE CODE BELOW CAUSES DICTIONARY GENERATION
//          PROBLEMS ON WINDOWS WHEN INCLUDED IN THE Particle.h FILE.
//          DO NOT REMOVE THIS FILE UNLESS YOU ARE SURE THIS PROBLEM HAS BEEN
//          FIXED. J. Palacios 28/07/2006 
//-----------------------------------------------------------------------------

//=============================================================================
// Create and return the covariance matrix
//=============================================================================
Gaudi::SymMatrix7x7 LHCb::Particle::covMatrix() const 
{

  Gaudi::Matrix7x7 full;

  full.Place_at(m_posCovMatrix,0,0);
  full.Place_at(m_momCovMatrix,3,3);
  full.Place_at(m_posMomCovMatrix,3,0);
  full.Place_at(ROOT::Math::Transpose(m_posMomCovMatrix), 0,3);

   return Gaudi::SymMatrix7x7( full.LowerBlock() );
	
}
//=============================================================================
