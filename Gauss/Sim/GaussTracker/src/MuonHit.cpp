// $Id: MuonHit.cpp,v 1.1.1.1 2002-12-12 14:42:11 witoldp Exp $
// Include files

// local
#include "MuonHit.h"

G4Allocator<MuonHit> MuonHitAllocator;


void* MuonHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) MuonHitAllocator.MallocSingle();
  return aHit;
}

void MuonHit::operator delete(void *aHit)
{
  MuonHitAllocator.FreeSingle((MuonHit*) aHit);
}



//-----------------------------------------------------------------------------
// Implementation file for class : MuonHit
//
// 2002-08-16 : Witold Pokorski
//-----------------------------------------------------------------------------

MuonHit::MuonHit( ) : m_chamberID(0), m_gapID(0) {;}
MuonHit::~MuonHit( ){;}


//=============================================================================
