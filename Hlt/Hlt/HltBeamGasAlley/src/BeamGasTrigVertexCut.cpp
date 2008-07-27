// $Id: BeamGasTrigVertexCut.cpp,v 1.1.1.3 2008-07-27 11:35:09 phopchev Exp $
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
  declareProperty( "RZTracksInputLocation", m_RZTracksLocation = "Rec/Track/RZVeloBeamGas" );
  declareProperty( "HistoBinWidth", m_binWidth = 10);
  declareProperty( "MaxBinValueCut", m_maxCut = 5);
  declareProperty( "ZExclusionRangeLow", m_zExclusionRangeLow = -265.0);
  declareProperty( "ZExclusionRangeUp",  m_zExclusionRangeUp  =  265.0);
  declareProperty( "HistoZRangeLow", m_histoZMin = -1500.0);
  declareProperty( "HistoZRangeUp",  m_histoZMax =  1500.0);
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
  
  m_nBins = (int)(m_histoZMax - m_histoZMin)/m_binWidth; 
  
  info() << "========== Algorithm parameters ======"            << endreq
         << "RZTracksLocation     = " << m_RZTracksLocation     << endreq   
         << "HistoZRangeLow       = " << m_histoZMin << " mm"   << endreq
         << "HistoZRangeUp        = " << m_histoZMax << " mm"   << endreq
	 << "HistoBinWidth	  = " << m_binWidth  << " mm"	<< endreq
	 << "NumberOfBins	  = " << m_nBins    		<< endreq
	 << "MaxBinValueCut       = " << m_maxCut		<< endreq
	 << "ZExclusionRangeLow   = " << m_zExclusionRangeLow   << " mm"    << endreq
	 << "ZExclusionRangeUp    = " << m_zExclusionRangeUp    << " mm"    << endreq
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
  
  AIDA::IHistogram1D* h_z_r0BGTracks = book("z at r=0 of BG tracks" , m_histoZMin, m_histoZMax, m_nBins);
  
  
  for ( LHCb::Tracks::const_iterator itT = BGtracks->begin(); BGtracks->end() != itT ; ++itT ) 
  {  
    //New Idea From DAVID !!!
    z = (*itT)->firstState().z();
    r = (*itT)->firstState().x();
    t = (*itT)->firstState().tx();
    z_r0 = z - r/t;
    
    if( z_r0 < m_zExclusionRangeLow || z_r0 > m_zExclusionRangeUp ) 
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
        if( std::fabs(z_r0 - zPosOfMaxBin) < m_binWidth*0.5 ) m_outputTracks->push_back( *itT );
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

