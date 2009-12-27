//-----------------------------------------------------------------------------
#ifndef Alignment_GALIGN_H
#define Alignment_GALIGN_H
/** @file TAlignment.h
 *
 *  Header file for Tstation alignment algorithm: TAlignment
 *
 *  CVS Log :-
 *  $Id: GAlign.h,v 1.6 2009-12-27 14:27:21 jblouw Exp $
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
#include "AlignmentInterfaces/IATrackSelectorTool.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "AlignmentInterfaces/IDerivatives.h"

// GaudiKernel
#include "GaudiKernel/HashMap.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
// GaudiAlg
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiAlg/Tuple.h"
// Tracking
#include "TrackInterfaces/ITrajectoryProvider.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
// Poca 
#include "Kernel/ITrajPoca.h"
// Magnetic field service 
#include "GaudiKernel/IMagneticFieldSvc.h"
#include "OTDet/DeOTDetector.h"
// geometry
#include "DetDesc/IDetectorElement.h"


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
  bool writeParameters();

  // Tool initialization
  virtual StatusCode initialize();

  //  Algorithm execution
  virtual StatusCode execute();

  // Algorithm finalization
  virtual StatusCode finalize();

  // Overloaded function inherited from IIncidentListener:
  StatusCode queryInterface( const InterfaceID &, void ** );
  void handle(const Incident& incident);

  //int InvMatrix(double v[][4], double vec[],int n);
  //void ZeroMatrVec( double v[][4], double vec[]);
  void VectortoArray(const std::vector<double>& , double[] );
  
  void MatrixToVector(double* mat,int dim1, int dim2,std::vector< std::vector<double> >& );
  void LagMultRef();
  
  bool m_converged;
  std::vector<double> m_align;
  std::vector<double> m_align_err;
 private:
  // Interfaces:
  ITAConfigTool *m_taConfig;
  IMeasurementProvider *m_measProvider;
  ITrajPoca *m_trpoca;
  IMagneticFieldSvc *m_bField;
  ITrackExtrapolator* m_extrapolator;
  IATrackSelectorTool* m_trackselection;    

  // 
  string m_AlignConfTool;
  string m_inputcontainer;
  std::vector<std::string> m_detectors;
  int m_nGlPars; // Number of alignment parameters
  bool velo_detector, tt_detector, it_detector, ot_detector;
  int m_evtsPerRun;
  int m_MaxIterations;
  bool m_ForceIterations;
  int m_iterations;
  Gaudi::XYZVector m_res;
  std::vector<double> m_pull;
  std::vector<double> m_estimated;
  std::vector<double> m_new_par;
  std::vector<double> m_prev_par;
  std::vector<double> m_gaRes; //track residual from GAlign
  int m_tr_cnt;  
  std::vector<Gaudi::Transform3D> m_otmap;
  
  int m_meas_cnt;
  std::vector<double> misalInput_X;
  std::vector<double> misalInput_Y;
  std::vector<double> misalInput_Z;
  std::vector<double> misalInput_A;
  std::vector<double> misalInput_B;
  std::vector<double> misalInput_C;
  
  std::vector<double> m_layZ;
  double m_zmean;  
  double m_sigZmean;
  double m_chi2;
  double m_chi2rej;
  double m_newScale;
  double m_outlier;
  bool   m_out;
  bool   m_badXval;
  bool   m_fitconv;
  double m_resMax;
  int    m_locrank;
  bool   m_skipBigCluster;
  int    m_slopeCut;
  int    m_acceptedTr;
  int    m_badCluster;
  int    m_nNotConv;
  int    m_totalTracks;
  int    m_RejectbySelector;
  int    m_nEvents;
  std::vector<double> m_hitRnk;
  
};


#endif // TRACKTOOLS_JBSelector_H
