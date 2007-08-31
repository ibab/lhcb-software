//-----------------------------------------------------------------------------
/** @file TAConfig.cpp
 *
 *  Implementation file for Millepede configuration tool : TAConfig
 *
 *  CVS Log :-
 *  $Id: TAConfig.cpp,v 1.8 2007-08-31 09:59:31 jblouw Exp $
 *
 *  @author J. Blouw (johan.blouw@mpi-hd.mpg.de)
 *  @date   12/04/2007
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/ToolFactory.h"


// Kernel
#include "Kernel/LHCbID.h"
#include "Kernel/DifTraj.h"
#include "Kernel/AlignTraj.h"
#include "Kernel/SerializeStl.h"

//#include "StringConvert.h"
// Event
#include "Event/StateTraj.h"
#include "Event/Track.h"

#include "STDet/DeSTLayer.h"
#include "STDet/DeITLayer.h"
#include "STDet/DeTTLayer.h"
#include "STDet/DeITSector.h"
#include "STDet/DeSTDetector.h"
#include "STDet/DeITDetector.h"
#include "STDet/DeTTDetector.h"
#include "STDet/DeTTHalfModule.h"
#include "OTDet/DeOTDetector.h"
#include "OTDet/DeOTLayer.h"
#include "OTDet/DeOTQuarter.h"
#include "OTDet/DeOTStation.h"
#include "OTDet/DeOTModule.h"
#include "VeloDet/DeVelo.h"

// Conditions
#include "DetDesc/IAlignment.h"
#include "DetDesc/AlignmentCondition.h"

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
  ot_detector(false) {

  // Interface
  declareInterface<ITAConfigTool>(this);
  info() << "Creating TAConfig object" << endreq;
  // options
  declareProperty("MillepedeTool" , m_MillepedeTool  = "Millepede");
  declareProperty("DerivativeTool", m_derivativTool = "Derivatives");
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
  //  declareProperty("TT_objects", m_TTmap );
  declareProperty("TT_system",m_ttSys = false );
  declareProperty("TT_station",m_ttStation = false );
  declareProperty("TT_layer",m_ttLayer = false );
  declareProperty("IT_alignment_condition_location", m_itCond = "");
  declareProperty("TT_alignment_condition_location", m_ttCond = "");
  declareProperty("OT_alignment_condition_location", m_otCond = "");
  debug() << "Millepede tool " << m_MillepedeTool << endreq;
  debug() << "Derivative tool " << m_derivativTool << endreq;
  debug() << "nTrackModelParameters " << m_ntrack_pars << endreq;
  debug() << "Degrees_of_Freedom " << m_dof << endreq;
  m_DOF = new bool[m_dof.size()];
}

TAConfig::~TAConfig() {delete m_DOF;};

StatusCode TAConfig::Initialize( std::vector<std::string> &m_dets ) {
  m_detectors = m_dets;
  // Millepede tool
  m_Millepede = tool<IMillepede>( m_MillepedeTool );
  // derivatives tool
  m_derivatives = tool<IDerivatives>( "Derivatives");
  // Track extrapolator
  m_extrapolator = tool<ITrackExtrapolator>( "TrackMasterExtrapolator" );
  // - trajecory poca tool
  m_poca = tool<ITrajPoca>( "TrajPoca" );
  // magnetic field tool
  m_pIMF = svc<IMagneticFieldSvc>( "MagneticFieldSvc",true );
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
      //      VELOmap.resize( m_velo->numberRightRSensors() + m_velo->numberRightPhiSensors() );// 42 sensors... (2 X 21 stations)
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
  for ( ; it != map.end(); it++ ) {
    rank = it->second;
    for ( unsigned int i = 0; i < m_dof.size(); i++ ) {
      m_Millepede->ParSig(rank+i*m_DETmap.size(), 20.0 );
    }
  }
  it = map.begin();
  for ( ; it != map.end(); it++ ) {
    name = it->first;
    rank = it->second;
    ConstrainPositions( rank, name, m_DETmap.size() );
  }
  info() << "(2) Map = " << map << endreq;
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
  m_dett->geometry()->matrixInv().GetDecomposition( Rotation, position);
  info() << "TT system name & position: " << m_dett->name() << " "
	 << position.x() << ", "
	 << position.y() << ", "
	 << position.z() << endreq;
  if ( m_ttSys && ! m_ttStation && ! m_ttLayer ) {
    TTmap.push_back( m_dett->geometry()->matrixInv() );
    AlignmentCondition *tt_cond = const_cast<AlignmentCondition*>
      (m_dett->geometry()->alignmentCondition());
    m_ALImap.push_back( tt_cond->matrix() ); // save the alignment conditions of the tt sys
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
      (m_TTLayers[j]->geometry()->matrixInv()).GetDecomposition(Rotation, position);
      s_zmoy_tt += (position.z()-m_zmoy_tt)*(position.z()-m_zmoy_tt);
    }
  }
  s_zmoy_tt /= nStations;
  return StatusCode::SUCCESS;
}

StatusCode TAConfig::ConfigOT( std::vector<Gaudi::Transform3D> &OTmap ) {
  unsigned int nStations = m_ot->nStation();
  //  ot = OTmap.size();
  Gaudi::Rotation3D Rotation;
  Gaudi::XYZVector position;
  m_deot->geometry()->matrixInv().GetDecomposition( Rotation, position );
  info() << "OT system name & position: " << m_deot->name() << " "
	 << position.x() << " " 
	 << position.y() << " "
	 << position.z() << endreq;
  if ( m_otSys && ! m_otStation && ! m_otLayer && ! m_otQuadrant && ! m_otModule ) {
    OTmap.push_back( m_deot->geometry()->matrixInv() );
    AlignmentCondition *ot_cond = const_cast<AlignmentCondition*>
      (m_deot->geometry()->alignmentCondition() );
    m_ALImap.push_back( ot_cond->matrixInv() );
  }
  
  m_zmoy_ot = 0;
  int rank = m_rank.size();
  m_OTStations = m_deot->childIDetectorElements();
  for ( unsigned int i = 0; i < m_OTStations.size(); i++ ) {
    m_OTLayers = m_OTStations[i]->childIDetectorElements();
    for ( unsigned int j = 0; j < m_OTLayers.size(); j++ ) {
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
	    m_OTModules[l]->geometry()->matrixInv().GetDecomposition( Rotation, position );
	    s_zmoy_tt += (position.z()-m_zmoy_ot)*(position.z()-m_zmoy_ot);
	  }
	}
	if ( m_otQuadrant && ! m_otModule ) {
	  m_OTQuadrants[k]->geometry()->matrixInv().GetDecomposition( Rotation, position );
	  s_zmoy_tt += (position.z()-m_zmoy_ot)*(position.z()-m_zmoy_ot);
	}
      }
      if ( m_otLayer && ! m_otQuadrant && ! m_otModule ) {
	m_OTLayers[j]->geometry()->matrixInv().GetDecomposition( Rotation, position );
	s_zmoy_tt += (position.z()-m_zmoy_ot)*(position.z()-m_zmoy_ot);
      }
    }
    if ( m_otStation && ! m_otLayer && ! m_otQuadrant && ! m_otModule ) {
      m_OTStations[i]->geometry()->matrixInv().GetDecomposition( Rotation, position );
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
  m_deit->geometry()->matrixInv().GetDecomposition( Rotation, position );
  info() << "IT system name & position: " << m_deit->name() << " "
	 << position.x() << ", "
	 << position.y() << ", "
	 << position.z() << endreq;
  if ( m_itSys && ! m_itStation && ! m_itLayer && ! m_itBox ) {
    ITmap.push_back( m_deit->geometry()->matrixInv() );
    AlignmentCondition *it_cond = const_cast<AlignmentCondition*>
      (m_deit->geometry()->alignmentCondition() );
    m_ALImap.push_back( it_cond->matrixInv() );
  }

  m_zmoy_it = 0.0;
  int rank = m_rank.size();
  m_ITStations = m_deit->childIDetectorElements();
  for ( unsigned int i = 0; i < m_ITStations.size(); i++ ) { // Loop over stations
    // Note: there are actually 4 IT stations, since one of them is the radiation monitor...
    m_ITBoxes = m_ITStations[i]->childIDetectorElements();
    for ( unsigned int j = 0; j < m_ITBoxes.size(); j++ ) { // Loop over boxes
      m_ITLayers = m_ITBoxes[j]->childIDetectorElements();
      //      info() << "Inv Matrix of De " << m_ITBoxes[j]->name() << " = " << m_ITBoxes[j]->geometry()->matrixInv() << endreq;
      for ( unsigned int k = 0; k < m_ITLayers.size(); k++ ) { // Loop over layers
	//	info() << "Inv Matrix of De " << m_ITLayers[k]->name() << " = " << m_ITLayers[k]->geometry()->matrixInv() << endreq;
	info() << "Align De " << m_ITLayers[k]->name() << endreq;
	m_ITLadders = m_ITLayers[k]->childIDetectorElements();
	for ( unsigned int l = 0; l < m_ITLadders.size(); l++ ) {
	  if ( m_itLadder ) {
	    //	    info() << "Inv Matrix of De " << m_ITLadders[l]->name() << " = " << m_ITLadders[l]->geometry()->matrixInv() << endreq;
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
  info() << "(1) Map = " << constrain_it << endreq;
  m_zmoy_it /= nStations;
  s_zmoy_it = 0.0;
  for ( unsigned int i = 0; i < m_ITStations.size(); i++ ) {
    m_ITBoxes = m_ITStations[i]->childIDetectorElements();
    for ( unsigned int j = 0; j < m_ITBoxes.size(); j++ ) {
      m_ITLayers = m_ITBoxes[j]->childIDetectorElements();
      for ( unsigned int k = 0; k < m_ITLayers.size(); k++ ) {
	if ( m_itLayer ) {
	  (m_ITLayers[k]->geometry()->matrixInv()).GetDecomposition(Rotation, position);
	  s_zmoy_it += (position.z() - m_zmoy_it)*(position.z() - m_zmoy_it);
	}
      }
      if ( m_itBox && ! m_itLayer ) {
	(m_ITBoxes[j]->geometry()->matrixInv()).GetDecomposition(Rotation, position);
	s_zmoy_it += (position.z() - m_zmoy_it)*(position.z() - m_zmoy_it);
      }
    }
    if ( m_itStation && ! m_itBox && ! m_itLayer ) {
      (m_ITStations[i]->geometry()->matrixInv()).GetDecomposition(Rotation, position);
      s_zmoy_it += (position.z() - m_zmoy_it)*(position.z() - m_zmoy_it);
    }
  }
  s_zmoy_it /= nStations;
  return StatusCode::SUCCESS;
}

Gaudi::XYZVector TAConfig::Residual( LHCb::Track &t, LHCb::LHCbID &id ) {
  debug() << "Into TAConfig::Residual (1)" << endreq;
  Gaudi::XYZVector distance(-999999.9999,-9999.9999,-99999.99999);

  // Measurement
  Measurement& hit = t.measurement( id );
  
  // State of track at z of hit
  State &state = t.closestState( hit.z() );

  // Set refVector in case it was not set before
  if ( !hit.refIsSet() ) 
    hit.setRefVector( state.stateVector() );

  Gaudi::XYZVector bfield;
  const Gaudi::TrackVector &refVec = hit.refVector();
  const Gaudi::XYZPoint refPoint( refVec[0], refVec[1], hit.z() );
  m_pIMF->fieldVector( refPoint, bfield );
  const StateTraj refTraj( refVec, hit.z(), bfield );

  // Get the measurement trajectory
  const Trajectory& measTraj = hit.trajectory();

  // Determine initial estimates of s1 and s2
  double s1 = 0.0; // Assume state is already close to the minimum
  double s2 = measTraj.arclength( refTraj.position(s1) );

  // Determine the actual minimum with the Poca tool
  m_poca->minimize( refTraj, s1, measTraj, s2, distance, m_tolerance );
  
  return distance;
}
void TAConfig::CreateMap( int & r,  IDetectorElement* id, double &m_zmoy ) {
  Gaudi::Rotation3D R;
  Gaudi::XYZVector T;
  //  info() << "Name of object = " << id->name() << endreq;
  m_C_pos.insert(make_pair(id->name(),r ));
  m_rank.push_back( r );
  m_ALImap.push_back( id->geometry()->alignmentCondition()->matrixInv() );
  m_DETmap.push_back( id->geometry()->matrixInv() );
  info() << "Z-position" << id->geometry()->matrixInv() << endreq;
  id->geometry()->matrixInv().GetDecomposition( R, T );
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
  int m_nfits = 1;
  info() << "Calling InitMille with m_sigma = " << m_sigma << "..." << endreq;
  m_Millepede->InitMille( m_DOF,
			  m_SIGMA,
			  num_objects, nlocal,  
			  m_l_chimax, m_nstd, 
			  m_residual_cut, m_ini_residual_cut,
			  m_nfits); 
  
  /* HD Millepede initialization call
  m_Millepede->InitMille( m_DOF,
			  num_objects, nlocal,  
			  m_l_chimax, m_nstd, 
			  m_residual_cut, m_ini_residual_cut);
  */

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
    sc = m_Millepede->ParSig(rank, 0.0); // fix x-position
    if ( sc.isFailure() ) {
      error() << "Failure in fixing x-position of object " << de_name << " at rank " << rank << endreq;
      return sc;
    }
    info() << "Fixing x-position of object " << de_name << " at rank " << rank << " using ParSig(" << rank << ", 0) "<< endreq;
    dof_cnt++;
  }
  if ( m_dof[1] && m_fix_y) {
    sc = m_Millepede->ParSig(rank+dof_cnt*rs, 0.0); // fix y-position
    if ( sc.isFailure() ) {
      error() << "Failure in fixing y-position of object " << de_name << " at rank " << rank << endreq;
      return sc;
    }
    info() << "Fixing y-position of object " << de_name << " at rank " << rank+rs << " using ParSig(" << rank+dof_cnt*rs << ", 0) "<< endreq;
    dof_cnt++;
  }
  if ( m_dof[2] && m_fix_z) {
    sc = m_Millepede->ParSig(rank+dof_cnt*rs, 0.0); // fix z-position
    if ( sc.isFailure() ) {
      error() << "Failure in fixing z-position of object " << de_name << " at rank " << rank << endreq;
      return sc;
    }
    info() << "Fixing z-position of object " << de_name << " at rank " << rank+dof_cnt*rs << " using ParSig(" << rank+dof_cnt*rs << ", 0) " << endreq;
    dof_cnt++;
  }
  if ( m_dof[3] && m_fix_a) {
    sc = m_Millepede->ParSig(rank+dof_cnt*rs, 0.0); // fix alpha rotation
    if ( sc.isFailure() ) {
      error() << "Failure in fixing alpha rotation of object " << de_name << " at rank " << rank << endreq;
      return sc;
    }
    info() << "Fixing angle alpha of object " << de_name << " at rank " << rank+dof_cnt*rs << " using ParSig(" << rank+dof_cnt*rs << ", 0) " << endreq;
    dof_cnt++;
  }
  if ( m_dof[4] && m_fix_b) {
    sc = m_Millepede->ParSig(rank+dof_cnt*rs, 0.0); // fix beta rotation
    if ( sc.isFailure() ) {
      error() << "Failure in fixing beta rotation of object " << de_name << " at rank " << rank << endreq;
      return sc;
    }
    info() << "Fixing angle beta of object " << de_name << " at rank " << rank+dof_cnt*rs << " using ParSig(" << rank+dof_cnt*rs << ", 0) " << endreq;
    dof_cnt++;
  }
  if ( m_dof[5] && m_fix_g) {
    sc = m_Millepede->ParSig(rank+dof_cnt*rs, 0.0); // fix gamma rotation
    if ( sc.isFailure() ) {
      error() << "Failure in fixing gamma rotation of object " << de_name << " at rank " << rank << endreq;
      return sc;
    }
    info() << "Fixing angle gamma of object " << de_name << " at rank " << rank+dof_cnt*rs << " using ParSig(" << rank+dof_cnt*rs << ", 0) " << endreq;
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
StatusCode TAConfig::FillMatrix( LHCb::Track &t, LHCb::LHCbID &id, const int &rank, const double &weight ) {
  //get OTTimes info
  m_otTimes = get<OTTimes>( OTTimeLocation::Default );  
  //  info() << "Into TAConfig::FillMatrix" << endreq;
  // For the time being, we do not know what to do
  // with the case where both mono-layers
  // had a hit on the same track. 
  // Therefore, we chuck the 2nd hit in that layer.
  
    // Measurement from track t, and lhcbID id:
  Measurement& hit = t.measurement( id );
  // z position of hit (??)
  double z = hit.z();
  // Get the measurement trajectory
  const Trajectory& hitTraj = hit.trajectory();
  // Track State close to hit
  State &state = t.closestState( z );
  State trState;
  StatusCode sc = m_extrapolator->propagate( t, z, trState );
  if ( sc.isFailure() ) {
    error() << "Error in propagating track state to desired z-position" << endreq;
    return sc;
  }
  // set the refVector of the hit trajectory. (why I don't know...)
  //  hit.setRefVector( trState.stateVector() );
  //
  //  info() << "Hit trajectory's RefVector   " << hit.refVector() << endreq;
  //  info() << "Track trajectory's RefVector " << trState.stateVector() << endreq;
  Gaudi::XYZVector bfield;
  // TrackVector is a vector with 5 elements.
  // refVec is the 5D vector describing the hit
  const Gaudi::TrackVector &refVec = trState.stateVector(); 
  const Gaudi::XYZPoint refPoint( refVec[0], refVec[1], z );
  // Get the vector describing the magnetic field at the position of the hit:
  m_pIMF->fieldVector( refPoint, bfield );
  // create a StateTraj of the hit-vector (refVec)
  // describing the curve of the hit at the z-position
  // under the influence of the magnetic field.
  const StateTraj refTraj( refVec, z, bfield );
    // Determine initial estimates of s1 and s2
  double s1 = 0.0; // Assume state is already close to the minimum
  double s2 = hitTraj.arclength( refTraj.position(s1) );

  Gaudi::XYZVector distance(-999999.9999,-9999.9999,-99999.99999);
  // minimize distance between trajectory describing track
  // and trajectory describing the hit 
  sc = m_poca->minimize( refTraj, s1, hitTraj, s2, distance, m_tolerance );
  if ( sc.isFailure() ) {
    error() << "Error in calculating distance of closest approach between track and hit" << endreq;
    return StatusCode::FAILURE;
  }
  // create linetraj from poca vector, that is
  // use point (s1) on refTraj and point s2 on hitTraj:
  const Gaudi::XYZPoint hitPoint = refTraj.position(s1);
  LineTraj pocaTraj( hitPoint, hitTraj.position(s2) );
  const Gaudi::XYZVector slope = state.slopes();
  double stereo_angle;
  if ( id.isOT() ) {
    OTChannelID otid = id.otID();
    //use OTTime !!
    OTTime* time = m_otTimes->object( otid );
    const double caltime = time->calibratedTime();
//    const double rawdrDist = m_ot->driftDistance(caltime); //Tprop+Tdrift
    const double propTime = m_ot->propagationTime( otid, 
						   hitTraj.position(s1).x(),
						   hitTraj.position(s1).y() );
    const double drDist = m_ot->driftDistance(caltime-propTime);
    double hit_residual = sqrt(distance.Mag2()) - drDist;
    plot( drDist, "OT drift distance", -1.0, 5.0, 200 );
    plot( sqrt(distance.Mag2()), "poca distance", -1.0, 5.0, 200 );
    plot( hit_residual, "OT hit residual", -2.0, 2.0, 200 );
    Gaudi::XYZPoint chClusterPoint = pocaTraj.position(drDist);
    z = chClusterPoint.z();
    // Create translation matrix:
    const double alpha = 0;
    const double beta = 0;
    const double gamma = 0.0;
    const double t_x = chClusterPoint.x() - refPoint.x();
    const double t_y = chClusterPoint.y() - refPoint.y();
    const double t_z = chClusterPoint.z() - refPoint.z();
    if ( otid.layer() == 0 ) {
      plot( t_x, "translation in x", -15,15,100);
      plot( t_y, "translation in y", -1,1,100);
      plot( t_z, "translation in z", -7,7,100);
    }
    Vector6 translation( t_x, t_y, t_z, alpha, beta, gamma);
    // translate the hit-trajectory in parallel to the pocaPoint
    AlignTraj chCluster = AlignTraj( hitTraj, translation );
    // Make a Gaudi::XZYPoint: (should be LHCb co-ordinate system!)
    // at the z-position of the hit
    // so that we can calculate the distance wrt (0,0,z)
    Gaudi::XYZPoint point = Gaudi::XYZPoint(0.,0.,z);
    // find poca between charge cluster and (0,0,z)
    s1 = 0;
    sc = m_poca->minimize( hitTraj, s1, point, distance, m_tolerance);
    if ( sc.isFailure() ) {
      error() << "Error in calculating distance of closest approach between hit wire and (0,0,z)" << endreq;
      return StatusCode::FAILURE;
    }
    double si = 1.0;
    double px,py,pz;
    distance.GetCoordinates(px,py,pz);
    if ( px < 0 ) si = -1.0;
    if ( otid.layer() == 0 ) 
      plot( si*sqrt(distance.Mag2()), "poca between z-axis and hit-wire", -3000.0,3000.0,100);
    sc = m_poca->minimize( chCluster, s1, point, distance, m_tolerance);
    if ( sc.isFailure() ) {
      error() << "Error in calculating distance of closest approach between cluster and (0,0,z)" << endreq;
      return StatusCode::FAILURE;
    }
    distance.GetCoordinates(px,py,pz);
    if ( px < 0 ) si = -1.0;
    if ( otid.layer() == 0 ) 
      plot( si*sqrt(distance.Mag2()), "poca between z-axis and cluster", -3000.0,3000.0,100);
    stereo_angle = m_ot->findModule( id.otID() )->angle();
  } else if ( id.isST() ) {
    if ( id.stID().isTT() && tt_detector )
      stereo_angle = m_tt->findLayer( id.stID() )->angle();
    else if ( id.stID().isIT() && it_detector )
      stereo_angle = m_it->findLayer( id.stID() )->angle();
  }
  double *derLC = new double[m_derLC.size()];
  double *derGB = new double[m_derGB.size()];
  // Convert derivative vectors to array(so desired by millepede tool)
  VectortoArray(m_derLC, derLC);
  VectortoArray(m_derGB, derGB);
  // need 2 dummy variable to be consistent with Sebastien's Millepede
  double *dernl = new double [m_derGB.size()]; 
  double *dernl_i = new double[m_derGB.size()];
  
  m_Millepede->ZerLoc( derGB, derLC, dernl, dernl_i );
  // configure the local (= track model) and global (= geometry) models:
  m_derivatives->SetLocal( m_derLC, rank, z, stereo_angle );
  m_derivatives->SetGlobal( slope, m_DETmap[rank], m_derGB, rank, z, stereo_angle );

  VectortoArray(m_derLC, derLC);
  VectortoArray(m_derGB, derGB);
  
  double si = 1.0;
  double px,py,pz;
  distance.GetCoordinates(px,py,pz);
  if ( px < 0 ) si = -1.0;

  plot2D(z, si*sqrt(distance.Mag2()),"hits",7000,10000,-7000,7000,1000,1400);
  plot( si*sqrt(distance.Mag2()),"Measurement co-ordinate",-2500,2500,100);

  // Need additional dummy variable to be consistent with Sebastien's Millepede
  //  m_Millepede->EquLoc( derGB, derLC, si*sqrt(distance.Mag2()), hit.errMeasure()/weight );
  m_Millepede->EquLoc( derGB, derLC, dernl, dernl_i, si*sqrt(distance.Mag2()), hit.errMeasure()/weight );
  ArraytoVector( &derLC[0], m_derLC );
  ArraytoVector( &derGB[0], m_derGB );
  
  delete [] derLC;
  delete [] derGB;
  delete [] dernl; 
  delete [] dernl_i;
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
  } else if ( velo_detector && id.isVelo() ) {
    // not yet done...
  }
  std::map<std::string,int>::iterator t = m_C_pos.find( name );
  if ( t != m_C_pos.end() ) {
    debug() << "key of map = " << (*t).first << endreq;
    debug() << "value of map = Rank nr. = " << (*t).second << endreq;
    r = (*t).second;
    return StatusCode::SUCCESS;
  } else
    return StatusCode::FAILURE;
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

