#include <vector>

#include "GaudiKernel/IIncidentSvc.h" 
#include "Kernel/MuonTileID.h"
#include "MuonDet/DeMuonDetector.h"

// local
#include "MuonInterfaces/MuonLogHit.h"
#include "MuonInterfaces/MuonLogPad.h"
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
DECLARE_TOOL_FACTORY( MuonPadRec )


MuonPadRec::MuonPadRec( const std::string& type,
                        const std::string& name,
                        const IInterface* parent )
  : GaudiTool ( type, name , parent ), m_TileVeto(0),
    m_padsReconstructed(false),
    m_muonDetector(NULL)
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
  m_pads.clear();
}

void MuonPadRec::handle ( const Incident& incident )
{ 
  if ( IncidentType::EndEvent == incident.type() ) {
    if ( msgLevel(MSG::DEBUG) )debug() << "End Event: clear pads"<<endmsg;
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
    err()<<"error retrieving the Muon detector element "<<endmsg;
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
    Warning("MuonPadRec::buildLogicalPads not called yet",StatusCode::SUCCESS).ignore();

    return (const std::vector<MuonLogPad*>*) (&m_pads);
}


//---------------------------------------------------------------------------------------//
// private logical pad reconstruction, based on MuonRec algorithm from MuonDAQ package   //
//---------------------------------------------------------------------------------------//


StatusCode MuonPadRec::buildLogicalPads(const std::vector<MuonLogHit*> *myhits ) {
  std::vector<MuonLogHit*> hits;
  if((!m_padsReconstructed) && myhits) {
    // save locally current hits 
    std::copy(myhits->begin(), myhits->end(), std::back_inserter(hits));
    if ( msgLevel(MSG::DEBUG) ) debug() << "buildLogicalPads: raw hits are "<<hits.size()<<endmsg;
    removeDoubleHits(hits);
    if ( msgLevel(MSG::DEBUG) ) debug() << "                  after cleaning doubles "<<hits.size()<<endmsg;
    if (!hits.empty()) {
      int station;
      for( station = 0 ; station < m_muonDetector->stations() ; station++ ){
        int region;
        for( region = 0 ; region < m_muonDetector->regions() ; region++ ){
          
          // get mapping of input to output from region
          // in fact we are reversing the conversion done in the digitisation
          int NLogicalMap = m_muonDetector->getLogMapInRegion(station,region);     
          if ( msgLevel(MSG::VERBOSE) ) verbose()<<" station and region "<<station<<" "<<region<<" maps "<<NLogicalMap<<endmsg;
          
          if(1 == NLogicalMap){
            // straight copy of the input + making SmartRefs to the MuonDigits
            StatusCode sc = addCoordsNoMap(hits,station,region);
            if(!sc.isSuccess()){
              return Error("Failed to map digits to coords in a one to one manner");
            }
          }else{
            // need to cross the input strips to get output strips
            StatusCode sc = 
              addCoordsCrossingMap(hits,station,region);
            if(!sc.isSuccess()){
              return Error("Failed to map digits to coords by crossing strips");
            }
          }
        }    
      }
    }
    m_padsReconstructed = true;
  }
  return StatusCode::SUCCESS;
}


//=============================================================================

// Adding entries to coords 1 to 1 from digits, need to make the references
StatusCode MuonPadRec::addCoordsNoMap(std::vector<MuonLogHit*> &hits,
                                      int station,
                                      int region){
  for(const auto& hit : hits) {
    if( hit->tile().station() == static_cast<unsigned int>(station) &&
        hit->tile().region() == static_cast<unsigned int>(region) ){
      // make the coordinate to be added to coords
      if ( msgLevel(MSG::DEBUG) ) debug()<<" LOGPAD OK nomap ODE "<< hit->odeName() <<" ch "<< hit->odeChannel()
                                         <<        " tile "<<hit->tile()<< " time)="<< hit->time() << endmsg;
      
      if(!m_TileIsVetoed[(long int) hit->tile()]) {
        m_pads.emplace_back(new MuonLogPad(hit));
        m_pads.back()->settruepad();
      }
      else {
        if ( msgLevel(MSG::DEBUG) ) debug() << "applied veto on pad "<<((long int) hit->tile()) << endmsg;
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode MuonPadRec::addCoordsCrossingMap(std::vector<MuonLogHit*> &hits,
                                            int station,
                                            int region){

  // get local MuonLayouts for strips
  MuonLayout layoutOne,layoutTwo;
  StatusCode sc = makeStripLayouts(station,region, layoutOne,layoutTwo);
  if(!sc.isSuccess()){
    return sc;
  }
  
  // seperate the two types of logical channel, flag if used with the pair
  std::vector<std::pair<MuonLogHit*,bool> > typeOnes;
  std::vector<std::pair<MuonLogHit*,bool> > typeTwos;
  for(const auto& hit : hits) {
    if( (hit->tile()).station() == static_cast<unsigned int>(station) &&
        (hit->tile()).region() == static_cast<unsigned int>(region) ){
      if( hit->tile().layout() == layoutOne ){
        typeOnes.emplace_back(hit,false);
      }else if( hit->tile().layout() == layoutTwo ){
        typeTwos.emplace_back(hit,false);
      } else {
        Warning("MuonDigits in list are not compatible with expected shapes").ignore();
      }    
    }
  }
  // now cross the two sets of channels
  // sorry about this std::pair stuff but it is the easiest way of matching
  // a bool to each digit
  
  for(auto&  iOne : typeOnes) {
    for(auto&  iTwo : typeTwos) {
      // who said C++ did not make lovely transparent code?
      auto pad = iOne.first->tile().intercept(iTwo.first->tile()) ;
      if(pad.isValid()) {
        if (!m_TileIsVetoed[(long int) pad]) { 
          m_pads.emplace_back(new MuonLogPad(iOne.first,iTwo.first));
          m_pads.back()->settruepad();
          if ( msgLevel(MSG::DEBUG) ) debug() << " LOGPAD OK crossed ODE "<< iOne.first->odeName() 
                                              <<" ch "<< iOne.first->odeChannel()<<" and "
                                              << iTwo.first->odeChannel() << " tiles "
                                              << iOne.first->tile() << " and "
                                              << iTwo.first->tile() << " times="
                                              << iOne.first->time() << " and "
                                              << iTwo.first->time() << endmsg;
          
        } 
        else {
          if ( msgLevel(MSG::DEBUG) ) debug() << "applied veto on pad "<<((long int) pad) << endmsg;
        }
        // set flags to used on iOne and iTwo
        iOne.second = true;
        iTwo.second = true;        
      }
    }
  }

  // now copy across directly all strips that have not yet been used
  for(const auto& iOne : typeOnes) {
    if(!(iOne.second)) m_pads.emplace_back(new MuonLogPad(iOne.first));
  }
  for(const auto& iTwo : typeTwos) {
    if(!(iTwo.second)) m_pads.emplace_back(new MuonLogPad(iTwo.first));
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
  layout1= MuonLayout{ x1,y1 };
  layout2= MuonLayout{ x2,y2 };
  return StatusCode::SUCCESS;
}

StatusCode MuonPadRec::removeDoubleHits(std::vector<MuonLogHit*> &hits) {
  int mytime;
  auto ih1= hits.begin();
  while (ih1 != hits.end()) {
    bool ok=true;
    for (auto ih2= hits.begin(); ih2 != ih1 ; ih2++) {
      if ((*ih1)->tile() == (*ih2)->tile()) {
        if ( msgLevel(MSG::VERBOSE) ) verbose() << "Found double hit in Tile "<< (*ih1)->tile() <<endmsg;
        bool chooseone =
          m_getfirsthit ? ( (*ih1)->time() < (*ih2)->time() ) // take the first one
          : ( std::abs((*ih1)->time() - 7.5) < std::abs((*ih2)->time() - 7.5));  // or take time closer to 0 , i.e. 7.5 bits
        if(chooseone) {
          if ( msgLevel(MSG::VERBOSE) ) verbose() << "  choose time "<<(*ih1)->time() <<" rather than "<<(*ih2)->time()<<endmsg;
          mytime=(*ih1)->rawtime();
          (*ih2)->setTime( mytime );
        }
        else {
          if ( msgLevel(MSG::VERBOSE) ) verbose() << "  choose time "<<(*ih2)->time() <<" rather than "<<(*ih1)->time()<<endmsg;
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
