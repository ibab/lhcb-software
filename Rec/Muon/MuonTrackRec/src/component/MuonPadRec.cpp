// $Id: MuonPadRec.cpp,v 1.2 2009-09-10 13:20:54 ggiacomo Exp $
#include <vector>

#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/IIncidentSvc.h" 
#include "Kernel/MuonTileID.h"
#include "MuonDet/DeMuonDetector.h"

// local
#include "MuonTrackRec/MuonLogHit.h"
#include "MuonTrackRec/MuonLogPad.h"
#include "MuonPadRec.h"
using namespace LHCb;
using namespace std;

//-----------------------------------------------------------------------------
// Implementation file for class : MuonPadRec
//
// 2008-01-25 : G.Graziani 
//              special reconstruction tool for Monitoring
//              code based on MuonRec by A.Satta et al.
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MuonPadRec );


MuonPadRec::MuonPadRec( const std::string& type,
                        const std::string& name,
                        const IInterface* parent )
  : GaudiTool ( type, name , parent ), 
     m_padsReconstructed(false)
{
  declareInterface<IMuonPadRec>(this);

  m_pads.clear(); m_pads.reserve(1000);
}

MuonPadRec::~MuonPadRec() {
  clearPads();
} 

void MuonPadRec::clearPads() 
{
  std::vector<MuonLogPad*>::iterator ih;
  for (ih=m_pads.begin() ; ih != m_pads.end(); ih++) {
    delete (*ih);
  }
  m_pads.clear();
}

void MuonPadRec::handle ( const Incident& incident )
{ 
  if ( IncidentType::EndEvent == incident.type() ) {
    debug() << "End Event: clear pads"<<endmsg;
    clearPads() ;
    m_padsReconstructed = false;
  }
}




//=============================================================================
StatusCode 	MuonPadRec::initialize ()
{
  StatusCode sc = GaudiTool::initialize() ;
  if (!sc) return sc;
  m_muonDetector = getDet<DeMuonDetector>(DeMuonLocation::Default);
  if(!m_muonDetector){
    err()<<"error retrieving the Muon detector element "<<endreq;
    return StatusCode::FAILURE;
  }  
  
  incSvc()->addListener( this, IncidentType::EndEvent );

  return StatusCode::SUCCESS ;
}


StatusCode 	MuonPadRec::finalize ()
{
  return   GaudiTool::finalize() ;
   
}

const std::vector<MuonLogPad*>* MuonPadRec::pads() { 
  if (! m_padsReconstructed) 
    warning() << "MuonPadRec::buildLogicalPads not called yet" << endmsg;

    return (const std::vector<MuonLogPad*>*) (&m_pads);
}


//---------------------------------------------------------------------------------------//
// private logical pad reconstruction, based on MuonRec algorithm from MuonDAQ package   //
//---------------------------------------------------------------------------------------//


StatusCode MuonPadRec::buildLogicalPads(const std::vector<MuonLogHit*> *myhits ) {
  std::vector<MuonLogHit*> hits;
  if((!m_padsReconstructed) && myhits) {
    std::vector<MuonLogHit*>::const_iterator ih;
    // save locally current hits 
    for (ih=myhits->begin() ; ih != myhits->end(); ih++) {
      hits.push_back(*ih);
    }
    debug() << "buildLogicalPads: raw hits are "<<hits.size()<<endmsg;
    removeDoubleHits(hits);
    debug() << "                  after cleaning doubles "<<hits.size()<<endmsg;
    if (!hits.empty()) {
      // regions without crossing: straight copy of the input + making SmartRefs to the MuonDigits
      StatusCode sc = addCoordsNoMap(hits);
      if(!sc.isSuccess()){
	error()
	  << "Failed to map digits to coords in a one to one manner"
	  << endreq;
	return sc;
      }

      sc = addCoordsCrossingMap(hits);
      if(!sc.isSuccess()){
	error()  << "Failed to map digits to coords by crossing strips"
		 << endreq;
	return sc;
      }      
    }
    m_padsReconstructed = true;
  }
  return StatusCode::SUCCESS;
};


//=============================================================================

