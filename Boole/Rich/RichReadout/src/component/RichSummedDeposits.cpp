
//===============================================================================
/** @file RichSummedDeposits.cpp
 *
 *  Implementation file for RICH digitisation algorithm : SummedDeposits
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @author Alex Howard   a.s.howard@ic.ac.uk
 *  @date   2003-11-06
 */
//===============================================================================

#include "RichSummedDeposits.h"

// From Gaudi
#include "GaudiKernel/AlgFactory.h"

using namespace Rich::MC::Digi;

DECLARE_ALGORITHM_FACTORY( SummedDeposits )

// Standard constructor, initializes variables
SummedDeposits::SummedDeposits( const std::string& name,
                                ISvcLocator* pSvcLocator )
  : Rich::AlgBase ( name, pSvcLocator )
{
  declareProperty( "SummedDepositLocation",
                   m_RichSummedDepositLocation = LHCb::MCRichSummedDepositLocation::Default );
  declareProperty( "DepositLocation",
                   m_RichDepositLocation = LHCb::MCRichDepositLocation::Default );
}

SummedDeposits::~SummedDeposits() {}

StatusCode SummedDeposits::initialize()
{
  // Initialize base class
  StatusCode sc = Rich::AlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // randomn number generator
  sc = m_rndm.initialize( randSvc(), Rndm::Flat(0.,1.) );
  if ( sc.isFailure() )
  {
    return Error( "Unable to create Random generator" );
  }

  return sc;
}

StatusCode SummedDeposits::execute()
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "Execute" << endmsg;

  // Get containers of MCRichDeposits
  LHCb::MCRichDeposits * deps = get<LHCb::MCRichDeposits>( m_RichDepositLocation );
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Located " << deps->size() << " MCRichDeposts at " << m_RichDepositLocation
            << endmsg;
  }

  // Form new containers of MCSummedDeposits
  LHCb::MCRichSummedDeposits * sumDeps = new LHCb::MCRichSummedDeposits();
  put( sumDeps, m_RichSummedDepositLocation );

  unsigned int nDuplicateDeps(0);

  // loop over deposits
  for ( LHCb::MCRichDeposits::const_iterator iDep = deps->begin();
        iDep != deps->end(); ++iDep )
  {

    // RichSmartID for this deposit
    const LHCb::RichSmartID id = (*iDep)->smartID();
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << "Deposit ID " << id << endmsg;

    // Find out if we already have a hit for this super-pixel
    LHCb::MCRichSummedDeposit * sumDep = sumDeps->object(id);
    if ( (*iDep)->history().prevPrevEvent() && sumDep )
    {
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << " --> simulating dead sub-pixels in PrevPrev" << endmsg;
      // Toss a coin to see if we add this hit to the existing deposits
      // Simulate a 1/8 chance of additional hit falling in same sub-pixel as
      // already existing hit
      if ( 0 != static_cast<int>(m_rndm()*8.) )
      {
        if ( msgLevel(MSG::VERBOSE) )
          verbose() << "   --> Dumping deposit" << endmsg;
        continue;
      }
    }

    // Add to the set of other deposits in the pixel
    if ( !sumDep )
    {
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << " --> No Summed deposit. Creating one" << endmsg;
      sumDep = new LHCb::MCRichSummedDeposit();
      sumDeps->insert( sumDep, id );
    }
    else
    {
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << " --> Found existing Summed deposit" << endmsg;
      ++nDuplicateDeps;
    }

    sumDep->addToDeposits( *iDep );

    // summed energy
    sumDep->setSummedEnergy( sumDep->summedEnergy() + (*iDep)->energy() );

    // copy current history to local object to update
    LHCb::MCRichDigitHistoryCode hist = sumDep->history();

    // add deposit history to summed deposit
    hist.setFlags( (*iDep)->history() );

    // Update history in sum dep
    sumDep->setHistory( hist );

  }

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Created " << sumDeps->size() << " MCRichSummedDeposits from "
            << deps->size() << " MCRichDeposts" << endmsg
            << "Found " << nDuplicateDeps << " deposits in the same pixel" << endmsg;
  }

  return StatusCode::SUCCESS;
}

StatusCode SummedDeposits::finalize()
{
  // finalize random number generator
  const StatusCode sc = m_rndm.finalize();
  if ( sc.isFailure() ) Warning( "Failed to finalise random number generator" ).ignore();

  // finalize base class
  return Rich::AlgBase::finalize();
}

