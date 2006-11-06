
//===============================================================================
/** @file RichSimpleChargeSharing.cpp
 *
 *  Implementation file for RICH digitisation algorithm : RichSimpleChargeSharing
 *
 *  CVS Log :-
 *  $Id: RichSimpleChargeSharing.cpp,v 1.9 2006-11-06 09:41:56 cattanem Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   23/01/2006
 */
//===============================================================================

#include "RichSimpleChargeSharing.h"

// From Gaudi
#include "GaudiKernel/AlgFactory.h"

DECLARE_ALGORITHM_FACTORY( RichSimpleChargeSharing );

// Standard constructor, initializes variables
RichSimpleChargeSharing::RichSimpleChargeSharing( const std::string& name,
                                                  ISvcLocator* pSvcLocator )
  : RichAlgBase        ( name, pSvcLocator )
{
  declareProperty( "DepositLocation",
                   m_RichDepositLocation = MCRichDepositLocation::Default );
  declareProperty( "ChargeShareFraction", m_shareFrac = 0.025 );
}

RichSimpleChargeSharing::~RichSimpleChargeSharing() {};

StatusCode RichSimpleChargeSharing::initialize()
{
  // Initialize base class
  const StatusCode sc = RichAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // randomn number generator
  if ( !m_rndm.initialize( randSvc(), Rndm::Flat(0.,1.) ) )
  {
    return Error( "Unable to create Random generator" );
  }

  // tools
  acquireTool( "RichSmartIDTool", m_smartIDTool, 0, true );

  // printout
  info() << "Will add charge sharing at " << 100*m_shareFrac << " % level" << endreq;

  return sc;
}

StatusCode RichSimpleChargeSharing::execute()
{
  debug() << "Execute" << endreq;

  // Get containers of MCRichDeposits
  MCRichDeposits * deps = get<MCRichDeposits>( m_RichDepositLocation );

  // Temporary container of MCRichDeposits to add
  MCRichDeposit::Vector depsToAdd;

  // loop over deposits
  for ( MCRichDeposits::const_iterator iDep = deps->begin();
        iDep != deps->end(); ++iDep )
  {

    // do we charge share for this pixel ?
    if ( m_shareFrac > m_rndm() )
    {

      // RichSmartID for this deposit
      const RichSmartID id = (*iDep)->smartID();

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
      if ( nTries >= 100 ) { Warning( "Charge sharing loop maxed out !" ); continue; }

      // Create new deposit with same info
      MCRichDeposit * newDep = new MCRichDeposit( *iDep );
      depsToAdd.push_back( newDep );

      // Update RichSmartID in new deposit
      RichSmartID newid = id;
      newid.setPixelRow(row);
      newid.setPixelCol(col);
      newDep->setSmartID( newid );

      // add charge sharing flag
      MCRichDigitHistoryCode hist = newDep->history();
      hist.setChargeShareHit(true);
      newDep->setHistory(hist);

    } // do charge sharing

  } // loop over segments

  // Loop over new deposit and add to main container
  for ( MCRichDeposit::Vector::iterator iDep = depsToAdd.begin();
        iDep != depsToAdd.end(); ++iDep )
  {
    deps->insert( *iDep );
  }

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Created " << depsToAdd.size()
            << " new charge share MCRichDeposits" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichSimpleChargeSharing::finalize()
{
  // finalize random number generator
  m_rndm.finalize();

  // finalize base class
  return RichAlgBase::finalize();
}

