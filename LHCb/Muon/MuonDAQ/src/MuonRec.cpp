// Include files
#include <cstdio>

#include "Event/MuonCoord.h"

// local
#include "MuonRec.h"
using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : MuonRec
//
// 22/03/2002 : David Hutchcroft
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( MuonRec )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonRec::MuonRec( const std::string& name,
                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  m_forceResetDAQ=(context()=="TAE");//@TODO: is 'context()' actually (always) properly set at construction time???
  declareProperty("OutputLocation",m_coordOutputLocation=LHCb::MuonCoordLocation::MuonCoords ,"Where to store the output co-ordinates, defaults to LHCb::MuonCoordLocation::MuonCoords");
}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MuonRec::initialize() {
  StatusCode sc=GaudiAlgorithm::initialize();
  if(sc.isFailure())return sc;

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Initialise" << endmsg;
  m_muonDetector=getDet<DeMuonDetector>(DeMuonLocation::Default);
  if(m_muonDetector==NULL)
    return Error("Could not read /dd/Structure/LHCb/DownstreamRegion/Muon from TDS");

  m_NStation = 0;
  m_NRegion = 0;
  MuonBasicGeometry basegeometry( detSvc(),msgSvc());
  m_NStation= basegeometry.getStations();
  m_NRegion = basegeometry.getRegions();
  if(m_forceResetDAQ){
    //if TAE mode use the private tool to reset the meomory.
    m_muonBuffer=tool<IMuonRawBuffer>("MuonRawBuffer",this);
    if(!m_muonBuffer) return Error("Cannot create private MuonRawBuffer tool");
  }else{
    //if not TAE mode use the public tool
    m_muonBuffer=tool<IMuonRawBuffer>("MuonRawBuffer");
    if(!m_muonBuffer) return Error("Cannot retrieve MuonRawBuffer tool");
  }

  // Propagate RootInTES
  IProperty* prop = dynamic_cast<IProperty*>( m_muonBuffer );
  if( !prop ) {
    return Error( "Unable to locate IProperty interface of MuonRawBuffer" );
  }
  sc = prop->setProperty( "RootInTES", rootInTES() );
  if( sc.isFailure() )
    return Error( "Unable to set RootInTES property of MuonRawBuffer", sc );
  //    sc = prop->setProperty( "GlobalTimeOffset", globalTimeOffset() );
  // if( sc.isFailure() )
  // return Error("Unable to set GlobalTimeOffset property of MuonRawBuffer",
  //                 sc );

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug()<<" station number "<<m_NStation<<" "<<m_NRegion <<endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MuonRec::execute() {

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Execute" << endmsg;

  // need to loop over input vector of MuonDigits
  // and make output vectors of MuonCoords one for each station
  std::vector<std::pair<LHCb::MuonTileID,unsigned int > > decoding;

  StatusCode sc = m_muonBuffer->getTileAndTDC(decoding);
  if(sc.isFailure()) return Error( "Error in decoding the muon raw data " );

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug()<<decoding.size()<<" digits in input "<<endmsg;

  int station;
  MuonCoords *coords = new MuonCoords;
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
        StatusCode sc = addCoordsNoMap(coords,decoding,station,region);
        if(!sc.isSuccess()){
          error()
              << "Failed to map digits to coords in a one to one manner"
              << endmsg;
          put( coords, m_coordOutputLocation );
          return StatusCode::SUCCESS;
        }
      }else{
        // need to cross the input strips to get output strips
        StatusCode sc =
          addCoordsCrossingMap(coords,decoding,station,region);
        if(!sc.isSuccess()){error()
              << "Failed to map digits to coords by crossing strips"
              << endmsg;
         put( coords, m_coordOutputLocation );
         return StatusCode::SUCCESS;
        }
      }
    }
  }

  put( coords, m_coordOutputLocation );
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MuonRec::finalize() {

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Finalize" << endmsg;

  if(   m_Exccounter>0){
    info()<<" during the reconstruction there were problem with duplicated coords "<<endmsg;
    info()<<"The error occured " <<  m_Exccounter<<endmsg;

  }

  return GaudiAlgorithm::finalize();
}

//=============================================================================

