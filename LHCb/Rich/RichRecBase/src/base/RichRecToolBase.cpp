
//-----------------------------------------------------------------------------
/** @file RichRecToolBase.cpp
 *
 *  Implementation file for RICH reconstruction tool base class : RichRecToolBase
 *
 *  CVS Log :-
 *  $Id: RichRecToolBase.cpp,v 1.12 2005-06-23 15:13:05 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-26
 */
//-----------------------------------------------------------------------------

// local
#include "RichRecBase/RichRecToolBase.h"

// Standard constructor, initializes variables
RichRecToolBase::RichRecToolBase( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : RichToolBase ( type, name, parent ),
    m_pixTool        ( 0 ),
    m_tkTool         ( 0 ),
    m_segTool        ( 0 ),
    m_photTool       ( 0 ),
    m_statTool       ( 0 ),
    m_ckAngleTool    ( 0 ),
    m_expTkSigTool   ( 0 ),
    m_exPhotSigTool  ( 0 ),
    m_ckAngleResTool ( 0 ),
    m_geomEffTool    ( 0 ),
    m_geometryTool   ( 0 )
{

  // job options
  declareProperty( "ProcessingStage", m_procStage = "Undefined" );

}

StatusCode RichRecToolBase::initialize()
{
  // Initialise base class
  const StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // Common initialisation
  #include "RichRecInitOptions.icpp"

  return sc;
}

StatusCode RichRecToolBase::finalize()
{
  // base class finalize
  return RichToolBase::finalize();
}
