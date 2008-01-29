#ifndef Alignment_TAConfig_H
#define Alignment_TAConfig_H
//-----------------------------------------------------------------------------
/** @file TAConfig.h
 *
 *  Header file for Tstation alignment : TAConfig
 *
 *  CVS Log :-
 *  $Id: TAConfig.h,v 1.10 2008-01-29 16:31:40 jblouw Exp $
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
#include "AlignmentInterfaces/ICentipede.h"
#include "AlignmentInterfaces/IDerivatives.h"
// Poca 
#include "Kernel/ITrajPoca.h"
// Magnetic field service 
#include "GaudiKernel/IMagneticFieldSvc.h"
// Tracking
#include "TrackInterfaces/ITrajectoryProvider.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "TrackInterfaces/IMeasurementProvider.h"

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
#include "MuonDet/DeMuonDetector.h"


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
   
    /** Millepede-style alignment problem was properly configured
    *
    *  @param 
    *
    *  @return 
    *  @retval 
    *  @retval 
    */
   StatusCode Initialize( std::vector<std::string> &);
   StatusCode ConfMatrix( const double &,
			  const double &,
			  const double &,
			  const double &,
			  double [],
			  double [][4] );
   StatusCode ZeroMatrVec( double [][4], 
			   double [] );
   int InvMatrix( double [][4], 
		  double [], 
		  const int & ); 
   StatusCode CalcResidual( const LHCb::Track &,
			    const LHCb::Measurement *,
			    const int &,
			    const double &,
			    double &,
			    double &,
			    double &,
			    LHCb::State & );
   StatusCode FillMatrix( const int &,
			  const double [],
			  const double &,
			  const double &, 
			  const double &);
   StatusCode CacheDetElements();
   StatusCode ResetGlVars();
   StatusCode GlobalFit( std::vector<double> & parameter, 
			 std::vector<double> & error, 
			 std::vector<double> & pull) 
   {
     StatusCode sc = m_Centipede->MakeGlobalFit( parameter, error, pull );
     if ( sc.isFailure() ) {
       info() << "Error in Millepede's !";
       return StatusCode::FAILURE;
     }
     return StatusCode::SUCCESS;
   };
   
   StatusCode LocalTrackFit( int &tr_cnt,
                             std::vector<double> &trpar,
                             const int & single_fit,
                             std::vector<double> & estimated,
                             double & chi2,
                             double & residual) {
     StatusCode sc = m_Centipede->FitLoc(tr_cnt, trpar, 0, estimated, chi2, residual);
     if ( sc.isFailure() )
       info() << "Millepede local fit returns " << sc.getCode() << endreq;
     //     chi2 = tr_par[trpar.size()+1]; //decode chi2 from tracking parameters variable
     //     residual = tr_par[trpar.size()+2]; //decode residual
     return StatusCode::SUCCESS;
   };

  
  
  bool AlignDetector( std::string &det ) {
    if ( det == "Velo" )
      return velo_detector;
    else if ( det == "TT" )
      return tt_detector;
    else if ( det == "IT" ) 
      return it_detector;
     else if ( det == "OT" )
       return ot_detector;
     else if ( det == "MUON" || det == "Muon" )
       return muon_detector;
     return false;
   };
   
   int NumTrPars() {
     return m_ntrack_pars;
   };
  //    int NumAlignPars() {
  //      return 6 * m_DETmap.size();
  //    };
  //MD
   int NumAlignPars() {
     return m_n_dof * m_DETmap.size();
   };
   
  /*********************************************
   * MD  getMPTrackPoints                       *
   * get the track information of Millepede     *
   *********************************************/
  std::vector< std::vector<double> > TAConfig::getMPHits(){
    
    return m_trackpoints;
   };
  /*********************************************
   * MD  clearMPTrackPoints                       *
   * clear the track information of Millepede     *
   * for the next track                         *
   *********************************************/
  void TAConfig::clearMPHits(){
    m_trackpoints.clear();
  };
  

   StatusCode Rank( LHCb::LHCbID &, int & );
 private:
   
   StatusCode GetAlignmentConstants( const std::string & );
   StatusCode ConfigTT( std::vector<Gaudi::Transform3D> & );
   StatusCode ConfigOT( std::vector<Gaudi::Transform3D> & );
   StatusCode ConfigIT( std::vector<Gaudi::Transform3D> & );
   StatusCode ConfigMuon( std::vector<Gaudi::Transform3D> & );
   void CreateMap( int &, IDetectorElement*, double & );
   StatusCode RetrieveAPars( const std::string & );
   StatusCode ConfigMillepede();
   StatusCode ConstrainPositions( std::map<std::string,int> & );
   StatusCode ConstrainPositions( const int &, const std::string &, const int & );
   std::string MuonDetName( const unsigned int & );
   //  StatusCode GlobalFit();
   void VectortoArray(const std::vector<double>& , double[] );
   void ArraytoVector(const double[], std::vector<double>& );
   std::string m_inputcontainer;
   std::string m_CentipedeTool;
   std::string m_MilleConfTool;
   std::string m_derivativTool;
   std::string m_MeasProvider;
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
   double m_zmoy_mu;
   double m_zmoy_ot, s_zmoy_ot;
   double m_zmoy_it, s_zmoy_it;
   double m_zmoy_tt, s_zmoy_tt;
   std::vector<int> m_rank;
   std::vector<Gaudi::Transform3D> m_DETmap;
   std::vector<Gaudi::Transform3D> m_ALImap;
   std::vector<bool> m_VELOmap_l;
   std::vector<bool> m_VELOmap_r;
   bool m_fix;
   //   std::multiset m_fix_dofs;
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
   bool m_muSys, m_muStation, m_muChamber;
   std::string m_itCond, m_ttCond, m_otCond;
   //
   // Millepede configuration variables
   std::map<std::string, int> m_C_pos;
   std::map<std::string, int> constrain_it, 
     constrain_ot, constrain_tt, constrain_velo, constrain_muon;

   bool *m_DOF;
   int m_n_dof;
   bool m_CONSTRAINT[9];
   double m_SIGMA[6];
   std::vector<double>   m_derLC; 
   std::vector<double>   m_derGB;
   std::vector<double>   m_derNonLin;
   std::vector<double>   m_aliConstants;
   
   // The detectors
   bool velo_detector;
   bool tt_detector, it_detector, ot_detector, muon_detector;
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
  DeMuonDetector *m_muon;
  DetectorElement *m_demuon;
  std::vector<IDetectorElement*> m_MUStations;
  std::vector<IDetectorElement*> m_MUChambers;

  std::string m_previous_de;



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
  IMagneticFieldSvc *m_bField;
  ICentipede *m_Centipede;
  IDerivatives *m_derivatives;
  ITrackExtrapolator* m_extrapolator;
  IMeasurementProvider *m_measprovider;

  //MD check MP track fit
  std::vector< std::vector<double> > m_trackpoints;
};



#endif // TRACKTOOLS_JBSelector_H
