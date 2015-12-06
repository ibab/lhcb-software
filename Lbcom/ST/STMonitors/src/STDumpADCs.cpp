// Include files 

// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"


// LHCbKernel
#include "Kernel/STDAQDefinitions.h"
#include "Kernel/STBoardMapping.h"
#include "Kernel/LHCbConstants.h"
#include "Kernel/ISTReadoutTool.h"

// STTELL1Event
#include "Event/STTELL1Data.h"

// local
#include "STDumpADCs.h"

//-----------------------------------------------------------------------------
// Implementation file for class : STDumpADCs
//
// 2006-06-12 : Mark TOBIN
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
namespace ST {
  DECLARE_ALGORITHM_FACTORY( STDumpADCs )
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ST::STDumpADCs::STDumpADCs( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : ST::TupleAlgBase ( name , pSvcLocator )
{
  declareSTConfigProperty("InputLocation" , m_dataLocation, LHCb::STTELL1DataLocation::TTFull);
}
//=============================================================================
// Destructor
//=============================================================================
ST::STDumpADCs::~STDumpADCs() {
} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode ST::STDumpADCs::initialize() {
  StatusCode sc = ST::TupleAlgBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiTupleAlg
  debug() << "==> Initialize" << endmsg;
  debug() << "Reading ADC values from " << m_dataLocation << endmsg;
  for(unsigned int chan=0; chan < STDAQ::nStripsPerBoard; chan++) {
    m_channelNumbers.push_back(chan);
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ST::STDumpADCs::execute() {
  // Skip if there is no Tell1 data
  counter("Number of events") += 1; 
  
  const LHCb::STTELL1Datas* data = getIfExists<LHCb::STTELL1Datas>(m_dataLocation);
  if ( NULL != data ) {
    Tuple tuple = nTuple( "ADCs", "ADC values from TELL1s" );
    // Get the data
    // loop over the data
    LHCb::STTELL1Datas::const_iterator iterBoard = data->begin(); 
    for (; iterBoard != data->end() ; ++iterBoard){

      // get the tell board and the data headers
      int sourceID = (*iterBoard)->TELL1ID();
      std::string idh = "Mean ADC for sourceID" + std::to_string(sourceID);
      const LHCb::STTELL1Data::Data& dataValues = (*iterBoard)->data();

      std::vector<double> ADCValues(3072,0.);
      // Loop over the PPs that have sent data
      std::vector<unsigned int> sentPPs = (*iterBoard)->sentPPs();
      std::vector<unsigned int>::const_iterator iPP = sentPPs.begin();
      for( ; iPP != sentPPs.end(); ++iPP ) {
        unsigned int pp = *iPP;
        // Loop over the links (i.e. Beetles)
        unsigned int iBeetle = 0;
        for ( ; iBeetle < STDAQ::nBeetlesPerPPx; ++iBeetle ){
          unsigned int beetle = iBeetle + pp*STDAQ::nBeetlesPerPPx;
          // Loop over the strips in this link
          unsigned int iStrip = 0;
          for ( ; iStrip < LHCbConstants::nStripsInBeetle ; ++iStrip){
            // Get the ADC value
            const int value = dataValues[beetle][iStrip];
	    
            // Calculate the pedestal and the pedestal squared
            int strip = iStrip + beetle * LHCbConstants::nStripsInBeetle;
            ADCValues[strip] = value;
            profile1D(strip, value, idh ,-0.5 ,STDAQ::nStripsPerBoard-0.5, STDAQ::nStripsPerBoard);
          } // strip
        }  // beetle
      } // Loop on PPs
      std::string tupleName = "TELL" + std::to_string(readoutTool()->SourceIDToTELLNumber(sourceID));
      tuple->farray(tupleName, ADCValues, "channel", 3072 );
      
    } // Loop on boards
    tuple->farray("channels", m_channelNumbers, "channel", 3072 );
    tuple->write();

  } // end of getIfExist condition
  //debug() << "Found " << data->size() << " boards." << endmsg;
  return StatusCode::SUCCESS;
}
