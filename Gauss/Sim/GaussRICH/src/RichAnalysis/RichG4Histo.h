#ifndef RichG4Histo_H
#define RichG4Histo_H 1

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
class RichG4Histo {

public:
  // Constructor of this form must be provided
  //  RichG4Histo(const std::string& name, ISvcLocator* pSvcLocator); 
  RichG4Histo();
  virtual ~RichG4Histo();
  // Three mandatory member functions of any algorithm
  //  StatusCode initialize();
  // StatusCode execute();
  // StatusCode finalize();

private:
  // These data members are used in the execution of this algorithm
  // They are set in the initialisation phase by the job options service
  // bool        m_ProduceRichG4Histogram;  // flag for histo production
  /// Book histograms
  void bookRichG4Histograms();

  // Histograms ( used if m_produceHistogram = 1 (true) )
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

  std::string m_RichG4HistoPath; ///< Histo path
  ISvcLocator* svcLoc;



};

#endif  //end of RichG4Histo




