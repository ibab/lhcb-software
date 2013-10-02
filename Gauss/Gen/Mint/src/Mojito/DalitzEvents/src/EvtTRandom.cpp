// Include files

// local
#include "Mint/EvtTRandom.h"

//-----------------------------------------------------------------------------
// Implementation file for class : EvtTRandom
//
// 2013-09-20 : Philip Hunt (LHCB)
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MINT::EvtTRandom::EvtTRandom( MINT::IEvtRandom* rnd ) : m_rnd(rnd)
{
  SetName("EvtRandom");
  SetTitle("Random number generator: EvtRandom");
}


Double_t MINT::EvtTRandom::Rndm(Int_t i)
{
  if (m_rnd==NULL) {
    Error("MINT::EvtTRandom::Rndm",
          "EvtRandom engine not set. Returning 'kBigNumber'.");
    return kBigNumber;
  }
  return m_rnd->Rndm(i);
}

void MINT::EvtTRandom::RndmArray(Int_t n, Float_t *array)
{
  // Return an array of n random numbers uniformly distributed in ]0,1]

  for(Int_t i=0; i<n; i++) array[i]=(Float_t)this->Rndm();
}

void MINT::EvtTRandom::RndmArray(Int_t n, Double_t *array)
{
  // Return an array of n random numbers uniformly distributed in ]0,1]

  for(Int_t i=0; i<n; i++) array[i]=this->Rndm();
}

void MINT::EvtTRandom::SetSeed(UInt_t seed)
{
  if (m_rnd==NULL) {
    Error("MINT::EvtTRandom::SetSeed",
          "EvtRandom engine not set.");
    return;
  }
  m_rnd->SetSeed(seed);
}

UInt_t MINT::EvtTRandom::GetSeed() const 
{
  if (m_rnd==NULL) {
    Error("MINT::EvtTRandom::GetSeed",
          "EvtRandom engine not set. Returning zero.");
    return 0;
  }
  return m_rnd->GetSeed();
}

//=============================================================================
// Destructor
//=============================================================================
MINT::EvtTRandom::~EvtTRandom()
{
  if (m_rnd!=NULL) {
    delete m_rnd;
    m_rnd=NULL;
  }
} 

//=============================================================================
