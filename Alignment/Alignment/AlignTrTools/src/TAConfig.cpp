//-----------------------------------------------------------------------------
/** @file TAConfig.cpp
 *
 *  Implementation file for Millepede configuration tool : TAConfig
 *
 *  CVS Log :-
 *  $Id: TAConfig.cpp,v 1.15 2008-03-04 09:37:51 jblouw Exp $
 *
 *  @author J. Blouw (johan.blouw@mpi-hd.mpg.de)
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
  declareProperty("residual_cut", m_residual_cut = 20.0 );
  declareProperty("initial_residual_cut", m_ini_residual_cut = 10.0 );
  declareProperty("n_l_stdev", m_nstd = 10 );
  declareProperty("Velo_left_modules", m_VELOmap_l);
  declareProperty("Velo_right_modules",m_VELOmap_r);
  declareProperty("Fix_alignable_objects", m_fix = true);
  //  declareProperty("FixDofs", m_fix_dofs );
  declareProperty("Fix_first_object", m_fix_first = true);
  declareProperty("Fix_last_object", m_fix_last = true);
  declareProperty("Fix_x_position", m_fix_x = true);
  declareProperty("Fix_y_position", m_fix_y = false);
  declareProperty("Fix_z_position", m_fix_z = false);
  declareProperty("Fix_alpha_rotation", m_fix_a = false);
  declareProperty("Fix_beta_rotation", m_fix_b = false);
  declareProperty("Fix_gamma_rotation", m_fix_g = false);
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
  m_DOF = new bool[m_dof.size()];
}

TAConfig::~TAConfig() {delete m_DOF;};

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
  return StatusCode::SUCCESS;
}

StatusCode TAConfig::CacheDetElements() {
  // loop over detectors to be aligned and retrieve geometry information
  std::vector<std::string>::iterator t_i;
  info() << "Number of detectors to be aligned: " << m_detectors.end() - m_detectors.begin() << endreq;
  StatusCode sc;
  for (  t_i = m_detectors.begin(); t_i != m_detectors.end(); t_i++ ) {
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
  // Ok, Millepede has been initalized, now we can 
  // do the constraints.
  // For Outer Tracker
  if ( m_fix && ot_detector ) sc = ConstrainPositions( constrain_ot );
  // For Inner Tracker
  if ( m_fix && it_detector ) sc = ConstrainPositions( constrain_it );
  // For Trigger Tracker
  if ( m_fix && tt_detector ) sc = ConstrainPositions( constrain_tt );
  // For Muon
  if ( m_fix && muon_detector ) sc = ConstrainPositions( constrain_muon );
  // For Velo
  if ( m_fix && velo_detector ) sc = ConstrainPositions( constrain_velo );
  return StatusCode::SUCCESS;
}

StatusCode TAConfig::ConstrainPositions( std::map<std::string,int> &map ) {
  std::string name;
  int rank;
  std::map<std::string,int>::iterator it = map.begin();
  // SV uses Millepede's ParSig function a little strange.
  // we now have to explicitly set the range within which the
  // alignment parameters may vary.
  //MD  for ( ; it != map.end(); it++ ) {
  //     rank = it->second;
  //     for ( unsigned int i = 0; i < m_dof.size(); i++ ) {
  //       m_Centipede->ParSig(rank+i*m_DETmap.size(), 20.0 );
  //     }
  //   }
  //it = map.begin();
  for ( ; it != map.end(); it++ ) {
    name = it->first;
    rank = it->second;
    ConstrainPositions( rank, name, m_DETmap.size() );
  }
//  info() << "(2) Map = " << map << endreq;
  return StatusCode::SUCCESS;
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
      if ( m_fix ) {
        if ( i == 1 && m_fix_last ) {
          if ( j == 1 ) 
            constrain_tt.insert( make_pair( m_TTLayers[j]->name(), rank ) );
        } else if ( i == 0 && m_fix_first ) {
          if ( j == 0 ) 
            constrain_tt.insert( make_pair( m_TTLayers[j]->name(), rank ) );
        }
      }
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
  unsigned int nStations = m_muon->stations();
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
        if ( m_fix ) {
          if ( i == 0 && m_fix_first ) {// M1
            // fix all chambers...
            constrain_muon.insert(make_pair( m_MUChambers[j]->name(), rank ));
          }
          if ( i == 4 && m_fix_last ) {// M5
            info() << "Constrain chambers in M5:" << endreq;
            constrain_muon.insert(make_pair( m_MUChambers[j]->name(), rank ));
          }
        }
        rank++;
      }
    }
    if ( m_muStation && ! m_muChamber ) {
      CreateMap( rank, m_MUStations[i], m_zmoy_mu );
      info() << "De " << m_MUStations[i]->name() << " has rank " << rank << endreq;
      if ( m_fix ) {
        if ( i == 0 && m_fix_first ) { //fix first station
          constrain_muon.insert( make_pair( m_MUStations[i]->name(), rank ));
        }
        if ( i == 4 && m_fix_last ) { //fix last station
          constrain_muon.insert( make_pair( m_MUStations[i]->name(), rank ));
        }
      }
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
            if ( m_fix ) {
              if ( i == 0 && m_fix_first ) // T1
                if ( j == 0 || j == 1 )  // X1 or U layer
                  constrain_ot.insert( make_pair( m_OTModules[l]->name(),rank) );
              if ( i == 2 && m_fix_last ) // T3
                if ( j == m_OTLayers.size() -1 || j == m_OTLayers.size() -2 )
                  constrain_ot.insert( make_pair( m_OTModules[l]->name(),rank) );
            }
            rank++;
          }
        }
        if ( m_otQuadrant && ! m_otModule ) {
          CreateMap( rank, m_OTQuadrants[k], m_zmoy_ot );
          info() << "De " << m_OTQuadrants[k]->name() << " has rank " << rank << endreq;
          if ( m_fix ) {
            if ( i == 0 && m_fix_first ) // T1
              if ( j == 0 || j == 1 ) // X1 or U layer
                constrain_ot.insert( make_pair( m_OTQuadrants[k]->name(), rank ));
            if ( i == 2 && m_fix_last ) // T3
              if ( j == m_OTLayers.size() -1 || j == m_OTLayers.size() -2 )
                constrain_ot.insert( make_pair( m_OTQuadrants[k]->name(), rank ));
          }
          rank++;
        }
      }
      if ( m_otLayer && ! m_otQuadrant && ! m_otModule ) {
        CreateMap( rank, m_OTLayers[j], m_zmoy_ot );
        info() << "De " << m_OTLayers[j]->name() << " has rank " << rank << endreq;
        if ( m_fix ) {
          if ( i == 0 && m_fix_first ) // T1
            if ( j == 0 || j == 1 ) //  X1 or U layer
              constrain_ot.insert( make_pair( m_OTLayers[j]->name(), rank ));
          if ( i == 2 && m_fix_last ) // T3
            if ( j == m_OTLayers.size() -1 || j == m_OTLayers.size() -2 ) 
              constrain_ot.insert( make_pair( m_OTLayers[j]->name(), rank ));
        }
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
            //	    info() << "Inv Matrix of De " << m_ITLadders[l]->name() 
            //<< " = " << m_ITLadders[l]->geometry()->matrixInv() << endreq;
            CreateMap( rank, m_ITLadders[l], m_zmoy_it );
            if ( m_fix ) {
              if ( i == 0 && m_fix_first )  // T1
                if ( k == 0 || k == 1 )
                  constrain_it.insert(make_pair(m_ITLadders[l]->name(),rank));
              if ( i == 3 && m_fix_last ) 
                if ( k == m_ITLayers.size() -1 || k == m_ITLayers.size() -2 )
                  constrain_it.insert(make_pair(m_ITLadders[l]->name(),rank));
            }
            rank++;
          }
        }
        if ( m_itLayer && ! m_itLadder && ! m_itBox && ! m_itStation ) {
          CreateMap( rank, m_ITLayers[k], m_zmoy_it );
          if ( m_fix ) {
            if ( i == 0 && m_fix_first )
              if ( k == 0 || k == 1 )
                constrain_it.insert( make_pair( m_ITLayers[k]->name(), rank ) );
            if ( i == 3 && m_fix_last )
              if ( k == m_ITLayers.size() -1 || k == m_ITLayers.size() -2 )
                constrain_it.insert( make_pair( m_ITLayers[k]->name(), rank ) );
          }
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
//  info() << "(1) Map = " << constrain_it << endreq;
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



StatusCode TAConfig::PrintParameters( std::vector<double> &ali_par ) {
  // Alternative:
  std::vector<double>::iterator i = ali_par.begin();
  std::map<std::string,int>::iterator t = m_C_pos.begin();
  unsigned int msize = m_C_pos.size();
  // loop over the detector elements we wanted to align.
  // check for each element which degrees of freedom were determined.
  for ( ; t != m_C_pos.end(); ++t ) {
    info() << "Parameter " << t->first << " " << t->second  << " " << ali_par[t->second] << endreq;
    IDetectorElement *det = getDet<IDetectorElement>(t->first);
    IGeometryInfo *geo = det->geometry();
    SmartRef<Condition> cond =  det->condition(t->first);
    IAlignment *aliconst = dynamic_cast<IAlignment*>( cond.target() );
    // Convert alignment parameters into
    // a vector containing translations
    // a vector containing rotations and
    // a vector with the pivot point. 
    // That vector should actually be obtained from the detector element!
    std::vector<double> translation;
    translation.push_back(0.0);
    translation.push_back(0.0);
    translation.push_back(0.0);
    std::vector<double> rotation;
    rotation.push_back(0.0);
    rotation.push_back(0.0);
    rotation.push_back(0.0);
    std::vector<double> pivot;
    // Get the translation, rotation and pivot vectors from
    // the geometry object:
    info() << "Get transformation matrix from detector element" << endreq;
    Gaudi::Transform3D position = geo->toLocalMatrixNominal();
    EulerAngles EuAng( 0.0, 0.0, 0.0 );
    Gaudi::Rotation3D R( EuAng );
    Gaudi::XYZVector P( 0.0, 0.0, 0.0 );
    position.GetDecomposition( R, P );
    pivot.push_back( P.X() );
    pivot.push_back( P.Y() );
    pivot.push_back( P.Z() );
    // fill the translation & rotation vectors
    // by looping over the number of degrees of freedom
    // the vector m_align is filled according to the following scheme:
    // first loop over detector elements, then loop over the degrees of 
    // freedom.
    for ( unsigned int d = 0; d < 3; d++ ) {
      unsigned int pos1 = t->second + msize * d; // translations
      unsigned int pos2 = t->second + msize * (d + 3); //rotations
      if ( m_dof[d] ) translation[d] = ali_par[pos1];
      if ( m_dof[d+3] ) rotation[d] = ali_par[pos2];
    }

    info() << "Translation vector: " << translation << "\n"
	   << "Rotation vector   : " << rotation << "\n"
	   << "pivot point       : " << pivot << endreq;

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
  int nlocal = m_ntrack_pars;
  for ( int i = 0; i < m_ntrack_pars; i++ )
    m_derLC.push_back(0.0);
  for ( unsigned int i = 0; i < m_n_dof*m_DETmap.size(); i++ )
    m_derGB.push_back(0.0);
  for (int j = 0; j < 6; j++) {m_DOF[j] = m_dof[j];}   // What are the parameters to align ?
  //  for (int j = 0; j < 9; j++) {m_CONSTRAINT[j] = m_sigma[j];}
  // Needed for Sebastien's Millepede initialization
  for ( unsigned int i = 0; i < m_dof.size(); i++ ) {// loop over degrees of freedom per detector
    m_SIGMA[i] = m_sigma[i];
    info() << "m_SIGMA[" << i << "] = " << m_SIGMA[i] << endreq;
  }
  info() << "Calling InitMille with m_sigma = " << m_sigma << "..." << endreq;
  /* HD Millepede initialization call */
  m_Centipede->InitMille( m_DOF,
                          num_objects, nlocal,  
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
StatusCode TAConfig::ConstrainPositions( const int &rank, const std::string & de_name, const int &rs ) {
  // ok we want to constrain the first detector layers
  // it is the first element in the detector map (m_DETmap) 
  //
  StatusCode sc;
  int dof_cnt = 0; // counts the number of degrees of freedom
  if ( m_dof[0] && m_fix_x) {
    sc = m_Centipede->ParSig(rank, 0.0); // fix x-position
    if ( sc.isFailure() ) {
      error() << "Failure in fixing x-position of object " << de_name << " at rank " << rank << endreq;
      return sc;
    }
    info() << "Fixing x-position of object " << de_name << " at rank " 
           << rank << " using ParSig(" << rank << ", 0) "<< endreq;
    dof_cnt++;
  }
  if ( m_dof[1] && m_fix_y) {
    sc = m_Centipede->ParSig(rank+dof_cnt*rs, 0.0); // fix y-position
    if ( sc.isFailure() ) {
      error() << "Failure in fixing y-position of object " << de_name << " at rank " << rank << endreq;
      return sc;
    }
    info() << "Fixing y-position of object " << de_name << " at rank " 
           << rank+rs << " using ParSig(" << rank+dof_cnt*rs << ", 0) "<< endreq;
    dof_cnt++;
  }
  if ( m_dof[2] && m_fix_z) {
    sc = m_Centipede->ParSig(rank+dof_cnt*rs, 0.0); // fix z-position
    if ( sc.isFailure() ) {
      error() << "Failure in fixing z-position of object " << de_name << " at rank " << rank << endreq;
      return sc;
    }
    info() << "Fixing z-position of object " << de_name << " at rank " 
           << rank+dof_cnt*rs << " using ParSig(" << rank+dof_cnt*rs << ", 0) " << endreq;
    dof_cnt++;
  }
  if ( m_dof[3] && m_fix_a) {
    sc = m_Centipede->ParSig(rank+dof_cnt*rs, 0.0); // fix alpha rotation
    if ( sc.isFailure() ) {
      error() << "Failure in fixing alpha rotation of object " << de_name 
              << " at rank " << rank << endreq;
      return sc;
    }
    info() << "Fixing angle alpha of object " << de_name << " at rank " 
           << rank+dof_cnt*rs << " using ParSig(" << rank+dof_cnt*rs << ", 0) " << endreq;
    dof_cnt++;
  }
  if ( m_dof[4] && m_fix_b) {
    sc = m_Centipede->ParSig(rank+dof_cnt*rs, 0.0); // fix beta rotation
    if ( sc.isFailure() ) {
      error() << "Failure in fixing beta rotation of object " << de_name << " at rank " << rank << endreq;
      return sc;
    }
    info() << "Fixing angle beta of object " << de_name << " at rank " 
           << rank+dof_cnt*rs << " using ParSig(" << rank+dof_cnt*rs << ", 0) " << endreq;
    dof_cnt++;
  }
  if ( m_dof[5] && m_fix_g) {
    sc = m_Centipede->ParSig(rank+dof_cnt*rs, 0.0); // fix gamma rotation
    if ( sc.isFailure() ) {
      error() << "Failure in fixing gamma rotation of object " << de_name << " at rank " << rank << endreq;
      return sc;
    }
    info() << "Fixing angle gamma of object " << de_name << " at rank " 
           << rank+dof_cnt*rs << " using ParSig(" << rank+dof_cnt*rs << ", 0) " << endreq;
  }
  return StatusCode::SUCCESS;
}





StatusCode TAConfig::ResetGlVars() {
  // Configure the variables needed to fill the C-matrix
  int m_Nstations = m_DETmap.size(); // number of alignable objects
  int m_Nlocal = m_ntrack_pars; // number of track parameters
  // Global parameters 
  m_derGB.resize( 6 * m_Nstations ,0.);
  m_derNonLin.resize( 6 * m_Nstations ,0.);
  m_derLC.resize( m_Nlocal ,0.);
  m_aliConstants.resize( 6 * m_Nstations ,0.);
  
  return StatusCode::SUCCESS;
}





StatusCode TAConfig::ConfMatrix( const double &stereo, 
				 const double &rmeas, 
				 const double &wght, 
				 const double &z_cor,
				 double local_vec[], 
				 double local_mat[][4] ) {
  local_vec[0] += wght*rmeas*cos(stereo); //'x'
  local_vec[1] += wght*rmeas*z_cor*cos(stereo); //'x'
  local_vec[2] += wght*rmeas*sin(stereo);  //'u'
  local_vec[3] += wght*rmeas*z_cor*sin(stereo); //'u'
  
  local_mat[0][0] += wght*cos(stereo)*cos(stereo);
  local_mat[0][1] += wght*cos(stereo)*z_cor*cos(stereo);
  local_mat[0][2] += wght*cos(stereo)*sin(stereo);
  local_mat[0][3] += wght*cos(stereo)*z_cor*sin(stereo);
  
  local_mat[1][0] += wght*z_cor*cos(stereo)*cos(stereo);
  local_mat[1][1] += wght*z_cor*cos(stereo)*z_cor*cos(stereo);
  local_mat[1][2] += wght*z_cor*cos(stereo)*sin(stereo);
  local_mat[1][3] += wght*z_cor*cos(stereo)*z_cor*sin(stereo);
  
  local_mat[2][0] += wght*sin(stereo)*cos(stereo);
  local_mat[2][1] += wght*sin(stereo)*z_cor*cos(stereo);
  local_mat[2][2] += wght*sin(stereo)*sin(stereo);
  local_mat[2][3] += wght*sin(stereo)*z_cor*sin(stereo);
  
  local_mat[3][0] += wght*z_cor*sin(stereo)*cos(stereo);
  local_mat[3][1] += wght*z_cor*sin(stereo)*z_cor*cos(stereo);
  local_mat[3][2] += wght*z_cor*sin(stereo)*sin(stereo);
  local_mat[3][3] += wght*z_cor*sin(stereo)*z_cor*sin(stereo);

  return StatusCode::SUCCESS;
}

StatusCode TAConfig::ZeroMatrVec(double mat[][4], double vec[]) {
  for (int i=0; i<4; i++) {// reset local params
    vec[i] = 0.0;
    for (int j=0; j<4; j++)
      mat[i][j] = 0.0;
  }
  return StatusCode::SUCCESS;
}




int TAConfig::InvMatrix(double v[][4], double b[],const int &n) {
  int i, j, jj, k;
  double vkk, *temp;
  double eps = 0.0000000000001;
  bool *flag = new  bool[n];
  double *diag = new double[n];
  
  temp = new double[n];
  
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
  return nrank;
}


//-----------------------------------------------------------------
// FillMatrix:
// this method fills the matrix using the following values:
//             - const int rank: position in big matrix to put the data
//             - LHCb::State &trState (to get an estimate for the slopes for local track fit)
//             - double const &measMP (residual)
//             - double const &referenceZ (at this z-position)
//             - double const &stereo_angle (for this stereo angle)
//-----------------------------------------------------------------
StatusCode TAConfig::FillMatrix( const int &rank, 
				 const double trPar[4],
				 const double &measMP,
				 const double &referenceZ, 
				 const double &gamma ) {
  
  //-----------------------------
  // prepare Millepede;
  // configure the local (= track model) and global (= geometry) models:
  //-----------------------------
  m_derLC.resize( m_ntrack_pars ,0.);
  m_derGB.resize( m_n_dof*m_DETmap.size() ,0.);
  // initialize the derivatives      
  m_derivatives->SetLocal( m_derLC, rank, referenceZ, 0.0, gamma );
  Gaudi::XYZVector origin( trPar[0], trPar[2], 0.0);
  Gaudi::XYZVector slopes( trPar[1], trPar[3], 0.0);
  info() << "Reference Z = " << referenceZ << " " << "Angle = " << gamma/(2*3.141592)*360 << endreq;
  info() << "Local derivatives: " << m_derLC << endreq;
  StatusCode sc = m_derivatives->SetGlobal( slopes, origin, m_DETmap[rank], m_derGB, rank, referenceZ, gamma );
  info() << "Global derivatives: " << m_derGB << endreq;
  if ( sc == StatusCode::FAILURE ) {
    error() << "Error in calculating derivatives; will exit!" << endreq;
    return StatusCode::FAILURE;
  }
  //fill arrays of MP       
  m_Centipede->EquLoc( m_derGB, m_derLC, measMP , 1.44 ); // resolution = 1.44 mm since no drift time info...
  return StatusCode::SUCCESS;
}
//-------------------------------------------------------
// ConfMatrix: configure matrix with measurements & tracks
// method uses as input:
//                      LHCb::Track &track (a track)
//                      LHCb::LHCbID &id (a detector hit)
//                      const int &rank (position in big matrix)
//                      const double &weight (error on the measurement == hit resolution)
// and as output
//                      double &measMP (residual)    
//                      double &referenceZ (at this z-position)
//                      double &stereo_angle (with this stereo angle)
//                      LHCb::State &trState (at this state)
//-------------------------------------------------------

StatusCode TAConfig::CalcResidual( const LHCb::Track &track, 
				   const LHCb::Measurement *trMeas, 
				   const int &rank, 
				   const double &weight,
				   double &measMP,
				   double &referenceZ,
				   double &gamma,
				   LHCb::State &trState ) {
  //     const Gaudi::TrackVector& trRefVec = trMeas.refVector(); //get ref vector
  // Check if this was a muon measurement.
  const LHCb::Trajectory *ptraj;
  double z = 0;
  if ( trMeas->type() == LHCb::Measurement::Muon ) {
    // This is a muon hit!
    const  LHCb::MuonMeasurement *muMeas = dynamic_cast<const MuonMeasurement*>(trMeas);
    z = muMeas->z();
    trState = track.closestState( z );
    // trajectory describing measurement
    // e.g. the 'wire' in case of OT
    ptraj = &muMeas->trajectory();
  } else {
    z = trMeas->z();
    trState = track.closestState( z ); //get state
    // trajectory describing measurement
    ptraj = &trMeas->trajectory();
  }
  info() << "Measurement is at z = " << z << endreq;
  StatusCode sc = m_extrapolator->propagate( track, z, trState );
  if ( sc.isFailure() ) {
    error() << "Error in propagating track state to desired z-position" << endreq;
    return sc;
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
  double alongWire =  ptraj->arclength(statetraj.position( alongState ) );
  // !! get Poca between statetraj & ptraj	           
  Gaudi::XYZVector pocaVec;
  sc = m_poca->minimize( *ptraj, alongWire,  statetraj, alongState, pocaVec, 0.01);
  Gaudi::XYZPoint hit_point = ptraj->position(alongWire);
  if( sc.isFailure() ) return sc;
  double mag = sqrt(pocaVec.Mag2() );
  sign = (pocaVec.x() > 0) ? 1 : -1; // -1 = left of straw
  //-----------------------------------------------
  // !! make LineTraj object of straw and of zAxis
  //-----------------------------------------------
  Gaudi::XYZPoint pointZero; pointZero.SetCoordinates(0,0,0); 
  Gaudi::XYZPoint pointatZ; pointatZ.SetCoordinates(0,0,20000); 
  // x-axis through the point on the wire where the hit occured
  Gaudi::XYZPoint XpointatZ1; XpointatZ1.SetCoordinates(hit_point.x(),hit_point.y(),hit_point.z() ); 
  Gaudi::XYZPoint XpointatZ2; XpointatZ2.SetCoordinates(6000,hit_point.y(),hit_point.z()); 
  Gaudi::XYZVector ZVec;     
  double onstraw = 0.; double onZaxis = 0.; 
  int direction = 0; //direction of measMP seen from (0,0,z)
  // make a line trajectory between the begin and end point of measurement.
  const LineTraj* linearhit = new LineTraj(ptraj->beginPoint(), ptraj->endPoint() );
  // make a line trajectory describing z and x axes.
  const LineTraj* ZaxisTraj = new LineTraj( pointZero, pointatZ );
  const LineTraj* XaxisTraj = new LineTraj( XpointatZ1, XpointatZ2 );
  // find the poca between (linearized) hit trajectory and z-axis.
  sc = m_poca->minimize( (*linearhit), onstraw, false, (*ZaxisTraj), onZaxis, false, ZVec, 0.01);    
  if( sc.isFailure() ) return sc;
  info() << "Direction of hit: " << linearhit->direction() << endreq;
  // get angle between x-axis and trajectory describing the hit:
  // First, calculate the product of the absolute values of the lengths of both vectors
  double l1 = sqrt( XaxisTraj->direction().Mag2() );
  double l2 = sqrt( linearhit->direction().Mag2() );
  double dp = XaxisTraj->direction().Dot( linearhit->direction() );
  dp = dp / ( l1 * l2 );
  gamma = acos( dp );
  Gaudi::XYZVector z_direction = XaxisTraj->direction().Cross( linearhit->direction() );
  info() << "z-direction = " << z_direction << " z-axis = " << ZaxisTraj->direction() << endreq;
  if ( z_direction.Dot( ZaxisTraj->direction() ) > 0 ) {
    // the hit trajectory may point downward or upward, and hence
    gamma =  - gamma;
  }
  gamma = 3.14159265359/2.0 - gamma;
  //  projection of wire on z Axis -> for XPLANES it gives right x&z value
  Gaudi::XYZPoint pointZonAxis = ZaxisTraj->position(onZaxis);
  direction = (ZVec.x() > 0 ) ? 1 : -1;  //get the right side
  //measMP = direction*( sqrt( ZVec.x()*ZVec.x() + ZVec.y()*ZVec.y() )) - (sign*drDist); 
  //----------------------------------
  //  no drift distance info !!
  //---------------------------------
  measMP = direction*( sqrt( ZVec.x()*ZVec.x() + ZVec.y()*ZVec.y() )) ; 
  info() << "Residual = " << measMP << endreq;
  referenceZ = ptraj->position(alongWire).z();
  return StatusCode::SUCCESS;
}

StatusCode TAConfig::Rank( LHCb::LHCbID &id, int & r ) {
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
    info() << "key of map = " << (*t).first << " value of map = Rank nr. = " << (*t).second << endreq;
    r = (*t).second;
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
  for ( ;it != the_vector.end(); it++, cnt++ ) {
    *it = the_array[cnt];
  }
}

std::string TAConfig::MuonDetName( const unsigned int &lsta ) {
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

StatusCode TAConfig::GlobalFit( std::vector<double> & parameter,
                      std::vector<double> & error,
                      std::vector<double> & pull) {
  StatusCode sc = m_Centipede->MakeGlobalFit( parameter, error, pull );
  if ( sc.isFailure() ) {
    info() << "Error in Millepede's !";
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode TAConfig::LocalTrackFit( int &tr_cnt,
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
}


  /*
  std::string name = "";
  //Getting stations
  IDetectorElement::IDEContainer::iterator itSt;
  debug() << "Finding muon station name which has hit: " << mstation << "..." << endreq;
  for(itSt=m_muon->childBegin(); itSt<m_muon->childEnd(); itSt++){
    debug() << "name = " << (*itSt)->name() << endreq;
    //    const DeMuonChamber *must = dynamic_cast<const DeMuonChamber*>(*itSt);
    //    debug() << "station id = " << must->stationNumber() << endreq;
    //      if( mstation == must->stationNumber() ) {
    name=(*itSt)->name();
    //    }
  }
  return name;
}
  */
