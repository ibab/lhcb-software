#ifndef Alignment_TAConfig_H
#define Alignment_TAConfig_H
//-----------------------------------------------------------------------------
/** @file TAConfig.h
 *
 *  Header file for Tstation alignment : TAConfig
 *
 *  CVS Log :-
 *  $Id: TAConfig.h,v 1.1 2007-04-25 14:07:06 jblouw Exp $
 *
 *  @author J. Blouw johan.blouw@cern.ch
 *  @date   12/04/2007
 */
//-----------------------------------------------------------------------------
// Standard 
#include <string>
#include <map>
// Local interface
#include "AlignmentInterfaces/ITAConfigTool.h"
// Interfaces
// Alignment
#include "AlignmentInterfaces/IMillepede.h"
#include "AlignmentInterfaces/IDerivatives.h"
// Poca 
#include "Kernel/ITrajPoca.h"
// Magnetic field service 
#include "GaudiKernel/IMagneticFieldSvc.h"
// Tracking
#include "TrackInterfaces/ITrajectoryProvider.h"
#include "TrackInterfaces/ITrackExtrapolator.h"

// geometry
#include "DetDesc/IDetectorElement.h"


// GaudiKernel
#include "GaudiAlg/GaudiTupleTool.h"
#include "GaudiKernel/HashMap.h"
#include "GaudiAlg/Tuple.h"

// LHCb kernel
#include "Kernel/OTChannelID.h"
#include "Kernel/STChannelID.h"

#include "Event/Track.h"
#include "Event/VeloCluster.h"
#include "Event/STCluster.h"
#include "Event/OTTime.h"

//from DetDesc
#include "DetDesc/DetectorElement.h"
#include "VeloDet/DeVelo.h"
#include "OTDet/DeOTDetector.h"
#include "STDet/DeITDetector.h"
#include "STDet/DeTTDetector.h"

// boost
#include "boost/assign/list_of.hpp"
#include "boost/numeric/conversion/bounds.hpp"
#include "boost/limits.hpp"

