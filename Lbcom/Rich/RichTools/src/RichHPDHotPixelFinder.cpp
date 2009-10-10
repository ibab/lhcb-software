
//-----------------------------------------------------------------------------
/** @file RichHPDHotPixelFinder.cpp
 *
 * Implementation file for class : Rich::HPDHotPixelFinder
 *
 * CVS Log :-
 * $Id: RichHPDHotPixelFinder.cpp,v 1.1 2009-10-10 18:28:42 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 21/03/2006
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/ToolFactory.h"

// local
#include "RichHPDHotPixelFinder.h"

// RICH DAQ
using namespace Rich;

DECLARE_TOOL_FACTORY( HPDHotPixelFinder );

// Standard constructor
HPDHotPixelFinder::HPDHotPixelFinder( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : ToolBase ( type, name, parent ),
    m_nEvts  ( 0                  )
{

  // Define interface
  declareInterface<IGenericHPDAnalysisTool>(this);

  // job opts
  declareProperty( "HotOcc",            m_hotOcc = 0.9 );
  declareProperty( "NEventsForAverage", m_nEventsForAverage = 100 );

}

StatusCode HPDHotPixelFinder::initialize()
{
  // Sets up various tools and services
  StatusCode sc = ToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // incidents
  incSvc()->addListener( this, IncidentType::EndEvent );

  // return
  return sc;
}

StatusCode
HPDHotPixelFinder::analyse( const LHCb::RichSmartID hpdID,
                            const LHCb::RichSmartID::Vector & smartIDs,
                            IGenericHPDAnalysisTool::Results & results ) const
{
  // Loop over raw RichSmartIDs
  for ( LHCb::RichSmartID::Vector::const_iterator iR = smartIDs.begin();
        iR != smartIDs.end(); ++iR )
  {
    // count hits per channel
    ++m_hitCount[*iR];
  }

  // Do we have any results to return for this HPD
  if ( !m_results.empty() )
  {
    ResultCache::iterator iR = m_results.find(hpdID);
    if ( iR != m_results.end() )
    {
      if ( !iR->second.empty() )
      {
        // copy results to returned list
        results = iR->second;
        // clear internal list
        iR->second.clear();
      }
      // remove entry for this HPD from map
      m_results.erase(iR);
    }
  }

  return StatusCode::SUCCESS;
}

void HPDHotPixelFinder::handle( const Incident& )
{
  ++m_nEvts;

  // Look for hot pixels
  if ( m_nEvts == m_nEventsForAverage )
  {
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << "Searching for hot pixels after " << m_nEvts << " events" << endmsg;

    // clear current results
    m_results.clear();

    for ( HitCountMap::const_iterator iHPDC = m_hitCount.begin();
          iHPDC != m_hitCount.end(); ++iHPDC )
    {
      // HPD occupancy
      const double occ = (double)(iHPDC->second) / (double)m_nEventsForAverage;
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << iHPDC->first << " occupancy = " << 100.0*occ << " %" << endmsg;

      // Too hot ?
      if ( occ >= m_hotOcc )
      {
        if ( msgLevel(MSG::VERBOSE) )
          verbose() << "  -> pixel is hot ...." << endmsg;
        std::ostringstream mess;
        mess << iHPDC->first << " (" << (unsigned int)(iHPDC->first) << ") At least " 
             << 100.0*m_hotOcc
             << "% hot in " << m_nEventsForAverage << " events";
        // make result object
        IGenericHPDAnalysisTool::Result result;
        result.id      = iHPDC->first;
        result.message = mess.str();
        result.status  = StatusCode::FAILURE;
        m_results[iHPDC->first.hpdID()].push_back(result);
      }

    }

    // Reset counters
    m_nEvts = 0;
    m_hitCount.clear();

  }

}
