// $Id: BeamGasTrigVertexCut.cpp,v 1.1.1.1 2008-07-15 13:53:25 phopchev Exp $
#include "GaudiKernel/AlgFactory.h"
#include "AIDA/IHistogram1D.h"
#include "GaudiUtils/Aida2ROOT.h"
#include "TH1D.h"

#include "Event/Track.h"

#include "BeamGasTrigVertexCut.h"

//#include "HltBase/IHltConfSummaryTool.h"
//-----------------------------------------------------------------------------
// Implementation file for class : BeamGasTrigVertexCut
//
// 2008-06-16 : Plamen Hopchev <plamen.hopchev@cern.ch>
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( BeamGasTrigVertexCut );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BeamGasTrigVertexCut::BeamGasTrigVertexCut( const std::string& name, ISvcLocator* pSvcLocator):
                HltAlgorithm ( name , pSvcLocator ), m_trigEventsZnegative(0), m_trigEventsZpositive(0)		
{
  declareProperty( "RZBeamGasTracks", m_RZTracksLocation = "Rec/Track/RZVeloBeamGas" );
  declareProperty( "binWidth", m_binWidth = 10);
  declareProperty( "maxCut", m_maxCut = 5);
  declareProperty( "zMinPosCut", m_zMinPosCut = -265.0);
  declareProperty( "zMaxPosCut", m_zMaxPosCut = 265.0);
  declareProperty( "z_min", m_zMin = -1500.0);
  declareProperty( "z_max", m_zMax =  1500.0);
}
//=============================================================================
// Destructor
//=============================================================================
BeamGasTrigVertexCut::~BeamGasTrigVertexCut() {};

//=============================================================================
// Initialization
//=============================================================================
StatusCode BeamGasTrigVertexCut::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  
  m_nBins = (int)(m_zMax - m_zMin)/m_binWidth; 
  
  info() << "========== Algorithm parameters ======"            << endreq
         << "m_RZTracksLocation   = " << m_RZTracksLocation     << endreq   
         << "ZVertexMin           = " << m_zMin     << " mm"    << endreq
         << "ZVertexMax           = " << m_zMax     << " mm"    << endreq
	 << "BinWidth		  = " << m_binWidth << " mm"	<< endreq
	 << "NumberOfBins	  = " << m_nBins    		<< endreq
	 << "Max-Cut		  = " << m_maxCut		<< endreq
	 << "zOfTracksExclRegionLow = " << m_zMinPosCut  << " mm"    << endreq
	 << "zOfTracksExclRegionUpp = " << m_zMaxPosCut  << " mm"    << endreq
	 << "======================================"		<< endreq
	 << "========== HLT Algorithm related parameters ========="   << endreq
	 << "InputSelectionName  =         " << m_RZTracksLocation    << endreq
	 << "OutputSelectionName =         " << m_outputSelectionName << endreq;
  
  m_outputTracks = &( registerTSelection<LHCb::Track>() );
  
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode BeamGasTrigVertexCut::execute() {

  bool isDebug   = msgLevel( MSG::DEBUG );
  if(isDebug){  debug() << "==> Execute" << endmsg; }
  
  bool decision = false;
  
  LHCb::Tracks* BGtracks = get<LHCb::Tracks>( m_RZTracksLocation );
  debug() << "N tracks in the BG Tracks Container = " << BGtracks->size() << endmsg;
    
  float z, r, t;  
  float z_r0 = 0.;
  
  AIDA::IHistogram1D* h_z_r0BGTracks = book("z at r=0 of BG tracks" , m_zMin, m_zMax, m_nBins);
  
  
  for ( LHCb::Tracks::const_iterator itT = BGtracks->begin(); BGtracks->end() != itT ; ++itT ) 
  {  
    //New Idea From DAVID !!!
    z = (*itT)->firstState().z();
    r = (*itT)->firstState().x();
    t = (*itT)->firstState().tx();
    z_r0 = z - r/t;
    
    if( z_r0 < m_zMinPosCut || z_r0 > m_zMaxPosCut ) 
    { h_z_r0BGTracks->fill(z_r0, 1.0); }    
  }
  
  
  if( h_z_r0BGTracks->entries() > m_maxCut )
  {
    TH1D* z_r0HistROOT = Gaudi::Utils::Aida2ROOT::aida2root(h_z_r0BGTracks);
    
    int maxBin = z_r0HistROOT->GetMaximumBin();
    float zPosOfMaxBin = z_r0HistROOT->GetBinCenter( maxBin );
    float maxBinHeight = z_r0HistROOT->GetBinContent( maxBin );    
     
    if( maxBinHeight > m_maxCut )
    { 
      decision = true;
      if( zPosOfMaxBin > 0 ) m_trigEventsZpositive += 1;
      else m_trigEventsZnegative += 1;
      
      //Fill HLT SUMMARY with the big z_r0 tracks of the triggered event
      //If not using TES !!!
      m_outputTracks->clear();
      
      for ( LHCb::Tracks::const_iterator itT = BGtracks->begin(); BGtracks->end() != itT ; ++itT ) 
      {  
        z = (*itT)->firstState().z();
        r = (*itT)->firstState().x();
    	t = (*itT)->firstState().tx();
    	z_r0 = z - r/t;    
        if( std::fabs(z_r0 - zPosOfMaxBin) < m_binWidth ) m_outputTracks->push_back( *itT );
      }
      info() << "Number of Objects in the outputSelction = " << m_outputTracks->ncandidates() << endmsg;      
    }
  }
      
  h_z_r0BGTracks->reset();    
    
  setDecision( decision );
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode BeamGasTrigVertexCut::finalize() {

  std::cout << "Number of triggered Events with z<0 and z>0 : " 
            << m_trigEventsZnegative << "   " << m_trigEventsZpositive << std::endl;

  return HltAlgorithm::finalize () ;
}

