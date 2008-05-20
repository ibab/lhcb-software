/** @file TAConfig.cpp
 *
 *  Implementation file for Millepede configuration tool : TAConfig
 *
 *  CVS Log :-
 *  $Id: TAConfig.cpp,v 1.19 2008-05-20 09:29:20 jblouw Exp $
 *
 *  @author J. Blouw (johan.blouw@mpi-hd.mpg.de)
 *          M. Deissenroth (marc.deissenroth@physi.uni-heidelberg.de)
 *          F. Maciuc (florin.maciuc@mpi-hd.mpg.de)
 *  @date   12/04/2007
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartRef.h"


// Kernel
#include "Kernel/LHCbID.h"
#include "Kernel/DifTraj.h"
#include "Kernel/AlignTraj.h"
//#include "Kernel/SerializeStl.h"

//#include "StringConvert.h"
// Event
#include "Event/StateTraj.h"
#include "Event/Track.h"
#include "Event/MuonMeasurement.h"
#include "Event/OTMeasurement.h"
#include "Event/STMeasurement.h"

#include "STDet/DeSTLayer.h"
#include "STDet/DeITLayer.h"
#include "STDet/DeTTLayer.h"
#include "STDet/DeITSector.h"
#include "STDet/DeSTDetector.h"
#include "STDet/DeITDetector.h"
#include "STDet/DeTTDetector.h"
#include "STDet/DeTTHalfModule.h"
#include "MuonDet/DeMuonChamber.h"

// Conditions
#include "DetDesc/IAlignment.h"
#include "DetDesc/AlignmentCondition.h"
#include "DetDesc/IGeometryInfo.h"
//local header files
#include "TAConfig.h"

//#include "Event/STLiteCluster.h"
using namespace LHCb;
using namespace Gaudi;

DECLARE_TOOL_FACTORY( TAConfig );

//-----------------------------------------------------------------------------

// some useful functions:

void FChiMat4D( double weight,
		double rmeas,
                std::vector<double> &local_vec,
                std::vector<double> &derLC,
                ROOT::Math::SMatrix<double, 4> &local_mat );

void FChiMat5D( double weight,
		double rmeas,
                std::vector<double> &local_vec,
                std::vector<double> &derLC,
                ROOT::Math::SMatrix<double, 5> &local_mat );

int invMatrix( ROOT::Math::SMatrix<double, 4, 4> &v,
                         std::vector<double> &b,
                         int n );
 
int invMatrix( ROOT::Math::SMatrix<double, 5, 5> &v,
                         std::vector<double> &b,
                         int n );

double StereoAngle( IDetectorElement *det, const std::string &detname );

TAConfig::TAConfig( const std::string& type,
                    const std::string& name,
                    const IInterface* parent ):
  GaudiTupleTool ( type, name , parent ),
  m_n_dof( 0 ),
  velo_detector(false),
  tt_detector(false),
  it_detector(false),
  ot_detector(false),
  muon_detector(false) {

  // Interface
  declareInterface<ITAConfigTool>(this);
  info() << "Creating TAConfig object" << endreq;
  // options
  declareProperty("MillepedeTool" , m_CentipedeTool  = "Centipede");
  declareProperty("DerivativeTool", m_derivativTool = "Derivatives");
  declareProperty("MeasurementProvider", m_MeasProvider = "MuonMeasurementProvider");
  declareProperty("nTrackModelParameters", m_ntrack_pars = 4 );
  declareProperty("CommonXFraction", m_commonXFraction = 0.69 );
  declareProperty("Degrees_of_Freedom", m_dof );
  declareProperty("Constraint_Equations",m_constraint);
  declareProperty("PenaltyTerms", m_sigma );
  declareProperty("Chi_sq_factor", m_l_chimax = 10.0 );
  declareProperty("chi2_scale", m_scale = 10000.0 );
  declareProperty("n_f_stdev", m_nstand = 10.0 );
  declareProperty("residual_cut", m_residual_cut = 20.0 );
  declareProperty("initial_residual_cut", m_ini_residual_cut = 10.0 );
  declareProperty("n_l_stdev", m_nstd = 10 );
  declareProperty("Velo_left_modules", m_VELOmap_l);
  declareProperty("Velo_right_modules",m_VELOmap_r);
  declareProperty("Fix_alignable_objects", m_fix = true);
  declareProperty("Constrain_IT", m_ITConstrain); //MD
  declareProperty("Constrain_OT", m_OTConstrain); //MD
  declareProperty("Constrain_VeLo", m_VeLoConstrain); //MD
  declareProperty("Constrain_Muon", m_MuonConstrain); //MD
  declareProperty("Constrain_TT", m_TTConstrain); //MD
  //  declareProperty("FixDofs", m_fix_dofs );
  //  declareProperty("OT_objects", m_OTmap );
  declareProperty("OT_system",m_otSys = false );
  declareProperty("OT_station",m_otStation = false );
  declareProperty("OT_layer",m_otLayer = false );
  declareProperty("OT_quadrant",m_otQuadrant = false );
  declareProperty("OT_module",m_otModule = false );
  //  declareProperty("IT_objects", m_ITmap );
  declareProperty("IT_system",m_itSys = false );
  declareProperty("IT_station",m_itStation = false );
  declareProperty("IT_box",m_itBox = false );
  declareProperty("IT_layer",m_itLayer = false );
  declareProperty("IT_ladder",m_itLadder = false );
  // Muon chamber
  declareProperty("MUON_system", m_muSys = false );
  declareProperty("MUON_stations", m_muStation = false );
  declareProperty("MUON_chambers", m_muChamber = true );
  //  declareProperty("TT_objects", m_TTmap );
  declareProperty("TT_system",m_ttSys = false );
  declareProperty("TT_station",m_ttStation = false );
  declareProperty("TT_layer",m_ttLayer = false );
  declareProperty("IT_alignment_condition_location", m_itCond = "");
  declareProperty("TT_alignment_condition_location", m_ttCond = "");
  declareProperty("OT_alignment_condition_location", m_otCond = "");
  debug() << "Millepede tool " << m_CentipedeTool << endreq;
  debug() << "Derivative tool " << m_derivativTool << endreq;
  debug() << "nTrackModelParameters " << m_ntrack_pars << endreq;
  debug() << "Degrees_of_Freedom " << m_dof << endreq;
  //  m_DOF = new bool[m_dof.size()];
}

TAConfig::~TAConfig() {}

StatusCode TAConfig::Initialize( std::vector<std::string> &m_dets ) {
  m_detectors = m_dets;
  // Millepede tool
  m_Centipede = tool<ICentipede>( m_CentipedeTool );
  if ( m_Centipede == NULL ) {
     error() << "Error instantiating Centipede class" << endreq;
     return StatusCode::FAILURE;
  }  
  // Measurement provider 
  m_measprovider = tool<IMeasurementProvider>("MeasurementProvider");
  // derivatives tool
  m_derivatives = tool<IDerivatives>( "Derivatives");
  // Track extrapolator
  m_extrapolator = tool<ITrackExtrapolator>( "TrackMasterExtrapolator" );
  // - trajecory poca tool
  m_poca = tool<ITrajPoca>( "TrajPoca" );
  // magnetic field tool
  m_bField = svc<IMagneticFieldSvc>( "MagneticFieldSvc",true );
  // Count number of degrees of freedom
  for( unsigned int i = 0; i < m_dof.size(); i++ ) 
    if ( m_dof[i] )
      m_n_dof++;
  // set size of vector containing derivatives to track parameters
  m_derLC.resize( m_ntrack_pars, 0.0);
  return StatusCode::SUCCESS;
}

StatusCode TAConfig::CacheDetElements() {
  // loop over detectors to be aligned and retrieve geometry information
  std::vector<std::string>::iterator t_i;
  info() << "Number of detectors to be aligned: " << m_detectors.end() - m_detectors.begin() << endreq;
  StatusCode sc;
  for (  t_i = m_detectors.begin(); t_i != m_detectors.end(); ++t_i ) {
    info() << "Detector " << *t_i << " will be aligned" << endreq;
    if ( *t_i == "Velo" ) {
      velo_detector = true;
      // geometry information (i.e. nominal positions...)
      m_velo = getDet<DeVelo>(DeVeloLocation::Default);
      m_develo = getDet<DetectorElement>(DeVeloLocation::Default);
      if ( m_velo == 0 ) {
        error() << "Error retrieving pointer to VeLo geometry!" << endmsg;
        return StatusCode::FAILURE;
      }
      // information from conditions db on positions...
      if ( sc.isFailure() ) {
        error() << "Error retrieving VeLo alignment conditions" << endreq;
        return sc;
      }
      // Configure alignment procedure for the VeLo. Conform S.Viret's code
      //      VELOmap.resize( m_velo->numberRightRSensors() + m_velo->numberRightPhiSensors() ) ;
      // 42 sensors... (2 X 21 stations)
      //      sc = DefineVeloMap( m_DETmap );
      if ( sc.isFailure() ) {
        error() << "Error in DefineVeloMap" << endreq;
        return sc;
      }
      info() << "Succesfully defined the velomap" << endreq;
      //      sc = ConfigVelo( m_DETmap );
      if ( sc.isFailure() ) {
        error() << "Error in ConfigVelo" << endreq;
        return sc;
      }
    }
    if ( *t_i == "TT" ) {
      info() << "Get the TT geometry info" << endreq;
      m_tt = getDet<DeSTDetector>(DeSTDetLocation::TT);
      m_dett = getDet<DetectorElement>(DeSTDetLocation::TT);
      tt_detector = true;
      sc = ConfigTT( m_DETmap );
      if ( sc.isFailure() ) {
        return sc;
      }
    }
    if ( *t_i == "IT" ) {
      info() << "Get the IT geometry info" << endreq;
      m_it = getDet<DeSTDetector>(DeSTDetLocation::IT);
      m_deit = getDet<DetectorElement>(DeSTDetLocation::IT);
      it_detector = true;
      sc = ConfigIT( m_DETmap );
      if ( sc.isFailure() ) {
      	return sc;
      }
    }
    if ( *t_i == "OT" ) {
      info() << "Get the OT geometry info" << endreq;
      m_ot = getDet<DeOTDetector>(DeOTDetectorLocation::Default);
      m_deot = getDet<DetectorElement>(DeOTDetectorLocation::Default);
      ot_detector = true;
      sc = ConfigOT( m_DETmap );
      if ( sc.isFailure() ) {
        return sc;
      }
    }
    if ( *t_i == "MUON" || *t_i == "Muon" ) {
      info() << "Get the Muon geometry info" << endreq;
      m_muon = getDet<DeMuonDetector>(DeMuonLocation::Default);
      m_demuon = getDet<DetectorElement>(DeMuonLocation::Default);
      muon_detector = true;
      sc = ConfigMuon( m_DETmap );
      if ( sc.isFailure() ) {
        return sc;
      }
    }
    info() << "Continue initialization..." << endreq;
    info() << "Number of elements in m_DETmap : " << m_DETmap.size() << endreq;
  }
  // Initialize the derivative tool with the number of track parameters,
  // the number of alignable objects, and the degrees of freedom
  info() << "Degrees of freedom: " << m_dof << endreq;
  info() << "Total number of global parameters:" << m_n_dof * m_DETmap.size() << endreq;
  m_derivatives->initialize( m_ntrack_pars, m_DETmap.size(), m_dof );

  info() << "Number of elements in m_DETmap : " << m_DETmap.size() << endreq;
  //  m_totStations = m_VeloStations + m_TTStations + m_ITStations + m_OTStations;
  info() << "Printing ranking vector..." << endreq;
  info() << "Rank = " << m_rank << endreq;
  info() << "Finished initialization!" << endreq;
  info() << "Configuring Millepede..." << endreq;
  sc = ConfigMillepede();
  if ( sc.isFailure() ) {
    error() << "Error configuring Millepede" << endreq;
    return StatusCode::FAILURE;
  }
  // Ok, Millepede has been initalized, now we can do the constraints.
  // For Outer Tracker
  if ( m_fix && ot_detector ) {
    sc = ConstrainPositions( m_OTConstrain, "OT" );
  }
  // For Inner Tracker
  if ( m_fix && it_detector ) {
    sc = ConstrainPositions( m_ITConstrain, "IT" );
  }
  // For Trigger Tracker
  if ( m_fix && tt_detector ) sc = ConstrainPositions( m_TTConstrain, "TT" );
  // For Muon
  if ( m_fix && muon_detector ) sc = ConstrainPositions( m_MuonConstrain, "Muon" );
  // For Velo
  if ( m_fix && velo_detector ) sc = ConstrainPositions( m_VeLoConstrain, "VeLo" );
  // initialize estimates of misalignments:
  m_estimatedB4.resize( m_n_dof * m_DETmap.size(), 0.0); //need to account for first
  m_estimated.resize( m_n_dof * m_DETmap.size(), 0.0);
  return sc;
}

StatusCode TAConfig::ConfigTT( std::vector<Gaudi::Transform3D> & TTmap ) {
  unsigned int nStations = m_tt->nStation();
  //  tt = TTmap.size();
  Gaudi::Rotation3D Rotation;
  Gaudi::XYZVector position;
  // =========================
  // First define the 'map': this is a vector with
  // positions in 3d of the alignable detector elements
  // =========================
  //m_dett->geometry()->matrixInv().GetDecomposition( Rotation, position);
  m_dett->geometry()->toLocalMatrix().GetDecomposition( Rotation, position);
  info() << "TT system name & position: " << m_dett->name() << " "
         << position.x() << ", "
         << position.y() << ", "
         << position.z() << endreq;
  if ( m_ttSys && ! m_ttStation && ! m_ttLayer ) {
    TTmap.push_back( m_dett->geometry()->toLocalMatrix() );
    AlignmentCondition *tt_cond = const_cast<AlignmentCondition*>
      (m_dett->geometry()->alignmentCondition());
    m_ALImap.push_back( tt_cond->offNominalMatrix() ); // save the alignment conditions of the tt sys
  }

  m_zmoy_tt = 0;
  int rank = m_rank.size();
  m_TTStations = m_dett->childIDetectorElements();
  for ( unsigned int i = 0; i < m_TTStations.size(); i++ ) {
    info() << "TT station name = " << m_TTStations[i]->name() << endreq;
    m_TTLayers = m_TTStations[i]->childIDetectorElements();
    for ( unsigned int j = 0; j < m_TTLayers.size(); j++ ) {
      info() << "TT layer name = " << m_TTLayers[j]->name() << endreq;
      CreateMap( rank, m_TTLayers[j], m_zmoy_tt );
      rank++;
    }
  }
  m_zmoy_tt /= nStations;

  for ( unsigned int i = 0; i < m_TTStations.size(); i++ ) {
    m_TTLayers = m_TTStations[i]->childIDetectorElements();
    for ( unsigned int j = 0; j < m_TTLayers.size(); j++ ) {
      (m_TTLayers[j]->geometry()->toLocalMatrix()).GetDecomposition(Rotation, position);
      s_zmoy_tt += (position.z()-m_zmoy_tt)*(position.z()-m_zmoy_tt);
    }
  }
  s_zmoy_tt /= nStations;
  return StatusCode::SUCCESS;
}
StatusCode TAConfig::ConfigMuon( std::vector<Gaudi::Transform3D> &MUmap ) {
//  unsigned int nStations = m_muon->stations();
  Gaudi::Rotation3D Rotation;
  Gaudi::XYZVector position;
  m_demuon->geometry()->toGlobalMatrix().GetDecomposition( Rotation, position );
  info() << "Muon system name & position: " << m_demuon->name() << " "
         << position.x() << " "
         << position.y() << " "
         << position.z() << endreq;
  m_zmoy_mu = 0;
  int rank = m_rank.size();
  if ( m_muSys && ! m_muStation && ! m_muChamber ) {
    MUmap.push_back( m_demuon->geometry()->toGlobalMatrix() );
    AlignmentCondition *mu_cond = const_cast<AlignmentCondition*>
      (m_demuon->geometry()->alignmentCondition() );
    m_ALImap.push_back( mu_cond->offNominalMatrix() );
    CreateMap( rank, m_demuon, m_zmoy_mu );
  }
  m_MUStations = m_demuon->childIDetectorElements();
  info() << "Number of Muon stations: " << m_MUStations.size() << endreq;
  for ( unsigned int i = 0; i < m_MUStations.size(); i++ ) { //Loop over stations
    m_MUChambers = m_MUStations[i]->childIDetectorElements();
    info() << "At DE : " << m_MUStations[i]->name() << endreq;
    for ( unsigned int j = 0; j < m_MUChambers.size(); j++ ) { // loop over chambers
      if ( m_muChamber ) {
        CreateMap( rank, m_MUChambers[j], m_zmoy_mu );
        info() << "Align DE " << m_MUChambers[j]->name() << endreq;
        rank++;
      }
    }
    if ( m_muStation && ! m_muChamber ) {
      CreateMap( rank, m_MUStations[i], m_zmoy_mu );
      info() << "De " << m_MUStations[i]->name() << " has rank " << rank << endreq;
      rank++;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode TAConfig::ConfigOT( std::vector<Gaudi::Transform3D> &OTmap ) {
  unsigned int nStations = m_ot->nStation();
  //  ot = OTmap.size();
  Gaudi::Rotation3D Rotation;
  Gaudi::XYZVector position;
  m_ot->geometry()->toLocalMatrix().GetDecomposition( Rotation, position );
  info() << "OT system name & position: " << m_ot->name() << " "
         << position.x() << " " 
         << position.y() << " "
         << position.z() << endreq;
  if ( m_otSys && ! m_otStation && ! m_otLayer && ! m_otQuadrant && ! m_otModule ) {
    OTmap.push_back( m_ot->geometry()->toLocalMatrix() );
    AlignmentCondition *ot_cond = const_cast<AlignmentCondition*>
      (m_ot->geometry()->alignmentCondition() );
    m_ALImap.push_back( ot_cond->offNominalMatrix() );
  }
  
  m_zmoy_ot = 0;
  int rank = m_rank.size();
  m_OTStations = m_ot->childIDetectorElements();
  for ( unsigned int i = 0; i < m_OTStations.size(); i++ ) {
    m_OTLayers = m_OTStations[i]->childIDetectorElements();
    for ( unsigned int j = 0; j < m_OTLayers.size(); j++ ) {
      info() << "-----------------------------" << endreq;
      info() << "Layer name = " << m_OTLayers[j]->name() << endreq;
      const AlignmentCondition *ot_cond = m_OTLayers[j]->geometry()->alignmentCondition();
      debug() << "Alignment condition (1) = " << ot_cond->offNominalMatrix() << endreq;
      debug() << "Alignment condition (2) = " << ot_cond->toNominalMatrix() << endreq;
      m_OTQuadrants = m_OTLayers[j]->childIDetectorElements();
      for ( unsigned int k = 0; k < m_OTQuadrants.size(); k++ ) {
        m_OTModules = m_OTQuadrants[k]->childIDetectorElements();
        for ( unsigned int l = 0; l < m_OTModules.size(); l++ ) {
          if ( m_otModule ) {
            CreateMap( rank, m_OTModules[l], m_zmoy_ot );
            info() << "De " << m_OTModules[l]->name() << " has rank " << rank << endreq;
            rank++;
          }
        }
        if ( m_otQuadrant && ! m_otModule ) {
          CreateMap( rank, m_OTQuadrants[k], m_zmoy_ot );
          info() << "De " << m_OTQuadrants[k]->name() << " has rank " << rank << endreq;
          rank++;
        }
      }
      if ( m_otLayer && ! m_otQuadrant && ! m_otModule ) {
        CreateMap( rank, m_OTLayers[j], m_zmoy_ot );
        info() << "De " << m_OTLayers[j]->name() << " has rank " << rank << endreq;
        rank++;
      }
    }
    if ( m_otStation && ! m_otLayer && ! m_otQuadrant && ! m_otModule ) {
      CreateMap( rank, m_OTStations[i], m_zmoy_ot );
      rank++;
    }
  }
  m_zmoy_ot /= nStations;
  for ( unsigned int i = 0; i < m_OTStations.size(); i++ ) {
    m_OTLayers = m_OTStations[i]->childIDetectorElements();
    for ( unsigned int j = 0; j < m_OTLayers.size(); j++ ) {
      m_OTQuadrants = m_OTLayers[j]->childIDetectorElements();
      for ( unsigned k = 0; k < m_OTQuadrants.size(); k++ ){
        m_OTModules = m_OTQuadrants[k]->childIDetectorElements();
        for ( unsigned int l = 0; l < m_OTModules.size(); l++ ) {
          if ( m_otModule ) {
            m_OTModules[l]->geometry()->toLocalMatrix().GetDecomposition( Rotation, position );
            s_zmoy_tt += (position.z()-m_zmoy_ot)*(position.z()-m_zmoy_ot);
          }
        }
        if ( m_otQuadrant && ! m_otModule ) {
          m_OTQuadrants[k]->geometry()->toLocalMatrix().GetDecomposition( Rotation, position );
          s_zmoy_tt += (position.z()-m_zmoy_ot)*(position.z()-m_zmoy_ot);
        }
      }
      if ( m_otLayer && ! m_otQuadrant && ! m_otModule ) {
        m_OTLayers[j]->geometry()->toLocalMatrix().GetDecomposition( Rotation, position );
        s_zmoy_tt += (position.z()-m_zmoy_ot)*(position.z()-m_zmoy_ot);
      }
    }
    if ( m_otStation && ! m_otLayer && ! m_otQuadrant && ! m_otModule ) {
      m_OTStations[i]->geometry()->toLocalMatrix().GetDecomposition( Rotation, position );
      s_zmoy_tt += (position.z()-m_zmoy_ot)*(position.z()-m_zmoy_ot);
    }
  }
  s_zmoy_ot /= nStations;
  return StatusCode::SUCCESS;
}

StatusCode TAConfig::ConfigIT( std::vector<Gaudi::Transform3D> &ITmap ) {
  unsigned int nStations = m_it->nStation();
  //  it = ITmap.size();
  Gaudi::Rotation3D Rotation;
  Gaudi::XYZVector position;
  m_deit->geometry()->toLocalMatrix().GetDecomposition( Rotation, position );
  info() << "IT system name & position: " << m_deit->name() << " "
         << position.x() << ", "
         << position.y() << ", "
         << position.z() << endreq;
  if ( m_itSys && ! m_itStation && ! m_itLayer && ! m_itBox ) {
    ITmap.push_back( m_deit->geometry()->toLocalMatrix() );
    AlignmentCondition *it_cond = const_cast<AlignmentCondition*>
      (m_deit->geometry()->alignmentCondition() );
    m_ALImap.push_back( it_cond->offNominalMatrix() );
  }

  m_zmoy_it = 0.0;
  int rank = m_rank.size();
  m_ITStations = m_deit->childIDetectorElements();
  for ( unsigned int i = 0; i < m_ITStations.size(); i++ ) { // Loop over stations
    // Note: there are actually 4 IT stations, since one of them is the radiation monitor...
    m_ITBoxes = m_ITStations[i]->childIDetectorElements();
    for ( unsigned int j = 0; j < m_ITBoxes.size(); j++ ) { // Loop over boxes
      m_ITLayers = m_ITBoxes[j]->childIDetectorElements();
      //      info() << "Inv Matrix of De " << m_ITBoxes[j]->name() << " = " 
      //<< m_ITBoxes[j]->geometry()->matrixInv() << endreq;
      for ( unsigned int k = 0; k < m_ITLayers.size(); k++ ) { // Loop over layers
        //	info() << "Inv Matrix of De " << m_ITLayers[k]->name() << " = " 
        //<< m_ITLayer.s[k]->geometry()->matrixInv() << endreq;
        info() << "Align De " << m_ITLayers[k]->name() << endreq;
        const AlignmentCondition *it_cond = m_ITLayers[k]->geometry()->alignmentCondition();
        debug() << "Alignment condition (1) = " << it_cond->offNominalMatrix() << endreq;
        debug() << "Alignment condition (2) = " << it_cond->toNominalMatrix() << endreq;
        m_ITLadders = m_ITLayers[k]->childIDetectorElements();
        for ( unsigned int l = 0; l < m_ITLadders.size(); l++ ) {
          if ( m_itLadder ) {
            CreateMap( rank, m_ITLadders[l], m_zmoy_it );
            rank++;
          }
        }
        if ( m_itLayer && ! m_itLadder && ! m_itBox && ! m_itStation ) {
          CreateMap( rank, m_ITLayers[k], m_zmoy_it );
          rank++;
        }
      }
      if ( m_itBox && ! m_itLayer && ! m_itLadder && ! m_itStation) {
        CreateMap( rank, m_ITBoxes[j], m_zmoy_it );
        rank++;
      }
    }
    if ( m_itStation && (! m_itBox || ! m_itLayer) ) {
      CreateMap( rank, m_ITStations[i], m_zmoy_it );
      rank++;
    }
  }
  m_zmoy_it /= nStations;
  s_zmoy_it = 0.0;
  for ( unsigned int i = 0; i < m_ITStations.size(); i++ ) {
    m_ITBoxes = m_ITStations[i]->childIDetectorElements();
    for ( unsigned int j = 0; j < m_ITBoxes.size(); j++ ) {
      m_ITLayers = m_ITBoxes[j]->childIDetectorElements();
      for ( unsigned int k = 0; k < m_ITLayers.size(); k++ ) {
        if ( m_itLayer ) {
          (m_ITLayers[k]->geometry()->toGlobalMatrix()).GetDecomposition(Rotation, position);
          s_zmoy_it += (position.z() - m_zmoy_it)*(position.z() - m_zmoy_it);
        }
      }
      if ( m_itBox && ! m_itLayer ) {
        (m_ITBoxes[j]->geometry()->toGlobalMatrix()).GetDecomposition(Rotation, position);
        s_zmoy_it += (position.z() - m_zmoy_it)*(position.z() - m_zmoy_it);
      }
    }
    if ( m_itStation && ! m_itBox && ! m_itLayer ) {
      (m_ITStations[i]->geometry()->toGlobalMatrix()).GetDecomposition(Rotation, position);
      s_zmoy_it += (position.z() - m_zmoy_it)*(position.z() - m_zmoy_it);
    }
  }
  s_zmoy_it /= nStations;
  return StatusCode::SUCCESS;
}



StatusCode TAConfig::StoreParameters( std::vector<double> &ali_par ) {
  // Alternative:
  std::vector<double>::iterator i = ali_par.begin();
  unsigned int msize = m_C_pos.size();
  // loop over the detector elements we wanted to align.
  // check for each element which degrees of freedom were determined.
  for (std::map<std::string,int>::iterator t = m_C_pos.begin(); t != m_C_pos.end(); ++t ) {
    info() << "Parameter " << t->first << " " << t->second  << " " << ali_par[t->second] << endreq;
    IDetectorElement *det = getDet<IDetectorElement>(t->first);
    IGeometryInfo *geo = det->geometry();
    SmartRef<Condition> cond =  det->condition(t->first);
    //    IAlignment *alicond = dynamic_cast<IAlignment*>( cond.target() );
    
    // Convert alignment parameters into
    // a vector containing translations
    // a vector containing rotations and
    // a vector with the pivot point. 
    // That vector should actually be obtained from the detector element!
    std::vector<double> translation(3,0.0);
    std::vector<double> rotation(3,0.0);
    std::vector<double> pivot(3,0.0);
    // fill the translation & rotation vectors
    // by looping over the number of degrees of freedom
    // the vector m_align is filled according to the following scheme:
    // first loop over detector elements, then loop over the degrees of 
    // freedom.
    unsigned int k = m_dof[0] + m_dof[1] + m_dof[2];
    unsigned int l = 0;
    unsigned int m = 0;
    // Store M'pede alignment parameters in a translation
    // and rotation std::vector
    for ( unsigned int d = 0; d < 3; d++ ) {
      unsigned int pos1 = t->second;// translations
      unsigned int pos2 = t->second + msize * k; //rotations
      if ( m_dof[d] ) {
	translation[d] = ali_par[pos1 + m * msize ];
	m++;
      }
      if ( m_dof[d+3] ) {
	rotation[d] = ali_par[pos2 + l * msize];
	l++;
      }
    }
    // Get the stereo angle of the detector element
    double stereo_angle = StereoAngle( det, t->first );
    double aa11 = 1.0;
    double aa12 = -rotation[2];
    double aa13 = rotation[1];
    double aa21 = rotation[2];
    double aa22 = 1.0;
    double aa23 = -rotation[0];
    double aa31 = -rotation[1];
    double aa32 = rotation[0];
    double aa33 = 1.0;

    Gaudi::Rotation3D Rotation2, Rotation3, Rotation5;
    Gaudi::XYZVector position2, position3, position5;

    Rotation3.SetComponents( aa11, aa12, aa13, aa21, aa22, aa23, aa31, aa32, aa33 );
    position3.SetX( -1 * translation[0] * cos(stereo_angle) );
    position3.SetY( -1 * translation[0] * sin(stereo_angle) );
    position3.SetZ( translation[2] );

    geo->toGlobalMatrixNominal().GetDecomposition( Rotation2, position2);
    geo->alignmentCondition()->offNominalMatrix().GetDecomposition( Rotation5, position5 );
    double cc11,cc12,cc13,cc21,cc22,cc23,cc31,cc32,cc33;
    Rotation5.GetComponents(cc11,cc12,cc13,cc21,cc22,cc23,cc31,cc32,cc33);
    double alphaC =atan2(-cc12,cc11);
    double betaC = atan2(cc13,sqrt(cc11*cc11+cc12*cc12));
    double gammaC = atan2(-cc23,cc33);
    Gaudi::Rotation3D RTotalL;
    Gaudi::XYZVector PTotalL;
    double bb11,bb12,bb13,bb21,bb22,bb23,bb31,bb32,bb33;
    RTotalL.GetComponents(bb11,bb12,bb13,bb21,bb22,bb23,bb31,bb32,bb33);
    double alpha=atan2(-bb12,bb11);
    double beta = atan2(bb13,sqrt(bb11*bb11+bb12*bb12));
    double gamma = atan2(-bb23,bb33);
    RTotalL=(Rotation2.Inverse())*(Rotation3.Inverse())*Rotation2;
    PTotalL =  (Rotation2.Inverse())*position3 *(-1.0) ;
    
    info() << "Translation vector: " << translation << "\n"
	   << "Rotation vector   : " << rotation << "\n"
	   << "pivot point       : " << pivot << endreq;

    translation[0]=position5.x()-PTotalL.x();
    translation[1]=position5.y()-PTotalL.y();
    translation[2]=position5.z()-PTotalL.z();
    rotation[2]= alphaC- alpha;
    rotation[1]= betaC-beta;
    rotation[0]= gammaC-gamma; 
    StatusCode sc = geo->ownToOffNominalParams(translation,
    					       rotation,
    					       pivot);
    if ( sc.isFailure() ) {
      error() << "Error storing translation, rotation and pivot vectors\n"
	      << "from geometry object" << endreq;
      error() << "Translation vector : " << translation << "\n"
	      << "Rotation vector    : " << rotation << "\n"
	      << "pivot vector       : " << pivot << endreq;
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}




void TAConfig::CreateMap( int & r,  IDetectorElement* id, double &m_zmoy ) {
  Gaudi::Rotation3D R;
  Gaudi::XYZVector T;
  //  info() << "Name of object = " << id->name() << endreq;
  m_C_pos.insert(make_pair(id->name(),r ));
  m_rank.push_back( r );
  m_ALImap.push_back( id->geometry()->alignmentCondition()->offNominalMatrix() );
  m_DETmap.push_back( id->geometry()->toLocalMatrix() );
  info() << "Z-position" << id->geometry()->toLocalMatrix() << endreq;
  id->geometry()->toLocalMatrix().GetDecomposition( R, T );
  m_zmoy += T.z();
}

StatusCode TAConfig::ConfigMillepede() {
  int rs = m_rank.size();
  int num_objects = m_n_dof * rs;
  m_derGB.resize( m_n_dof * m_DETmap.size(), 0.0);
  //  for (unsigned int j = 0; j < 6; j++) 
  //    m_DOF[j] = m_dof[j];   // What are the parameters to align ?
  //  for (int j = 0; j < 9; j++) {m_CONSTRAINT[j] = m_sigma[j];}
  // Needed for Sebastien's Millepede initialization
  for ( unsigned int i = 0; i < m_dof.size(); i++ ) {// loop over degrees of freedom per detector
    m_SIGMA[i] = m_sigma[i];
  }
  //  info() << "Calling InitMille with m_sigma = " << m_sigma << "..." << endreq;
  /* HD Millepede initialization call */
  m_Centipede->InitMille( m_dof,
                          num_objects, m_ntrack_pars,  
                          m_l_chimax, m_nstd, 
                          m_residual_cut, m_ini_residual_cut);
  return StatusCode::SUCCESS;
}
/************************
 * Constrain positions of elements
 * rank: rank number in detector elements vector
 * de_name: detector element name 
 * rs: size of detector map
 ************************************/
