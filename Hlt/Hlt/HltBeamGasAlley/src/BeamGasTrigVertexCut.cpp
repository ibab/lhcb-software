
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
BeamGasTrigVertexCut::BeamGasTrigVertexCut(const std::string& name, ISvcLocator* pSvcLocator)
    : HltAlgorithm(name, pSvcLocator, false)
    , m_trigEventsZnegative(0)
    , m_trigEventsZpositive(0)	
    , m_trackSelection(*this)
{
  m_trackSelection.declareProperties();
  declareProperty( "HistoBinWidth", m_binWidth = 14);
  declareProperty( "MaxBinValueCut", m_maxCut = 4);
  declareProperty( "ZExclusionRangeLow", m_zExclusionRangeLow = 0.0);
  declareProperty( "ZExclusionRangeUp",  m_zExclusionRangeUp  = 0.0);
  declareProperty( "HistoZRangeLow", m_histoZMin = -2000.0);
  declareProperty( "HistoZRangeUp",  m_histoZMax =  2000.0);
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
  m_trackSelection.retrieveSelections();
  m_trackSelection.registerSelection();
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm 

  verbose() << "==> Initialize" << endmsg;
  
  debug() << "========== Algorithm parameters ======"            << endreq
         << "HistoZRangeLow       = " << m_histoZMin << " mm"   << endreq
         << "HistoZRangeUp        = " << m_histoZMax << " mm"   << endreq
	 << "HistoBinWidth        = " << m_binWidth  << " mm"	<< endreq
	 << "MaxBinValueCut       = " << m_maxCut		<< endreq
	 << "ZExclusionRangeLow   = " << m_zExclusionRangeLow   << " mm"    << endreq
	 << "ZExclusionRangeUp    = " << m_zExclusionRangeUp    << " mm"    << endreq;
	 
  
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode BeamGasTrigVertexCut::execute() {

  std::vector<double> vect_z_r0;
  vect_z_r0.reserve(200);
  
  debug() << "==> Execute" << endmsg;
  
  const Hlt::TSelection<LHCb::Track>* BGtracks = m_trackSelection.input<1>();
  debug() << "Number of tracks in the BG Tracks Container = " << BGtracks->size() << endmsg;
    
  
  // Fill the vector with the tracks z position at r=0
  for ( Hlt::TSelection<LHCb::Track>::const_iterator itT = BGtracks->begin(); BGtracks->end() != itT ; ++itT ) 
  {  
    double z = (*itT)->firstState().z();
    double r = (*itT)->firstState().x();
    double t = (*itT)->firstState().tx();
    double z_r0 = z - r/t;
        
    if ( (z_r0 > m_histoZMin) && (z_r0 < m_histoZMax  ) )
    { 
      if ( (z_r0 < m_zExclusionRangeLow) || (z_r0 > m_zExclusionRangeUp) )
        vect_z_r0.push_back( z_r0 );    
    }  
  }
  
  // If we have enough entries, search for a peak 
  if( vect_z_r0.size() < m_maxCut ) return StatusCode::SUCCESS;

  debug() << "\nVect Entered loop with entries =" << vect_z_r0.size() << endmsg;

  std::sort( vect_z_r0.begin(), vect_z_r0.end() );

  std::vector<double>::iterator it_z_r0     = vect_z_r0.begin();
  std::vector<double>::iterator it_z_r0_end = vect_z_r0.end(); // This is *AFTER* the last element !!!
  
  while( it_z_r0 != (it_z_r0_end - m_maxCut + 1) ) { 
    double firstEl = *it_z_r0;
    double lastEl  = *(it_z_r0 + m_maxCut - 1);

    // at every step check if the distance between the first and the 
    // last track is < m_binWidth; if Yes --> trigger
    // The histo bin-width is variable: at z=0 it is = m_binWidth, and
    // increases to 2*m_binWidth at z=2000 mm 
    double meanOf2Els = (firstEl + lastEl)/2;
    double effectiveBinWidth = m_binWidth*( 1 + std::abs(meanOf2Els)/2000. );

    if ( (lastEl - firstEl) < effectiveBinWidth ) {
      // Trigger!
      debug() << "Vect Triggered Event \n\t start = " << firstEl << "\n\t end = " << lastEl << endmsg;
  
      //Fill the candidates container with the tracks with good  z_r0        
      for ( Hlt::TSelection<LHCb::Track>::const_iterator itT = BGtracks->begin(); BGtracks->end() != itT ; ++itT ) {  
        double z = (*itT)->firstState().z();
        double r = (*itT)->firstState().x();
        double t = (*itT)->firstState().tx();
        double z_r0 = z - r/t;
        if( z_r0 > (firstEl-0.1) && z_r0 < (lastEl+0.1) ) {  
          m_trackSelection.output()->push_back( *itT );
        }
      }
  
      debug() << "Number of Objects in the outputSelection = " << m_trackSelection.output()->size() << endmsg;

      if( firstEl > 0 ) m_trigEventsZpositive += 1;
      else              m_trigEventsZnegative += 1;	

      break; // the while loop
    }

    ++it_z_r0;
  }   
   
           
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode BeamGasTrigVertexCut::finalize() {

  debug() << "Number of triggered Events with z<0 and z>0 : " 
            << m_trigEventsZnegative << "   " << m_trigEventsZpositive << endmsg;

  return HltAlgorithm::finalize () ;

}




