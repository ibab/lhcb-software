#ifndef GaussRICH_RichG4HistoDefineSet3_H
#define GaussRICH_RichG4HistoDefineSet3_H 1

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
class RichG4HistoDefineSet3 {

public:
  RichG4HistoDefineSet3();
  virtual ~RichG4HistoDefineSet3();

private:
  // These data members are used in the execution of this algorithm
  // They are set in the initialisation phase by the job options service
  // for the RichG4RunAction.
  // bool        m_DefineRichG4HistogramSet3;  // flag for histo production
  /// Book histograms
  void bookRichG4HistogramsSet3();

  // Histograms ( used if m_produceHistogramSet3 = 1 (true) )

  IHistogram1D*         m_hNumPhotGasProdRich1 ;
  IHistogram1D*         m_hNumPhotGasMirror1Rich1;
  IHistogram1D*         m_hNumPhotGasMirror2Rich1;
  IHistogram1D*          m_hNumPhotGasQWRich1;
  IHistogram1D*          m_hNumPhotGasHpdQWRich1;
  IHistogram1D*           m_hNumPhotGasBeforeQERich1;
  IHistogram1D*        m_hNumPhotGasAfterQERich1;
  IHistogram1D*        m_hNumPeGasSiDetRich1;

  IHistogram1D*         m_hNumPhotAgelProdRich1 ;
  IHistogram1D*         m_hNumPhotAgelDownstrAgelRich1;
  IHistogram1D*         m_hNumPhotAgelMirror1Rich1;
  IHistogram1D*         m_hNumPhotAgelMirror2Rich1;
  IHistogram1D*          m_hNumPhotAgelQWRich1;
  IHistogram1D*          m_hNumPhotAgelHpdQWRich1;
  IHistogram1D*           m_hNumPhotAgelBeforeQERich1;
  IHistogram1D*        m_hNumPhotAgelAfterQERich1;
  IHistogram1D*        m_hNumPeAgelSiDetRich1;

  IHistogram1D*        m_hNumAgelRadiator;  
  IHistogram1D*        m_hNumc4f10Radiator;  
  IHistogram1D*        m_hNumcf4Radiator;  
  IHistogram1D*        m_hNumfilterd263Radiator;  
  IHistogram1D*        m_hNumfiltergenericRadiator;  
  IHistogram1D*        m_hNumRich1GasqwRadiator;  
  IHistogram1D*        m_hNumRich2GasqwRadiator;  
  IHistogram1D*        m_hNumRich1HpdqwRadiator;  
  IHistogram1D*        m_hNumRich2HpdqwRadiator;  

  IHistogram1D* m_hNumRich1HpdNoRefl;
  IHistogram1D* m_hNumRich2HpdNoRefl;
  IHistogram1D* m_hNumRich1HpdNoReflSmall;
  IHistogram1D* m_hNumRich2HpdNoReflSmall;

  IHistogram1D* m_hNumRich1HpdRefl;
  IHistogram1D* m_hNumRich2HpdRefl;
  IHistogram1D* m_hNumRich1HpdReflSmall;
  IHistogram1D* m_hNumRich2HpdReflSmall;

  IHistogram1D* m_hNumRich1GasHpdRefl;
  IHistogram1D* m_hNumRich1GasHpdReflSmall;
  IHistogram1D* m_hNumRich1GasHpdBackScat;
  IHistogram1D* m_hNumRich1GasHpdQWPCRefl;
  IHistogram1D* m_hNumRich1GasHpdQWPCReflSmall;
  IHistogram1D* m_hNumRich1GasHpdChromiumRefl;
  IHistogram1D* m_hNumRich1GasHpdSiliconRefl;
  IHistogram1D* m_hNumRich1GasHpdKovarRefl;
  IHistogram1D* m_hNumRich1GasHpdKaptonRefl;

  IHistogram1D* m_hNumRich1AerogelHpdKaptonRefl;

  IHistogram1D* m_hNumRich2GasHpdRefl;
  IHistogram1D* m_hNumRich2GasHpdReflSmall;
  IHistogram1D* m_hNumRich2GasHpdBackScat;
  IHistogram1D* m_hNumRich2GasHpdQWPCRefl;
  IHistogram1D* m_hNumRich2GasHpdQWPCReflSmall;
  IHistogram1D* m_hNumRich2GasHpdChromiumRefl;
  IHistogram1D* m_hNumRich2GasHpdSiliconRefl;
  IHistogram1D* m_hNumRich2GasHpdKovarRefl;
  IHistogram1D* m_hNumRich2GasHpdKaptonRefl;


  std::string m_RichG4HistoPathSet3; ///< Histo path
  ISvcLocator* m_svcLocSet3;



};

#endif  //end of GaussRICH_RichG4HistoDefineSet3