// Adding entries to coords 1 to 1 from digits, need to make the references
StatusCode MuonPadRec::addCoordsNoMap(std::vector<MuonLogHit*> &hits){
  std::vector<MuonLogHit*>::iterator iD;
  for( iD = hits.begin() ; iD != hits.end() ; iD++ ){
    if( m_muonDetector->getLogMapInRegion
	( ((*iD)->tile())->station(), ((*iD)->tile())->region() ) == 1) {
      // make the coordinate to be added to coords
      debug()<<" LOGPAD OK nomap ODE "<< (*iD)->odeName() <<" ch "<< (*iD)->odeChannel()<<
        " tile "<<*((*iD)->tile())<< " time)="<< (*iD)->time() << endreq;
      
      MuonLogPad *current = new MuonLogPad(*iD);
      current->settruepad();
      m_pads.push_back(current);
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode MuonPadRec::addCoordsCrossingMap(std::vector<MuonLogHit*> &hits){

  // separate the two types of logical channel, flag if used with the pair
  std::vector<std::pair<MuonLogHit*,bool> > typeOnes;
  std::vector<std::pair<MuonLogHit*,bool> > typeTwos;
  std::vector<MuonLogHit*>::iterator it;
  for( it = hits.begin() ; it != hits.end() ; it++ ){
    if( m_muonDetector->getLogMapInRegion( ((*it)->tile())->station(), ((*it)->tile())->region() ) != 1) {
      if( ((*it)->tile())->layout().xGrid() == 
	  m_muonDetector->getLayoutX(0,((*it)->tile())->station(), ((*it)->tile())->region() ) ){
        typeOnes.push_back(std::pair<MuonLogHit*,bool>((*it),false));
      } else {
        typeTwos.push_back(std::pair<MuonLogHit*,bool>((*it),false));
      } 
    }
  }
  // now cross the two sets of channels
  // sorry about this std::pair stuff but it is the easiest way of matching
  // a bool to each digit
  std::vector<std::pair<MuonLogHit*,bool> >::iterator iOne;
  std::vector<std::pair<MuonLogHit*,bool> >::iterator iTwo;
  
  for( iOne = typeOnes.begin() ; iOne != typeOnes.end() ; iOne++ ){
    for( iTwo = typeTwos.begin() ; iTwo != typeTwos.end() ; iTwo++ ){
      // who said C++ did not make lovely transparent code?
      MuonTileID pad = (iOne->first->tile())->intercept(*(iTwo->first->tile()) );
      if(pad.isValid()){ 
        MuonLogPad *current = new MuonLogPad(iOne->first,iTwo->first);
        current->settruepad();
        debug() << " LOGPAD OK crossed ODE "<< iOne->first->odeName() 
                  <<" ch "<< iOne->first->odeChannel()<<" and "
                  << iTwo->first->odeChannel() << " tiles "
                  << *(iOne->first->tile()) << " and "
                  << *(iTwo->first->tile()) << " times="
                  << iOne->first->time()<< " and "
                  << iTwo->first->time()   <<      endreq;
        
        m_pads.push_back(current);
        // set flags to used on iOne and iTwo
        iOne->second = true;
        iTwo->second = true;
        
      }
    }
  }

  // now copy across directly all strips that have not yet been used
  for( iOne = typeOnes.begin() ; iOne != typeOnes.end() ; iOne++ ){
    if(!(iOne->second)){
      MuonLogPad *current = new MuonLogPad(iOne->first);
      m_pads.push_back(current);
    }
  }
  
  for( iTwo = typeTwos.begin() ; iTwo != typeTwos.end() ; iTwo++ ){
    if(!(iTwo->second)){
      MuonLogPad *current = new MuonLogPad(iTwo->first);
      m_pads.push_back(current);
    }
  } 
  return StatusCode::SUCCESS;
}



StatusCode MuonPadRec::removeDoubleHits(std::vector<MuonLogHit*> &hits) {
  std::vector<MuonLogHit*>::iterator ih1,ih2;
  int mytime;
  ih1= hits.begin();
  while (ih1 != hits.end()) {
    bool ok=true;
    for (ih2= hits.begin(); ih2 != ih1 ; ih2++) {
      if (*((*ih1)->tile()) == *((*ih2)->tile())) {
        // choose time closer to 0 , i.e. 7.5 bits
        verbose() << "Found double hit in Tile "<< *((*ih1)->tile()) <<endmsg;
        if (fabs((*ih1)->time() - 7.5) < fabs((*ih2)->time() - 7.5)) {
          verbose() << "  choose time "<<(*ih1)->time() <<" rather than "<<(*ih2)->time()<<endmsg;
          mytime=(*ih1)->rawtime();
          (*ih2)->setTime( mytime );
        }
        else {
          verbose() << "  choose time "<<(*ih2)->time() <<" rather than "<<(*ih1)->time()<<endmsg;
        }
        ih1 = hits.erase(ih1);
        ok = false;
        break;
      }
    }
    if(ok) ih1++;
  }
  return StatusCode::SUCCESS;
}
