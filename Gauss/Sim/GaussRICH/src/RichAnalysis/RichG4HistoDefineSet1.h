#ifndef RichG4HistoDefineSet1_H
#define RichG4HistoDefineSet1_H 1

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
class RichG4HistoDefineSet1 {

public:
  RichG4HistoDefineSet1();
  virtual ~RichG4HistoDefineSet1();

private:
  // These data members are used in the execution of this algorithm
  // They are set in the initialisation phase by the job options service
  // for the RichG4RunAction.
  // bool        m_DefineRichG4HistogramSet1;  // flag for histo production
  /// Book histograms
  void bookRichG4HistogramsSet1();

  // Histograms ( used if m_produceHistogramSet1 = 1 (true) )
  IHistogram1D*         m_hNumTotHitRich1;
  IHistogram1D*         m_hNumTotHitAgelRich1;
  IHistogram1D*         m_hNumTotHitC4F10Rich1;
  IHistogram1D*         m_hNumTotHitCF4Rich1;
  IHistogram2D*         m_hGlobalPEOriginXY;
  IHistogram2D*         m_hGlobalHitXY;
  IHistogram2D*         m_hRefIndC4F10Rich1;
  IHistogram2D*         m_hRefIndAgelRich1;
  IHistogram2D*         m_hRefIndCF4Rich2;
  IHistogram1D*         m_hCkvAgelRich1;
  IHistogram1D*         m_hCkvC4F10Rich1;
  IHistogram1D*         m_hCkvCF4Rich2;
  IHistogram1D*         m_hCkvZEmissionPtRich1;
  IHistogram1D*         m_hWaveLenBeforeRich1Mirror1;
  IHistogram1D*         m_hWaveLenAfterRich1Mirror1;
  IHistogram1D*         m_hWaveLenBeforeRich1GasQW;
  IHistogram1D*         m_hWaveLenAfterRich1GasQW;
  IHistogram1D*         m_hWaveLenBeforeRich1QE;
  IHistogram1D*         m_hWaveLenAfterRich1QE;
  IHistogram2D*         m_hGlobalPEOriginAgelXY;
  IHistogram2D*         m_hGlobalPEOriginC4F10XY;
  IHistogram2D*         m_hGlobalPEOriginCF4XY;
  IHistogram2D*         m_hGlobalPEOriginCF4PosXY;
  //  IHistogram2D*         m_hGlobalPEOriginAgelTopXY;
  // IHistogram2D*         m_hGlobalPEOriginC4F10TopXY;
  // IHistogram2D*         m_hGlobalPEOriginAgelBotXY;
  // IHistogram2D*         m_hGlobalPEOriginC4F10BotXY;

  std::string m_RichG4HistoPathSet1; ///< Histo path
  ISvcLocator* m_svcLocSet1;



};

#endif  //end of RichG4Histo




