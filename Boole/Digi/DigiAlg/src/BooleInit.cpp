// $Id: BooleInit.cpp,v 1.26 2008-10-09 06:28:36 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IEventTimeDecoder.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiAlg/IGenericTool.h"

// from EventBase
#include "Event/ProcessHeader.h"
#include "Event/GenCollision.h"

// from MCEvent
#include "Event/MCHeader.h"

// from DAQEvent
#include "Event/RawEvent.h"
#include "Event/RawBank.h"
#include "Event/ODIN.h"

// local
#include "BooleInit.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BooleInit
//
// 2005-12-15 : Marco Cattaneo
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( BooleInit );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BooleInit::BooleInit( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : LbAppInit ( name , pSvcLocator ), 
    m_memoryTool(0), m_odinTool(0)
{ 
  m_thresInteraction.clear();
  m_thresInteraction.push_back( 0.01 );
  m_thresInteraction.push_back( 0.03 );
  m_thresInteraction.push_back( 0.05 );

  m_thresDiffractive.clear();
  m_thresDiffractive.push_back( 0.1 );
  m_thresDiffractive.push_back( 0.3 );
  m_thresDiffractive.push_back( 0.6 );

  m_thresElastic.clear();
  m_thresElastic.push_back( 0.25 );
  m_thresElastic.push_back( 0.5  );
  m_thresElastic.push_back( 0.75 );

  declareProperty ( "ModifyOdin",  m_modifyOdin = true );
  declareProperty ( "GenCollisionLocation", m_genCollisionLocation = LHCb::GenCollisionLocation::Default ) ;
  declareProperty ( "ThresInteraction", m_thresInteraction );
  declareProperty ( "ThresDiffractive",  m_thresDiffractive );
  declareProperty ( "ThresElastic",  m_thresElastic );
  declareProperty ( "ThresTiggerType",  m_threstrigger= 0.05 );

}
//=============================================================================
// Destructor
//=============================================================================
BooleInit::~BooleInit() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode BooleInit::initialize() {
  StatusCode sc = LbAppInit::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by LbAppInit

  debug() << "==> Initialize" << endmsg;

  // Private tool to plot the memory usage
  if ( "" == rootInTES() )
    m_memoryTool = tool<IGenericTool>( "MemoryTool", "BooleMemory", this, true );

  // Tool to put the ODIN object on the TES
  m_odinTool = tool<IEventTimeDecoder>( "OdinTimeDecoder" );

  // Initialize thresholds if we want to modify Odin:
  if ( m_modifyOdin ) {

    bool ok = m_FlatDist.initialize(randSvc(),Rndm::Flat(0.,1.));
    if (!ok){
      info()<<"Flat distribution could not be initialized"<<endmsg;
    }

    info()<<"Thresholds for Interaction processes : " << m_thresInteraction <<endmsg;
    info()<<"Thresholds for Diffractive processes : " << m_thresDiffractive <<endmsg;
    info()<<"Thresholds for Elastic processes : " << m_thresElastic <<endmsg;
  }
  

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode BooleInit::execute() {

  StatusCode sc = LbAppInit::execute(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by LbAppInit

  debug() << "==> Execute" << endmsg;

  // Plot the memory usage
  if ( "" == rootInTES() ) m_memoryTool->execute();

  // Get the run and event number from the MC Header
  LHCb::MCHeader* evt = get<LHCb::MCHeader>( LHCb::MCHeaderLocation::Default,
                                             IgnoreRootInTES                 );
  if ( "" == rootInTES() ) printEventRun( evt->evtNumber(), evt->runNumber() );
  
  
  // Initialize the random number: 
  
  if ( "" == rootInTES() ) {
    std::vector<long int> seeds = getSeeds( evt->runNumber(), evt->evtNumber() );
    sc = this->initRndm( seeds );
    if ( sc.isFailure() ) return sc;  // error printed already by initRndm  

    // Create the Boole event header
    LHCb::ProcessHeader* header = new LHCb::ProcessHeader();
    header->setApplicationName( this->appName() );
    header->setApplicationVersion( this->appVersion() );
    header->setRunNumber( evt->runNumber() );
    header->setRandomSeeds( seeds );
    header->setCondDBTags( this->condDBTags() );
    put( header, LHCb::ProcessHeaderLocation::Digi );
  }

  // Create an empty RawEvent
  LHCb::RawEvent* raw = new LHCb::RawEvent();
  put( raw, LHCb::RawEventLocation::Default );

  // Add the ODIN bank (EDMS 704084 v2.0)
  unsigned int odin[9];
  odin[LHCb::ODIN::RunNumber]   = evt->runNumber();
  odin[LHCb::ODIN::EventType]   = 0;
  odin[LHCb::ODIN::OrbitNumber] = 0;
  odin[LHCb::ODIN::L0EventIDHi] = 0xFFFFFFFF & (evt->evtNumber() >> 32);
  odin[LHCb::ODIN::L0EventIDLo] = 0xFFFFFFFF & evt->evtNumber();
  odin[LHCb::ODIN::GPSTimeHi]   = 0xFFFFFFFF & (evt->evtTime()) >> 32;
  odin[LHCb::ODIN::GPSTimeLo]   = 0xFFFFFFFF & evt->evtTime();
  odin[LHCb::ODIN::Word7]       = 0; // Error bits, Detector status
  odin[LHCb::ODIN::Word8]       = 0; // Bunch current, Force bit, Bx/Readout/Triger type, Bunch ID

  // now modify the Odin bank if requested
  if ( m_modifyOdin ) {
    modifyOdin(odin);
  }
  
  LHCb::RawBank* odinBank = raw->createBank(0,LHCb::RawBank::ODIN,2, 8+36, odin);
  raw->adoptBank(odinBank, true);

  // Now decode the bank and put it on the TES, for use by other algorithms
  m_odinTool->getTime();

  return StatusCode::SUCCESS;
};


//=============================================================================
// modify ODIN bank
//=============================================================================
void BooleInit::modifyOdin(unsigned int *odin) {
  
  //Get info from Gen
  LHCb::GenCollisions* Collisions = get<LHCb::GenCollisions>( m_genCollisionLocation );
  int numCollisions = Collisions->size();
  if(msgLevel(MSG::DEBUG))
    debug() << "Number of Collisions: "<< numCollisions << endmsg;
    
  int interaction = 0;
  
  for (std::vector< LHCb::GenCollision* >::iterator process = Collisions->begin(); process != Collisions->end(); process++){      
    if(msgLevel(MSG::DEBUG))
      debug() << "Collision type: " << (*process)->processType() << endmsg;
    int type= (*process)->processType();
    if (numCollisions==1) {
      if (type==91 && msgLevel(MSG::DEBUG)) {
        debug()<< "Single Proton Elastic collision!"<<endmsg;
      }
      else if ((type==92 || type ==93) && msgLevel(MSG::DEBUG)) {
        debug()<< "Single Diffractive collision!"<<endmsg; 
      };
    };
    if (type==91) {
    }
    else if (type==92 || type ==93) {
      interaction=std::max(1, interaction);
    }
    
    else {
      interaction=std::max(2, interaction);
    };
  };
  
  if(msgLevel(MSG::DEBUG))
    debug() << "Interaction class " << interaction << endmsg;
  
  // Random number for BX type and Trigger type
  double randNumber = m_FlatDist.shoot();
  double trigRandNumber = m_FlatDist.shoot();
  if(msgLevel(MSG::DEBUG)) debug()<<"Random number:"<< randNumber<<endmsg;
    
  // set the types
  unsigned int TriggerType = 0;
  unsigned int BXType = 0;
  unsigned int BunchCurrent = 0;
  if (interaction==2) {
    if (trigRandNumber > m_threstrigger){
      TriggerType = LHCb::ODIN::PhysicsTrigger;
      BXType = LHCb::ODIN::BeamCrossing;
    }
    else {
      TriggerType = LHCb::ODIN::RandomTrigger;
      if (randNumber<m_thresInteraction[0]) BXType = LHCb::ODIN::NoBeam;
      if (randNumber>m_thresInteraction[0] && randNumber<m_thresInteraction[1]) BXType = LHCb::ODIN::SingleBeamLeft;
      if (randNumber>m_thresInteraction[1] && randNumber<m_thresInteraction[2]) BXType = LHCb::ODIN::SingleBeamRight;
      if (randNumber>m_thresInteraction[2]) BXType = LHCb::ODIN::BeamCrossing;
    }
  }

  if (interaction==1) {
    if (randNumber<m_thresDiffractive[0]) BXType = LHCb::ODIN::NoBeam;
    if (randNumber>m_thresDiffractive[0] && randNumber<m_thresDiffractive[1]) BXType = LHCb::ODIN::SingleBeamLeft;
    if (randNumber>m_thresDiffractive[1] && randNumber<m_thresDiffractive[2]) BXType = LHCb::ODIN::SingleBeamRight;
    if (randNumber>m_thresDiffractive[2]) BXType = LHCb::ODIN::BeamCrossing;
    TriggerType = LHCb::ODIN::RandomTrigger;
    // with random number some will be Reserve
  }
  if (interaction==0) {
    if (randNumber<m_thresElastic[0]) BXType = LHCb::ODIN::NoBeam;
    if (randNumber>m_thresElastic[0] && randNumber<m_thresElastic[1]) BXType = LHCb::ODIN::SingleBeamLeft;
    if (randNumber>m_thresElastic[1] && randNumber<m_thresElastic[2]) BXType = LHCb::ODIN::SingleBeamRight;
    if (randNumber>m_thresElastic[2]) BXType = LHCb::ODIN::BeamCrossing;
    TriggerType = LHCb::ODIN::RandomTrigger;
    // with random number some will be Reserve
  }
  
  if ( BXType == LHCb::ODIN::BeamCrossing ) BunchCurrent = 8 + (8<<4); // some current..
  if ( BXType == LHCb::ODIN::SingleBeamLeft ) BunchCurrent = 8;        // some current..
  if ( BXType == LHCb::ODIN::SingleBeamRight ) BunchCurrent = (8<<4);  // some current..
  
  if(msgLevel(MSG::DEBUG)) debug() <<"Bunch crossing type: " << BXType 
                                   << " TriggerType " << TriggerType 
                                   << " BunchCurrent  "  << BunchCurrent
                                   << endmsg;

  unsigned int temp32 = 0;
  temp32 |= (BXType << LHCb::ODIN::BXTypeBits) & LHCb::ODIN::BXTypeMask;
  temp32 |= (TriggerType << LHCb::ODIN::TriggerTypeBits) & LHCb::ODIN::TriggerTypeMask;
  temp32 |= (BunchCurrent << LHCb::ODIN::BunchCurrentBits) & LHCb::ODIN::BunchCurrentMask;
  odin[LHCb::ODIN::Word8]       = temp32; // Bunch current, Force bit, Bx/Readout/Triger type, Bunch ID
  
}

//=============================================================================