class TAConfig : public GaudiTupleTool, 
  virtual public ITAConfigTool {
  
 public:

  /// constructer
   TAConfig( const std::string& type,
		   const std::string& name,
		   const IInterface* parent);
   
   virtual ~TAConfig(); // Destructor
     
   /** Returns Millepede-style alignment problem was properly configured
    *
    *  @param 
    *
    *  @return 
    *  @retval 
    *  @retval 
    */
   StatusCode Initialize( std::vector<std::string> &);
   StatusCode FillMatrix( LHCb::Track &, 
			  LHCb::LHCbID &, 
			  const int &, 
			  const double &);
   StatusCode CacheDetElements();
   StatusCode ResetGlVars();
   bool AlignDetector( std::string &det ) {
     if ( det == "Velo" )
       return velo_detector;
     else if ( det == "TT" )
       return tt_detector;
     else if ( det == "IT" ) 
       return it_detector;
     else if ( det == "OT" )
       return ot_detector;
     return false;
   };
   StatusCode LocalTrackFit( int &tr_cnt, 
			     std::vector<double> &trpar, 
			     const int & single_fit, 
			     std::vector<double> & estimated, 
			     double & chi2, 
			     double & residual) {
     double tr_par[trpar.size()];
     double est[estimated.size()];
     VectortoArray( trpar, tr_par );
     VectortoArray( estimated, est );
     m_Millepede->FitLoc(tr_cnt, tr_par, 0, est, chi2, residual);
     ArraytoVector( tr_par, trpar);
     ArraytoVector( est, estimated );
     return StatusCode::SUCCESS;
   };
   int NumTrPars() {
     return m_ntrack_pars;
   };
   int NumAlignPars() {
     return 6 * m_DETmap.size();
   };
   Gaudi::XYZVector Residual( LHCb::Track &, LHCb::LHCbID & );
   StatusCode GlobalFit( std::vector<double> & parameter, 
			 std::vector<double> & error, 
			 std::vector<double> & pull) {
     double par[parameter.size()];
     double err[error.size()];
     double pul[pull.size()];
     VectortoArray(parameter,par);
     VectortoArray(error,err);
     VectortoArray(pull,pul);
     m_Millepede->MakeGlobalFit( par, err, pul );
     ArraytoVector( par, parameter );
     ArraytoVector( err, error );
     ArraytoVector( pul, pull );
     return StatusCode::SUCCESS;
   };
   StatusCode Rank( LHCb::LHCbID &, int & );
 private:
     
  StatusCode GetAlignmentConstants( const string & );
  StatusCode ConfigTT( std::vector<Gaudi::Transform3D> & );
  StatusCode ConfigOT( std::vector<Gaudi::Transform3D> & );
  StatusCode ConfigIT( std::vector<Gaudi::Transform3D> & );
  void CreateMap( int &, IDetectorElement*, double & );
  StatusCode RetrieveAPars( const std::string & );
  StatusCode ConfigMillepede();
  StatusCode ConstrainPositions( std::map<string,int> & );
  StatusCode ConstrainPositions( const int &, const string &, const int & );
  //  StatusCode GlobalFit();
  void VectortoArray(const std::vector<double>& , double[] );
  void ArraytoVector(const double[], std::vector<double>& );
  std::string m_inputcontainer;
  std::string m_MillepedeTool;
  std::string m_MilleConfTool;
  std::string m_derivativTool;
  std::vector<std::string> m_detectors;
  int m_ntrack_pars; // number of local track parameters
  double m_commonXFraction;
  std::vector<bool> m_dof;
  std::vector<bool> m_constraint;
  std::vector<double> m_sigma;
  double m_l_chimax;
  double m_residual_cut;
  double m_ini_residual_cut;
  int m_nstd;
  double m_zmoy;
  double m_zmoy_velo;
  double m_zmoy_ot, s_zmoy_ot;
  double m_zmoy_it, s_zmoy_it;
  double m_zmoy_tt, s_zmoy_tt;
  std::vector<int> m_rank;
  std::vector<Gaudi::Transform3D> m_DETmap;
  std::vector<Gaudi::Transform3D> m_ALImap;
  std::vector<bool> m_VELOmap_l;
  std::vector<bool> m_VELOmap_r;
  bool m_fix;
  bool m_fix_first, m_fix_last;
  bool m_fix_x, m_fix_y, m_fix_z;
  bool m_fix_a, m_fix_b, m_fix_g;
  double m_zmoy_l, m_zmoy_r;
  double s_zmoy_l, s_zmoy_r;
  //  std::vector<bool> m_OTmap;
  bool m_otSys, m_otStation, m_otLayer, m_otQuadrant, m_otModule;
  //  std::vector<bool> m_ITmap;
  bool m_itSys, m_itStation, m_itBox, m_itLayer, m_itLadder;
  //  std::vector<bool> m_TTmap;
  bool m_ttSys, m_ttStation, m_ttLayer, m_ttLadder;
  std::string m_itCond, m_ttCond, m_otCond;
  //
  // Millepede configuration variables
  std::map<std::string, int> m_C_pos;
  std::map<std::string, int> constrain_it, constrain_ot, constrain_tt, constrain_velo;
  bool m_DOF[6];
  int m_n_dof;
  bool m_CONSTRAINT[9];
  double m_SIGMA[6];
  std::vector<double>   m_derLC; 
  std::vector<double>   m_derGB;
  std::vector<double>   m_derNonLin;
  std::vector<double>   m_aliConstants;

  // The detectors
  bool velo_detector;
  bool tt_detector, it_detector, ot_detector;
  int m_totStations; // total number of alignable objects
  //  int m_VeloStations; // total number of alignable velo objects
  int m_TTObjects; // total number of alignable tt objects
  int m_ITObjects; // total number of alignable it objects
  int m_OTObjects; // total number of alignable ot objects
  //  vector<int> m_Rank; // rank of alignable objects
  DeVelo *m_velo; 
  DetectorElement *m_develo;
  std::vector<IDetectorElement* > m_VeloStations;
  std::vector<IDetectorElement* > m_VeloModules;
  DeSTDetector *m_tt;
  DetectorElement *m_dett;
  std::vector<IDetectorElement* > m_TTStations;
  std::vector<IDetectorElement* > m_TTLayers;
  std::vector<IDetectorElement* > m_TTLadders;
  DeSTDetector *m_it;
  DetectorElement *m_deit;
  std::vector<IDetectorElement* > m_ITStations;
  std::vector<IDetectorElement* > m_ITBoxes;
  std::vector<IDetectorElement* > m_ITLayers;
  std::vector<IDetectorElement* > m_ITLadders;
  DeOTDetector *m_ot;
  DetectorElement *m_deot;
  std::vector<IDetectorElement* > m_OTStations;
  std::vector<IDetectorElement* > m_OTLayers;
  std::vector<IDetectorElement* > m_OTQuadrants;
  std::vector<IDetectorElement* > m_OTModules;

  string m_previous_de;

  // VELO specific stuff


  // bools to define which is to be aligned (system, stations or layers)  
  // Translation and Rotations as obtained
  // from the Cond db, for all detectors
  std::vector<Gaudi::XYZVector> m_translation;
  std::vector<Gaudi::RotationX> m_rotationX;
  std::vector<Gaudi::RotationY> m_rotationY;
  std::vector<Gaudi::RotationZ> m_rotationZ;



  // The clusters
  LHCb::VeloClusters *m_veloClusters;
  LHCb::OTTimes *m_otTimes;
  LHCb::STClusters *m_ITClusters;
  LHCb::STClusters *m_TTClusters;

  // Distance of closest approach vector
  double m_tolerance;

  // Interfaces
  ITrajPoca *m_poca;
  IMagneticFieldSvc *m_pIMF;
  IMillepede *m_Millepede;
  IDerivatives *m_derivatives;
  ITrackExtrapolator* m_extrapolator;
};

#endif // TRACKTOOLS_JBSelector_H
