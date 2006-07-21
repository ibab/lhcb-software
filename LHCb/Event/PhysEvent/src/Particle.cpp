// $Id: Particle.cpp,v 1.22 2006-07-21 12:56:32 jpalac Exp $
// Include files 

// STD and STL
#include <iostream> // from Gaudi

// local
#include "Event/Particle.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Particle
//                                 derived from class KeyedObjeect
//
// 19/03/2002 : Gloria Corti
// 25/07/2005 : P. Koppenburg
//-----------------------------------------------------------------------------
//=============================================================================
// Create and return the covariance matrix
//=============================================================================
// Gaudi::SymMatrix7x7 LHCb::Particle::covMatrix() const 
// {

//   Gaudi::Matrix7x7 full;

//   full.Place_at(m_posCovMatrix,0,0);
//   full.Place_at(m_momCovMatrix,3,3);
//   full.Place_at(m_posMomCovMatrix,3,0);
//   full.Place_at(ROOT::Math::Transpose(m_posMomCovMatrix), 0,3);
//   return Gaudi::SymMatrix7x7( full.LowerBlock() );
	
// }

