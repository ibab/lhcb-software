// $Id: PlotTrackDeviation.h,v 1.1 2005-12-09 07:00:20 pkoppenb Exp $
#ifndef PLOTTRACKDEVIATION_H 
#define PLOTTRACKDEVIATION_H 1

// Include files
#include "Kernel/DVAlgorithm.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "TrgTools/TrgDataProvider.h"
#ifdef DC06
#include "GaudiKernel/IMonitorSvc.h"
#endif


/** @class PlotTrackDeviation PlotTrackDeviation.h
 *  Plots deviation in x and y of the tracks at the Z of the PV
 *
 *  @author Hugo Ruiz
 *  @date   2004-12-01
 */

class PlotTrackDeviation : public DVAlgorithm {

public:

  PlotTrackDeviation( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PlotTrackDeviation( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
protected:


private:

  IHistogram1D* m_nPVs;
  IHistogram2D* m_xDeviation;
  IHistogram2D* m_yDeviation;
  
  std::string m_inputTracksLocation ;
  TrgVertexContainer* m_PVs;
  TrgTrackContainer* m_Tracks;
  
}; 

#endif // PLOTTRACKDEVIATION_H
