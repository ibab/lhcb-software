#ifndef Alignment_TAConfig_H
#define Alignment_TAConfig_H
//-----------------------------------------------------------------------------
/** @file TAConfig.h
 *
 *  Header file for Tstation alignment : TAConfig
 *
 *  CVS Log :-
 *  $Id: TAConfig.h,v 1.19 2009-12-26 23:23:48 jblouw Exp $
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

// geometry
#include "DetDesc/IDetectorElement.h"
#include "DetDesc/IGeometryInfo.h"

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
   
  /** Millepede-style alignment problem was properly configured
   *
   *  @param 
   *
   *  @return 
   *  @retval 
   *  @retval 
   */
  StatusCode Initialize( std::vector<std::string> &);
  StatusCode finalize();


  StatusCode TAConfig::ConfMatrix( double rmeasI, 
                                   double wght,
                                   unsigned int rank, 
                                   const double,
                                   double local_vec[],
                                   double local_mat[][4],
                                   double stereo_angle
                                   );
  
  
  StatusCode ZeroMatrVec( double [][4], 
                          double [] );
  int InvMatrix( double [][4], 
                 double [], 
                 const int & ); 

  StatusCode TAConfig::ConfMatrixP( double rmeasI, 
                                    double wght,
                                    unsigned int rank, 
                                    const double,
                                    double local_vec[],
                                    double local_mat[][5],
                                    double stereo_angle
                                    );
  
  
  StatusCode ZeroMatrVecP( double [][5], 
                           double [] );
  int InvMatrixP( double [][5], 
                  double [], 
                  const int & ); 

  StatusCode CalcResidual( const LHCb::Track*,
                           const LHCb::LHCbID &,
                           int ,
                           struct Point &,
                           double &,
                           bool, double &, double & ,
                           double &, struct Point &);
  

  StatusCode FillMatrix( int ,
                         const struct Point ,
                         const double,
                         const double,
                         const double,
                         double );
  
  
  
  StatusCode CacheDetElements();
  StatusCode ResetGlVars();
  StatusCode GlobalFit( std::vector<double> & parameter, 
                        std::vector<double> & error,
                        std::vector<double> hitRnk) 
  {
    //exclude objects with too less hits
    for(unsigned p=0;p<hitRnk.size();p++){
      if(hitRnk.at(p) <=10 ){
        m_Centipede->ParSig(p,0.);
        info() <<" parameter removed from fit : "<< p << " ( "<< hitRnk.at(p)<< " hits ) "<<endreq;
      }
    }
    
    
    

    StatusCode sc = m_Centipede->MakeGlobalFit( parameter, error );
    if ( sc.isFailure() ) {
      info() << "Error in Millepede's !";
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  };
  
  StatusCode LocalTrackFit( int &tr_cnt,
                            std::vector< std::vector< double> >  locVec,
                            std::vector<double> &trpar,
                            const int  single_fit,
                            std::vector<double> & estimated,
                            double & chi2,
                            double & residual,
                            unsigned int itera,
                            int locrank) {
     
    StatusCode sc = m_Centipede->FitLoc(tr_cnt, locrank,locVec,trpar, single_fit , estimated, chi2, residual, itera);
    if ( sc.isFailure() ){
      debug() << "Local Fit rejected: Millepede local fit returns " << sc.getCode() << endreq;
      return StatusCode::FAILURE;
    }
    else
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
    return false;
  };

  StatusCode GetHalflayerReference(DeOTLayer* lay, Gaudi::XYZPoint& ref,Gaudi::XYZPoint& nom, int rank);
  

  std::vector<bool> NumDOF() {
    return m_dof;
  };
  int NumTrPars() {
    return m_ntrack_pars;
  };
  //MD
  int NumAlignPars() {
    return m_n_dof * m_nAlignObj;
  };
  //MD get number of consrainted objects
  inline  unsigned int GetNoFixedObj() {
    return m_cntfix;
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
  DetectorElement* TAConfig::GetDetEl(){
    return m_deot;
  };
  void CheckLChi2(const double &, const int &,const int &, bool &,double );
  //MD useless...   Gaudi::XYZVector Residual( LHCb::Track &, LHCb::LHCbID & );
  StatusCode Rank( LHCb::LHCbID &, int & );
  //MD
  inline double GetChi2Scale(){
    return m_chi2Scale;
  };
  inline double GetMinChi2(){
    return m_minChi2;
  };
  inline double GetOutlierMax(){
    return m_outlier;
  };
  inline std::multimap<int, std::pair<double, double> > GetRnkZAngleMap(){
    return m_RnkZAngle;
  };
  inline void SetMeanZ(double meanz) {
    m_meanZ = meanz;
  };
  inline double GetMeanZ(){
    return m_meanZ;
  };
  inline double GetDetEl_meanZ(){
    return m_misalDetEl_meanZ;
  };  
  inline std::map<std::string, int> GetCMap(){
    return m_C_pos;
  };  
  inline std::vector<double> GetDetEl_Z(){
    return m_misalDetEl_Z;
  };  
  inline std::vector<double> GetStereoAngle(){
    return m_stereoAngle;
  };  
  inline std::vector<double> GetMisalInput_X(){
    return m_misalInput_X;
  };  
  inline std::vector<double> GetMisalInput_Y(){
    return m_misalInput_Y;
  };  
  inline std::vector<double> GetMisalInput_Z(){
    return m_misalInput_Z;
  };  
  inline std::vector<double> GetMisalInput_A(){
    return m_misalInput_A;
  };  
  inline std::vector<double> GetMisalInput_B(){
    return m_misalInput_B;
  };  
  inline std::vector<double> GetMisalInput_C(){
    return m_misalInput_C;
  };  
  inline int nRanks(){
    return m_nAlignObj;
  };  
  /*****************************
   *  MD get NewMeasuremen    *
   *****************************/
  StatusCode NewMeasurement(double & meas, const int rank  ,
                            const struct Point pred, double zpos,
                            double stereo_angle);

private:
  
  StatusCode GetAlignmentConstants( const std::string & );
  StatusCode ConfigTT( std::vector<Gaudi::Transform3D> & );
  StatusCode ConfigOT( std::vector<Gaudi::Transform3D> & ,DeOTDetector*);
  StatusCode ConfigIT( std::vector<Gaudi::Transform3D> & );
  void CreateMap( int &, IDetectorElement*, double & );
  void CreateHalflayerReferenceMap( int , Gaudi::XYZPoint);
  StatusCode RetrieveAPars( const std::string & );
  StatusCode ConfigMillepede();
  //MD  StatusCode ConstrainPositions( std::map<std::string,int> & );
  //MD StatusCode ConstrainPositions( const int &, const std::string &, const int & );
  StatusCode ConstrainPositionsOT(); //MD
  /*****************************
   *  MD Constrain Movements of objects  *
   * via ParSig in MP            *
   *****************************/
  StatusCode ConstrainMovements();
  StatusCode ConstrainLagrangeMultOT(); //MD
  //make xml
  StatusCode PrintParameters( std::vector<double> & , int);
  
  //  StatusCode GlobalFit();
  void VectortoArray(const std::vector<double>& , double[] );
  void ArraytoVector(const double[], std::vector<double>& , int);
  std::string m_inputcontainer;
  std::string m_CentipedeTool;
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
  double m_zmean_ot,m_Sigzmean_ot;
  double m_zmoy_it, s_zmoy_it;
  double m_zmoy_tt, s_zmoy_tt;
  //MD
  std::vector< double > m_consTX;//constraint via LM for shift in x
  std::vector< double > m_consTU;
  std::vector< double > m_consTV;
  std::vector< double > m_consTZ;
  std::vector< double > m_shearXZ;
  std::vector< double > m_shearXU;
  std::vector< double > m_shearXV;
  std::vector< double > m_scaleZ;
  std::vector< double > m_layerZ;
  
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
  bool m_otSys, m_otStation, m_otLayer, m_otQuadrant, m_otModule, m_otHalfLayer, m_otModuleShort;
  //  std::vector<bool> m_ITmap;
  bool m_itSys, m_itStation, m_itBox, m_itLayer, m_itLadder;
  //  std::vector<bool> m_TTmap;
  bool m_ttSys, m_ttStation, m_ttLayer, m_ttLadder;
  std::string m_itCond, m_ttCond, m_otCond;
  //
  // Millepede configuration variables
  std::map<std::string, int> m_C_pos;
  std::map<int, Gaudi::XYZPoint> m_HLRefmap;
  std::multimap<int, std::pair<double,double> > m_RnkZAngle; //MD
  std::map<std::string, int> constrain_it, constrain_ot, constrain_tt, constrain_velo;

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

  std::string m_previous_de;



  // VELO specific stuff


  // bools to define which is to be aligned (system, stations or layers)  
  // Translation and Rotations as obtained
  // from the Cond db, for all detectors
  std::vector<Gaudi::XYZVector> m_translation;
  std::vector<Gaudi::RotationX> m_rotationX;
  std::vector<Gaudi::RotationY> m_rotationY;
  std::vector<Gaudi::RotationZ> m_rotationZ;

  //M
  std::vector<std::string> m_OTConstrain; // the constrains
  std::vector<std::string> m_OTConstrainMove; // constrain movements

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

  //MD check MP track fit
  std::vector< std::vector<double> > m_trackpoints;

  //MD all new Florin stuff
  void SetTrackPar(const std::vector<double>);
  StatusCode SetAlignmentPar(const std::vector<double> & );
  void TAConfig::MakeAlignParVec() ;
  void TAConfig::MakeTrackParVec() ;

  unsigned int m_cntfix;
  
  
  //Florin adds
  // track parameters vectors 
  std::vector<double> m_trx0;
  std::vector<double> m_trtx;
  std::vector<double> m_try0;
  std::vector<double> m_trty;
  std::vector<double> m_trQ;

  std::vector<double> m_estimate;  
  std::vector<double> m_estimateB4;  
  std::vector<double> m_stereoAngle;  

  std::vector<double> m_misalInput_X;
  std::vector<double> m_misalInput_Y;
  std::vector<double> m_misalInput_Z;
  std::vector<double> m_misalInput_A;
  std::vector<double> m_misalInput_B;
  std::vector<double> m_misalInput_C;

  std::vector<double> m_prevAlipar;

  std::vector<double> m_misalDetEl_Z;
  double m_misalDetEl_meanZ;

  double m_scale;
  double m_scaleIter;
  double m_meanZ;
  double m_chi2Scale;  
  double m_minChi2;  
  double m_outlier;  
  int m_nAlignObj;
  Gaudi::XYZPoint  m_pointa;
  Gaudi::XYZPoint  m_pointb;
  double m_ConstrainMove;
  bool m_Simulation;
};



#endif // TRACKTOOLS_JBSelector_H