//StatusCode TAConfig::ConstrainPositions( std::vector<std::string> &description, std::string &type ) {
StatusCode TAConfig::ConstrainPositions( std::vector<std::string> &description, char *ty ) {
  const unsigned int alipar = m_DETmap.size(); //nof params to align
  const unsigned int size = description.size();
  std::vector<int> cons(size,0);
  std::string type( ty );
  int det_type = 0;
  if ( type == "VeLo" )
    det_type = 1;
  else if ( type == "TT" )
    det_type = 2;
  else if ( type == "IT" )
    det_type = 3;
  else if ( type == "OT" )
    det_type = 4;
  else if ( type == "Muon" )
    det_type = 5;
  // find the offset in the rank
  int offset = 0;
  int range = 0;
  for ( std::map<std::string,int>::iterator t = m_C_pos.begin();
	t != m_C_pos.end(); ++t, offset++ ) {
    std::string f = (*t).first;
    if ( f.find( type ) != std::string::npos )
      break;
  }
  // calculate the range
  info() << "Counting back..." << endreq;
  for ( std::map<std::string,int>::iterator t = m_C_pos.begin();
	t != m_C_pos.end(); ++t ) {
    std::string f = (*t).first;
    if ( f.find( type ) != std::string::npos )
      range++;
  }
  info() << "For detector " << type << " offset = " << offset << " and range = " << range << endreq;
  for (unsigned int i = 0; i < size; i++){
    //interpret every string
    std::string str_fix = description[i];
    int dcnt =0; //count no dof
    std::ostringstream geo_string;
    std::ostringstream geo_string_2;
    geo_string << type << "/";
    geo_string_2 << "";
    info() << "Fixing : " << str_fix << endreq;
    for(unsigned int c=0; c < str_fix.size(); c++){
      switch (str_fix[c]) {
      case  'X' :
        if(m_dof[0] != 1 ) {
 	  error() << "--> problem constraining X" << endreq;
          error() << "--> Constrain does not match number of degree of freedom" << endreq;
          return StatusCode::FAILURE;
        } else 
	  dcnt = 1;
       break;
      case  'Y' :
        if(m_dof[1] != 1 ) {
 	  error() << "--> problem constraining Y" << endreq;
          error() << "--> Constrain does not match number of degree of freedom" << endreq;
          return StatusCode::FAILURE;
        }
        for ( int n=0; n < 2; n++ ) 
	  if( m_dof[n] ) 
	    dcnt++;
       break;
      case 'Z' :
        if(m_dof[2] != 1 ) {
 	  error() << "--> problem constraining Z" << endreq;
          error() << "--> Constrain does not match number of degree of freedom" << endreq;
          return StatusCode::FAILURE;
        }
        for ( int n=0; n < 3; n++ ) if(m_dof[n]) dcnt++;
       break;
      case 'a':
        if(m_dof[3] != 1 ) {
	  error() << "degree of freedom: " << m_dof[3] << endreq;
 	  error() << "--> problem constraining rotation alpha" << endreq;
          error() << "--> Constrain does not match number of degree of freedom" << endreq;
          return StatusCode::FAILURE;
        }
        for( int n=0; n < 4; n++ ) if(m_dof[n]) dcnt++;
       break;
      case 'b' :
        if(m_dof[4] != 1 ) {
 	  error() << "--> problem constraining rotation beta" << endreq;
          error() << "--> Constrain does not match number of degree of freedom" << endreq;
          return StatusCode::FAILURE;
        }
        for ( int n=0; n < 5; n++) if(m_dof[n]) dcnt++;
	info() << "dcnt = " << dcnt << " alipar = " << alipar << endreq;
       break;
      case 'c' :
        if ( m_dof[5] != 1 ) {
 	  error() << "--> problem constraining rotation gamma" << endreq;
          error() << "--> Constrain does not match number of degree of freedom" << endreq;
          return StatusCode::FAILURE;
        }
        for(int n=0; n < 6; n++) if(m_dof[n]) dcnt++;
	break;
      case '_' :
          int num = (str_fix[c+2] - '0');
	  switch ( str_fix[c+1] ) {
	  case 'S' :
	    if ( type == "OT" ) {
	      geo_string << "T" << str_fix[c+2] << "/";
	    } else if ( type == "IT" ) {
	      geo_string << "Station" << str_fix[c+2] << "/";
	    }
	    if ( num != 0 )
	      cons[i] = num;
            break;
	  case 'L' : // layers (for IT/OT)
	    switch ( det_type ) {
	    case 4 : // OT
	      switch ( str_fix[c+2] ) {
	      case '1' : 
		geo_string << "X1layer";
		break;
	      case '2' : 
		geo_string << "Ulayer";
		break;
	      case '3' :
		geo_string << "Vlayer";
		break;
	      case '4' :
		geo_string << "X2layer";
		break;
	      }
	      break;
	    case 3 : // IT
	      switch ( str_fix[c+2] ) {
	      case '1' : 
		geo_string_2 << "LayerX1";
		break;
	      case '2' :
		geo_string_2 << "LayerU";
		break;
	      case '3' :
		geo_string_2 << "LayerV";
		break;
	      case '4' :
		geo_string_2 << "LayerX2";
		break;
	      }
	      break;
	    }
	    if ( num != 0 ) {
	      cons[i] = 4 * ( cons[i] - 1 ) + num;
	    }
            break;
	  case 'B' : // boxes (for IT)
	    if ( type == "IT" ) {
	      switch ( str_fix[c+2] ) {
	      case '1' :
		geo_string << "TopBox/";
		break;
	      case '2' : 
		geo_string << "BottomBox/";
		break;
	      case '3' :
		geo_string << "LeftBox/";
		break;
	      case '4' :
		geo_string << "RightBox/";
		break;
	      }
	      geo_string << geo_string_2.str();
	    }
	    if ( num != 0 )
	      cons[i] = 4 * ( cons[i] - 1 ) + num;
            break;
	  case 'Q' :  // quadrants (for OT)
	    if ( num != 0 )
	      cons[i] = 4 * ( cons[i] - 1 ) + num;
            break;
	  case 'M' : 
	    if ( num != 0 && ot_detector ) // modules (OT)
	      cons[i] = 9 * ( cons[i] - 1 ) + num;
	    else if ( num != 0 && it_detector ) // modules (IT)
	      cons[i] = 7 * ( cons[i] - 1 ) + num;
            break;
	  }
	  break;
      }
    }
    int rank = -1;
    for ( std::map<std::string,int>::iterator t = m_C_pos.begin();
	  t != m_C_pos.end(); ++t ) {
      std::string f = (*t).first;
      if ( f.find( geo_string.str() ) != std::string::npos ) {
	rank = (*t).second;
	break;
      }
    }
    // Calculate the 'rank' of the constraint:
    int fix = offset * m_n_dof + (rank - offset) + range * (dcnt - 1);
    StatusCode sc;
    info() << "Detelement is at rank " << rank << " dof = " << dcnt << endreq;
    info() << "Trying to fix it at pos = " << fix << endreq;
    sc = m_Centipede->ParSig(fix, 0.0);
    if ( sc.isFailure() ) {
      error() << "Failure in fixing position of object "<< endreq;
      return sc;
    } else {
      info() << "Successfully fixed object no. " << fix << endreq;
    }
  }
  info() << "Degrees of freedom: " << m_dof << endreq;
  return StatusCode::SUCCESS;
}


