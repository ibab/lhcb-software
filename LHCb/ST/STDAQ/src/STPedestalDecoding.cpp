// Event
//#include "Event/RawBank.h"
#include "Event/RawEvent.h"
#include "Event/STTELL1Data.h"

// Kernel
#include "Kernel/STRawBankMap.h"
#include "Kernel/ISTReadoutTool.h"

// standard
#include "boost/lexical_cast.hpp"

// local
#include "STPedestalDecoding.h"

using namespace LHCb;
using namespace STDAQ;

//-----------------------------------------------------------------------------
// Implementation file for class : STPedestalDecoding
//
// 2007-09-11: Mathias Knecht, Jeroen van Tilburg
//-----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( STPedestalDecoding )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
STPedestalDecoding::STPedestalDecoding( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : ST::AlgBase ( name , pSvcLocator )
{
  declareSTConfigProperty("InputLocation", m_inputLocation , 
                          RawEventLocation::Default );
  declareSTConfigProperty("OutputLocation", m_outputLocation,
                          STTELL1DataLocation::TTPedestal );
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode STPedestalDecoding::initialize() 
{
  StatusCode sc = ST::AlgBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  // bank type
  m_bankType = STRawBankMap::stringToType(detType()+"Pedestal");

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode STPedestalDecoding::execute()
{
  RawEvent* raw = get<RawEvent>( m_inputLocation );

  // make container of TELL1 boards
  STTELL1Datas* outputPedestals = new STTELL1Datas();
  
  // put the object on the transient event store
  put(outputPedestals, m_outputLocation);
  
  // Pick up pedestal bank 
  const std::vector<RawBank*>& itf = raw->banks(RawBank::BankType(m_bankType));
  if (msgLevel(MSG::DEBUG)) {
    debug() << "Starting to decode " << itf.size() << detType() 
            << "Pedestal bank(s)" <<  endmsg;
  }
  
  std::vector<RawBank*>::const_iterator itB = itf.begin();
  for( ; itB != itf.end(); ++itB ) {
    
    const LHCb::RawBank* p = *itB;
    if (msgLevel(MSG::DEBUG)) {
      debug() << "Decoding bank of type " << detType() << "Pedestal (TELL1 ID: "
              << p->sourceID() << ", Size: " << p->size() << " bytes)"<< endmsg;
    }
    
    // Check if the board is valid
    STTell1ID tell1ID = STTell1ID((unsigned int)(p->sourceID()), detType()=="UT"); 
    const STTell1Board* aBoard =  readoutTool()->findByBoardID(tell1ID);
    if( !aBoard ) {
      std::string invalidSource = "Invalid source ID --> skip bank"+
        boost::lexical_cast<std::string>(p->sourceID());  
      Warning(invalidSource,StatusCode::SUCCESS,2).ignore(); 
      ++counter("skipped Banks");
      continue;
    }

    // Create an empty tell1 pedestal object  
    STTELL1Data::Data pedestals;
    pedestals.resize(noptlinks);
    STTELL1Data::Data::iterator i = pedestals.begin();
    for(; i!= pedestals.end(); ++i ) {
      i->resize(nports*nstrips, 0);
    }

    if( (unsigned int)p->size() != sizebankPedestal ){
      error() << "Wrong bank size for this type!! You should have "
              << sizebankPedestal << " bytes" << endmsg;
    }
      	
    // Counters
    unsigned int cntWD = 0; // Word counter, resets for each PP. Range 0 to 191.
    unsigned int cntPP = 0; // PP-FPGA counter, goes from 0 to 3.    
   
    // Now loop over all WORDS in a bank
    const unsigned int* w=p->begin<unsigned int>(); 
    for( ; w != p->end<unsigned int>(); ++w ) {
 			
      if(cntWD == 192){	// Each 192 words we have a new PP-FPGA
        cntWD = 0;        
        ++cntPP;     
      }

      if (cntWD==0&&msgLevel(MSG::DEBUG)) {	
        debug() << "#######  Parsing now data from PP " << cntPP 
                << " ##################" << endmsg; 
      }
      
      // Unpack the 32-bit word into 8-bit chunks
      unsigned int p1 = ( *w & mask1);
      unsigned int p2 = ((*w & mask2)/0x100);
      unsigned int p3=  ((*w & mask3)/0x10000);
      unsigned int p4 = ((*w & mask4)/0x1000000);

      int iPort = cntWD/(nbeetles*nstrips); // range 0 to 1
      int iWord = (cntWD%(nbeetles*nstrips))/nbeetles; // range: 0 to 32
      int iBeetle = 2*(cntWD%nbeetles) + nBeetlesPerPPx*cntPP; // range: 0 to 22
      
      pedestals[iBeetle][iWord+nstrips*iPort] = p1;
      pedestals[iBeetle][iWord+nstrips*(iPort+2)] = p2;
      pedestals[iBeetle+1][iWord+nstrips*iPort] = p3;
      pedestals[iBeetle+1][iWord+nstrips*(iPort+2)] = p4;
			
      ++cntWD;
    } // Loop over all words

    // make an empty tell1 data object
    STTELL1Data* myPedestals = new STTELL1Data(pedestals);   
     
    // put into the container, second argument is Tell1 id
    outputPedestals->insert(myPedestals, int(p->sourceID()));
      
  }// end of loop over banks of a certain type
  
 return StatusCode::SUCCESS;
} 