// Adding entries to coords 1 to 1 from digits, need to make the references
StatusCode MuonRec::addCoordsNoMap(MuonCoords *coords,
                                   std::vector< std::pair<LHCb::MuonTileID,
                                   unsigned int> > & digits,
                                   int station,
                                   int region) const{
  for(auto iD = digits.begin() ; iD != digits.end() ; iD++ ){
    if( (iD->first).station() == static_cast<unsigned int>(station) &&
        (iD->first).region() == static_cast<unsigned int>(region) ){
      // make the coordinate to be added to coords
      if( UNLIKELY( msgLevel(MSG::VERBOSE) ) )
        verbose()<<" digit tile "<<iD->first<<endmsg;

      auto current = std::unique_ptr<MuonCoord>(new MuonCoord());

      current->setUncrossed(true);
      current->setDigitTDC1(iD->second);

      // make a SmartRef to the MuonDigit
      current->setDigitTile({ iD->first });
      // add it to the current digit

      // need to clear the layer and readout bits
      MuonTileID pad = iD->first;
      if( UNLIKELY( msgLevel(MSG::VERBOSE) ) ) verbose()<<pad<<endmsg;

      // as no change between digit and coord in this mapping key is the same
      try {
        coords->insert( current.release(), pad );
      } catch( const GaudiException& exc  ) {

        error() << "The error is caused by the duplication of  pad " <<pad<<endmsg;
        error() << "It is likely due to data corruption " <<endmsg;
        m_Exccounter++;
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode MuonRec::addCoordsCrossingMap(MuonCoords *coords,
                                         std::vector<std::pair<LHCb::MuonTileID,
                                         unsigned int > > &
                                         digits,
                                         int station,
                                         int region) const {



  // need to calculate the shape of the horizontal and vertical logical strips
  //  if( 2 != m_muonDetector->getLogMapInRegion(station,region) ){
  // log << MSG::ERROR << "There are " << pRegion->numberLogicalMap()
  //     << " logical maps, I expect either 1 or 2" << endmsg;
  //}

  // get local MuonLayouts for strips
  MuonLayout layoutOne,layoutTwo;
  StatusCode sc = makeStripLayouts(station,region, layoutOne,layoutTwo);
  if(!sc.isSuccess()) return sc;

  // seperate the two types of logical channel, flag if used with the pair
  std::vector<std::pair<std::pair<MuonTileID,unsigned int>,bool> > typeOnes;
  std::vector<std::pair<std::pair<MuonTileID,unsigned int>,bool> > typeTwos;
  for(auto it = digits.begin() ; it != digits.end() ; it++ ){
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
  for(auto iOne = typeOnes.begin() ; iOne != typeOnes.end() ; iOne++ ){
    for(auto iTwo = typeTwos.begin() ; iTwo != typeTwos.end() ; iTwo++ ){
      // who said C++ did not make lovely transparent code?
      MuonTileID pad = ((iOne->first).first).intercept((iTwo->first).first);
      if(pad.isValid()){
        // have a pad to write out
        // make the coordinate to be added to coords
        auto current = std::unique_ptr<MuonCoord>(new MuonCoord());
        current->setUncrossed(false);
        current->setDigitTile({ iOne->first.first, iTwo->first.first });
        current->setDigitTDC1((iOne->first).second);
        current->setDigitTDC2((iTwo->first).second);

        // as no change between digit and coord in this mapping key is the same

        try {
          coords->insert( current.release(), pad );
        } catch( const GaudiException& exc  ) {

          error() << "The error is caused by the duplication of  pad " <<pad<<endmsg;
          error() << "It is likely due to data corruption " <<endmsg;
          m_Exccounter++;
        }
        // set flags to used on iOne and iTwo
        iOne->second = true;
        iTwo->second = true;

        if( UNLIKELY( msgLevel(MSG::VERBOSE) ) )
          verbose() << " Made an crossed pad " << pad
                    << " from " << (iOne->first) << " and "
                    << (iTwo->first) << endmsg;
      }
    }
  }

  // now copy across directly all strips that have not yet been used
  for(auto iOne = typeOnes.begin() ; iOne != typeOnes.end() ; iOne++ ){
    if(!(iOne->second)){
      // no crossing map here
      MuonTileID pad = (iOne->first).first;

      // make the coordinate to be added to coords
      auto current = std::unique_ptr<MuonCoord>(new MuonCoord());
      current->setUncrossed(true);
      current->setDigitTile( { iOne->first.first } ) ;
      current->setDigitTDC1((iOne->first).second);

      if( UNLIKELY( msgLevel(MSG::VERBOSE) ) )
        verbose() << " Found an uncrossed pad type 1 " << pad << endmsg;


      try {
        coords->insert( current.release(), pad );
      } catch( const GaudiException& exc  ) {

        error() << "The error is caused by the duplication of  pad " <<pad<<endmsg;
        error() << "It is likely due to data corruption " <<endmsg;
        m_Exccounter++;
      }

    }
  }


  for(auto iTwo = typeTwos.begin() ; iTwo != typeTwos.end() ; iTwo++ ){
    if(!(iTwo->second)){
      // no crossing map here
      MuonTileID pad = iTwo->first.first;

      // make the coordinate to be added to coords
      auto current = std::unique_ptr<MuonCoord>(new MuonCoord());
      current->setUncrossed(true);
      current->setDigitTDC1((iTwo->first).second);
      current->setDigitTile( { iTwo->first.first } );

      if( UNLIKELY( msgLevel(MSG::VERBOSE) ) )
        verbose() << " Found an uncrossed pad type 2 " << pad << endmsg;
      try {
        coords->insert( current.release(), pad );
      } catch( const GaudiException& exc  ) {
        error() << "The error is caused by the duplication of  pad " <<pad<<endmsg;
        error() << "It is likely due to data corruption " <<endmsg;
        m_Exccounter++;
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode MuonRec::makeStripLayouts(int station, int region,
                                     MuonLayout &layout1,
                                     MuonLayout &layout2) const{

  unsigned int x1 = m_muonDetector->getLayoutX(0,station,region);
  unsigned int y1 = m_muonDetector->getLayoutY(0,station,region);
  unsigned int x2 = m_muonDetector->getLayoutX(1,station,region);
  unsigned int y2 = m_muonDetector->getLayoutY(1,station,region);
  layout1= MuonLayout(x1,y1);
  layout2= MuonLayout(x2,y2);
  return StatusCode::SUCCESS;
}
