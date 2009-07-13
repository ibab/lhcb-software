
#include "GaudiKernel/AlgFactory.h"
#include "AIDA/IHistogram1D.h"
#include "GaudiUtils/Aida2ROOT.h"
#include "TH1D.h"

#include "Event/Track.h"

#include "BeamGasTrigVertexCut.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BeamGasTrigVertexCut
// 2008-06-16 : Plamen Hopchev <plamen.hopchev@cern.ch>
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( BeamGasTrigVertexCut );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BeamGasTrigVertexCut::BeamGasTrigVertexCut(const std::string& name, ISvcLocator* pSvcLocator):
                HltAlgorithm(name, pSvcLocator, false), m_trigEventsZnegative(0), m_trigEventsZpositive(0)	
{
  declareProperty( "RZTracksInputLocation", m_RZTracksLocation = "Rec/Track/RZVeloBeamGas" );
  declareProperty( "HistoBinWidth", m_binWidth = 10);
  declareProperty( "MaxBinValueCut", m_maxCut = 5);
  declareProperty( "ZExclusionRangeLow", m_zExclusionRangeLow = -265.0);
  declareProperty( "ZExclusionRangeUp",  m_zExclusionRangeUp  =  265.0);
  declareProperty( "HistoZRangeLow", m_histoZMin = -1500.0);
  declareProperty( "HistoZRangeUp",  m_histoZMax =  1500.0);
  declareProperty( "OutputSelection", m_outputSelectionName = name );
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

  verbose() << "==> Initialize" << endmsg;
  
  info() << "========== Algorithm parameters ======"            << endreq
         << "RZTracksLocation     = " << m_RZTracksLocation     << endreq   
         << "HistoZRangeLow       = " << m_histoZMin << " mm"   << endreq
         << "HistoZRangeUp        = " << m_histoZMax << " mm"   << endreq
	 << "HistoBinWidth        = " << m_binWidth  << " mm"	<< endreq
	 << "MaxBinValueCut       = " << m_maxCut		<< endreq
	 << "ZExclusionRangeLow   = " << m_zExclusionRangeLow   << " mm"    << endreq
	 << "ZExclusionRangeUp    = " << m_zExclusionRangeUp    << " mm"    << endreq
	 << "OutputSelectionName  = " << m_outputSelectionName  << endreq;
	 
  m_trackSelection = &( registerTSelection<LHCb::Track>( m_outputSelectionName ) );
  
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode BeamGasTrigVertexCut::execute() {

  bool trigDecision  = false;  
  std::vector<double> vect_z_r0;
  vect_z_r0.reserve(200);
  
  debug() << "==> Execute" << endmsg;
  
  LHCb::Tracks* BGtracks;
  if( exist<LHCb::Tracks>( m_RZTracksLocation ) ){ BGtracks = get<LHCb::Tracks>( m_RZTracksLocation ); }
  else { error() << "Can't find input tracks" << endmsg; return StatusCode::SUCCESS; }  
  debug() << "Number of tracks in the BG Tracks Container = " << BGtracks->size() << endmsg;
    
  float z, r, t;  
  float z_r0 = 0.;
  
  // Fill the vector with the tracks z position at r=0
  for ( LHCb::Tracks::const_iterator itT = BGtracks->begin(); BGtracks->end() != itT ; ++itT ) 
  {  
    z = (*itT)->firstState().z();
    r = (*itT)->firstState().x();
    t = (*itT)->firstState().tx();
    z_r0 = z - r/t;
        
    if ( (z_r0 > m_histoZMin) && (z_r0 < m_histoZMax  ) )
    { 
      if ( (z_r0 < m_zExclusionRangeLow) || (z_r0 > m_zExclusionRangeUp) )
        vect_z_r0.push_back( z_r0 );    
    }  
  }
  
  // If we have enough entries, look for the entries in the "max bin"
       
  if( vect_z_r0.size() > m_maxCut )
  {
    debug() << "\nVect Enetered loop with entries =" << vect_z_r0.size() << endmsg;

    std::sort( vect_z_r0.begin(), vect_z_r0.end() );

    std::vector<double>::iterator it_z_r0     = vect_z_r0.begin();
    std::vector<double>::iterator it_z_r0_end = vect_z_r0.end(); // This is *AFTER* the last element !!!
    
    while( it_z_r0 != (it_z_r0_end - m_maxCut) )
    { 
      double firstEl = *it_z_r0;
      double lastEl  = *(it_z_r0 + m_maxCut);
      
      // at every step check if the distance between the first and the (first+m_binWidth)^th
      // track is < m_binWidth; if Yes --> trigger
      // !!!TO-DO: modify m_binWidth, depending on the z-position!!!
      
      if ((lastEl - firstEl) < m_binWidth)
      {
        // Trigger!
	trigDecision = true;
        debug() << "Vect Triggered Event \n\t start = " << firstEl << "\n\t end = " << lastEl << endmsg;
    
	//Fill the candidates container with the tracks with good  z_r0        
        for ( LHCb::Tracks::const_iterator itT = BGtracks->begin(); BGtracks->end() != itT ; ++itT )
	{  
          z = (*itT)->firstState().z();
          r = (*itT)->firstState().x();
          t = (*itT)->firstState().tx();
          z_r0 = z - r/t;
          if( std::fabs(z_r0 > (firstEl-0.1) && z_r0 < (lastEl+0.1)) ) 
          {  
            m_trackSelection->push_back( *itT );         
          }
        }
	
	debug() << "Number of Objects in the outputSelection = " << m_trackSelection->size() << endmsg;

	if( firstEl > 0 ) m_trigEventsZpositive += 1;
	else m_trigEventsZnegative += 1;	

	break; // the while loop
      }
                      
      ++it_z_r0;
    }   
  }  
           
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




