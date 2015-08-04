
//-----------------------------------------------------------------------------
/** @file RichHighOccHPDSuppressionTool.cpp
 *
 * Implementation file for class : RichHighOccHPDSuppressionTool
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 21/03/2006
 */
//-----------------------------------------------------------------------------

// local
#include "RichHighOccHPDSuppressionTool.h"

// RICH DAQ
using namespace Rich::DAQ;

DECLARE_TOOL_FACTORY( HighOccHPDSuppressionTool )

// Standard constructor
HighOccHPDSuppressionTool::
HighOccHPDSuppressionTool( const std::string& type,
                           const std::string& name,
                           const IInterface* parent )
  : HPDOccupancyTool ( type, name, parent ),
    m_richSys        ( NULL               )
{

  // Define interface
  declareInterface<IPixelSuppressionTool>(this);

  // job ops
  declareProperty( "PrintHPDSuppressions", m_sumPrint  = true   );
  declareProperty( "AbsoluteMaxHPDOcc", m_overallMax   = 100    );
  declareProperty( "OccCutScaleFactor", m_scale        = 4      );

}

StatusCode HighOccHPDSuppressionTool::initialize()
{
  // Sets up various tools and services
  StatusCode sc = HPDOccupancyTool::initialize();
  if ( sc.isFailure() ) return sc;

  // RichDet
  m_richSys = getDet<DeRichSystem>( DeRichLocations::RichSystem );

  // summary printout of options
  if( msgLevel(MSG::DEBUG) )
    debug() << "  Occupancy scale factor                = " << m_scale << std::endl
            << "  Absolute max HPD occupancy            = " << m_overallMax << endmsg;

  // return
  return sc;
}

bool
HighOccHPDSuppressionTool::
applyPixelSuppression( const LHCb::RichSmartID hpdID,
                       LHCb::RichSmartID::Vector & smartIDs ) const
{
  // default is no suppression
  bool suppress = false;

  // HPD ID OK ?
  if ( !hpdID.isValid() ) return suppress;

  // Get occupancy HPD data
  HPDData & data = hpdData(hpdID);

  // Occupancy for this HPD in current event
  const unsigned int occ = smartIDs.size();

  // is this HPD suppressed
  suppress = ( data.fillCount() >= m_minFills &&
               (occ > m_overallMax || occ > data.avOcc()*m_scale) );
  if ( suppress )
  {
    // Print message
    if ( m_sumPrint )
    {
      std::ostringstream hpd;
      hpd << hpdID;
      Info( "Fully suppressed     "+hpd.str(), StatusCode::SUCCESS, 0 ).ignore();
    }
    // clear vector (i.e. actually do the suppression)
    smartIDs.clear();
  }

  // return status
  return suppress;
}
