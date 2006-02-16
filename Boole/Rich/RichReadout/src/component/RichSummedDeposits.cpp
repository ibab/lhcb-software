
//===============================================================================
/** @file RichSummedDeposits.cpp
 *
 *  Implementation file for RICH digitisation algorithm : RichSummedDeposits
 *
 *  CVS Log :-
 *  $Id: RichSummedDeposits.cpp,v 1.3 2006-02-16 16:01:19 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @author Alex Howard   a.s.howard@ic.ac.uk
 *  @date   2003-11-06
 */
//===============================================================================

#include "RichSummedDeposits.h"

// Declaration of the Algorithm Factory
static const  AlgFactory<RichSummedDeposits>  s_factory;
const         IAlgFactory& RichSummedDepositsFactory = s_factory ;

// Standard constructor, initializes variables
RichSummedDeposits::RichSummedDeposits( const std::string& name,
                                        ISvcLocator* pSvcLocator )
  : RichAlgBase        ( name, pSvcLocator )
{
  declareProperty( "SummedDepositLocation",
                   m_RichSummedDepositLocation = MCRichSummedDepositLocation::Default );
  declareProperty( "DepositLocation",
                   m_RichDepositLocation = MCRichDepositLocation::Default );
}

RichSummedDeposits::~RichSummedDeposits() {};

StatusCode RichSummedDeposits::initialize()
{
  // Initialize base class
  const StatusCode sc = RichAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // randomn number generator
  if ( !m_rndm.initialize( randSvc(), Rndm::Flat(0.,1.) ) )
  {
    return Error( "Unable to create Random generator" );
  }

  return sc;
}

StatusCode RichSummedDeposits::execute()
{
  debug() << "Execute" << endreq;

  // Get containers of MCRichDeposits
  MCRichDeposits * deps = get<MCRichDeposits>( m_RichDepositLocation );
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Located " << deps->size() << " MCRichDeposts at " << m_RichDepositLocation
            << endreq;
  }

  // Form new containers of MCRichSummedDeposits
  MCRichSummedDeposits * sumDeps = new MCRichSummedDeposits();
  put( sumDeps, m_RichSummedDepositLocation );

  unsigned int nDuplicateDeps(0);

  // loop over deposits
  for ( MCRichDeposits::const_iterator iDep = deps->begin();
        iDep != deps->end(); ++iDep )
  {

    // RichSmartID for this deposit
    const RichSmartID id = (*iDep)->smartID();
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << "Deposit ID " << id << endreq;

    // Find out if we already have a hit for this super-pixel
    MCRichSummedDeposit * sumDep = sumDeps->object(id);
    if ( (*iDep)->history().prevPrevEvent() && sumDep )
    {
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << " --> simulating dead sub-pixels in PrevPrev" << endreq;
      // Toss a coin to see if we add this hit to the existing deposits
      // Simulate a 1/8 chance of additional hit falling in same sub-pixel as
      // already existing hit
      if ( 0 != static_cast<int>(m_rndm()*8.) ) 
      {
        if ( msgLevel(MSG::VERBOSE) )
          verbose() << "   --> Dumping deposit" << endreq;
        continue;
      }
    }

    // Add to the set of other deposits in the pixel
    if ( !sumDep )
    {
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << " --> No Summed deposit. Creating one" << endreq;
      sumDep = new MCRichSummedDeposit();
      sumDeps->insert( sumDep, id );
    }
    else
    {
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << " --> Found existing Summed deposit" << endreq;
      ++nDuplicateDeps;
    }

    sumDep->addToDeposits( *iDep );

    // summed energy
    sumDep->setSummedEnergy( sumDep->summedEnergy() + (*iDep)->energy() );

    // copy current history to local object to update
    MCRichDigitHistoryCode hist = sumDep->history();

    // add deposit history to summed deposit
    hist.setFlags( (*iDep)->history() );

    // Update history in sum dep
    sumDep->setHistory( hist );

  }

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Created " << sumDeps->size() << " MCRichSummedDeposits from " 
            << deps->size() << " MCRichDeposts" << endreq
            << "Found " << nDuplicateDeps << " deposits in the same pixel" << endreq;
  }

  return StatusCode::SUCCESS;
}

StatusCode RichSummedDeposits::finalize()
{
  // finalize random number generator
  m_rndm.finalize();

  // finalize base class
  return RichAlgBase::finalize();
}

