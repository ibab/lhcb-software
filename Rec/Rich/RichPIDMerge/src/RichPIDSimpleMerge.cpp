// $Id: RichPIDSimpleMerge.cpp,v 1.3 2003-11-25 13:56:16 jonesc Exp $
// Include files

// local
#include "RichPIDSimpleMerge.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichPIDSimpleMerge
//
// 2002-07-10 : Chris Jones
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichPIDSimpleMerge>          s_factory ;
const        IAlgFactory& RichPIDSimpleMergeFactory = s_factory ;


// Standard constructor, initializes variables
RichPIDSimpleMerge::RichPIDSimpleMerge( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : RichAlgBase ( name , pSvcLocator ) {

  // Output location in TDS for RichPIDs
  declareProperty( "OutputPIDLocation",
                   m_richPIDLocation = RichPIDLocation::Default );
  // Input location in TDS for RichGlobalPIDs
  declareProperty( "InputGlobalPIDLocation",
                   m_richGlobalPIDLocation = RichGlobalPIDLocation::Default );
  // Input location in TDS for RichLocalPIDs
  declareProperty( "InputLocalPIDLocation",
                   m_richLocalPIDLocation = RichLocalPIDLocation::Default );
  // Location of processing status object in TES
  declareProperty( "ProcStatusLocation",
                   m_procStatLocation = ProcStatusLocation::Default );

}

// Destructor
RichPIDSimpleMerge::~RichPIDSimpleMerge() {};

// Initialisation.
StatusCode RichPIDSimpleMerge::initialize() {

  MsgStream msg(msgSvc(), name());

  // initialise base classclean
  if ( !RichAlgBase::initialize() ) return StatusCode::FAILURE;

  msg << MSG::DEBUG << "Initialize" << endreq
      << " Output RichPIDs location     = " << m_richPIDLocation << endreq;

  return StatusCode::SUCCESS;
};

// Main execution
StatusCode RichPIDSimpleMerge::execute() {

  //
  // For the moment take global then local result in that order of preference
  // In the future need some kind of proper merging of results
  //

  if ( msgLevel(MSG::DEBUG) ) {
    MsgStream  msg( msgSvc(), name() );
    msg << MSG::DEBUG << "Execute" << endreq;
  }

  // Obtain Global PID results
  SmartDataPtr<RichGlobalPIDs> gPIDs( eventSvc(), m_richGlobalPIDLocation );
  if ( msgLevel(MSG::VERBOSE) ) {
    MsgStream  msg( msgSvc(), name() );
    if ( !gPIDs ) {
      msg << MSG::VERBOSE << "Cannot locate RichGlobalPIDs at "
          << m_richGlobalPIDLocation << endreq;
    } else {
      msg << MSG::VERBOSE << "Successfully located " << gPIDs->size()
          << " RichGlobalPIDs at " << m_richGlobalPIDLocation << endreq;
    }
  }

  // Obtain Local PID results
  SmartDataPtr<RichLocalPIDs> lPIDs(eventSvc(), m_richLocalPIDLocation);
  if ( msgLevel(MSG::VERBOSE) ) {
    MsgStream  msg( msgSvc(), name() );
    if ( !lPIDs ) {
      msg << MSG::VERBOSE << "Cannot locate RichLocalPIDs at "
          << m_richLocalPIDLocation << endreq;
    } else {
      msg << MSG::VERBOSE << "Successfully located " << lPIDs->size()
          << " RichLocalPIDs at " << m_richLocalPIDLocation << endreq;
    }
  }

  // See if PIDs already exist at requested output location
  RichPIDs * newPIDs = NULL;
  bool pidsExist = false;
  int originalSize = 0;
  SmartDataPtr<RichPIDs> outPIDs( eventSvc(),m_richPIDLocation );
  if ( outPIDs ) {
    pidsExist = true;
    newPIDs = outPIDs;
    originalSize = newPIDs->size();
    newPIDs->clear();
  } else {
    // Form new container for output PIDs
    newPIDs = new RichPIDs();
  }

  // Locate the processing status object
  SmartDataPtr<ProcStatus> procStat( eventSvc(), m_procStatLocation );
  if ( !procStat ) {
    MsgStream  msg( msgSvc(), name() );
    msg << MSG::WARNING
        << "Failed to locate ProcStatus at " << m_procStatLocation << endreq;
    return StatusCode::FAILURE;
  }

  int nUsedglobalPIDs = 0;
  int nUsedlocalPIDs = 0;
  if ( !procStat->aborted() ) {

    // iterate over Global PID results and form output persistent objects
    if ( gPIDs ) {
      for ( RichGlobalPIDs::const_iterator gPID = gPIDs->begin();
            gPID != gPIDs->end();
            ++gPID ) {

        // Form new PID object
        RichPID * outPID = new RichPID();
        newPIDs->insert( outPID, (*gPID)->key() );
        ++nUsedglobalPIDs;

        // data members
        outPID->setBestParticleID( (*gPID)->bestParticleID() );
        outPID->setParticleLLValues( (*gPID)->particleLLValues() );
        outPID->setParticleRawProbValues( (*gPID)->particleRawProbValues() );
        outPID->setRecTrack( (*gPID)->recTrack() );
        outPID->setPidResultCode( (*gPID)->pidResultCode() );

      }
    }

    // iterate over Local PID results and place in output container if
    // global PID result not present
    if ( lPIDs ) {
      for ( RichLocalPIDs::const_iterator lPID = lPIDs->begin();
            lPID != lPIDs->end();
            ++lPID ) {

        // if pid with this key exists, skip
        if ( newPIDs->object( (*lPID)->key() ) ) continue;

        // Form new PID object
        RichPID * outPID = new RichPID();
        newPIDs->insert( outPID, (*lPID)->key() );
        ++nUsedlocalPIDs;

        // data members
        outPID->setBestParticleID( (*lPID)->bestParticleID() );
        outPID->setParticleLLValues( (*lPID)->particleLLValues() );
        outPID->setParticleRawProbValues( (*lPID)->particleRawProbValues() );
        outPID->setRecTrack( (*lPID)->recTrack() );
        outPID->setPidResultCode( (*lPID)->pidResultCode() );

      }
    }

  }

  // Update Rich status words
  procStat->addAlgorithmStatus( name()+":UsedGlobalPIDs", nUsedglobalPIDs );
  procStat->addAlgorithmStatus( name()+":UsedLocalPIDs",  nUsedlocalPIDs  );

  // register to Gaudi
  if ( !pidsExist ) {
    if ( !eventSvc()->registerObject(m_richPIDLocation, newPIDs) ) {
      MsgStream  msg( msgSvc(), name() );
      msg << MSG::WARNING << "Unable to register RichPIDs at "
          << m_richPIDLocation << endreq;
      return StatusCode::FAILURE;
    } else {
      if ( msgLevel(MSG::DEBUG) ) {
        MsgStream  msg( msgSvc(), name() );
        msg << MSG::DEBUG << "Successfully registered " << newPIDs->size()
            << " RichPIDs at " << m_richPIDLocation 
            << " : Global=" << nUsedglobalPIDs
            << " Local=" << nUsedlocalPIDs
            << endreq;
      }
    }
  } else {
    if ( msgLevel(MSG::DEBUG) ) {
      MsgStream  msg( msgSvc(), name() );
      msg << MSG::DEBUG
          << "Replaced " << originalSize << " pre-existing RichPIDs at "
          << m_richPIDLocation << " with " << newPIDs->size()
          << " new RichPIDs" 
          << " : Global=" << nUsedglobalPIDs
          << " Local=" << nUsedlocalPIDs
          << endreq;
    }
  }

  return StatusCode::SUCCESS;
};

//  Finalize
StatusCode RichPIDSimpleMerge::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Finalize" << endreq;

  // base class finalise
  return RichAlgBase::finalize();
}
