
//-----------------------------------------------------------------------------
/** @file RichHPDPixelClusterSuppressionTool.cpp
 *
 * Implementation file for class : RichHPDPixelClusterSuppressionTool
 *
 * CVS Log :-
 * $Id: RichHPDPixelClusterSuppressionTool.cpp,v 1.1 2006-03-01 09:56:12 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 14/01/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichHPDPixelClusterSuppressionTool.h"

// Declaration of the Tool Factory
static const  ToolFactory<RichHPDPixelClusterSuppressionTool>          s_factory ;
const        IToolFactory& RichHPDPixelClusterSuppressionToolFactory = s_factory ;

// Standard constructor
RichHPDPixelClusterSuppressionTool::RichHPDPixelClusterSuppressionTool( const std::string& type,
                                                                        const std::string& name,
                                                                        const IInterface* parent )
  : RichToolBase   ( type, name, parent ),
    m_richSys      ( 0                  ),
    m_overallCheck ( 0 )
{

  // Define interface
  declareInterface<IRichPixelSuppressionTool>(this);

}

StatusCode RichHPDPixelClusterSuppressionTool::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // RichDet
  m_richSys = getDet<DeRichSystem>( DeRichLocation::RichSystem );

  // HPD supression
  acquireTool( "RichHPDSuppress", m_overallCheck, this );

  return sc;
}

StatusCode RichHPDPixelClusterSuppressionTool::finalize()
{


  // Execute base class method
  return RichToolBase::finalize();
}

bool
RichHPDPixelClusterSuppressionTool::
applyPixelSuppression( const LHCb::RichSmartID hpdID,
                       LHCb::RichSmartID::Vector & smartIDs ) const
{
  // check overall HPD suppression
  bool suppress = m_overallCheck->applyPixelSuppression( hpdID, smartIDs );
  if ( suppress ) return true;

  // Carry on with pixel cluster finding

  // return status
  return suppress;
}
