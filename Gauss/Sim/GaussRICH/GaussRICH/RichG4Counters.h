// $Id: RichG4Counters.h,v 1.8 2008-01-21 16:55:33 seaso Exp $
#ifndef GAUSSRICH_RICHG4COUNTERS_H
#define GAUSSRICH_RICHG4COUNTERS_H 1

// Include files
#include <vector>
#include "G4ThreeVector.hh"

/** @class RichG4Counters RichG4Counters.h RichAnalysis/RichG4Counters.h
 *
 *
 *  @author Sajan EASO
 *  @date   2003-06-07
 */
class RichG4Counters {
public:

  virtual ~RichG4Counters( ); ///< Destructor
  static  RichG4Counters* getInstance();
  void InitRichEventCounters();


protected:

private:
  /// Standard constructor
  RichG4Counters( );
  static  RichG4Counters*  RichG4CountersInstance;

  int m_NumPhotProdRich1Gas;
  int m_NumPhotGasOnRich1Mirror1;
  int m_NumPhotGasOnRich1Mirror2;
  int m_NumPhotGasOnRich1GasQW;
  int m_NumPhotGasOnRich1HpdQW;
  int m_NumPhotGasRich1BeforeQE;
  int m_NumPhotGasRich1AfterQE;
  int m_NumPhotGasRich1SiDet;

  int m_NumPhotProdRich1Agel;
  int m_NumPhotAgelAtAgelDownstrZ;
  int m_NumPhotAgelOnRich1Mirror1;
  int m_NumPhotAgelOnRich1Mirror2;
  int m_NumPhotAgelOnRich1GasQW;
  int m_NumPhotAgelOnRich1HpdQW;
  int m_NumPhotAgelRich1BeforeQE;
  int m_NumPhotAgelRich1AfterQE;
  int m_NumPhotAgelRich1SiDet;


  int m_NumHitTotRich1Gas;
  int m_NumHitTotRich1Agel;
  int m_NumHitTotRich2Gas;
  int m_NumHitTotNoRadiator;

  int m_NumHitTotRich1HpdQw;
  int m_NumHitTotRich2HpdQw;
  int m_NumHitTotRich1GasQw;
  int m_NumHitTotRich2GasQw;
  int m_NumHitTotRich1FilterD263;
  int m_NumHitTotRich1FilterGeneric;

  int m_NumHitTotRich1HpdRefl;
  int m_NumHitTotRich2HpdRefl;

  int m_NumHitTotRich1GasHpdRefl;
  int m_NumHitTotRich1GasHpdBackScat;
  int m_NumHitTotRich1GasHpdQWPCRefl;
  int m_NumHitTotRich1GasHpdChromRefl;
  int m_NumHitTotRich1GasHpdSiliconRefl;
  int m_NumHitTotRich1GasHpdKovarRefl;
  int m_NumHitTotRich1GasHpdKaptonRefl;
  
  int m_NumHitTotRich1AerogelHpdKaptonRefl;

  int m_NumHitTotRich2GasHpdRefl;
  int m_NumHitTotRich2GasHpdBackScat;
  int m_NumHitTotRich2GasHpdQWPCRefl;
  int m_NumHitTotRich2GasHpdChromRefl;
  int m_NumHitTotRich2GasHpdSiliconRefl;
  int m_NumHitTotRich2GasHpdKovarRefl;
  int m_NumHitTotRich2GasHpdKaptonRefl;

  int m_NumHitTotRich2GasCherenkovProc;
  int m_NumHitTotRich2GasScintProc;
  int m_NumHitTotRich1GasCherenkovProc;
  int m_NumHitTotRich1AgelCherenkovProc;
  

  std::vector<int> m_NumHitPerTrackRich1Gas;
  std::vector<int> m_NumHitPerTrackRich1Agel;
  std::vector<int> m_NumHitPerTrackRich2Gas;

  std::vector<int> m_NumHitSaturatedPerTrackRich1Gas;
  std::vector<int> m_NumHitSaturatedPerTrackRich1Agel;
  std::vector<int> m_NumHitSaturatedPerTrackRich2Gas;

  std::vector<int> m_NumHitSaturatedPerTrackRich1GasNoHpdRefl;
  std::vector<int> m_NumHitSaturatedPerTrackRich1AgelNoHpdRefl;
  std::vector<int> m_NumHitSaturatedPerTrackRich2GasNoHpdRefl;

  std::vector<int> m_NumHitSaturatedPerTrackRich1GasNoHpdReflHighMom;
  std::vector<int> m_NumHitSaturatedPerTrackRich2GasNoHpdReflNoScintHighMom;
  
  std::vector<int> m_NumHitSaturatedPerTrackRich2GasNoHpdReflNoScint;
  std::vector<int> m_NumHitSaturatedPerTrackRich2GasScint;
  


  std::vector<int> m_NumHitPerTrackRich1WithRlyAgel;
  std::vector<int> m_NumHitSaturatedPerTrackRich1WithRlyAgel;
  std::vector<int> m_NumHitSaturatedPerTrackRich1WithRlyAgelNoHpdRefl;

  int m_NumHitPartGunPrimaryPartRich1Gas;
  int m_NumHitPartGunPrimaryPartRich1Agel;
  int m_NumHitPartGunPrimaryPartRich2Gas;


  int m_NumHitTotRich1All;
  int m_NumHitTotRich2All;

  std::vector<int> m_TrackIdTraverseRich1Gas;
  std::vector<int> m_TrackIdTraverseRich1Agel;
  std::vector<int> m_TrackIdTraverseRich2Gas;

  std::vector<int> m_TrackIdFullAcceptRich1Gas;
  std::vector<int> m_TrackIdFullAcceptRich1Agel;
  std::vector<int> m_TrackIdFullAcceptRich2Gas;

  std::vector<G4ThreeVector> m_TrackMomFullAcceptRich1Gas; // not used for now
  std::vector<G4ThreeVector> m_TrackMomFullAcceptRich1Agel; // not used for now
  std::vector<G4ThreeVector> m_TrackMomFullAcceptRich2Gas; // not used for now

  std::vector<int> m_NumHitFullAcceptSatPerTrackR1Gas;
  std::vector<int> m_NumHitFullAcceptSatPerTrackR1Agel;
  std::vector<int> m_NumHitFullAcceptSatPerTrackR2Gas;

  std::vector<int> m_NumSignalHitFullAcceptSatPerTrackR1Gas; //not used for now
  std::vector<int> m_NumSignalHitFullAcceptSatPerTrackR2Gas; //not used for now

  std::vector<int>  m_NumHitFullAcceptSatPerTrackHighMomR1Gas;
  std::vector<int>  m_NumHitFullAcceptSatPerTrackNoScintR2Gas;
  std::vector<int>   m_NumHitFullAcceptSatPerTrackNoScintHighMomR2Gas;
  
public:

#include "GaussRICH/RichG4Counters.icpp"

};
#endif // GAUSSRICH_RICHG4COUNTERS_H