double TAConfig::GetLChi2() {
 return m_chi2;
}

void TAConfig::ResetLVars() {
  m_chi2 = 0.0;
}

void TAConfig::ResetGlVars() {
  // Configure the variables needed to fill the C-matrix
  int m_Nstations = m_DETmap.size(); // number of alignable objects
  // Global parameters 
  m_derGB.resize( m_n_dof * m_Nstations ,0.);
  m_derNonLin.resize( m_n_dof * m_Nstations ,0.);
  m_derLC.resize( m_ntrack_pars ,0.);
  m_aliConstants.resize( m_n_dof * m_Nstations ,0.);
  std::vector<double> d;
  d.resize(16,0.0);
  m_chiMat4.SetElements( d.begin(), d.end() );
  d.resize(25,0.0);
  m_chiMat5.SetElements( d.begin(), d.end() );
}


//------------------------------
//
// Store the track parameters for later use
//
//------------------------------
void TAConfig::SetTrackPar( const std::vector<double> & vt, unsigned int ntr ) {
  // for the linear track model
  //MD
  m_trx0[ntr] = vt[0];
  if ( isnan(m_trx0[ntr]) )
    error() << "in SetTrackPar: nan in m_trx0 for track " << ntr << endreq;
  m_trtx[ntr] = vt[1];
  if ( isnan(m_trtx[ntr]) )
    error() << "in SetTrackPar: nan in m_trx0 for track " << ntr << endreq;
  m_try0[ntr] = vt[2];
  if ( isnan(m_try0[ntr]) )
    error() << "in SetTrackPar: nan in m_trx0 for track " << ntr << endreq;
  m_trty[ntr] = vt[3];
  if ( isnan(m_trty[ntr]) )
    error() << "in SetTrackPar: nan in m_trx0 for track " << ntr << endreq;
  if ( vt.size() == 5 ) {
    m_trQ[ntr] += vt[4];
  }
}


