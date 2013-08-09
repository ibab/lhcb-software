#ifndef CherenkovG4HistoDefineSet2_H
#define CherenkovG4HistoDefineSet2_H 1

// Include files
// #include "GaudiKernel/Algorithm.h"
// from STL
#include <string>
#include <cmath>
#include <vector>
#include "GaudiKernel/ISvcLocator.h"

// Forward declarations
class IHistogram1D;
class IHistogram2D;

// Author SE 21-8-2002
class CherenkovG4HistoDefineSet2 {

public:
  CherenkovG4HistoDefineSet2();
  virtual ~CherenkovG4HistoDefineSet2();

private:
  // These data members are used in the execution of this algorithm
  // They are set in the initialisation phase by the job options service
  // for the RichG4RunAction.
  // bool        m_DefineRichG4HistogramSet2;  // flag for histo production
  /// Book histograms
  void bookRichG4HistogramsSet2();

  // Histograms ( used if m_produceHistogramSet2 = 1 (true) )

  IHistogram1D*         m_hMomChPartRich1;
  IHistogram1D*         m_hMomChPartRich2;
  IHistogram1D*         m_hNumTotHitAgelRich1;
  IHistogram1D*         m_hNumHitAgelPrim;
  IHistogram1D*         m_hNumHitAgelSat;
  IHistogram1D*         m_hNumHitAgelSatNoRefl;
  IHistogram1D*         m_hNumHitAgelWithRlySat;
  IHistogram1D*         m_hNumTotHitC4F10Rich1;
  IHistogram1D*         m_hNumHitC4F10Prim;
  IHistogram1D*         m_hNumHitC4F10Sat;
  IHistogram1D*         m_hNumHitC4F10SatNoRefl;
  IHistogram1D*         m_hNumHitC4F10SatNoReflHighMom;
  IHistogram1D*         m_hNumTotHitCF4Rich2;
  IHistogram1D*         m_hNumTotHitCF4NonScintRich2;
  IHistogram1D*         m_hNumTotHitCF4ScintRich2;
  
  IHistogram1D*         m_hNumTotHitNoRadiatorRich1;
  IHistogram1D*         m_hNumTotHitNoRadiatorRich2;
  IHistogram1D*         m_hNumTotHitNoRadiator;
  IHistogram1D*          m_hNumHitCF4Prim;
  IHistogram1D*          m_hNumHitCF4Sat;
  IHistogram1D*          m_hNumHitCF4SatNoRefl;
  IHistogram1D*          m_hNumHitCF4SatNoReflNoScintHighMom;
  IHistogram1D*          m_hNumHitCF4SatNoReflNoScint;
  IHistogram1D*          m_hNumHitCF4SatScin;
  IHistogram1D*         m_hNumTotHitRich1Large;
  IHistogram1D*         m_hNumTotHitAgelRich1Large;
  IHistogram1D*         m_hNumTotHitGasRich1Large;
  IHistogram1D*         m_hNumTotHitRich2Large;
 
  IHistogram2D*         m_hCkvProdAgelRich1;
  IHistogram2D*         m_hCkvProdC4F10Rich1;
  IHistogram2D*         m_hCkvProdCF4Rich2;

  IHistogram1D*        m_hNumTotHitAgelFullAcceptSat;  
  IHistogram1D*        m_hNumTotHitC4F10FullAcceptSat;
  IHistogram1D*        m_hNumTotHitCF4FullAcceptSat;
  IHistogram1D*        m_hNumTotSignalHitC4F10FullAcceptSat;
  IHistogram1D*        m_hNumTotSignalHitCF4FullAcceptSat;
  
  IHistogram2D*        m_hNumHitVsAngAgelFullAcceptSat;  
  IHistogram2D*        m_hNumHitVsAngC4F10FullAcceptSat;
  IHistogram2D*        m_hNumHitVsAngCF4FullAcceptSat;

  IHistogram2D*        m_hNumHitVsTrPhiC4F10FullAcceptSat;
  IHistogram2D*        m_hNumHitVsTrPhiCF4FullAcceptSat;  

  std::string m_RichG4HistoPathSet2; ///< Histo path
  ISvcLocator* m_svcLocSet2;



};

#endif  //end of CherenkovG4DefineHistoSet2




