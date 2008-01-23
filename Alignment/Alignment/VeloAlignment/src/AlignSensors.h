// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Alignment/VeloAlignment/src/AlignSensors.h,v 1.1 2008-01-23 12:30:03 gersabec Exp $
#ifndef ALIGNSENSORS_H 
#define ALIGNSENSORS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"



/** @class AlignSensors AlignSensors.h
 *  
 *
 *  @author Marco Gersabeck
 *  @date   2008-01-23
*/

using namespace LHCb;
#include "Event/Track.h"
#include "Event/State.h"


// from TrackInterfaces
#include "TrackInterfaces/ITrackFitter.h"
#include "TrackInterfaces/ITrackInterpolator.h"
#include "TrackInterfaces/ITrackProjectorSelector.h"

// root

//#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TF1.h"

//class IMeasurementProvider;




class AlignSensors : public GaudiTupleAlg, virtual public IIncidentListener {
public: 
  /// Standard constructor
  AlignSensors( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~AlignSensors( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  void handle( const Incident& incident );
  StatusCode queryInterface( const InterfaceID& id, void** ppI );

  LHCb::Tracks* TrackContainer()
  {
    return m_trackContainer;
    
  }

  LHCb::VeloClusters* ClusterContainer()
  {
    return m_clusters;

  }

  DeVelo* DetectorElement()
  {
    return m_veloDet;
    
  }
  
  

private:
 
protected:

  StatusCode fitSensorResiduals();
  StatusCode resetHistos();
  StatusCode cleanHisto( TH2 *hist );


  StatusCode getTrackContainer();
  StatusCode getClusterContainer();
  
  StatusCode trackMonitor();
  StatusCode makeKalmanPlots( LHCb::Track* track );
  StatusCode makeStraightLinePlots( LHCb::Track* track );
  Gaudi::XYZPoint extrapolateToZ(LHCb::Track *track, double toZ);
  
  DeVelo* m_veloDet;
  
  LHCb::Tracks* m_trackContainer;
  LHCb::VeloClusters* m_clusters;

  
//  IMeasurementProvider* m_measurementprovider;
  /// interface to tracks fitter tool
  ITrackFitter* m_tracksFitter;
  ITrackInterpolator* m_trackInterpolator;
  ITrackProjectorSelector* m_projectorSelector;
  IUpdateManagerSvc* m_updMgrSvc;
  IIncidentSvc* m_incSvc;

  std::string m_clusterCont;
  std::string m_trackCont;

  
  int m_iteration;  
  int m_numberOfEvents;

  //Variables from options file:
  bool m_printInfo;  
  bool m_trackMoni;
  int m_trackValue;
  bool m_ntuple;
  int m_minSenR; 
  int m_minSenPhi; 
  int m_minModule; 
  bool m_kalmanResidual;
  std::string m_fitterName;
  int m_minNResiduals;
  double m_minDelta;
  bool m_ACDC;


  TH2D *m_h_res_2d[84];
  TH2D *m_h_resr_2d[84];
  TF1 *m_fit_func;
  TF1 *m_r_fit_func;
  TProfile *m_h_profile;
 

  std::vector<int> m_testSensors;
  
};


#endif // ALIGNSENSORS_H
