// $Id: RichDigiDataObjVerifier.cpp,v 1.16 2007-02-02 10:12:44 jonrob Exp $

// local
#include "RichDigiDataObjVerifier.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// LHCb namespace
using namespace Rich::MC::Digi;

//-----------------------------------------------------------------------------
// Implementation file for class : RichDigiDataObjVerifier
//
// 2002-11-20 : Andy Presland   (Andrew.Presland@cern.ch)
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( DataObjVerifier )

// Standard constructor, initializes variables
DataObjVerifier::DataObjVerifier( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : Rich::AlgBase ( name, pSvcLocator )
{

  // Declare job options
  declareProperty( "CheckMCRichDigits",           m_bdMcDigits = true );
  declareProperty( "CheckMCRichHits",             m_bdMCHits   = true );
  declareProperty( "CheckMCRichOpticalPhotons",   m_bdMCPhots  = true );

}

// Destructor
DataObjVerifier::~DataObjVerifier() { }

void DataObjVerifier::checkHitsAt( const std::string & location ) const
{
  // MCRichHits
  if ( !exist<LHCb::MCRichHits>(location) )
  {
    debug() << "Cannot locate MCRichHits at " << location << endreq;
  }
  else
  {
    const LHCb::MCRichHits * mcHits = get<LHCb::MCRichHits>(location);
    debug() << "Successfully located " << mcHits->size()
            << " MCRichHits at " << location << endreq;
    unsigned int nHit(0);
    for ( LHCb::MCRichHits::const_iterator iHit = mcHits->begin();
          iHit != mcHits->end(); ++iHit, ++nHit )
    {
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "MCRichHit " << nHit << " " << *iHit
                << " container = " << location << endreq;
        std::cout << "   Data members " << **iHit << std::endl;
      }
      const LHCb::MCParticle * mcPart = (*iHit)->mcParticle();
      if ( mcPart )
      {
        const std::string mcploc = objectLocation(mcPart->parent());
        debug() << "  Parent MCParticle " << mcPart->key() << " " << mcPart
                << " container = " << mcploc << endreq;
      }
      else
      {
        Warning( "NULL pointer to parent MCParticle" );
      }
    }
  }

}

void DataObjVerifier::checkPhotsAt( const std::string & location ) const
{
  // MCRichHits
  if ( !exist<LHCb::MCRichOpticalPhotons>(location) )
  {
    debug() << "Cannot locate MCRichOpticalPhotons at " << location << endreq;
  }
  else
  {
    const LHCb::MCRichOpticalPhotons * mcPhots = get<LHCb::MCRichOpticalPhotons>(location);
    debug() << "Successfully located " << mcPhots->size()
            << " MCRichOpticalPhotons at " << location << endreq;
    for ( LHCb::MCRichOpticalPhotons::const_iterator iP = mcPhots->begin();
          iP != mcPhots->end(); ++iP )
    {
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "MCRichOpticalPhoton " << (*iP)->key() << " " << *iP
                << " container = " << location
                << endreq;
        std::cout << "   Data members " << **iP << std::endl;
      }
      // get associated MCRichHit
      const LHCb::MCRichHit * mchit = (*iP)->mcRichHit();
      if ( mchit )
      {
        const std::string mchloc = objectLocation(mchit->parent());
        debug() << "  Associated MCRichHit " << mchit
                << " container = " << mchloc << endreq;
      }
      else
      {
        Warning( "NULL pointer to associated MCRichHit" );
      }

    }
  }

}

// Main execution
StatusCode DataObjVerifier::execute()
{
  debug() << "Execute" << endreq;

  // MCRichHits
  if ( m_bdMCHits )
  {
    checkHitsAt (               LHCb::MCRichHitLocation::Default );
    checkHitsAt ( "Prev/"     + LHCb::MCRichHitLocation::Default );
    checkHitsAt ( "PrevPrev/" + LHCb::MCRichHitLocation::Default );
    checkHitsAt ( "Next/"     + LHCb::MCRichHitLocation::Default );
    checkHitsAt ( "NexNext/"  + LHCb::MCRichHitLocation::Default );
  }

  // MCRichOpticalPhotons
  if ( m_bdMCPhots )
  {
    checkPhotsAt (               LHCb::MCRichOpticalPhotonLocation::Default );
    checkPhotsAt ( "Prev/"     + LHCb::MCRichOpticalPhotonLocation::Default );
    checkPhotsAt ( "PrevPrev/" + LHCb::MCRichOpticalPhotonLocation::Default );
    checkPhotsAt ( "Next/"     + LHCb::MCRichOpticalPhotonLocation::Default );
    checkPhotsAt ( "NexNext/"  + LHCb::MCRichOpticalPhotonLocation::Default );
  }

  // MCRichDigits
  if ( m_bdMcDigits )
  {
    if ( !exist<LHCb::MCRichDigits>(LHCb::MCRichDigitLocation::Default) )
    {
      Warning("Cannot locate MCRichDigits at "+LHCb::MCRichDigitLocation::Default);
    }
    else
    {
      const LHCb::MCRichDigits * richMcDigits = get<LHCb::MCRichDigits>(LHCb::MCRichDigitLocation::Default);
      debug() << "Successfully located " << richMcDigits->size()
              << " MCRichDigits at " << LHCb::MCRichDigitLocation::Default << endreq;
      for ( LHCb::MCRichDigits::const_iterator imcDigit = richMcDigits->begin();
            imcDigit != richMcDigits->end(); ++imcDigit )
      {
        debug() << "MCRichDigit " << (*imcDigit)->key() << endreq;
        debug() << " MCRichDigitHits(" << (*imcDigit)->hits().size() << ") =";
        for ( LHCb::MCRichDigitHit::Vector::const_iterator iHit = (*imcDigit)->hits().begin();
              iHit != (*imcDigit)->hits().end(); ++iHit )
        {
          debug() << " " << *iHit;
        }
        debug() << endreq;
      }
    }
  } // end MCRichDigit print

  return StatusCode::SUCCESS;
}
