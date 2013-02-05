// $Id: RichG4Counters.cpp,v 1.8 2008-01-21 16:55:33 seaso Exp $
// Include files

// local
#include "GaussRICH/RichG4Counters.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichG4Counters
//
// 2003-06-07 : Sajan EASO
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichG4Counters* RichG4Counters::RichG4CountersInstance=0;

RichG4Counters::RichG4Counters(  ) { }

RichG4Counters::~RichG4Counters(  ) { }

RichG4Counters* RichG4Counters::getInstance()
{
  if ( RichG4CountersInstance == 0 ){
    RichG4CountersInstance = new RichG4Counters();
  }

  return  RichG4CountersInstance;
}

void RichG4Counters::InitRichEventCounters()
{
  m_NumPhotProdRich1Gas = 0 ;
  m_NumPhotGasOnRich1Mirror1 = 0 ;
  m_NumPhotGasOnRich1Mirror2 = 0;
  m_NumPhotGasOnRich1GasQW = 0;
  m_NumPhotGasOnRich1HpdQW = 0;
  m_NumPhotGasRich1BeforeQE = 0;
  m_NumPhotGasRich1AfterQE = 0;
  m_NumPhotGasRich1SiDet=0;

  m_NumPhotProdRich1Agel =0;
  m_NumPhotAgelAtAgelDownstrZ=0;
  m_NumPhotAgelOnRich1Mirror1=0;
  m_NumPhotAgelOnRich1Mirror2=0;
  m_NumPhotAgelOnRich1GasQW =0;
  m_NumPhotAgelOnRich1HpdQW =0;
  m_NumPhotAgelRich1BeforeQE =0;
  m_NumPhotAgelRich1AfterQE = 0;
  m_NumPhotAgelRich1SiDet =0;


  m_NumHitTotRich1Gas =0;
  m_NumHitTotRich1Agel =0;
  m_NumHitTotRich2Gas =0;
  m_NumHitTotNoRadiator=0;

  m_NumHitTotRich1HpdQw=0;
  m_NumHitTotRich2HpdQw=0;
  m_NumHitTotRich1GasQw=0;
  m_NumHitTotRich2GasQw=0;
  m_NumHitTotRich1FilterD263=0;
  m_NumHitTotRich1FilterGeneric=0;

  m_NumHitPerTrackRich1Gas.clear();
  m_NumHitPerTrackRich1Agel.clear();
  m_NumHitPerTrackRich1WithRlyAgel.clear();
  m_NumHitPerTrackRich2Gas.clear();

  m_NumHitSaturatedPerTrackRich1Gas.clear();
  m_NumHitSaturatedPerTrackRich1Agel.clear();
  m_NumHitSaturatedPerTrackRich1WithRlyAgel.clear();
  m_NumHitSaturatedPerTrackRich2Gas.clear();

  m_NumHitSaturatedPerTrackRich1GasNoHpdRefl.clear();
  m_NumHitSaturatedPerTrackRich1AgelNoHpdRefl.clear();
  m_NumHitSaturatedPerTrackRich1WithRlyAgelNoHpdRefl.clear();
  m_NumHitSaturatedPerTrackRich2GasNoHpdRefl.clear();

  m_NumHitSaturatedPerTrackRich1GasNoHpdReflHighMom.clear();
  m_NumHitSaturatedPerTrackRich2GasNoHpdReflNoScintHighMom.clear();

  m_NumHitSaturatedPerTrackRich2GasNoHpdReflNoScint.clear();
  m_NumHitSaturatedPerTrackRich2GasScint.clear();
  
  m_NumHitPartGunPrimaryPartRich1Gas=0;
  m_NumHitPartGunPrimaryPartRich1Agel=0;
  m_NumHitPartGunPrimaryPartRich2Gas=0;
  m_NumHitTotRich1All =0;
  m_NumHitTotRich2All =0;
  m_TrackIdTraverseRich1Gas.clear();
  m_TrackIdTraverseRich1Agel.clear();
  m_TrackIdTraverseRich2Gas.clear();

  m_TrackIdFullAcceptRich1Gas.clear();
  m_TrackIdFullAcceptRich1Agel.clear();
  m_TrackIdFullAcceptRich2Gas.clear();

  m_TrackMomFullAcceptRich1Gas.clear();
  m_TrackMomFullAcceptRich1Agel.clear();
  m_TrackMomFullAcceptRich2Gas.clear();


  m_NumHitFullAcceptSatPerTrackR1Gas.clear();
  m_NumHitFullAcceptSatPerTrackR1Agel.clear();
  m_NumHitFullAcceptSatPerTrackR2Gas.clear();

  m_NumSignalHitFullAcceptSatPerTrackR1Gas.clear();
  m_NumSignalHitFullAcceptSatPerTrackR2Gas.clear();


  m_NumHitFullAcceptSatPerTrackHighMomR1Gas.clear();
  m_NumHitFullAcceptSatPerTrackNoScintR2Gas.clear();
  m_NumHitFullAcceptSatPerTrackNoScintHighMomR2Gas.clear();

  m_NumHitTotRich1HpdRefl=0;
  m_NumHitTotRich2HpdRefl=0;
  
  m_NumHitTotRich1GasHpdRefl=0;
  m_NumHitTotRich1GasHpdBackScat=0;  
  m_NumHitTotRich1GasHpdQWPCRefl=0;
  m_NumHitTotRich1GasHpdChromRefl=0;
  m_NumHitTotRich1GasHpdSiliconRefl=0;
  m_NumHitTotRich1GasHpdKovarRefl=0;
  m_NumHitTotRich1GasHpdKaptonRefl=0;

  m_NumHitTotRich2GasCherenkovProc=0;
  m_NumHitTotRich2GasScintProc=0;
  m_NumHitTotRich1GasCherenkovProc=0;
  m_NumHitTotRich1AgelCherenkovProc=0;
  

  m_NumHitTotRich1AerogelHpdKaptonRefl=0;

  m_NumHitTotRich2GasHpdRefl=0;
  m_NumHitTotRich2GasHpdBackScat=0;  
  m_NumHitTotRich2GasHpdQWPCRefl=0;
  m_NumHitTotRich2GasHpdChromRefl=0;
  m_NumHitTotRich2GasHpdSiliconRefl=0;
  m_NumHitTotRich2GasHpdKovarRefl=0;
  m_NumHitTotRich2GasHpdKaptonRefl=0;

}


//=============================================================================
