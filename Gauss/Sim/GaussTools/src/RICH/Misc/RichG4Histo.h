#ifndef RichG4Histo_H
#define RichG4Histo_H 1

// Include files
#include "GaudiKernel/ISvcLocator.h"
// from STL
#include <string>
#include <cmath>
#include <vector>

// Forward declarations
class IHistogram1D;
class IHistogram2D;

// Author SE 21-8-2002
class RichG4Histo {

public:
  // Constructor of this form must be provided
  RichG4Histo(ISvcLocator*);
  virtual ~RichG4Histo();

private:
  // These data members are used in the execution of this algorithm
  // They are set in the initialisation phase by the job options service

  /// Book histograms
  void bookRichG4Histograms();

  // Histograms ( used if m_produceHistogram = 1 (true) )
  IHistogram1D*         m_hNumTotHitRich1;
  IHistogram2D*         m_hGlobalPEOriginXY;
  IHistogram2D*         m_hGlobalHitXY;

  std::string m_RichG4HistoPath; ///< Histo path
  ISvcLocator* svcLoc;

};

#endif  //end of RichG4Histo




