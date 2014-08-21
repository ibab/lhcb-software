// $Id: OTDoubleHitStudy.cpp,v 1.1.1.1 2009-04-21 10:57:19 jblouw Exp $
// Include files


// from Gaudi
#include <GaudiAlg/GaudiTupleAlg.h>
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ToolHandle.h"

// Event
#include "OTDAQ/IOTRawBankDecoder.h"
#include "OTDAQ/IndexedModuleDataHolder.h"
#include <boost/assign/list_of.hpp>
#include "Event/RawEvent.h"

class OTDoubleHitStudy : public GaudiTupleAlg
{
public: 
  OTDoubleHitStudy(const std::string& name, ISvcLocator* pSvcLocator); 
  virtual ~OTDoubleHitStudy( ); ///< Destructor
  StatusCode execute();
  StatusCode initialize();
  StatusCode finalize();
private:
  ToolHandle<IOTRawBankDecoder> m_decoder ;
};

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( OTDoubleHitStudy )

//=============================================================================
OTDoubleHitStudy::OTDoubleHitStudy(const std::string& name,
                             ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator ),
    m_decoder("OTRawBankDecoder")
{
}

//=============================================================================
OTDoubleHitStudy::~OTDoubleHitStudy()
{
}

//=============================================================================
// Initialize
//=============================================================================

StatusCode OTDoubleHitStudy::initialize()
{
  StatusCode sc = GaudiTupleAlg::initialize() ;
  m_decoder.retrieve().ignore() ;
  return sc ;
}

//=============================================================================
// Finalize
//=============================================================================

StatusCode OTDoubleHitStudy::finalize()
{
  m_decoder.release().ignore() ;
  return GaudiTupleAlg::finalize() ;
}

//=============================================================================
// Main execution
//=============================================================================

struct RecurringHit
{
  RecurringHit(size_t s=0) : straw(s) { tdc[0] = tdc[1] = tdc[2] = 0 ; }
  size_t straw ;
  size_t tdc[3] ;
  bool operator==(size_t s) { return straw==s ; }
} ;

StatusCode OTDoubleHitStudy::execute()
{
  StatusCode sc = StatusCode::SUCCESS ;
  
  std::string m_rawEventLocations[3] ;
  m_rawEventLocations[0] = "Prev1/DAQ/RawEvent" ;
  m_rawEventLocations[1] = LHCb::RawEventLocation::Default ;
  m_rawEventLocations[2] = "Next1/DAQ/RawEvent" ;
  
  // create a structure that holds hits per module per BX
  typedef std::vector< RecurringHit > RecurringHits ;
  typedef OTDAQ::IndexedModuleDataHolder<RecurringHits> HitsPerModule ;
  HitsPerModule hits ;
  
  // now fill it
  for(size_t ilocation = 0; ilocation<3; ++ilocation) {
    const std::string& location = m_rawEventLocations[ilocation] ;
    const LHCb::RawEvent* event = get<LHCb::RawEvent>(location);
    sc = m_decoder->decodeGolHeaders( *event ) ;
    if( sc.isSuccess() ) {
      for( HitsPerModule::mapiterator imod = hits.begin() ; imod != hits.end(); ++imod) {
	// get the hits
	LHCb::OTLiteTimeRange thesemodulehits = m_decoder->decodeModule(imod.channel()) ;
	RecurringHits newhits ;
	newhits.reserve( thesemodulehits.size() ) ;
	for( LHCb::OTLiteTimeRange::const_iterator ihit = thesemodulehits.begin() ;
	     ihit != thesemodulehits.end(); ++ihit) {
	  // does it exist?
	  RecurringHits::iterator it = std::find(imod->begin(),imod->end(),ihit->channel().straw()) ;
	  if( it != imod->end() ) {
	    it->tdc[ilocation] = ihit->channel().tdcTime() ;
	  } else {
	    RecurringHit ahit( ihit->channel().straw()) ;
	    ahit.tdc[ilocation] = ihit->channel().tdcTime() ;
	    newhits.push_back( ahit ) ;
	  }
	}
	imod->insert(imod->end(),newhits.begin(),newhits.end()) ;
      }
    }
  }
  
  // finally create the ntuple
  size_t unit = 0 ;
  Tuple ntp = nTuple(unit, "OTHits" ,CLID_ColumnWiseTuple);
  
  for( HitsPerModule::mapiterator imod = hits.begin() ; imod != hits.end(); ++imod)
    for( RecurringHits::const_iterator ihit = imod->begin() ; ihit != imod->end(); ++ihit) {
      ntp->column("station",imod.station()) ;
      ntp->column("layer",imod.layer()) ;
      ntp->column("quadrant",imod.quadrant()) ;
      ntp->column("module",imod.module()) ;
      ntp->column("straw",ihit->straw) ;
      ntp->column("tdc0",ihit->tdc[0]) ;
      ntp->column("tdc1",ihit->tdc[1]) ;
      ntp->column("tdc2",ihit->tdc[2]) ;
      ntp->write();
    }
  return sc;
}

