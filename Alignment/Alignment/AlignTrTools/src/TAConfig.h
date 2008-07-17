#ifndef Alignment_TAConfig_H
#define Alignment_TAConfig_H
//-----------------------------------------------------------------------------
/** @file TAConfig.h
 *
 *  Header file for Tstation alignment : TAConfig
 *
 *  CVS Log :-
 *  $Id: TAConfig.h,v 1.16 2008-07-17 13:54:40 lnicolas Exp $
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
   

   StatusCode Initialize( std::vector<std::string> &);

   StatusCode ConfMatrix( unsigned int,
			  double,
			  std::vector<double> & );
   void ZeroMatrix();
   void ZeroMatrVec( ROOT::Math::SMatrix<double, 5,5> &, double *);
   void ZeroMatrVec( ROOT::Math::SMatrix<double, 4,4> &, double *);

   int InvMatrix( std::vector<double> &b, int n );


   StatusCode CalcResidual( unsigned int,
			    const LHCb::Track &,
			    const LHCb::Measurement *,
			    const LHCb::LHCbID &,
			    bool,
			    double & );
   
   bool CalcResidualOT( // unsigned int,
          // const LHCb::Track &,
			    const LHCb::Measurement *,
			    const LHCb::LHCbID &,
			    bool,
			    double & );

   StatusCode FillMatrix( unsigned int,
			  double );

   void MakeAlignParVec();
   void MakeTrackParVec();
   void SetFlag( bool,
		 unsigned int );
   void SetAlignmentPar( const std::vector<double> & );
   StatusCode CacheDetElements();
   void ResetLVars();
   void ResetGlVars();
   void SetTrackPar( const std::vector<double> &, unsigned int ); 
   StatusCode GlobalFit( std::vector<double> &, 
			 std::vector<double> &, 
			 std::vector<double> & );

   StatusCode LocalTrackFit( unsigned int, 
			     std::vector<double> &, 
			     std::vector<double> &, 
			     // double &, 
			     double & );
   StatusCode StoreParameters( std::vector<double> & );
   double GetLChi2();
   void CheckLChi2( int,
		    int,
		    bool,
		    bool & );
   const Gaudi::Transform3D FindHitModule( const LHCb::LHCbID &,
					   Gaudi::Transform3D & );
   bool AlignDetector( const std::string &det ) {
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
   int NumAlignPars() {
     m_n_apars = m_n_dof * m_DETmap.size();
     return m_n_apars;
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
  
  /*****************************
   *  MD get detMap            *
   *****************************/
  std::vector<Gaudi::Transform3D> TAConfig::GetDetMap(){
    return m_DETmap;
  };


  StatusCode Rank( const LHCb::LHCbID & );
private:
   int InvMatrix( double[][4], double [4], int);
   StatusCode GetAlignmentConstants( const std::string & );
   StatusCode ConfigTT( std::vector<Gaudi::Transform3D> & );
   StatusCode ConfigOT( std::vector<Gaudi::Transform3D> & );
   StatusCode ConfigIT( std::vector<Gaudi::Transform3D> & );
   StatusCode ConfigMuon( std::vector<Gaudi::Transform3D> & );
   void CreateMap( int &, IDetectorElement*, double & );
   StatusCode RetrieveAPars( const std::string & );
   StatusCode ConfigMillepede();
   StatusCode ConstrainPositions( std::vector<std::string> &, char* );
   double Measurement( const LHCb::Measurement *,
		       const LHCb::Track &,
		       const LHCb::Trajectory *,
		       LHCb::State & );

   std::string MuonDetName( int );
   void VectortoArray(const std::vector<double>& , double[] );
   void ArraytoVector(const double[], std::vector<double>& );
   std::string m_inputcontainer;
   std::string m_CentipedeTool;
   std::string m_MilleConfTool;
   std::string m_derivativTool;
   std::string m_MeasProvider;
   std::vector<std::string> m_detectors;
   int m_ntrack_pars; // number of local track parameters
   int m_n_apars; // number of alignment parameters
   double m_commonXFraction;
   std::vector<bool> m_dof;
   std::vector<bool> m_constraint;
   std::vector<double> m_sigma;
   double m_l_chimax;
   double m_scale;
   double m_nstand;
   double m_scale2;
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
   int m_rank_nr;
   std::vector<Gaudi::Transform3D> m_DETmap;
   std::vector<Gaudi::Transform3D> m_ALImap;
   std::vector<bool> m_VELOmap_l;
   std::vector<bool> m_VELOmap_r;
   bool m_fix;
   //   std::multiset m_fix_dofs;
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
   std::vector<std::string> m_ITConstrain, m_OTConstrain, m_VeLoConstrain, m_MuonConstrain, m_TTConstrain;
   bool *m_DOF;
   int m_n_dof;
   std::vector<double> m_estimated, m_estimatedB4;
   bool m_CONSTRAINT[9];
   double m_SIGMA[6];
   std::vector<double>   m_derLC; 
   std::vector<double>   m_derGB;
   std::vector<double>   m_derNonLin;
   std::vector<double>   m_aliConstants;
   double m_chi2; 
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

  // define two kinds of matrices: 4 X 4 for the case with 4 track parameters
  // and 5 X 5 for the case of 5 track parameters (e.g. for a magnetic field case).
  ROOT::Math::SMatrix<double, 4, 4> m_chiMat4;
  ROOT::Math::SMatrix<double, 5, 5> m_chiMat5;


  // internal variables
  Gaudi::XYZPoint cAlignD;
  Gaudi::XYZPoint sD;
  Gaudi::XYZPoint mC;
  Gaudi::XYZVector kv;
  Gaudi::XYZVector mv;
  double m_weight;
  double m_gamma;

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

  // track parameter vectors 
  std::vector<double> m_trx0;
  std::vector<double> m_trtx;
  std::vector<double> m_try0;
  std::vector<double> m_trty;
  std::vector<double> m_trQ;
  std::vector<double> m_trf;

  //MD
  double m_local_mat[4][4];
  
};



#endif // TRACKTOOLS_JBSelector_H