StatusCode TAConfig::ConfMatrix( unsigned int cnt,
				 double rmeas,
				 std::vector<double> &local_vec ) {
  // use the following track parameters
  std::vector<double> trpar( m_ntrack_pars, 0.0 );
  trpar[0] = m_trx0[cnt];
  if ( isnan(m_trx0[cnt]) )
    error() << "in ConfMatrix: nan in m_trx0 for track " << cnt << endreq;
  trpar[1] = m_trtx[cnt];
  if ( isnan(m_trtx[cnt]) )
    error() << "in ConfMatrix: nan in m_trtx for track " << cnt << endreq;
  trpar[2] = m_try0[cnt];
  if ( isnan(m_try0[cnt]) )
    error() << "in ConfMatrix: nan in m_try0 for track " << cnt << endreq;
  trpar[3] = m_trty[cnt];
  if ( isnan(m_trty[cnt]) )
    error() << "in ConfMatrix: nan in m_trty for track " << cnt << endreq;
  if ( m_ntrack_pars == 5 ) {
    trpar[4] = m_trQ[cnt];
  }
  std::vector<double> deltaB4( 6, 0.0 );  // and the following alignment parameters
  std::vector<double> deltaNew( 6, 0.0 );   // new misalignment parameters
  unsigned int sz = m_DETmap.size();
  int i = 0;
  if ( m_dof[0] ) {
    deltaB4[0] = m_estimatedB4[m_rank_nr + i];
    deltaNew[0] = m_estimated[m_rank_nr + i++];
  }
  if ( m_dof[1] ) {
    deltaB4[1] = m_estimatedB4[m_rank_nr + i * sz];
    deltaNew[1] = m_estimatedB4[m_rank_nr + (i++) * sz];
  }
  if ( m_dof[2] ) {
    deltaB4[2] = m_estimatedB4[m_rank_nr + i * sz];
    deltaNew[2] = m_estimated[m_rank_nr + (i++) * sz];
  }
  if ( m_dof[3] ) {
    deltaB4[3] = m_estimatedB4[m_rank_nr + i * sz];
    deltaNew[3] = m_estimated[m_rank_nr + (i++) * sz];
  }
  if ( m_dof[4] ) {
    deltaB4[4] = m_estimatedB4[m_rank_nr + i * sz];
    deltaNew[4] = m_estimated[m_rank_nr + (i++) * sz];
  }
  if ( m_dof[5] ) {
    deltaB4[5] = m_estimatedB4[m_rank_nr + i * sz]; 
    deltaNew[5] = m_estimated[m_rank_nr + (i++) * sz];
  }
  m_derLC.resize( m_ntrack_pars, 0.0 );
  m_derGB.resize( m_n_dof * sz, 0.0 );

  // get derivatives.
  // pass: track parameterst (trpar)
  // alignment parameter estimates (deltaB4), 
  //  int ra = m_rank_nr;
  m_derivatives->SetGlobalN( trpar, 
			     deltaB4,
			     cAlignD,
			     sD,
			     m_derGB,
			     m_rank_nr,
			     mv,
			     kv );
  // remove the global part from the measurements,
  // so decouping the tracks which were coupled via alignment parameters.
  double rm = rmeas;
  int j0 = 0;
  for (int i = 0; i < m_n_dof; ++i ) {
    int j = 0;
    for ( j = j0; j < 6; ++j ) {
      if ( m_dof[j] != 0 )
	break; 
    }
    j0 = j + 1;
    if ( j > 1 )
      j--;
    rm -= m_derGB[m_rank_nr + i * m_DETmap.size()] * (deltaNew[j] - deltaB4[j]);
  }
  if ( fabs(m_derLC[0]) > 1.0e50 
       || fabs(m_derLC[1]) > 1.0e50 
       || fabs(m_derLC[2]) > 1.0e50 
       || fabs(m_derLC[3]) > 1.0e50 ) {
    info() << "m_derLC = " << m_derLC << endreq;
    info() << "trpar = " << trpar << endreq;
    info() << "deltaB4 = " << deltaB4 << endreq;
  }
  m_derivatives->SetLocalN( m_derLC, m_rank_nr, trpar, deltaB4, cAlignD, sD, mv, kv );
  if ( fabs(m_derLC[0]) > 1.0e50 
       || fabs(m_derLC[1]) > 1.0e50 
       || fabs(m_derLC[2]) > 1.0e50 
       || fabs(m_derLC[3]) > 1.0e50 ) {
    info() << "m_derLC = " << m_derLC << endreq;
    info() << "trpar = " << trpar << endreq;
    info() << "deltaB4 = " << deltaB4 << endreq;
  }
  
  if ( m_ntrack_pars == 4 ) 
    FChiMat4D( m_weight, rm, local_vec, m_derLC, m_chiMat4 );
  else if ( m_ntrack_pars == 5 )
     FChiMat5D( m_weight, rm, local_vec, m_derLC, m_chiMat5 );
  return StatusCode::SUCCESS;
}

void TAConfig::ZeroMatrix() {
  if ( m_ntrack_pars == 4 ) 
    for ( int i = 0; i < m_ntrack_pars; i++ ) // reset local params
      for ( int j = 0; j < m_ntrack_pars; j++ )
	m_chiMat4(i,j) = 0.0;
  else if ( m_ntrack_pars == 5 )
    for ( int i = 0; i < m_ntrack_pars; i++ ) // reset local params
      for ( int j = 0; j < m_ntrack_pars; j++ )
	m_chiMat5(i,j) = 0.0;
}

void TAConfig::ZeroMatrVec( ROOT::Math::SMatrix<double, 5,5> &mat, double *vec) {
  for ( int i = 0; i < m_ntrack_pars; i++ ) {// reset local params
    vec[i] = 0.0;
    for ( int j = 0; j < m_ntrack_pars; j++ )
      mat(i,j) = 0.0;
  }
}

void TAConfig::ZeroMatrVec( ROOT::Math::SMatrix<double, 4,4> &mat, double *vec) {
  for ( int i = 0; i < m_ntrack_pars; i++ ) {// reset local params
    vec[i] = 0.0;
    for ( int j = 0; j < m_ntrack_pars; j++ )
      mat(i,j) = 0.0;
  }
}

int TAConfig::InvMatrix( std::vector<double> &b, int n ) {
  if(n==4 && ot_detector){
    double vec[4];
    VectortoArray(b,vec);
    int ret = this->InvMatrix( m_local_mat, vec, n );
    ArraytoVector(vec,b);
    return ret;
  }
  else if ( n == 4 && !ot_detector) 
    return invMatrix( m_chiMat4, b, n );
  else if ( n == 5 )
    return invMatrix( m_chiMat5, b, n );
  return -1;
}

int TAConfig::InvMatrix(double v[][4], double b[4], int n) {
  int i, j, jj, k;
  double vkk;
  double eps = 0.0000000000001;
  bool *flag = new  bool[n];
  double *diag = new double[n];
  double *temp = new double[n];
  
  for (i=0; i<n; i++) {
    flag[i] = true;
    diag[i] = fabs(v[i][i]);     // save diagonal elem absolute values 	
    for (j=0; j<=i; j++) {
      v[j][i] = v[i][j] ;
    }
  }
	
  int nrank = 0;

  for (i=0; i<n; i++) {
    vkk = 0.0;
    k = -1;

    //TRACE("&&& in local inverting fct &&& " << PRINTV(i) << PRINTV(diag[i]) << PRINTV(v[i][i]) );
		
    for (j=0; j<n; j++) {// First look for the pivot, ie max unused diagonal element 
      if (flag[j] && ( (fabs(v[j][j]) > std::max(fabs(vkk),eps*diag[j])) ) ){
        vkk = v[j][j];
        k = j;
      }
    }
		
    if (k >= 0) {  // pivot found
      nrank++;
      flag[k] = false;
      vkk = 1.0/vkk;
      v[k][k] = -vkk; // Replace pivot by its inverse
      for (j=0; j<n; j++) {
        for (jj=0; jj<n; jj++) {
          if (j != k && jj != k) {// Other elements 
            //(!!! do them first as you use old v[k][j]'s !!!)
            v[j][jj] = v[j][jj] - vkk*v[j][k]*v[k][jj];
          }					
        }					
      }

      for (j=0; j<n; j++) {
        if (j != k) {// Pivot row or column elements 
          v[j][k] = (v[j][k])*vkk; // Column
          v[k][j] = (v[k][j])*vkk; // Line
        }
      }					
    } else { // No more pivot value (clear those elements)
      for (j=0; j<n; j++) {
        if (flag[j]) {
          b[j] = 0.0;
          for (k=0; k<n; k++) {
            v[j][k] = 0.0;
          }
        }				
      }
      break;  // No more pivots anyway, stop here
    }
  }

  for (j=0; j<n; j++) {
    temp[j] = 0.0;
    for (jj=0; jj<n; jj++) { // Reverse matrix elements
      v[j][jj] = -v[j][jj];
      temp[j] += v[j][jj]*b[jj];
    }					
  }

  for (j=0; j<n; j++)
    b[j] = temp[j];

  delete[] temp;
  delete[] flag;
  delete[] diag;
  return nrank;
}


