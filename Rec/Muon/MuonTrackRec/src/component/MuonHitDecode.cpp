// Include files 

// from Gaudi
#include "GaudiKernel/IIncidentSvc.h"

// local
#include "MuonHitDecode.h"
#include "MuonDet/DeMuonDetector.h"
#include "MuonDAQ/IMuonRawBuffer.h"
#include "Kernel/MuonTileID.h"
#include "MuonInterfaces/MuonLogHit.h"


/** @class MuonHitDecode MuonHitDecode.h
 *
 *  interface standard decoding tool (MuonRawBuffer) to MuonLogHit objects
 *
 *  @author Giacomo Graziani
 *  @date   2009-03-16
 *
 */

using namespace LHCb;

DECLARE_TOOL_FACTORY( MuonHitDecode )


  MuonHitDecode::MuonHitDecode( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
    : GaudiTool ( type, name , parent ), m_hitsDecoded(false)
{
  declareInterface<IMuonHitDecode>(this);
  declareProperty( "NumberRawLocations", m_TAENum = 1 );
  declareProperty( "SkipHWNumber", m_skipHWNumber = false );
}

MuonHitDecode::~MuonHitDecode() {
  clearHits();
}

void MuonHitDecode::handle ( const Incident& incident )
{
  if ( IncidentType::EndEvent == incident.type() ) {
    if ( msgLevel(MSG::DEBUG) ) debug() << "End Event: clear hits"<<endmsg;
    clearHits() ;
    m_hitsDecoded = false;
  }
}

void MuonHitDecode::clearHits()
{
  m_tilesAndTDC.clear();
  std::vector<MuonLogHit*>::iterator ih;
  for (ih=m_hits.begin() ; ih != m_hits.end(); ih++) {
    delete (*ih);
  }
  m_hits.clear();
}

StatusCode MuonHitDecode::initialize() {
  StatusCode sc = GaudiTool::initialize() ;
  if (!sc) return sc;

  m_muonDetector = getDet<DeMuonDetector>(DeMuonLocation::Default);
  if(!m_muonDetector){
    err()<<"error retrieving the Muon detector element "<<endmsg;
    return StatusCode::FAILURE;
  }

  m_recTool = tool<IMuonRawBuffer>("MuonRawBuffer");
  if(!m_recTool){
    error()<<"error retrieving the muon raw buffer decoding tool "<<endmsg;
  }
  m_tilesAndTDC.clear(); m_tilesAndTDC.reserve(5000);
  m_hits.clear(); m_hits.reserve(5000);

  incSvc()->addListener( this, IncidentType::EndEvent );

  return StatusCode::SUCCESS;
}

StatusCode MuonHitDecode::decodeRawData() 
{
  StatusCode sc;

  typedef std::vector<std::pair<LHCb::MuonTileID,unsigned int> > TileAndTDCMap;
  TileAndTDCMap tileAndTDC;

  MuonLogHit * newhit = NULL;
  // run MuonRawBuffer on all avilable BXs

  bool validLocation = false;

  // -- Loop over locations. m_TAEN is the number of locations, so the counter has to be smaller by one.
  for ( int i = -m_TAENum+1; i<=m_TAENum-1; ++i )
  {

    if ( msgLevel(MSG::DEBUG) )
      debug()<<"Looking at BX: "<<i<<" "<<locBX(i)<<endmsg;

    // -- Check raw locations and load raw event. Second option is for backward compatibility
    LHCb::RawEvent* raw = getIfExists<LHCb::RawEvent>(evtSvc(),locBX(i)+LHCb::RawEventLocation::Muon);
    if ( raw )
    {
      validLocation = true;
    }
    else
    {
      raw = getIfExists<LHCb::RawEvent>(evtSvc(),locBX(i)+LHCb::RawEventLocation::Default);
      if ( raw ) { validLocation = true; }
    }
  
    if (!raw) continue;

    sc = m_recTool->getTileAndTDC(raw,tileAndTDC,locBX(i));
    if (!sc) return sc;

    for( TileAndTDCMap::iterator it = tileAndTDC.begin(); it != tileAndTDC.end(); ++it ) 
    {
      // set the time taking into account the BX. must be positive so I need to add an offset...
      unsigned int tprim = (*it).second;
      (*it).second += (7+i)*16;
      if ( msgLevel(MSG::VERBOSE) ) verbose()<<"time conversion: before "<<tprim
                                             <<" after"<<(*it).second<<endmsg;
    }

    m_tilesAndTDC.insert(m_tilesAndTDC.end(), tileAndTDC.begin(), tileAndTDC.end());

    if ( msgLevel(MSG::DEBUG) )
      debug()<<"Size of tileAndTDC tmp container in bunch " << i
             << " is: "<<tileAndTDC.size()<<endmsg;

    tileAndTDC.clear();
    m_recTool->forceReset();
  }

  if (!validLocation) error() << "No valid raw data found in: " << LHCb::RawEventLocation::Muon
                              << " and " << LHCb::RawEventLocation::Default
                              << " and corresponding Prev/Next locations" << endmsg;

  if ( msgLevel(MSG::DEBUG) )
    debug()<<"Size of tilesAndTDC container is: "<<m_tilesAndTDC.size()<<endmsg;

  // create list of MuonLogHit objects
  for ( TileAndTDCMap::iterator it = m_tilesAndTDC.begin(); it != m_tilesAndTDC.end(); ++it )
  {
    newhit = new MuonLogHit( &((*it).first) );
    long L1Number=0,link_number=0,ODE_number=0,ode_ch=0;
    if(!m_skipHWNumber)  m_muonDetector->getDAQInfo()->findHWNumber ( (*it).first,
                                                                      L1Number,link_number,ODE_number,ode_ch);
    unsigned int on=ODE_number, oc=ode_ch;
    newhit->setOdeNumber( on );
    newhit->setOdeChannel( oc );
    short int OdeIndex=1;
    newhit->setOdeIndex(OdeIndex ); // to be implemented
    int time=(*it).second - 7*16; // remove the positive-forcing offset
    newhit->setTime( time );

    m_hits.push_back(newhit);
  }

  if ( msgLevel(MSG::DEBUG) )
    debug()<<"Size of MuonLogHit container is: "<<m_hits.size()<<endmsg;

  m_hitsDecoded = true;
  return sc;
}


