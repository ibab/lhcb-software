// Include files 

// from ST
#include "Event/STTELL1Data.h"
#include "Kernel/STDAQDefinitions.h"

// local
#include "STFullEventDump.h"

using namespace LHCb;
using namespace STDAQ;
using namespace std;

//-----------------------------------------------------------------------------
// Implementation file for class : STFullEventDump
//
// 2007-09-11: Mathias Knecht
//-----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( STFullEventDump )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
STFullEventDump::STFullEventDump( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : ST::AlgBase ( name , pSvcLocator )
{ 
  // By default all links are dumped
  for( unsigned int i=0; i < noptlinks; ++i ) m_linkList.push_back(i);
  declareProperty("TELL1List", m_TELL1List );  
  declareProperty("LinkList",  m_linkList  );  
  declareSTConfigProperty("InputLocation", m_inputLocation, 
                          STTELL1DataLocation::TTFull );
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode STFullEventDump::execute()
{
  // retrieve the decoded full daTA    
  STTELL1Datas* input = get< STTELL1Datas>(m_inputLocation);
  
  // Loop over the tell1's
  vector<unsigned int>::iterator s = m_TELL1List.begin();
  for( ; s != m_TELL1List.end(); ++s){ 

    STTELL1Data* thisBoard = input->object(*s);
    if( !thisBoard ) { 
      warning() << "Tell1 " << *s << " board not found" << endmsg;
      continue;
    }

    // Get the header and the data
    const STTELL1Data::Data& header = thisBoard->header();
    const STTELL1Data::Data& data = thisBoard->data();

    // Loop over the optical links
    vector<unsigned int>::iterator iLink = m_linkList.begin();
    for( ; iLink != m_linkList.end(); ++iLink){
      unsigned int i = *iLink;
      
      // Information about tell1 and optical link
      info() << endmsg;
      info() << "TELL1 " << *s << ", optical link " << i  << endmsg;

      // Header part
      info() << setfill('#') << setw(29) << " HEADERS " << setw(20) << "#"  
             << endmsg;
      info() << "Port 1:    |Port 2:    |Port 3:    |Port 4:    |#" << endmsg;
      info() << "-----------|-----------|-----------|-----------|#" << endmsg;
      info() << setfill(' ');
      for( unsigned int iHead = 0 ; iHead < nports*nheaders ; ++iHead ) { 
        info() << setw(3) << header[i][iHead] ;
        if( iHead%3 == 2 ) info() << "|";
        else info() << " ";  
      }
      info() << "#" << endmsg;

      // Data part
      info() << setfill('#') << setw(29) << " DATA ##" <<  setw(105) << "#" 
             << endmsg;
      info() << "Port|    Channel:" <<  setfill(' ') << setw(117) << "#" 
             << endmsg;
      info() << "    |" ;
      for ( unsigned iChan = 0; iChan < nstrips; ++iChan ) {
        info() << setw(3) << iChan << " ";
      }
      info() << "#" << endmsg;
      info() << "    |" << setfill('-') << setw(129) << "#" << endmsg;
      for( unsigned int iPort=0; iPort < nports; ++iPort){
        info() << setfill(' ') << setw(4) << iPort  << "|";
        for ( unsigned iChan = 0; iChan < nstrips; ++iChan ) {
          info() << setw(3) << data[i][iPort*nports+iChan] << " ";
        }
        info() << "#" << endmsg;
      }
      info() << setfill('#') << setw(134) << "#" << endmsg;

    } // boards to printout
  } // links    
  
  return StatusCode::SUCCESS;
}
