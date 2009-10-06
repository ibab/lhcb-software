// $Id: MuonHitDecode.cpp,v 1.3 2009-10-06 17:16:22 ggiacomo Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/IIncidentSvc.h" 

// local
#include "MuonHitDecode.h"
#include "MuonDet/DeMuonDetector.h"
#include "MuonDAQ/IMuonRawBuffer.h"
#include "Kernel/MuonTileID.h"
#include "MuonTrackRec/MuonLogHit.h"

using namespace LHCb;

DECLARE_TOOL_FACTORY( MuonHitDecode );


MuonHitDecode::MuonHitDecode( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : GaudiTool ( type, name , parent ), m_hitsDecoded(false)
{
  declareInterface<IMuonHitDecode>(this);
}

MuonHitDecode::~MuonHitDecode() {
  clearHits();
} 

void MuonHitDecode::handle ( const Incident& incident )
{ 
  if ( IncidentType::EndEvent == incident.type() ) {
    debug() << "End Event: clear hits"<<endmsg;
    clearHits() ;
    m_hitsDecoded = false;
  }
}

void MuonHitDecode::clearHits() 
{
  m_tiles.clear();
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
    err()<<"error retrieving the Muon detector element "<<endreq;
    return StatusCode::FAILURE;
  }  

  m_recTool = tool<IMuonRawBuffer>("MuonRawBuffer");
  if(!m_recTool){
    error()<<"error retrieving the muon raw buffer decoding tool "<<endreq;
  }  
  m_tiles.clear(); m_tiles.reserve(5000);
  m_hits.clear(); m_hits.reserve(5000);

  incSvc()->addListener( this, IncidentType::EndEvent );
  return StatusCode::SUCCESS;
}

StatusCode MuonHitDecode::finalize () {
  return   GaudiTool::finalize() ;
}

StatusCode MuonHitDecode::decodeRawData() {
  StatusCode sc;
  std::vector<std::pair<LHCb::MuonTileID,unsigned int> >  tileAndTDC;
  std::vector<std::pair<LHCb::MuonTileID,unsigned int> >::iterator it;
  
  int TAENum=7;
  MuonLogHit* newhit=NULL;
  for(int i=-TAENum;i<=TAENum;i++){
    if (!exist<LHCb::RawEvent>(locBX(i) + LHCb::RawEventLocation::Default))
      continue;
    debug()<<"Looking at BX: "<<i<<" "<<locBX(i)<<endreq;

    // get tiles from the raw buffer
    LHCb::RawEvent* raw = get<LHCb::RawEvent>
      (locBX(i)+LHCb::RawEventLocation::Default);
    
    sc = m_recTool->getTileAndTDC(raw,tileAndTDC,locBX(i)); 
    if (!sc) return sc;

    for(it = tileAndTDC.begin(); it != tileAndTDC.end(); it++){

      // set the time taking into account the BX. must be unsigned...
      unsigned int tprim = (*it).second;
      (*it).second += (7+i)*16;
      
      verbose()<<"time conversion: before "<<tprim
               <<" after"<<(*it).second<<endmsg;
      m_tiles.push_back((*it).first);

      newhit = new MuonLogHit( &(m_tiles.back()) );
      long L1Number,link_number,ODE_number,ode_ch;
      m_muonDetector->getDAQInfo()->findHWNumber ( (*it).first, 
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
    debug()<<"Size of tileAndTDC container is: "<<tileAndTDC.size()<<endreq;
    debug()<<"Size of MuonLogHit container is: "<<m_hits.size()<<endreq;
    m_recTool->forceReset();
  }
  

  tileAndTDC.clear();
  m_hitsDecoded = true;
  return sc;
}


