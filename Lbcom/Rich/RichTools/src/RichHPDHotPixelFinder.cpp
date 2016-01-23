
//-----------------------------------------------------------------------------
/** @file RichHPDHotPixelFinder.cpp
 *
 * Implementation file for class : Rich::HPDHotPixelFinder
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 21/03/2006
 */
//-----------------------------------------------------------------------------

// local
#include "RichHPDHotPixelFinder.h"

// RICH DAQ
using namespace Rich;

DECLARE_TOOL_FACTORY( HPDHotPixelFinder )

// Standard constructor
HPDHotPixelFinder::HPDHotPixelFinder( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : ToolBase ( type, name, parent )
{
  // Define interface
  declareInterface<IGenericHPDAnalysisTool>(this);
  // job opts
  declareProperty( "HotOcc",            m_hotOcc            = 0.5 );
  declareProperty( "NEventsForAverage", m_nEventsForAverage = 100 );
}

StatusCode HPDHotPixelFinder::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = ToolBase::initialize();
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
  // Loop over raw RichSmartIDs and count hits per channel
  for ( const auto & id : smartIDs ) { ++m_hitCount[id]; }

  // Do we have any results to return for this HPD ?
  if ( !m_results.empty() )
  {
    const auto iR = m_results.find(hpdID);
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
  // Only one sort of incident subscribed to (EndEvent)
  // so no need to check type here.

  // Look for hot pixels every now and then
  if ( ++m_nEvts == m_nEventsForAverage ) { findHotPixels(); }
}

void HPDHotPixelFinder::findHotPixels()
{

  _ri_verbo << "Searching for hot pixels after " << m_nEvts << " events" << endmsg;

  // clear current results
  m_results.clear();

  for ( const auto& HPDC : m_hitCount )
  {
    // HPD occupancy
    const auto occ = (double)(HPDC.second) / (double)m_nEventsForAverage;
    _ri_verbo << HPDC.first << " occupancy = " << 100.0*occ << " %" << endmsg;

    // Too hot ?
    if ( occ >= m_hotOcc )
    {
      _ri_verbo << "  -> pixel is hot ...." << endmsg;
      std::ostringstream mess;
      mess << HPDC.first << " (" << (unsigned int)(HPDC.first) << ") At least "
           << 100.0*m_hotOcc
           << "% hot in " << m_nEventsForAverage << " events";
      // make result object
      auto & results = m_results[HPDC.first.pdID()];
      results.emplace_back( IGenericHPDAnalysisTool::Result() );
      auto & result = results.back();
      result.id      = HPDC.first;
      result.message = mess.str();
      result.status  = StatusCode::FAILURE;
      result.status.ignore(); // To (try and) avoid untested StatusCode warnings
    }

  }

  // Reset counters
  m_nEvts = 0;
  m_hitCount.clear();

}