//-----------------------------------------------------------------
// FillMatrix:
// this method fills the matrix using the following values:
//             - const int rank: position in big matrix to put the data
//             - double const &measMP (residual)
//-----------------------------------------------------------------
StatusCode TAConfig::FillMatrix( unsigned int trcnt,
				 double measMP) {
//				 ,double resolution ) {
  
  //-----------------------------
  // prepare Millepede;
  // configure the local (= track model) and global (= geometry) models:
  //-----------------------------
  m_derLC.resize( m_ntrack_pars ,0.);
  m_derGB.resize( m_n_dof*m_DETmap.size() ,0.);
  // get the tracks
  std::vector<double> trt;
  trt.push_back( m_trx0[trcnt] );
  trt.push_back( m_trtx[trcnt] );
  trt.push_back( m_try0[trcnt] );
  trt.push_back( m_trty[trcnt] );
  if ( m_ntrack_pars == 5 )
    trt.push_back( m_trQ[trcnt] );
  // store the alignment parameter estimates
  int i = 0;
  std::vector<double> deltaNew;
  deltaNew.resize(6, 0.0 );
  if ( m_dof[0] ) 
    deltaNew[0] = m_estimated[m_rank_nr + (i++)];
  if ( m_dof[1] ) 
    deltaNew[1] = m_estimated[m_rank_nr + (i++) * m_DETmap.size()];
  if ( m_dof[2] ) 
    deltaNew[2] = m_estimated[m_rank_nr + (i++) * m_DETmap.size()];
  if ( m_dof[3] ) 
    deltaNew[3] = m_estimated[m_rank_nr + (i++) * m_DETmap.size()];
  if ( m_dof[4] ) 
    deltaNew[4] = m_estimated[m_rank_nr + (i++) * m_DETmap.size()];
  if ( m_dof[5] ) 
    deltaNew[5] = m_estimated[m_rank_nr + (i++) * m_DETmap.size()];
  // initialize the derivatives      
  if ( fabs(m_derLC[0]) > 1.0e50 
       || fabs(m_derLC[1]) > 1.0e50 
       || fabs(m_derLC[2]) > 1.0e50 
       || fabs(m_derLC[3]) > 1.0e50 ) {
    info() << "Fillmatrix(1): m_derLC = " << m_derLC << endreq;
    info() << "                   trt = " << trt << endreq;
    info() << "              deltaNew = " << deltaNew << endreq;
  }
  StatusCode sc = m_derivatives->SetLocalN( m_derLC, m_rank_nr, trt, deltaNew, cAlignD, sD, mv, kv );
  if ( sc == StatusCode::FAILURE ) {
    error() << "Error in calculating derivatives; will exit!" << endreq;
    return StatusCode::FAILURE;
  }
  if ( fabs(m_derLC[0]) > 1.0e50 
       || fabs(m_derLC[1]) > 1.0e50 
       || fabs(m_derLC[2]) > 1.0e50 
       || fabs(m_derLC[3]) > 1.0e50 ) {
    info() << "Fillmatrix(2): m_derLC = " << m_derLC << endreq;
    info() << "                   trt = " << trt << endreq;
    info() << "              deltaNew = " << deltaNew << endreq;
  }

  sc = m_derivatives->SetGlobalN( trt, deltaNew,  cAlignD, sD, m_derGB, m_rank_nr, mv, kv );
  if ( sc == StatusCode::FAILURE ) {
    error() << "Error in calculating derivatives; will exit!" << endreq;
    return StatusCode::FAILURE;
  }
  //fill arrays of MP       
  double resolution = 1.0/sqrt(m_weight);
  sc = m_Centipede->EquLoc( m_derGB, m_derLC, measMP , resolution );
  if ( sc.isFailure() ) {
    error() << "Error in filling big C matrix" << endreq;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
//-------------------------------------------------------
// ConfMatrix: configure matrix with measurements & tracks
// method uses as input:
//                      LHCb::Track &track (a track)
//                      LHCb::LHCbID &id (a detector hit)
// and as output
//                      double &measMP (residual)    
//                      double &referenceZ (at this z-position)
//                      double &stereo_angle (with this stereo angle)
//                      LHCb::State &trState (at this state)
//-------------------------------------------------------

bool TAConfig::CalcResidualOT( unsigned int tr_cnt,
				   const LHCb::Track &track, 
				   const LHCb::Measurement *trMeas, 
				   const LHCb::LHCbID &id,
				   bool localF,
				   double &measMP ) {
  
  // const LHCb::Measurement trMeas(track.measurement(id)); // Get measurement    
  if(id.isOT()){

    info() << "----> --------------------" << m_rank_nr << endreq;
    info() << "----> CALC --> parameters: " << endreq;
    
    //    weight = 1./pow(trMeas.errMeasure(),2.);
    double weight = 1./1.44;
    double stereo_angle= stereo_angle = m_ot->findLayer( id.otID() )->angle();
    
    // referece of trajectory
    Gaudi::XYZPoint Ppoint = (trMeas->trajectory()).position(0.);
    
    //MD get another reference without measurement info
    std::auto_ptr<LHCb::Trajectory> lhcbidTraj = m_ot->findModule( id.otID() )->trajectory(id.otID());   
    Gaudi::XYZPoint idTrajPoint = lhcbidTraj->position(0.);
    Gaudi::XYZVector idTrajDir =  lhcbidTraj->direction(0.);    
    
    // get sensor where hit appeared
    DeOTLayer*  layer =  m_ot->findLayer( id.otID() );
    DeOTModule* module = m_ot->findModule( id.otID() );
    
    //get otchannelid and straw and monolayer
    const OTChannelID  chID= id.otID();
    const unsigned int straw = chID.straw();
    bool mLayA = module->monoLayerA(straw);
    
    //   get measurement trajectory and its direction
    const Trajectory& measTraj = trMeas->trajectory();
    Gaudi::XYZVector trajeDa =  measTraj.direction(0.);
    Gaudi::XYZVector trajeD;
    debug() << " idtrajDira = " << trajeDa << endmsg;    
    if(trajeDa.y() < 0.) //make sure all trajsdirection is upwards    //MD 15-04
      trajeD.SetXYZ(-trajeDa.x()/sqrt(trajeDa.Mag2()),-trajeDa.y()/sqrt(trajeDa.Mag2()),-trajeDa.z()/sqrt(trajeDa.Mag2()));
    else
      trajeD.SetXYZ(trajeDa.x()/sqrt(trajeDa.Mag2()),trajeDa.y()/sqrt(trajeDa.Mag2()),trajeDa.z()/sqrt(trajeDa.Mag2()));
    Gaudi::Rotation3D R, Rnom;
    Gaudi::XYZVector Lay,Mod,Pnom;

    //MD
    info() << "--------------------------------------------------------------" << endreq;
    //     debug() << " idtrajDir = " << trajeD << endmsg;
    info() << " idtraj   -->  trajPos = " << idTrajPoint << " idtrajDir = " << trajeD << endmsg;
    //     info() << " begin point of idtraj = " << lhcbidTraj->beginPoint()    << endmsg;
    //     info() << " end   point of idtraj = " << lhcbidTraj->endPoint()    << endmsg;

    // get center of Layer and Rotation matrix in LHCb coordinate frame
    layer->geometry()->toGlobalMatrix().GetDecomposition( R, Lay);
    const Gaudi::XYZPoint cenLayer(Lay.x(),Lay.y(),Lay.z());
    // get center of Module including misalignments
    module->geometry()->toGlobalMatrix().GetDecomposition( R, Mod);
    const Gaudi::XYZPoint cenMod(Mod.x(),Mod.y(),Mod.z());
    //     info() << " positioning vector of Module (Mod) " << Mod.x() << " " << Mod.y() << " " << Mod.z() << endmsg;    
    //     info() << " positioning vector of Layer (Lay)   " << Lay.x() << " " << Lay.y() << " " << Lay.z() << endmsg;
    // get center of Module nominal position 
    module->geometry()->toGlobalMatrixNominal().GetDecomposition( Rnom, Pnom);

    std::auto_ptr< LHCb::Trajectory > mLayTraj;
    std::auto_ptr< LHCb::Trajectory > bmLayTraj;
    //get the plane of first monolayer
    if(mLayA){
      //      info() << " use module : " << module->name() << " monoB? " << !mLayA << endreq;
      mLayTraj = module->trajectoryFirstWire(0);//first monolayerRight
      bmLayTraj = module->trajectoryLastWire(0);//first monolayerLeft
      //MD 14-04    
    }
    //get the plane of the second monolayer
    //MD14-04 
    else if(!mLayA){
      mLayTraj = module->trajectoryFirstWire(1);//second monolayerRight
      bmLayTraj = module->trajectoryLastWire(1);//second monolayerLeft
    }
    
    Gaudi::XYZPoint  mLayP  = mLayTraj->position(0.);
    Gaudi::XYZPoint  bmLayP = bmLayTraj->position(0.);
    //     Gaudi::XYZVector fmLayD  = fmLayTraj->direction(0.);
    //     Gaudi::XYZVector bfmLayD = bfmLayTraj->direction(0.);
    //dist between two trajs
    Gaudi::XYZVector dLay(mLayP.x()-bmLayP.x(),mLayP.y()-bmLayP.y(),mLayP.z()-bmLayP.z());
    //new center of  Monolayer
    //     LHCb::LineTraj montraj = LineTraj(mLayP,bmLayP);
    //     double path=0.;
    //     Gaudi::XYZVector perpCen(-999999.9999,-999999.9999,-999999.99999);
    //     m_poca-> minimize( montraj, path, cenMod, perpCen, m_tolerance); //module wise
    //     Gaudi::XYZVector perpCenUnit= perpCen/sqrt(perpCen.Mag2());
    //     Gaudi::XYZPoint mModCen =  cenMod+2.75*perpCenUnit;
    Gaudi::XYZPoint mModCen =  mLayP-0.5*dLay;

    //     info() <<" ---> mLayP  = " << mLayP << " Layer A? " << mLayA << " distance = " << dLay << endreq;
    //info() <<" ---> cenMod = " << cenMod<< " Layer A? " << mLayA << " newmModCen= "<< mModCen<<endreq;
    //     info() << "----------------------------------------------------------------------------------------" << endreq;
    
    /*********************
     ** def new Mod & Lay ***
     ********************/
    
    Mod.SetXYZ(mModCen.x(),mModCen.y(),mModCen.z());
    debug() << " NEW positing vector of Module NEW (Mod) " << Mod.x() << " " << Mod.y() << " " << Mod.z() << endmsg;    
    debug() << " positioning vector of Module (P Nominal) " << Pnom.x() << " " << Pnom.y() << " " 
           << Pnom.z() << endmsg;
    Gaudi::Plane3D         planeLay = layer->plane();
    const Gaudi::XYZVector planeN   = planeLay.Normal();    // Normal of plane
    Gaudi::Plane3D         planeMod = module->entryPlane(); //to get distance
    const double           planeD   = planeMod.Distance(cenMod); // Distance center - surface
    //seems to be no difference
    //MD16-04    const double           planeD   = planeMod.Distance(cenLayer); // Distance center - surface
    //propagate layer center to monolayer plane
    double dir = 0.;
    if(mLayA) dir = -1.;
    else if( !mLayA) dir = 1.;
    // !! multiplication with 0.5 maybe not accurate enough!! 11-04-08
    Gaudi::XYZPoint cenLayer_mono = cenLayer + dir*0.5*fabs(planeD)*planeN;
    //MD 27-03 test 
    Lay.SetXYZ(cenLayer_mono.x(),cenLayer_mono.y(),cenLayer_mono.z());
    //Lay.SetXYZ(mModCen.x(),mModCen.y(),mModCen.z());
    debug() << "rank = " << m_rank_nr << " positing vector of Layer NEW (Lay) " 
            << Lay.x() << " " << Lay.y() << " " << Lay.z() << endmsg;    
    
    //MD21-2, following is new
    //calculate poca between ???Center & wire
    Gaudi::XYZVector Mtraje(-999999.9999,-999999.9999,-999999.99999);
    double onwire=0.;

    //    m_poca-> minimize( (*lhcbidTraj), onwire, cenLayer, Mtraje, m_tolerance);
    //     m_poca-> minimize( (*lhcbidTraj), onwire, cenMod, Mtraje, m_tolerance); //module wise
    //     debug() <<" my minimization results (module)  : " << endreq
    //             << " ---> Mtraje = " << Mtraje << endmsg;
    m_poca-> minimize( (*lhcbidTraj), onwire, mModCen, Mtraje, m_tolerance); //module wise
    debug() <<"LayerA? " << mLayA << " my minimization results (mModCen)  :   ---> Mtraje = " << Mtraje << endmsg;
    
    //get the right sign & abs value
    double myU4=0.;
    myU4 = sqrt(Mtraje.Mag2()) * ( (Mtraje.x()*cos(stereo_angle) + Mtraje.y()*sin(stereo_angle) ) 
                                   /fabs( Mtraje.x()*cos(stereo_angle) + Mtraje.y()*sin(stereo_angle) ) ); 
    debug() << "my U4 (Mtraje length) =" << myU4 << endmsg;
    debug() << "stereo = " << stereo_angle << " x = " << Mtraje.x() << " y= " << Mtraje.y() << endreq;
    debug() << "Mtraje.x()*cos(stereo_angle)  "<<  Mtraje.x()*cos(stereo_angle) << endreq;
    
    if(mLayA)  plot(myU4,"MtrajABS for mono A",-200,200,200);
    if(!mLayA) plot(myU4,"MtrajABS for mono B",-200,200,200);
    debug() <<"LayerA? " << mLayA << " sqrt(Mtraj2) = myU4 = " << myU4 << endmsg;

    //normalize to unit vector
    if(Mtraje.Mag2()!=0.){
      if(Mtraje.x()>0.){
        Mtraje=Mtraje/sqrt(Mtraje.Mag2()); 
      }
      else{
        Mtraje=-Mtraje/sqrt(Mtraje.Mag2());
      }
    }
    else{
      debug() << "Mtraje is zero !!Mtraje is zero !!Mtraje is zero !!Mtraje is zero !! " << endmsg;
    }
        
    std::vector<double> trt;
    if(m_ntrack_pars==4){
      debug() << " trx0[0] = " << m_trx0[0] <<" trtx[0] = " << m_trtx[0] << " try0[0]= " 
             << m_try0[0] << " trty[0]= " << m_trty[0] <<endmsg;
      trt.push_back(m_trx0[0]);
      trt.push_back(m_trtx[0]);
      trt.push_back(m_try0[0]);
      trt.push_back(m_trty[0]);
    }
    
    int nr=m_rank_nr;
    std::vector<double> delta;
    if(localF){
      
      if(m_dof[0]){
        delta.push_back(m_estimatedB4[nr]);
        debug() << "---> CALCRES: estimatedB4[nr]= " << nr << " " << m_estimatedB4[nr] << endreq;
        debug() << "---> CALCRES: estimatedB4 delta.back()= " << nr << " " << delta.back() << endreq;
      }
      else {delta.push_back(0.0);}
      if(m_dof[2]){
        delta.push_back(m_estimatedB4[nr+2*m_DETmap.size()]);
      }
      else{delta.push_back(0.0);}
      if(m_dof[5]){
        delta.push_back(m_estimatedB4[nr+5*m_DETmap.size()]);
      }
      else{delta.push_back(0.0);}
      // need to swap alpha and gamma because of different def.  
      if(m_dof[4]){
        delta.push_back(m_estimatedB4[nr+4*m_DETmap.size()]);
      }
      else{delta.push_back(0.0);}
      if(m_dof[3]){
        delta.push_back(m_estimatedB4[nr+3*m_DETmap.size()]);
      }
      else{delta.push_back(0.0);}
        
    }else{
      if(m_dof[0]){
        delta.push_back(m_estimated[nr]);
        debug() << "---> II CALCRES: estimated [nr]= " << nr << " " << m_estimated[nr] << endreq;
        debug() << "---> II CALCRES: estimated delta.front()= " << nr << " " << delta.front() << endreq;
      }else {delta.push_back(0.0);}
      if(m_dof[2]){delta.push_back(m_estimated[nr+2*m_DETmap.size()]);}else{delta.push_back(0.0);}
      if(m_dof[5]){
        delta.push_back(m_estimated[nr+5*m_DETmap.size()]);
      }else{delta.push_back(0.0);}
      // need to swap alpha and gamma because of different def.  
      if(m_dof[4]){delta.push_back(m_estimated[nr+4*m_DETmap.size()]);}else{delta.push_back(0.0);}
      if(m_dof[3]){delta.push_back(m_estimated[nr+3*m_DETmap.size()]);}else{delta.push_back(0.0);}
    }

    // need to extract the part which is constant at this iteration
    double model = m_derivatives->Model(trt, delta,Lay, Mod ,trajeD,Mtraje);
    debug() <<  " myU4 = "<< myU4 << ";  Model = " << model << endreq;
    myU4=myU4 - model;
    measMP = myU4;
    debug() << "---> mLayA? " << mLayA << " measMP =" << measMP << " _________________________"<< nr<<endmsg;
    if(!localF){
      if(mLayA) plot(measMP,"measurement@CALC_A",-12,12,100);
      if(!mLayA) plot(measMP,"measurement@CALC_B",-12,12,100);
    }
    
    cAlignD.SetXYZ( Lay.x(),Lay.y(),Lay.z()); // center of alignable DeElement in LHCb coordinates
      
    sD.SetXYZ(Mod.x(),Mod.y(),Mod.z()); // sensor Center in LHCb coordinate frame

    //MD21-2 mC.SetXYZ(Cvec.x(),Cvec.y(),Cvec.z()); // measurement center in LHCb frame
    //MD 25-03
    mC.SetXYZ(Mod.x(),Mod.y(),Mod.z()); // measurement center in LHCb frame
      
    kv.SetXYZ(Mtraje.x(),Mtraje.y(),Mtraje.z()); // Mtraje already normalized , LHCb frame
    
    mv.SetXYZ(trajeD.x(),trajeD.y(), trajeD.z()) ;
    // make sure trajeD is normalized; LHCb frame
      
    debug() << "--->  sD = " << sD  << " kv = " << kv << "  mv = " << mv << endreq;
    //           << " cAlignD = " << cAlignD << " ||  mC = " << mC<< endreq;
     
    /*   
    }//MD 14-04
    else if(!mLayA){
      //info() << " rejt module : " << module->name() << " monoA? " << mLayA << endreq;  
      return StatusCode::FAILURE;
    }
    */
    
  }
  else if( !id.isOT()){
    info() << " no OT id given !!! " << endmsg;
  }
   return StatusCode::SUCCESS;
  
}


StatusCode TAConfig::CalcResidual( const LHCb::Track &track, 
				   const LHCb::Measurement *trMeas, 
				   const LHCb::LHCbID &id,
				   bool localF,
				   double &measMP ) {
  // Check if this was a muon measurement.
  const LHCb::Trajectory *measTraj;
  double z = 0;
  LHCb::State trState;
  if ( trMeas->type() == LHCb::Measurement::Muon ) {
    // This is a muon hit!
    const  LHCb::MuonMeasurement *muMeas = dynamic_cast<const MuonMeasurement*>(trMeas);
    z = muMeas->z();
    trState = track.closestState( z );
    // trajectory describing measurement
    // e.g. the 'wire' in case of OT
    measTraj = &muMeas->trajectory();
  } else {
    z = trMeas->z();
    trState = track.closestState( z ); //get state
    // trajectory describing measurement
    measTraj = &trMeas->trajectory();
  }
  // get the error on the measurement
  StatusCode sc = m_extrapolator->propagate( track, z, trState );
  if ( sc.isFailure() ) {
    error() << "Error in propagating track state to desired z-position" << endreq;
    return StatusCode::FAILURE;
  }
  // make a StateTraj Object to find Poca between wire & 'state/track-curve'
  int sign = 0;
  // track state at z = 0
  //get the bfield vector
  Gaudi::XYZVector bfVector;
  m_bField->fieldVector( trState.position(), bfVector );
  //make the state traj
  const StateTraj statetraj(trState, bfVector);
  //determine initial estimates of paths along the trajectories
  double alongState = 0.;
  double alongWire =  measTraj->arclength(statetraj.position( alongState ) );
  // !! get Poca between statetraj & measurement trajectory (measTraj)	           
  Gaudi::XYZVector pocaVec;
  sc = m_poca->minimize( *measTraj, alongWire,  statetraj, alongState, pocaVec, 0.01);
  Gaudi::XYZPoint hit_point = measTraj->position(alongWire);
  if( sc.isFailure() ) {
    error() << "Error determining poca between measurement trajectory and track-state" << endreq;
    return StatusCode::FAILURE;
  }
  sign = (pocaVec.x() > 0) ? 1 : -1; // -1 = left of straw

  //----------------------
  // equivalently: get poca between hit trajectory and center of hit module/sensor etc
  //----------------------
  // Find detector element lowest in DE hierarchy that was hit.
  // e.g. for OT case: module, for VeLo/IT case: sensor
  Gaudi::XYZVector P; // center of module which contained the hit (in global coordinates)
  Gaudi::Rotation3D R; // rotation of that hit in global coordinate system
  Gaudi::XYZVector P2; // center of object lowest in geo hierarchy
  Gaudi::Rotation3D R2; // rotation of that object in global coordinate system
  Gaudi::Transform3D M;
  FindHitModule( id, M ).GetDecomposition( R, P );
  if ( P.z() < 7900.0 ) {
    plot2D( P.x(), P.y(), "IT Hits in T1", -350.0, 350.0, -200.0, 200.0, 100, 100 );
  }
  M.GetDecomposition( R2, P2 );
  mC.SetCoordinates( P2.x(), P2.y(), P2.z() ); // center of alignable detector element which was hit

  //---------------------
  // calculate poca between hit-trajectory and center-of-module
  //---------------------
  Gaudi::XYZVector distance(-999999.9999,-9999.9999,-99999.99999);
  double onstraw = 0.; 
  double onZaxis = 0.; 
  sc = m_poca->minimize( *measTraj, onstraw, mC, distance, m_tolerance );
  if ( sc.isFailure() ) {
    error() << "Error in calculating poca between measurement trajectory and center of module"<< endreq;
    return sc;
  }
  if ( distance.Mag2() < 0.000001 ) {
    error() << "distance = " << distance.Mag2() 
	   << "           onstraw = " << onstraw << "\n"
	   << "                mC = " << mC << "\n"
	   << "measTraj.direction = " << measTraj->direction(0) << endreq;
    return StatusCode::FAILURE;
  }
  plot( measTraj->direction( onstraw ).x(), "Hit position (x)", -0.2,0.2,100);
  plot( measTraj->direction( onstraw ).y(), "Hit position (y)", 0.8,1.2,100);
  
  //---------------------
  // Error on the measurement at the position of the impact point
  //---------------------
  m_weight = Measurement ( trMeas, track, measTraj, trState );
  //-----------------------------------------------
  // !! make LineTraj object of straw and of zAxis
  //-----------------------------------------------
  Gaudi::XYZPoint pointZero; pointZero.SetCoordinates(0,0,0); 
  Gaudi::XYZPoint pointatZ; pointatZ.SetCoordinates(0,0,20000); 
  // x-axis through the point on the wire where the hit occured
  Gaudi::XYZPoint XpointatZ1; XpointatZ1.SetCoordinates(hit_point.x(),hit_point.y(),hit_point.z() ); 
  Gaudi::XYZPoint XpointatZ2; XpointatZ2.SetCoordinates(6000,hit_point.y(),hit_point.z()); 
  Gaudi::XYZVector ZVec;     
  int direction = 0; //direction of measMP seen from (0,0,z)
  // make a line trajectory between the begin and end point of measurement.
  const LineTraj* linearhit = new LineTraj(measTraj->beginPoint(), measTraj->endPoint() );
  // make a line trajectory describing z and x axes.
  const LineTraj* ZaxisTraj = new LineTraj( pointZero, pointatZ );
  const LineTraj* XaxisTraj = new LineTraj( XpointatZ1, XpointatZ2 );
  // find the poca between (linearized) hit trajectory and z-axis.
  double onstraw2 = 0.0;
  sc = m_poca->minimize( (*linearhit), onstraw2, false, (*ZaxisTraj), onZaxis, false, ZVec, 0.01);    
  if( sc.isFailure() ) {
    error() << "Error in calculating Poca!!!" << endreq;
    return StatusCode::FAILURE;
  }
  //  info() << "Direction of hit: " << linearhit->direction() << endreq;
  // get angle between x-axis and trajectory describing the hit:
  // First, calculate the product of the absolute values of the lengths of both vectors
  double l1 = sqrt( XaxisTraj->direction().Mag2() );
  double l2 = sqrt( linearhit->direction().Mag2() );
  double dp = XaxisTraj->direction().Dot( linearhit->direction() );
  dp = dp / ( l1 * l2 );
  m_gamma = acos( dp );
  Gaudi::XYZVector z_direction = XaxisTraj->direction().Cross( linearhit->direction() );
  //  info() << "z-direction = " << z_direction << " z-axis = " << ZaxisTraj->direction() << endreq;
  if ( z_direction.Dot( ZaxisTraj->direction() ) > 0 ) {
    // the hit trajectory may point downward or upward, and hence
    m_gamma =  - m_gamma;
  }
  m_gamma = 3.14159265359/2.0 - m_gamma;

  // Create a vector describing a track
  std::vector<double> trt(m_ntrack_pars, 0.0);
  trt[0] = m_trx0[tr_cnt];
  trt[1] = m_trtx[tr_cnt];
  trt[2] = m_try0[tr_cnt];
  trt[3] = m_trty[tr_cnt];
  if ( m_ntrack_pars == 5 )
    trt[4] = m_trQ[tr_cnt];
  // create a vector describing deltas
  std::vector<double> delta( 6, 0.0);
  unsigned int sz = m_DETmap.size();
  std::vector<double> dummy;
  if ( localF ) 
    dummy = m_estimatedB4;
  else 
    dummy = m_estimated;
  int i = 0;
  if ( m_dof[0] )
    delta[0] = dummy[m_rank_nr + i++];
  if ( m_dof[1])
    delta[1] = dummy[m_rank_nr + (i++) * sz];
  if ( m_dof[2] )
    delta[2] = dummy[m_rank_nr + (i++) * sz];
  if ( m_dof[3] )
    delta[3] = dummy[m_rank_nr + (i++) * sz];
  if ( m_dof[4] )
    delta[4] = dummy[m_rank_nr + (i++) * sz];
  if ( m_dof[5] )
    delta[5] = dummy[m_rank_nr + (i++) * sz];
  double mf = m_derivatives->Model( trt,
				    delta,
				    P,
				    P2,
				    measTraj->direction( onstraw )/sqrt(measTraj->direction( onstraw ).Mag2()),
				    distance );
  measMP = sqrt(distance.Mag2()) - mf;
   if ( isnan(mf) || fabs(measMP) > 1000.0 ) {
    error() << "Error in calculating poca!" << endreq;
    error() << "trt = " << trt << "\n"
	    << "m_trx0 = " << m_trx0[tr_cnt] << "\n"
	    << "m_rank_nr = " << m_rank_nr << "\n"
	    << "delta = " << delta << "\n"
	    << "m_estimated (x): " << m_estimated[m_rank_nr] << "\n"
	    << "m_estimatedB4 (x) " << m_estimatedB4[m_rank_nr] << "\n"
	    << "P = " << P << "\n"
	    << "P2 = " << P2 << "\n"
	    << "distance = " << distance << endreq;
    error() << "measurement traj: " << measTraj->direction(onstraw) << endreq;
    error() << "traj.Mag2() = " << measTraj->direction(onstraw).Mag2() << endreq;
    error() << "Center of module: " << mC << endreq;
    error() << "Model calculation failure: model = " << mf << endreq;
    //    return StatusCode::FAILURE;
     }

  // set a few variables which are also used in FillMatrix method.
  Gaudi::XYZVector trajeD = measTraj->direction( onstraw );
  trajeD = trajeD/sqrt(trajeD.Mag2());
  cAlignD.SetXYZ( P.x(), P.y(), P.z() ); // center of alignable object in LHCb coordinates
  sD.SetCoordinates( P2.x(), P2.y(), P2.z() ); // center of lowest hierarchial object in LHCb coordinates
  mC.SetCoordinates(P2.x(), P2.y(), P2.z() );
  kv.SetXYZ( distance.x()/sqrt(distance.Mag2()),
	     distance.y()/sqrt(distance.Mag2()),
	     distance.z()/sqrt(distance.Mag2()) );
  mv.SetXYZ( trajeD.x(), trajeD.y(), trajeD.z() );

  //  projection of wire on z Axis -> for XPLANES it gives right x&z value
  Gaudi::XYZPoint pointZonAxis = ZaxisTraj->position(onZaxis);
  direction = (ZVec.x() > 0 ) ? 1 : -1;  //get the right side
  //measMP = direction*( sqrt( ZVec.x()*ZVec.x() + ZVec.y()*ZVec.y() )) - (sign*drDist); 
  //----------------------------------
  //  no drift distance info !!
  //---------------------------------
  //  measMP = direction*( sqrt( ZVec.x()*ZVec.x() + ZVec.y()*ZVec.y() )) ; 
  //  info() << "Residual = " << measMP << endreq;
//  referenceZ = measTraj->position(alongWire).z();
  m_chi2 += measMP * measMP * m_weight;
  return StatusCode::SUCCESS;
}

StatusCode TAConfig::Rank( const LHCb::LHCbID &id ) {
  std::string name;
  if ( ot_detector && id.isOT() ) {
    // Check where this hit was:
    if ( m_otModule ) {
      name = (m_ot->findModule( id.otID() ))->name();
    } else if ( m_otQuadrant ) {
      name = (m_ot->findQuarter( id.otID() ))->name();
    } else if ( m_otLayer ) {
      name = (m_ot->findLayer( id.otID() ))->name();
    } else if ( m_otStation ) {
      name = (m_ot->findStation( id.otID() ))->name();
    }
  } else if ( it_detector && id.isIT() ) {
    if ( m_itLadder ) {
      // first find layer, then ladder
      DeITLayer * itl = dynamic_cast<DeITLayer*> (m_it->findLayer( id.stID() ));
      name = (itl->findLadder( id.stID() ))->name();
    } else if ( m_itLayer ) {
      name = (m_it->findLayer( id.stID() ))->name();
    } else if ( m_itBox ) {
      // first get station, then get box...
      DeITStation *its = dynamic_cast<DeITStation *> (m_it->findStation( id.stID() ));
      name = (its->findBox( id.stID() ))->name();
    } else if ( m_itStation ) {
      name = (m_it->findStation( id.stID() ))->name();
    }
  } else if ( tt_detector && id.isTT() ) {
    if ( m_ttLadder ) {
      name = (m_tt->findSector( id.stID() ))->name();
    } else if ( m_ttLayer ) {
      name = (m_tt->findLayer( id.stID() ))->name();
    } else if ( m_ttStation ) {
      name = (m_tt->findStation( id.stID() ))->name();
    }
  } else if ( muon_detector && id.isMuon() ) {
    // id.muonID() is a MuonTileID.
    if ( m_muStation ) {
      const unsigned int station = id.muonID().station();
      // DeMuonDetector.ReturnADetElement takes as arguments
      // the station nr, the region nr. and the chamber nr.
      // for this purpose, the station id suffices.
      // name = (m_muon->ReturnADetElement( id.muonID().station(), -1, -1 ))->name();
     name = MuonDetName( station ); 
     debug() << "Muon det id = " << station << endreq;
     debug() << "Muon det name = " << name << endreq;
    } else if ( m_muSys ) {
      name = m_muon->name();
    }
  } else if ( velo_detector && id.isVelo() ) {
    // not yet done...
  }
  std::map<std::string,int>::iterator t = m_C_pos.find( name );
  if ( t != m_C_pos.end() ) {
    m_rank_nr =  (*t).second;
    return StatusCode::SUCCESS;
  } else {
    error() << "Error calculating rank position in matrix" << endreq;
    error() << "Was trying to find the rank of element " << name << endreq;
    return StatusCode::FAILURE;
  }
  error() << "Failure calculating rank!" << endreq;
  return StatusCode::FAILURE;
}


/*********************************************
 *                                           *
 *   VectortoArray                           *
 * converting vectors to arrays              *
 *                                           *
 *********************************************/
void TAConfig::VectortoArray(const std::vector<double> &the_vector, double the_array[] ){
  std::vector<double>::const_iterator the_iterator = the_vector.begin();
  unsigned int counter = 0;
  while( the_iterator < the_vector.end() ){
    the_array[counter] = *the_iterator;
    //    the_array[counter] = the_vector[counter];
    the_iterator++;
    counter++;
  }
}

/*****************************************
 * Convert array to vector...
 ****************************************/

void TAConfig::ArraytoVector( const double the_array[], std::vector<double> &the_vector ) {
  std::vector<double>::iterator it = the_vector.begin();
  unsigned int cnt = 0;
  for ( ;it != the_vector.end(); ++it, cnt++ ) {
    *it = the_array[cnt];
  }
}

std::string TAConfig::MuonDetName( int lsta ) {
  int lreg = -1;
  int lchm = -1;
  IDetectorElement* myDet = (IDetectorElement*)0;
  int msta(0),mreg(0),mchm(0);

  //Getting stations
  IDetectorElement::IDEContainer::iterator itSt=m_muon->childBegin();
  for(itSt=m_muon->childBegin(); itSt<m_muon->childEnd(); ++itSt){
    if((msta == lsta) && (lreg == -1) && (lchm == -1)) {
      myDet = *itSt;
      return myDet->name();
    }
    //Getting regions
    mreg = 0;
    IDetectorElement::IDEContainer::iterator itRg=(*itSt)->childBegin();
    for(itRg=(*itSt)->childBegin(); itRg<(*itSt)->childEnd(); ++itRg){
      if((msta == lsta) && (mreg == lreg) && (lchm == -1)) {
        myDet = *itRg; 
        return myDet->name(); 
      } 
      //Getting chambers
      mchm = 0;
      IDetectorElement::IDEContainer::iterator itCh=(*itRg)->childBegin();
      for(itCh=(*itRg)->childBegin(); itCh<(*itRg)->childEnd(); ++itCh){
        
        if((msta == lsta) && (mreg == lreg) && (mchm == lchm)) {
          myDet = *itCh;
          return myDet->name();
        } 
        mchm++;
      }//Chamber Loop 
      mreg++;
    }//Region Loop
    msta++; 
  }//Stations Loop

  return myDet->name();

}

void TAConfig::CheckLChi2( int trsize,
			   int nmes, 
			   bool neXt,
			   bool & flagC ) {
  bool flagb = false;
  debug() << "scale is II: " << m_scale2 << endreq;
  if( neXt == true ) 
    m_scale2 = sqrt(m_scale2); // Florin: I do not think the measurement resolution estimates are reliable.
  double scale = m_scale2; 

  double sdtdevb = 3; // Florin: 3 standard deviations cut 
  if( m_nstand > 0.0 ) 
    sdtdevb = m_nstand;

  m_Centipede->CheckLChi2( m_chi2, 
			   trsize,
			   nmes, 
			   scale, 
			   sdtdevb, 
			   flagb );
  debug()<<"TAConfig flag: " <<flagb <<"scale is" <<m_scale2 << endreq;

  if( m_scale2 < 3.0 )
    m_scale2= 1;
  flagC = flagb;
}

StatusCode TAConfig::GlobalFit( std::vector<double> & parameter,
                      std::vector<double> & err,
                      std::vector<double> & pull) {
  StatusCode sc = m_Centipede->MakeGlobalFit( parameter, err, pull );
  if ( sc.isFailure() ) {
    error() << "Error in Millepede's !" << endreq;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode TAConfig::LocalTrackFit( unsigned int tr_cnt,
                             std::vector<double> &trpar,
                             std::vector<double> & estimated,
                             double & chi2,
                             double & residual) {
  StatusCode sc  = m_Centipede->FitLoc( (int) tr_cnt, 
					trpar, 
					0, 
					estimated, 
					chi2, 
					residual );
  if ( sc.isFailure() ) {
    error() << "Error in LocalTrackFit" <<  endreq;
    return sc;
  }
     //     chi2 = tr_par[trpar.size()+1]; //decode chi2 from tracking parameters variable
     //     residual = tr_par[trpar.size()+2]; //decode residual
  return sc.SUCCESS;
}

//---------------------------------
//
// FindHitModule, yields the global transformation matrix, including
// possible misalignments to go from global 
// coordinate system to the local coordinate system
// for the alignable object and always for the alignable object
// lowest in the hierarchy. For the IT, this means a sensor.
//
//---------------------------------
const Gaudi::Transform3D & TAConfig::FindHitModule( const LHCb::LHCbID &id, 
						    Gaudi::Transform3D &M ) {
  if ( id.isST() ) {
    if ( id.stID().isIT() ) {
      M = m_it->findSector( id.stID() )->geometry()->toGlobalMatrix();
      if ( m_itLayer ) {
        return m_it->findLayer( id.stID() )->geometry()->toGlobalMatrix();
      } else if ( m_itLadder ) {
        DeITLayer * itl = dynamic_cast<DeITLayer*> (m_it->findLayer( id.stID() ));
        return (itl->findLadder( id.stID() ))->geometry()->toGlobalMatrix();
      }
    }
  } else if ( id.isOT() ) {
    DeOTModule * otm = dynamic_cast<DeOTModule*> (m_ot->findModule( id.otID() ));
    M = otm->geometry()->toGlobalMatrix();
    if ( m_otLayer ) {
      return m_ot->findLayer( id.otID() )->geometry()->toGlobalMatrix();
    } else if ( m_otModule ) {
      DeOTModule * otm = dynamic_cast<DeOTModule*> (m_ot->findModule( id.otID() ));
      return otm->geometry()->toGlobalMatrix();
    }
  }
  // To do: same thing for Velo/TT/muon chambers

  // safety trap: return zero transform3D object:
  //  Gaudi::Transform3D a;
  //  return a;
}


void TAConfig::MakeAlignParVec() {
  //  m_estimateB4.resize(n,0.0); // initiate to null values the B4 vector and the vector.
  //  m_estimate.resize(n,0.0);

  for(unsigned int i=0; i< m_n_dof * m_DETmap.size(); i++ ){
    m_estimatedB4.push_back(0.0); //need to account for first
    m_estimated.push_back(0.0);
  }

}

void TAConfig::MakeTrackParVec() {

  m_trx0.push_back(0.0);
  m_trtx.push_back(0.0);
  m_try0.push_back(0.0);
  m_trty.push_back(0.0);
  
  m_trQ.push_back(0.0);
  m_trf.push_back(false);
}

void TAConfig::SetFlag(bool flag, 
		       unsigned int ntr){
  m_trf[ntr]=flag;   
}

 
void TAConfig::SetAlignmentPar(const std::vector<double> & estimated) {
  
  for( unsigned int i = 0; i < m_estimated.size(); i++) {
    m_estimatedB4[i]= m_estimated[i];
  }
  int j=0;
  for(unsigned int i=0; i < (estimated.size()/m_n_dof); i++ ){
    if ( m_dof[0] != 0 ) {
      m_estimated[i] = estimated[i];
      j++;
    }
    if ( m_dof[1] != 0 ) {
      m_estimated[i+estimated.size()/m_n_dof] = estimated[i+j*estimated.size()/m_n_dof];
      j++;
    }
    if ( m_dof[2] != 0 ) {
      m_estimated[i+2*estimated.size()/m_n_dof] = estimated[i+j*estimated.size()/m_n_dof];
      j++;
    }
    if ( m_dof[3] != 0 ) {
      m_estimated[i+3*estimated.size()/m_n_dof] = estimated[i+j*estimated.size()/m_n_dof];
      j++;
    }
    if ( m_dof[4] != 0 ) {
      m_estimated[i+4*estimated.size()/m_n_dof] = estimated[i+j*estimated.size()/m_n_dof];
      j++;
    }
    if ( m_dof[5] != 0 ) {
      m_estimated[i+5*estimated.size()/m_n_dof] = estimated[i+j*estimated.size()/m_n_dof];
      j++;
    }
    j=0;
  }
  info()<< "m_estimated " << m_estimated << " \n " <<" m_estimatedB4 " << m_estimatedB4 << "\n " 
        << "estimated = " << estimated << endreq;
}
// End of class implementation
 
 
double TAConfig::Measurement( const LHCb::Measurement *m,
			      const LHCb::Track &track,
			      const LHCb::Trajectory *t, 
			      LHCb::State &s ) {
  double m_resolution = 0.0;
  switch ( m->type() ) {
  case LHCb::Measurement::Muon :
    const LHCb::MuonMeasurement *muMeas = dynamic_cast<const MuonMeasurement*>(m);
    s = track.closestState( muMeas->z() );
    // trajectory describing measurement
    // e.g. the 'wire' in case of OT
    t = &muMeas->trajectory();
    m_resolution = muMeas->errMeasure();
    break;
  case LHCb::Measurement::OT :
    const  LHCb::OTMeasurement *otMeas = dynamic_cast<const OTMeasurement*>(m);
    s = track.closestState( otMeas->z() );
    t = &otMeas->trajectory();
    m_resolution = otMeas->errMeasure();
    m_resolution = 1.44;
    break;
  case LHCb::Measurement::IT :
    const  LHCb::STMeasurement *itMeas = dynamic_cast<const STMeasurement*>(m);
    s = track.closestState( itMeas->z() );
    t = &itMeas->trajectory();
    m_resolution = itMeas->errMeasure();
    break;
  case LHCb::Measurement::TT :
    const  LHCb::STMeasurement *ttMeas = dynamic_cast<const STMeasurement*>(m);
    s = track.closestState( ttMeas->z() );
    t = &ttMeas->trajectory();
    m_resolution = ttMeas->errMeasure();
    break;
  default :
    error() << "The measurement can not be used..." << endreq;
    break;
  }
  return m_resolution;
}



// Some stand-alone helper functions
void FChiMat4D( double weight, 
		double rmeas, 
		std::vector<double> &local_vec, 
 		std::vector<double> &derLC,
		ROOT::Math::SMatrix<double, 4> &local_mat ) {
  unsigned int i,j;
  for ( i = 0; i < 4; i++ ) {
    local_vec[i] += weight * rmeas * derLC[i];
    for ( j = 0; j < 4; j++ )
      local_mat(i,j) += weight * derLC[i] * derLC[j];
  }
} 

void FChiMat5D( double weight, 
		double rmeas,
		std::vector<double> &local_vec,
 		std::vector<double> &derLC,
		ROOT::Math::SMatrix<double, 5> &local_mat ) {
  unsigned int i,j;
  for ( i = 0; i < 5; i++ ) {
    local_vec[i] += weight * rmeas * derLC[i];
    for ( j = 0; j < 5; j++ ) 
      local_mat(i,j) += weight * derLC[i] * derLC[j];
  }
}

int invMatrix( ROOT::Math::SMatrix<double, 5, 5> &v,
	       std::vector<double> &b,
	       int n ) {
  int i, j, jj, k;
  double vkk, *temp;
  double eps = 0.0000000000001;
  bool *flag = new  bool[n];
  double *diag = new double[n];

  temp = new double[n];

  for (i=0; i<n; i++) {
    flag[i] = true;
    diag[i] = fabs(v(i,i));     // save diagonal elem absolute values   
    for (j=0; j<=i; j++) {
      v(j,i) = v(i,j) ;
    }
  }

  int nrank = 0;

  for (i=0; i<n; i++) {
    vkk = 0.0;
    k = -1;

    for (j=0; j<n; j++) {// First look for the pivot, ie max unused diagonal element 
      if (flag[j] && ( (fabs(v(j,j)) > std::max(fabs(vkk),eps*diag[j])) ) ){
        vkk = v(j,j);
        k = j;
      }
    }

    if (k >= 0) {  // pivot found
      nrank++;
      flag[k] = false;
      vkk = 1.0/vkk;
      v(k,k) = -vkk; // Replace pivot by its inverse
      for (j=0; j<n; j++) {
        for (jj=0; jj<n; jj++) {
          if (j != k && jj != k) {// Other elements 
                                 //(!!! do them first as you use old v[k][j]'s !!!)
            v(j,jj) = v(j,jj) - vkk*v(j,k)*v(k,jj);
          }
        }
      }

      for (j=0; j<n; j++) {
        if (j != k) {// Pivot row or column elements 
          v(j,k) = (v(j,k))*vkk; // Column
          v(k,j) = (v(k,j))*vkk; // Line
        }
      }
    } else { // No more pivot value (clear those elements)
      for (j=0; j<n; j++) {
        if (flag[j]) {
          b[j] = 0.0;
          for (k=0; k<n; k++) {
            v(j,k) = 0.0;
          }
        }
      }
      break;  // No more pivots anyway, stop here
    }
  }

  for (j=0; j<n; j++) {
    temp[j] = 0.0;
    for (jj=0; jj<n; jj++) { // Reverse matrix elements
      v(j,jj) = -v(j,jj);
      temp[j] += v(j,jj)*b[jj];
    }
  }

  for (j=0; j<n; j++)
    b[j] = temp[j];

  delete[] temp;
  delete[] flag;
  delete[] diag;
  return nrank;
}


int invMatrix( ROOT::Math::SMatrix<double, 4, 4> &v, 
	       std::vector<double> &b, 
	       int n ) {
  int i, j, jj, k;
  double vkk, *temp;
  double eps = 0.0000000000001;
  bool *flag = new  bool[n];
  double *diag = new double[n];
  
  temp = new double[n];
  
  for (i=0; i<n; i++) {
    if ( isnan(b[i]) ) {
      std::cout << "Error in trPar of invMatrix: " << b[i] << std::endl;
    }
    flag[i] = true;
    diag[i] = fabs(v(i,i));     // save diagonal elem absolute values 	
    for (j=0; j<=i; j++) {
      v(j,i) = v(i,j) ;
    }
  }
	
  int nrank = 0;

  for (i=0; i<n; i++) {
    vkk = 0.0;
    k = -1;

    for (j=0; j<n; j++) {// First look for the pivot, ie max unused diagonal element 
      if (flag[j] && ( (fabs(v(j,j)) > std::max(fabs(vkk),eps*diag[j])) ) ){
        vkk = v(j,j);
        k = j;
      }
    }
		
    if (k >= 0) {  // pivot found
      nrank++;
      flag[k] = false;
      vkk = 1.0/vkk;
      v(k,k) = -vkk; // Replace pivot by its inverse
      for (j=0; j<n; j++) {
        for (jj=0; jj<n; jj++) {
          if (j != k && jj != k) {// Other elements 
                                 //(!!! do them first as you use old v[k][j]'s !!!)
            v(j,jj) = v(j,jj) - vkk*v(j,k)*v(k,jj);
          }					
        }					
      }

      for (j=0; j<n; j++) {
        if (j != k) {// Pivot row or column elements 
          v(j,k) = (v(j,k))*vkk; // Column
          v(k,j) = (v(k,j))*vkk; // Line
        }
      }					
    } else { // No more pivot value (clear those elements)
      for (j=0; j<n; j++) {
        if (flag[j]) {
          b[j] = 0.0;
          for (k=0; k<n; k++) {
            v(j,k) = 0.0;
          }
        }				
      }
      break;  // No more pivots anyway, stop here
    }
  }

  for (j=0; j<n; j++) {
    temp[j] = 0.0;
    for (jj=0; jj<n; jj++) { // Reverse matrix elements
      v(j,jj) = -v(j,jj);
      temp[j] += v(j,jj)*b[jj];
    }					
  }
  
  for (j=0; j<n; j++) {
    if ( isnan(temp[j]) ) {
      std::cout << "Error in invMatrix(4x4): b["<<j<<"] = " << b[j] << std::endl;
      std::cout << "Error in invMatrix(4x4): temp["<<j<<"] = " << temp[j] << std::endl;
    }
    b[j] = temp[j];
  }

  delete[] temp;
  delete[] flag;
  delete[] diag;
  return nrank;
}


double StereoAngle( IDetectorElement *det, const std::string &detname ) {
  double stereo_angle = 0.0;
  if ( detname.find("OT") != std::string::npos ) {
    // this is an ot detector element
    if ( detname.find("layer") != std::string::npos 
	 || detname.find("Module") != std::string::npos ) {
      // at layer level or module level
      DeOTLayer *lay = dynamic_cast<DeOTLayer *>( det );
      stereo_angle = lay->angle();
    } 
  } else if ( detname.find( "IT" ) != std::string::npos ) {
    if ( detname.find("Layer") != std::string::npos 
	 || detname.find("ladder") != std::string::npos ) {
      // at layer or ladder level
      DeITLayer *lay = dynamic_cast<DeITLayer *>( det );
      stereo_angle = lay->angle();
    }
  }
  return stereo_angle;
}
