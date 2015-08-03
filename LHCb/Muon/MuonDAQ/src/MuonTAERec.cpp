// Include files 

#include "Event/MuonCoord.h"
// local
#include "MuonTAERec.h"
using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : MuonTAERec
//
// 2008-10-28 : Alessia Satta
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MuonTAERec )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonTAERec::MuonTAERec( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
   m_NStation(0), m_NRegion(0)
{
 declareProperty("IgnoreStrips" , m_ignoreUncrossedStrips=true) ;

  m_ignoreExecution=false;
}


//=============================================================================
// Destructor
//=============================================================================
MuonTAERec::~MuonTAERec() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode MuonTAERec::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Initialize" << endmsg;
  m_muonDetector=getDet<DeMuonDetector>
    (DeMuonLocation::Default);
  
  // TDS path to the Muon system  is of the form /dd/Structure/LHCb/Muon
  
  if(m_muonDetector==NULL){
    error() << 
      "Could not read /dd/Structure/LHCb/DownstreamRegion/Muon from TDS "
        << endmsg;
    return StatusCode::FAILURE;
  }


  m_NStation = 0;
  m_NRegion = 0;
  MuonBasicGeometry basegeometry( detSvc(),msgSvc());
  m_NStation= basegeometry.getStations();  
  m_NRegion = basegeometry.getRegions();  
  if(rootInTES()!="")m_ignoreExecution=true;
  //if TAE mode use the pribate tool to reset the meomory.
  m_muonBuffer=tool<IMuonRawBuffer>("MuonRawBuffer",this);
  if(!m_muonBuffer)info()<<"error retrieving the tool "<<endmsg;
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug()<<" station number "<<m_NStation<<" "<<m_NRegion <<endmsg;
  m_offsetLoc[0]="Prev7/";
  m_offsetLoc[1]="Prev6/";
  m_offsetLoc[2]="Prev5/";
  m_offsetLoc[3]="Prev4/";
  m_offsetLoc[4]="Prev3/";
  m_offsetLoc[5]="Prev2/";
  m_offsetLoc[6]="Prev1/";
  m_offsetLoc[7]="";
  m_offsetLoc[8]="Next1/";
  m_offsetLoc[9]="Next2/";
  m_offsetLoc[10]="Next3/";
  m_offsetLoc[11]="Next4/";
  m_offsetLoc[12]="Next5/";
  m_offsetLoc[13]="Next6/";
  m_offsetLoc[14]="Next7/";

  m_TAENum=7;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MuonTAERec::execute() {

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Execute" << endmsg;  
  MuonCoords *coords = new MuonCoords;
  if(!m_ignoreExecution){
      // need to loop over input vector of MuonDigits
  // and make output vectors of MuonCoords one for each station
  std::vector<std::pair<LHCb::MuonTileID,unsigned int > > decoding;
  std::vector<std::pair<LHCb::MuonTileID,unsigned int> >::iterator it;

  for(int i=-m_TAENum;i<=m_TAENum;i++){
    LHCb::RawEvent* raw = getIfExists<LHCb::RawEvent>(m_offsetLoc[7+i] +LHCb::RawEventLocation::Default);
    if ( NULL == raw )
      continue;
  
    // get tiles from the raw buffer
    StatusCode sc=m_muonBuffer->getTileAndTDC(raw,decoding,m_offsetLoc[7+i] );
    if(sc.isFailure()){
      return Error("Error in decoding the muon raw data");
    }
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug()<<decoding.size()<<" digits in input "<<endmsg;
  
    for(it = decoding.begin(); it != decoding.end(); it++){

      // set the time taking into account the BX. must be unsigned...
      std::vector< std::pair<LHCb::MuonTileID,unsigned int> >::iterator icheck;
      bool add=true;
     //info()<<" digit in input "<<(*it).first<<endmsg;      
      for(icheck= m_logChannels.begin();icheck!= m_logChannels.end();icheck++){
         if ( (*it).first  ==  (*icheck).first ) {
           if( UNLIKELY( msgLevel(MSG::VERBOSE) ) ) 
             verbose()<<" found double hit check time "<< (*it).second+(7+i)*16<<" "<<(*icheck).second<<endmsg;
           //take only the first one 
           unsigned int tone=(*it).second;
           tone=tone+(7+i)*16;
           if(tone> (*icheck).second){
             add =false;            
             if( UNLIKELY( msgLevel(MSG::VERBOSE) ) ) 
               verbose()<<" so keep the existing one and not add the new one "<<endmsg;
             break;
             
           }else{
             if( UNLIKELY( msgLevel(MSG::VERBOSE) ) ) 
               verbose()<<"remove the old one  "<<m_logChannels.size()<<endmsg;
             m_logChannels.erase(icheck);
             
           }
           
         }
      }
      if(add){
         (*it).second += (7+i)*16;
         std::pair<LHCb::MuonTileID,unsigned int>  ch((*it).first, (*it).second );
         if( UNLIKELY( msgLevel(MSG::VERBOSE) ) ) 
           verbose()<<"adding digit  "<<(*it).first<<" to toal number "
                    <<m_logChannels.size()<<endmsg;        
         m_logChannels.push_back(ch);
      }
    }
    m_muonBuffer->forceReset();
  }
    std::vector< std::pair<LHCb::MuonTileID,
                                   unsigned int> >::iterator itest;
                                
//for(    itest=m_logChannels.begin();itest<m_logChannels.end();itest++){
//info()<<" lisat dei digit "<<(*itest).first<<endmsg;
//}
  int station;

  for( station = 0 ; station < m_NStation ; station++ ){
    int region;
    for( region = 0 ; region < m_NRegion ; region++ ){ 
    
      // get mapping of input to output from region
      // in fact we are reversing the conversion done in the digitisation  
      int NLogicalMap = m_muonDetector->getLogMapInRegion(station,region);
      if( UNLIKELY( msgLevel(MSG::VERBOSE) ) ) 
        verbose()<<" station and region "<<station<<" "<<region<<" maps "<<NLogicalMap<<endmsg;    
      if(1 == NLogicalMap){
        // straight copy of the input + making SmartRefs to the MuonDigits
        StatusCode sc = addCoordsNoMap(coords,m_logChannels,station,region);
        if(!sc.isSuccess()){
          error()
              << "Failed to map digits to coords in a one to one manner"
              << endmsg;
          return sc;
        }
      }else{  
        // need to cross the input strips to get output strips
        StatusCode sc =
          addCoordsCrossingMap(coords,m_logChannels,station,region);
        if(!sc.isSuccess()){error()
              << "Failed to map digits to coords by crossing strips"
              << endmsg;
          return sc;
        }
      }
    }
  }
  
  }
  m_logChannels.clear();
  

  put( coords, "Raw/Muon/TAECoords");

  return StatusCode::SUCCESS;
  
}

//=============================================================================

// Adding entries to coords 1 to 1 from digits, need to make the references
StatusCode MuonTAERec::addCoordsNoMap(MuonCoords *coords,  
                                   std::vector< std::pair<LHCb::MuonTileID,
                                   unsigned int> > & digits,
                                   const int & station,
                                   const int & region){


  std::vector<std::pair<LHCb::MuonTileID, unsigned int> >::iterator iD;
  for( iD = digits.begin() ; iD != digits.end() ; iD++ ){
    if( (iD->first).station() == static_cast<unsigned int>(station) &&
        (iD->first).region() == static_cast<unsigned int>(region) ){
      // make the coordinate to be added to coords
      if( UNLIKELY( msgLevel(MSG::VERBOSE) ) ) 
        verbose()<<" digit tile "<<iD->first<<endmsg;
      
      MuonCoord *current = new MuonCoord();
      
      current->setUncrossed(true);
      current->setDigitTDC1(iD->second);
      
      // make a SmartRef to the MuonDigit
      std::vector<MuonTileID> link;
      link.push_back(iD->first);
      current->setDigitTile(link);     
      // add it to the current digit
      
      // need to clear the layer and readout bits
      MuonTileID pad = iD->first;
      if( UNLIKELY( msgLevel(MSG::VERBOSE) ) ) verbose()<<pad<<endmsg;
      
      // as no change between digit and coord in this mapping key is the same
      coords->insert(current,pad);
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode MuonTAERec::addCoordsCrossingMap(MuonCoords *coords, 
                                         std::vector<std::pair<LHCb::MuonTileID,
                                         unsigned int > > & 
                                         digits,
                                         const int & station,
                                         const int & region){

  

  // need to calculate the shape of the horizontal and vertical logical strips
  //  if( 2 != m_muonDetector->getLogMapInRegion(station,region) ){
  // log << MSG::ERROR << "There are " << pRegion->numberLogicalMap()
  //     << " logical maps, I expect either 1 or 2" << endmsg;
  //}

  // get local MuonLayouts for strips
  MuonLayout layoutOne,layoutTwo;
  StatusCode sc = makeStripLayouts(station,region, layoutOne,layoutTwo);
  if(!sc.isSuccess()){
    return sc;
  }
  
  // seperate the two types of logical channel, flag if used with the pair
  std::vector<std::pair<std::pair<MuonTileID,unsigned int>,bool> > typeOnes;
  std::vector<std::pair<std::pair<MuonTileID,unsigned int>,bool> > typeTwos;
  std::vector<std::pair<MuonTileID,unsigned int > >::iterator it;  
  for( it = digits.begin() ; it != digits.end() ; it++ ){
    if( (it->first).station() == static_cast<unsigned int>(station) &&
        (it->first).region() == static_cast<unsigned int>(region) ){
      if( (it->first).layout() == layoutOne ){
        typeOnes.push_back(std::pair<std::pair<MuonTileID,unsigned int> ,bool>((*it),false));
      }else if( (it->first).layout() == layoutTwo ){
          typeTwos.push_back(std::pair<std::pair<MuonTileID,unsigned int> ,bool>((*it),false));
      } else {
        error()
            << "MuonDigits in list are not compatable with expected shapes"
            << (*it)<<endmsg;
      }    
    }
  }
  // now cross the two sets of channels
  // sorry about this std::pair stuff but it is the easiest way of matching
  // a bool to each digit
  std::vector<std::pair< std::pair<MuonTileID,unsigned int >,bool> >::iterator iOne;
  std::vector<std::pair<std::pair<MuonTileID,unsigned int> ,bool> >::iterator iTwo;
  for( iOne = typeOnes.begin() ; iOne != typeOnes.end() ; iOne++ ){
    for( iTwo = typeTwos.begin() ; iTwo != typeTwos.end() ; iTwo++ ){
      // who said C++ did not make lovely transparent code?
      MuonTileID pad = ((iOne->first).first).intercept((iTwo->first).first);
      if(pad.isValid()){ 
        // have a pad to write out
        // make the coordinate to be added to coords
        MuonCoord *current = new MuonCoord();
        current->setUncrossed(false);
        std::vector<MuonTileID> link;
        link.push_back((iOne->first).first);
        link.push_back((iTwo->first).first);
        current->setDigitTile(link);     
        current->setDigitTDC1((iOne->first).second);
        current->setDigitTDC2((iTwo->first).second);
        if( UNLIKELY( msgLevel(MSG::VERBOSE) ) ) 
          verbose() << " Made an crossed pad " << pad 
                    << " from " << (iOne->first) << " and "
                    << (iTwo->first) << endmsg;
      
        // as no change between digit and coord in this mapping key is the same
        coords->insert(current,pad);

        // set flags to used on iOne and iTwo
        iOne->second = true;
        iTwo->second = true;
        
      }
    }
  }
if(!m_ignoreUncrossedStrips){
  // now copy across directly all strips that have not yet been used
  for( iOne = typeOnes.begin() ; iOne != typeOnes.end() ; iOne++ ){
    if(!(iOne->second)){
      // no crossing map here
      MuonTileID pad = (iOne->first).first;

      // make the coordinate to be added to coords
      MuonCoord *current = new MuonCoord();
      current->setUncrossed(true);

      std::vector<MuonTileID> link;
      link.push_back((iOne->first).first);
      current->setDigitTile(link);
      current->setDigitTDC1((iOne->first).second);

      if( UNLIKELY( msgLevel(MSG::VERBOSE) ) ) 
        verbose() << " Found an uncrossed pad type 1 " << pad << endmsg;

      coords->insert(current,pad);
    }
  }
  
  for( iTwo = typeTwos.begin() ; iTwo != typeTwos.end() ; iTwo++ ){
    if(!(iTwo->second)){
      // no crossing map here
      MuonTileID pad = (iTwo->first).first;

      // make the coordinate to be added to coords
      MuonCoord *current = new MuonCoord();
      current->setUncrossed(true);


      std::vector<MuonTileID> link;
      current->setDigitTDC1((iTwo->first).second);
      link.push_back((iTwo->first).first);
      current->setDigitTile(link);     

      if( UNLIKELY( msgLevel(MSG::VERBOSE) ) ) 
        verbose() << " Found an uncrossed pad type 2 " << pad << endmsg;

      coords->insert(current,pad);      

    }
  } 
}
  return StatusCode::SUCCESS;
}



StatusCode MuonTAERec::makeStripLayouts(int station, int region, 
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
