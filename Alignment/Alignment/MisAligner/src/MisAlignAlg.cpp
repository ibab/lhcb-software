//-----------------------------------------------------------------------------
/*
 *
 * Implementation file for Alignment algorithm : MisAlignAlg
 *
 *  CVS Log :-
 *  $Id: MisAlignAlg.cpp,v 1.3 2009-09-15 15:08:55 jblouw Exp $
 *
 *  @author J.Blouw Johan.Blouw@cern.ch
 *  @date   18/07/2008
 */
//-----------------------------------------------------------------------------

#include "MisAlignAlg.h"

#include "GaudiKernel/AlgFactory.h"

#include "GaudiKernel/SystemOfUnits.h"

// Conditions
#include "DetDesc/IAlignment.h"
#include "DetDesc/AlignmentCondition.h"

using namespace LHCb;
using namespace Gaudi;

DECLARE_ALGORITHM_FACTORY( MisAlignAlg )


MisAlignAlg::MisAlignAlg( const std::string &name,
			  ISvcLocator* pSvcLocator ):
	GaudiAlgorithm( name, pSvcLocator)  {
  declareProperty( "OTMisalignment_file", m_OTfile = "OT.opts" );
  declareProperty( "ITMisalignment_file", m_ITfile = "IT.opts" );
  declareProperty( "MuonMisalignment_file", m_MUONfile = "MUON.opts" );
  declareProperty( "TTMisalignment_file", m_TTfile = "TT.opts" );
  declareProperty( "VeLoMisalignment_file", m_VeLofile = "VELO.opts" );
  declareProperty( "Detectors", m_detectors );
}

StatusCode MisAlignAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) {
    error() << "Error initialising MisAlignAlg" << endreq;
    return sc;
  }
  info() << "Number of detectors to misalign: " << m_detectors.size() << endreq;
  info() << "Detectors to misalign: " << m_detectors << endreq;
  return StatusCode::SUCCESS;
}

MisAlignAlg::~MisAlignAlg() {
}

StatusCode MisAlignAlg::execute() {
  CacheDetElements( m_detectors );
  return StatusCode::SUCCESS;
}


StatusCode MisAlignAlg::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode MisAlignAlg::CacheDetElements( std::vector<std::string> &detectors  ) {
  // loop over detectors to be aligned and retrieve geometry information
  std::vector<std::string>::iterator t_i;
  info() << "Number of detectors to be mis-aligned: " << detectors.end() - detectors.begin() << endreq;
  StatusCode sc;
  for (  t_i = detectors.begin(); t_i != detectors.end(); ++t_i ) {
    info() << "Trying Detector " << *t_i << endl;
    if ( *t_i == "TT" ) {
      info() << "Get the TT geometry info" << endreq;
      parse = new MisalignmentParser( m_TTfile );
      m_tt = getDet<DeSTDetector>(DeSTDetLocation::TT);
      m_dett = getDet<IDetectorElement>(DeSTDetLocation::TT);
      tt_detector = true;
      Config( m_dett );
      delete parse;
    }
    if ( *t_i == "IT" ) {
      info() << "Get the IT geometry info" << endreq;
      parse = new MisalignmentParser( m_ITfile );
      m_it = getDet<DeSTDetector>(DeSTDetLocation::IT);
      m_deit = getDet<IDetectorElement>(DeSTDetLocation::IT);
      it_detector = true;
      Config( m_deit );
      delete parse;
    }
    if ( *t_i == "OT" ) {
      info() << "Get the OT geometry info" << endreq;
      parse = new MisalignmentParser( m_OTfile );
      m_ot = getDet<DeOTDetector>(DeOTDetectorLocation::Default);
      m_deot = getDet<IDetectorElement>(DeOTDetectorLocation::Default);
      ot_detector = true;
      Config( m_deot );
      delete parse;
    }
    if ( *t_i == "MUON" || *t_i == "Muon" ) {
      info() << "Get the Muon geometry info" << endreq;
      parse = new MisalignmentParser( m_MUONfile );
      m_muon = getDet<DeMuonDetector>(DeMuonLocation::Default);
      m_demuon = getDet<IDetectorElement>(DeMuonLocation::Default);
      muon_detector = true;
      Config( m_demuon );
      delete parse;
    }
    if ( *t_i == "Velo" || *t_i == "VELO" ) {
      info() << "Get the Velo geometry info" << endreq;
      parse = new MisalignmentParser( m_VeLofile );
      m_velo = getDet<DeVelo>(DeVeloLocation::Default);
      m_develo = getDet<IDetectorElement>(DeVeloLocation::Default);
      velo_detector = true;
      Config( m_develo );
      delete parse;
    }
    info() << "Continue initialization..." << endreq;
  }
  return StatusCode::SUCCESS;
}



void MisAlignAlg::Config( IDetectorElement *det ) {
  debug() << "Going for det element " << det->name() << endl;
  MisalignmentParser::AlignmentParameters ap = 
    parse->getAlignmentParameters( det->name() );
  if ( ap->isMisaligned() ) {
    std::vector<double> translation;
    std::vector<double> rotation;
    std::vector<double> pivot(3,0.0);
    translation = ap->getShifts();
    rotation = ap->getRotations();
    IGeometryInfo *geo = det->geometry();
    StatusCode sc = geo->ownToOffNominalParams( translation,
						rotation,
						pivot );
    
    info() << "Misaligning det element: " << det->name() << endreq;
    info() << "            translation : " << translation << endreq;
    info() << "               rotation : " << rotation << endreq;
  } 
  IDetectorElement::IDEContainer::iterator i = det->childBegin();
  for ( ; i < det->childEnd(); i++ ) {
    Config( *i );
  }
}
