//-----------------------------------------------------------------------------
#ifndef Alignment_GALIGN_H
#define Alignment_GALIGN_H
/** @file TAlignment.h
 *
 *  Header file for Tstation alignment algorithm: TAlignment
 *
 *  CVS Log :-
 *  $Id: GAlign.h,v 1.1 2007-04-25 14:35:45 jblouw Exp $
 *
 *  @author J. Blouw  Johan.Blouw@cern.ch
 *  @author M.Needham Matt.Needham@cern.ch
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   30/12/2005
 */
//-----------------------------------------------------------------------------



#include <string>
#include <map>
// Interfaces
#include "AlignmentInterfaces/ITAConfigTool.h"
#include "TrackInterfaces/IMeasurementProvider.h"

// GaudiKernel
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/HashMap.h"
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiAlg/Tuple.h"

namespace LHCb {
  class Track;
  class LHCbID;
}

class GAlign : public GaudiTupleAlg {

public:

  /// constructer
  GAlign( const std::string& name,
                 ISvcLocator* pSvcLocator);

  virtual ~GAlign();

  // Tool initialization
  virtual StatusCode initialize();

  //  Algorithm execution
  virtual StatusCode execute();

  // Algorithm finalization
  virtual StatusCode finalize();


  /** Returns 
   *
   *  @param aTrack Reference to the Track to test
   *
   *  @return Gaudi::XYZVector, which is the distance of closest approach.
   *          Note that this vector does not necessarily lie in the detector plane!
   */


 private:
  // Interfaces:
  ITAConfigTool *m_taConfig;
  IMeasurementProvider *m_measProvider;
  // 
  string m_AlignConfTool;
  string m_inputcontainer;
  std::vector<std::string> m_detectors;
  int m_nGlPars; // Number of alignment parameters
  bool velo_detector, tt_detector, it_detector, ot_detector;
  Gaudi::XYZVector m_res;
  std::vector<double> m_align;
  std::vector<double> m_align_err;
  std::vector<double> m_pull;
};

#endif // TRACKTOOLS_JBSelector_H
