//-----------------------------------------------------------------------------
#ifndef Alignment_GALIGN_H
#define Alignment_GALIGN_H
/** @file TAlignment.h
 *
 *  Header file for Tstation alignment algorithm: TAlignment
 *
 *  CVS Log :-
 *  $Id: GAlign.h,v 1.2 2008-01-29 16:40:39 jblouw Exp $
 *
 *  @author J. Blouw  Johan.Blouw@cern.ch
 *  @date   30/12/2007
 */
//-----------------------------------------------------------------------------




#include <string>
#include <map>
// Interfaces
#include "AlignmentInterfaces/ITAConfigTool.h"
#include "TrackInterfaces/IMeasurementProvider.h"

// GaudiKernel
#include "GaudiKernel/HashMap.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
// GaudiAlg
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiAlg/Tuple.h"

using namespace std;

namespace LHCb {
  class Track;
  class LHCbID;
  class State;
}


class GAlign : public GaudiTupleAlg, virtual public IIncidentListener {

public:

  /// constructer
  GAlign( const std::string& name,
                 ISvcLocator* pSvcLocator);

  virtual ~GAlign();

  // Global fit:
  StatusCode GloFit();

  // access to member variable (from python)
  bool Converged();

  // Tool initialization
  virtual StatusCode initialize();

  //  Algorithm execution
  virtual StatusCode execute();

  // Algorithm finalization
  virtual StatusCode finalize();

  // Overloaded function inherited from IIncidentListener:
  StatusCode queryInterface( const InterfaceID &, void ** );
  void handle(const Incident& incident);

  /** Returns 
   *
   *  @param aTrack Reference to the Track to test
   *
   *  @return Gaudi::XYZVector, which is the distance of closest approach.
   *          Note that this vector does not necessarily lie in the detector plane!
   */
  

  bool m_converged;
  std::vector<double> m_align;
  std::vector<double> m_align_err;
 private:
  // Interfaces:
  ITAConfigTool *m_taConfig;
  IMeasurementProvider *m_measProvider;
  // 
  string m_AlignConfTool;
  string m_inputcontainer;
  std::vector<std::string> m_detectors;
  int m_nGlPars; // Number of alignment parameters
  bool velo_detector, tt_detector, it_detector, ot_detector, muon_detector;
  int m_evtsPerRun;
  int m_MaxIterations;
  bool m_ForceIterations;
  int m_iterations;
  double m_chi2;
  int m_ntr;
  Gaudi::XYZVector m_res;
  std::vector<double> m_pull;
  std::vector<double> m_estimated;
  inline void add( std::vector<double> &c, std::vector<double> a, std::vector<double> b) {
    std::vector<double>::iterator t;
    std::vector<double>::iterator s;
    std::vector<double>::iterator end;
    if ( a.end() <= b.end() ) {
       c.resize( a.size() );
    } else {
       c.resize( b.size() );
    }
    info() << "a.size = " << a.size() << " b.size = " << b.size() << " c.size = " << c.size() << endreq;
    std::vector<double>::iterator r = c.begin();
    for( t = a.begin(), 
         s = b.begin(), 
         r = c.begin(); t < a.end(); t++, s++, r++ ) {
       info() << "r = " << *r << " t = " << *t << " s = " << *s << endreq;
       *r = *t+*s;
    }
  }

};


#endif // TRACKTOOLS_JBSelector_H
