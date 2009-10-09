// $Id: MuonPadRec.cpp,v 1.4 2009-10-09 09:38:43 ggiacomo Exp $
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
  : GaudiTool ( type, name , parent ), m_TileVeto(0),
     m_padsReconstructed(false)
{
  declareInterface<IMuonPadRec>(this);
  declareProperty( "TileVeto", m_TileVeto);
  declareProperty( "FirstComeFirstServed", m_getfirsthit = false);
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

  std::vector<long int>::iterator iV;
  for (iV=m_TileVeto.begin() ; iV!=m_TileVeto.end(); iV++) {
    m_TileIsVetoed[*iV]=true;
  }

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
      int station;
      for( station = 0 ; station < m_muonDetector->stations() ; station++ ){
        int region;
        for( region = 0 ; region < m_muonDetector->regions() ; region++ ){
          
          // get mapping of input to output from region
          // in fact we are reversing the conversion done in the digitisation
          int NLogicalMap = m_muonDetector->getLogMapInRegion(station,region);     
          verbose()<<" station and region "<<station<<" "<<region<<" maps "<<NLogicalMap<<endreq;
          
          if(1 == NLogicalMap){
            // straight copy of the input + making SmartRefs to the MuonDigits
            StatusCode sc = addCoordsNoMap(hits,station,region);
            if(!sc.isSuccess()){
              error()
                << "Failed to map digits to coords in a one to one manner"
                << endreq;
              return sc;
            }
          }else{
            // need to cross the input strips to get output strips
            StatusCode sc = 
              addCoordsCrossingMap(hits,station,region);
            if(!sc.isSuccess()){error()
                << "Failed to map digits to coords by crossing strips"
                << endreq;
              return sc;
            }
          }
        }    
      }
    }
    m_padsReconstructed = true;
  }
  return StatusCode::SUCCESS;
};


//=============================================================================

// Adding entries to coords 1 to 1 from digits, need to make the references
StatusCode MuonPadRec::addCoordsNoMap(std::vector<MuonLogHit*> &hits,
                                       const int & station,
                                       const int & region){
  std::vector<MuonLogHit*>::iterator iD;
  for( iD = hits.begin() ; iD != hits.end() ; iD++ ){
    if( ((*iD)->tile())->station() == static_cast<unsigned int>(station) &&
        ((*iD)->tile())->region() == static_cast<unsigned int>(region) ){
      // make the coordinate to be added to coords
      debug()<<" LOGPAD OK nomap ODE "<< (*iD)->odeName() <<" ch "<< (*iD)->odeChannel()<<
        " tile "<<*((*iD)->tile())<< " time)="<< (*iD)->time() << endreq;
      
      if(!m_TileIsVetoed[(long int) (*iD)->tile()]) {
        MuonLogPad *current = new MuonLogPad(*iD);
        current->settruepad();
        m_pads.push_back(current);
      }
      else {
        debug() << "applied veto on pad "<<((long int) (*iD)->tile()) << endmsg;
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode MuonPadRec::addCoordsCrossingMap(std::vector<MuonLogHit*> &hits,
                                             const int & station,
                                             const int & region){

  // get local MuonLayouts for strips
  MuonLayout layoutOne,layoutTwo;
  StatusCode sc = makeStripLayouts(station,region, layoutOne,layoutTwo);
  if(!sc.isSuccess()){
    return sc;
  }
  
  // seperate the two types of logical channel, flag if used with the pair
  std::vector<std::pair<MuonLogHit*,bool> > typeOnes;
  std::vector<std::pair<MuonLogHit*,bool> > typeTwos;
  std::vector<MuonLogHit*>::iterator it;
  for( it = hits.begin() ; it != hits.end() ; it++ ){
    if( ((*it)->tile())->station() == static_cast<unsigned int>(station) &&
        ((*it)->tile())->region() == static_cast<unsigned int>(region) ){
      if( ((*it)->tile())->layout() == layoutOne ){
        typeOnes.push_back(std::pair<MuonLogHit*,bool>((*it),false));
      }else if( ((*it)->tile())->layout() == layoutTwo ){
        typeTwos.push_back(std::pair<MuonLogHit*,bool>((*it),false));
      } else {
        error()
            << "MuonDigits in list are not compatible with expected shapes"
            << endreq;
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
      if(pad.isValid()) {
        if (!m_TileIsVetoed[(long int) pad]) { 
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
        } 
        else {
          debug() << "applied veto on pad "<<((long int) pad) << endmsg;
        }
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



StatusCode MuonPadRec::makeStripLayouts(int station, int region, 
                                     MuonLayout &layout1,
                                     MuonLayout &layout2){  
  unsigned int x1 = m_muonDetector->getLayoutX(0,station,region);
  unsigned int y1 = m_muonDetector->getLayoutY(0,station,region);
  unsigned int x2 = m_muonDetector->getLayoutX(1,station,region);
  unsigned int y2 = m_muonDetector->getLayoutY(1,station,region);
  MuonLayout layoutOne(x1,y1);
  MuonLayout layoutTwo(x2,y2);
  layout1=layoutOne;
  layout2=layoutTwo;
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
        verbose() << "Found double hit in Tile "<< *((*ih1)->tile()) <<endmsg;
        bool chooseone =
          m_getfirsthit ? ( (*ih1)->time() < (*ih2)->time() ) // take the first one
          : ( fabs((*ih1)->time() - 7.5) < fabs((*ih2)->time() - 7.5));  // or take time closer to 0 , i.e. 7.5 bits
        if(chooseone) {
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
