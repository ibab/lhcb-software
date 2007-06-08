// $Id: MuonRec.cpp,v 1.4 2007-06-08 15:38:45 asatta Exp $
// Include files 
#include <cstdio>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "Event/MuonCoord.h"

// local
#include "MuonRec.h"
using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : MuonRec
//
// 22/03/2002 : David Hutchcroft
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( MuonRec );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonRec::MuonRec( const std::string& name,
                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
   m_NStation(0), m_NRegion(0)
{

}

//=============================================================================
// Destructor
//=============================================================================
MuonRec::~MuonRec() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MuonRec::initialize() {
  StatusCode sc=GaudiAlgorithm::initialize();
  if(sc.isFailure())return sc;

  debug() << "==> Initialise" << endreq;
  m_muonDetector=getDet<DeMuonDetector>
    (DeMuonLocation::Default);
  
  // TDS path to the Muon system  is of the form /dd/Structure/LHCb/Muon
  
  if(m_muonDetector==NULL){
    error() << "Could not read /dd/Structure/LHCb/DownstreamRegion/Muon from TDS" 
        << endreq;
    return StatusCode::FAILURE;
  }


  m_NStation = 0;
  m_NRegion = 0;
  MuonBasicGeometry basegeometry( detSvc(),msgSvc());
  m_NStation= basegeometry.getStations();
  m_NRegion = basegeometry.getRegions();

  m_muonBuffer=tool<IMuonRawBuffer>("MuonRawBuffer");
  if(!m_muonBuffer)info()<<"error retrieving the toll "<<endreq;

  // Propagate RootInTES
  IProperty* prop = dynamic_cast<IProperty*>( m_muonBuffer );
  if( prop ) {
    sc = prop->setProperty( "RootInTES", rootInTES() );
    if( sc.isFailure() )
      return Error( "Unable to set RootInTES property of MuonRawBuffer", sc );
  }
  else
    return Error( "Unable to locate IProperty interface of MuonRawBuffer" );
  
  debug()<<" station number "<<m_NStation<<" "<<m_NRegion <<endreq;
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode MuonRec::execute() {


  debug() << "==> Execute" << endreq;

  // need to loop over input vector of MuonDigits
  // and make output vectors of MuonCoords one for each station
  std::vector<LHCb::MuonTileID> decoding=m_muonBuffer->getTile();
  debug()<<" digits number " <<decoding.size()<<" have been found "<<endreq;
  
  int station;
  MuonCoords *coords = new MuonCoords; 
  for( station = 0 ; station < m_NStation ; station++ ){
    int region;
    for( region = 0 ; region < m_NRegion ; region++ ){
      
      // get mapping of input to output from region
      // in fact we are reversing the conversion done in the digitisation
      int NLogicalMap = m_muonDetector->getLogMapInRegion(station,region);     
      debug()<<" station and region "<<station<<" "<<region<<" maps "<<NLogicalMap<<endreq;
      
      if(1 == NLogicalMap){
        // straight copy of the input + making SmartRefs to the MuonDigits
        StatusCode sc = addCoordsNoMap(coords,decoding,station,region);
        if(!sc.isSuccess()){
          error()
              << "Failed to map digits to coords in a one to one manner"
              << endreq;
          return sc;
        }
      }else{
        // need to cross the input strips to get output strips
        StatusCode sc = 
          addCoordsCrossingMap(coords,decoding,station,region);
        if(!sc.isSuccess()){error()
              << "Failed to map digits to coords by crossing strips"
              << endreq;
          return sc;
        }
      }
    }    
  }
  
  put( coords, LHCb::MuonCoordLocation::MuonCoords );
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MuonRec::finalize() {

  debug() << "==> Finalize" << endreq;
  return GaudiAlgorithm::finalize();
}

//=============================================================================

// Adding entries to coords 1 to 1 from digits, need to make the references
StatusCode MuonRec::addCoordsNoMap(MuonCoords *coords,  
                                   std::vector<LHCb::MuonTileID> & digits,
                                   const int & station,
                                   const int & region){


  std::vector<LHCb::MuonTileID>::iterator iD;
  for( iD = digits.begin() ; iD != digits.end() ; iD++ ){
    if( iD->station() == static_cast<unsigned int>(station) &&
        iD->region() == static_cast<unsigned int>(region) ){
      // make the coordinate to be added to coords
      debug()<<" digit tile "<<*iD<<endreq;
      
      MuonCoord *current = new MuonCoord();
      
      current->setUncrossed(true);
      
      // make a SmartRef to the MuonDigit
      std::vector<MuonTileID> link;
      link.push_back(*iD);
      current->setDigitTile(link);     
      // add it to the current digit
      
      // need to clear the layer and readout bits
      MuonTileID pad = *iD;
      debug()<<pad<<endreq;
      
      // as no change between digit and coord in this mapping key is the same
      coords->insert(current,pad);
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode MuonRec::addCoordsCrossingMap(MuonCoords *coords, 
                                         std::vector<LHCb::MuonTileID> & 
                                         digits,
                                         const int & station,
                                         const int & region){

  

  // need to calculate the shape of the horizontal and vertical logical strips
  //  if( 2 != m_muonDetector->getLogMapInRegion(station,region) ){
  // log << MSG::ERROR << "There are " << pRegion->numberLogicalMap()
  //     << " logical maps, I expect either 1 or 2" << endreq;
  //}

  // get local MuonLayouts for strips
  MuonLayout layoutOne,layoutTwo;
  StatusCode sc = makeStripLayouts(station,region, layoutOne,layoutTwo);
  if(!sc.isSuccess()){
    return sc;
  }
  
  // seperate the two types of logical channel, flag if used with the pair
  std::vector<std::pair<MuonTileID,bool> > typeOnes;
  std::vector<std::pair<MuonTileID,bool> > typeTwos;
  std::vector<MuonTileID>::iterator it;  
  for( it = digits.begin() ; it != digits.end() ; it++ ){
    if( (*it).station() == static_cast<unsigned int>(station) &&
        (*it).region() == static_cast<unsigned int>(region) ){
      if( (*it).layout() == layoutOne ){
        typeOnes.push_back(std::pair<MuonTileID,bool>((*it),false));
      }else if( (*it).layout() == layoutTwo ){
        typeTwos.push_back(std::pair<MuonTileID,bool>((*it),false));
      } else {
        error()
            << "MuonDigits in list are not compatable with expected shapes"
            << endreq;
      }    
    }
  }
  // now cross the two sets of channels
  // sorry about this std::pair stuff but it is the easiest way of matching
  // a bool to each digit
  std::vector<std::pair<MuonTileID,bool> >::iterator iOne;
  std::vector<std::pair<MuonTileID,bool> >::iterator iTwo;
  for( iOne = typeOnes.begin() ; iOne != typeOnes.end() ; iOne++ ){
    for( iTwo = typeTwos.begin() ; iTwo != typeTwos.end() ; iTwo++ ){
      // who said C++ did not make lovely transparent code?
      MuonTileID pad = (iOne->first).intercept((iTwo->first));
      if(pad.isValid()){ 
        // have a pad to write out
        // make the coordinate to be added to coords
        MuonCoord *current = new MuonCoord();
        current->setUncrossed(false);
        std::vector<MuonTileID> link;
        link.push_back(iOne->first);
        link.push_back(iTwo->first);
        current->setDigitTile(link);     
        // as no change between digit and coord in this mapping key is the same
        coords->insert(current,pad);

        // set flags to used on iOne and iTwo
        iOne->second = true;
        iTwo->second = true;
        
        debug() << " Made an crossed pad " << pad 
            << " from " << (iOne->first) << " and "
            << (iTwo->first) << endreq;
      }
    }
  }

  // now copy across directly all strips that have not yet been used
  for( iOne = typeOnes.begin() ; iOne != typeOnes.end() ; iOne++ ){
    if(!(iOne->second)){
      // no crossing map here
      MuonTileID pad = (iOne->first);

      // make the coordinate to be added to coords
      MuonCoord *current = new MuonCoord();
      current->setUncrossed(true);

      std::vector<MuonTileID> link;
      link.push_back(iOne->first);
      current->setDigitTile(link);     

      debug() << " Found an uncrossed pad type 1 " << pad 
          << endreq;

      coords->insert(current,pad);
    }
  }
  
  for( iTwo = typeTwos.begin() ; iTwo != typeTwos.end() ; iTwo++ ){
    if(!(iTwo->second)){
      // no crossing map here
      MuonTileID pad = (iTwo->first);

      // make the coordinate to be added to coords
      MuonCoord *current = new MuonCoord();
      current->setUncrossed(true);


      std::vector<MuonTileID> link;
      link.push_back(iTwo->first);
      current->setDigitTile(link);     
      debug() << " Found an uncrossed pad type 2 " << pad 
          << endreq;

      coords->insert(current,pad);      

    }
  } 
  return StatusCode::SUCCESS;
}



StatusCode MuonRec::makeStripLayouts(int station, int region, 
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
