// $Id: RawEventDump.cpp,v 1.1 2005-10-14 12:48:50 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "Event/RawEvent.h"
#include "RawEventDump.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : RawEventDump
//
// 2005-10-13 : Markus Frank
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RawEventDump>          s_factory ;
const        IAlgFactory& RawEventDumpFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RawEventDump::RawEventDump( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "DumpData", m_dump = false );
}
//=============================================================================
// Destructor
//=============================================================================
RawEventDump::~RawEventDump() {}; 

//=============================================================================
// Main execution
//=============================================================================
StatusCode RawEventDump::execute() {

  RawEvent* raw = get<RawEvent>(RawEventLocation::Default);

  for(int j=0; j<256; ++j)  {
    RawBank::BankType i = RawBank::BankType(j);
    const std::vector<RawBank*>& b = raw->banks(i);
    if ( b.size() > 0 )  {
      info() << b.size() << " banks of type " << i << ": [size, source, version, magic]";
      std::vector<RawBank*>::const_iterator itB;
      int k = 0;
      for( itB = b.begin(); itB != b.end(); itB++ ) {
        if ( ((k++)%4) == 0 ) info() << endmsg << "  " ;
        const RawBank* r = *itB;
        info() << "[" << int(r->size()) << ", "
               << int(r->sourceID()) << ", "
               << int(r->version()) << ", "
               << std::hex << r->magic() << "] ";
        if( m_dump ) {
          info() << "Data follows...";
          int cnt = 0;
          for(const int* p=r->begin<int>(); p != r->end<int>(); ++p)  {
            if ( ((cnt++)%10) == 0 ) info() << endmsg << "   ... " ;
            info() << "[" << *p << "] ";
          }
          info() << std::dec << endmsg << "  ";
        }
      }
      info() << endmsg;
    }
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
