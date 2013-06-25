
//===============================================================================
/** @file RichSimpleChargeSharing.cpp
 *
 *  Implementation file for RICH digitisation algorithm : RichSimpleChargeSharing
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   23/01/2006
 */
//===============================================================================

#include "RichSimpleChargeSharing.h"

// From Gaudi
#include "GaudiKernel/AlgFactory.h"

using namespace Rich::MC::Digi;

DECLARE_ALGORITHM_FACTORY( SimpleChargeSharing )

// Standard constructor, initializes variables
SimpleChargeSharing::SimpleChargeSharing( const std::string& name,
                                          ISvcLocator* pSvcLocator )
  : Rich::AlgBase ( name, pSvcLocator )
  , m_smartIDTool(NULL)
{
  declareProperty( "DepositLocation",
                   m_RichDepositLocation = LHCb::MCRichDepositLocation::Default );
  declareProperty( "ChargeShareFraction", m_shareFrac = 0.025 );
}

SimpleChargeSharing::~SimpleChargeSharing() {}

StatusCode SimpleChargeSharing::initialize()
{
  // Initialize base class
  const StatusCode sc = Rich::AlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // randomn number generator
  if ( !m_rndm.initialize( randSvc(), Rndm::Flat(0.,1.) ) )
  {
    return Error( "Unable to create Random generator" );
  }

  // tools
  acquireTool( "RichSmartIDTool", m_smartIDTool, 0, true );

  // printout
  info() << "Will add charge sharing at " << 100*m_shareFrac << " % level" << endmsg;

  return sc;
}

StatusCode SimpleChargeSharing::execute()
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "Execute" << endmsg;

  // Get containers of MCRichDeposits
  LHCb::MCRichDeposits * deps = get<LHCb::MCRichDeposits>( m_RichDepositLocation );

  // Temporary container of MCRichDeposits to add
  LHCb::MCRichDeposit::Vector depsToAdd;

  // loop over deposits
  for ( LHCb::MCRichDeposits::const_iterator iDep = deps->begin();
        iDep != deps->end(); ++iDep )
  {

    // do we charge share for this pixel ?
    if ( m_shareFrac > m_rndm() )
    {

      // RichSmartID for this deposit
      const LHCb::RichSmartID id = (*iDep)->smartID();

      int col(0), row(0);
      bool tryAgain ( true );
      int nTries = 0; // protect against infinite loops
      while ( tryAgain && nTries<100 )
      {
        ++nTries;

        // get current row and column info from pixel
        col = id.pixelCol();
        row = id.pixelRow();

        // Decide if we add this pixel up/down/left/right
        const double rand = m_rndm();
        if      ( rand < 0.25 ) { ++row; }
        else if ( rand < 0.5  ) { --row; }
        else if ( rand < 0.75 ) { ++col; }
        else                    { --col; }

        // check we are still in a valid pixel ( i.e. not on the edge )
        if ( row >= 0 && row <= 31 && col >= 0 && col <= 31 )
        {
          // valid pixel data
          tryAgain = false;
        }

      } // while loop
      if ( nTries >= 100 ) { Warning( "Charge sharing loop maxed out !" ).ignore(); continue; }

      // Create new deposit with same info
      LHCb::MCRichDeposit * newDep = new LHCb::MCRichDeposit( *iDep );
      depsToAdd.push_back( newDep );

      // Update RichSmartID in new deposit
      LHCb::RichSmartID newid = id;
      newid.setPixelRow(row);
      newid.setPixelCol(col);
      newDep->setSmartID( newid );

      // add charge sharing flag
      LHCb::MCRichDigitHistoryCode hist = newDep->history();
      hist.setChargeShareHit(true);
      newDep->setHistory(hist);

    } // do charge sharing

  } // loop over segments

  // Loop over new deposits and add to main container
  for ( LHCb::MCRichDeposit::Vector::iterator iDep = depsToAdd.begin();
        iDep != depsToAdd.end(); ++iDep )
  {
    deps->insert( *iDep );
  }

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Created " << depsToAdd.size()
            << " new charge share MCRichDeposits" << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode SimpleChargeSharing::finalize()
{
  // finalize random number generator
  const StatusCode sc = m_rndm.finalize();
  if ( sc.isFailure() ) Warning( "Failed to finalise random number generator" ).ignore();

  // finalize base class
  return Rich::AlgBase::finalize();
}

