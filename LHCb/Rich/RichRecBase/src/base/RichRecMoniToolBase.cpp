
//-----------------------------------------------------------------------------
/** @file RichRecMoniToolBase.cpp
 *
 *  Implementation file for RICH reconstruction tool base class : RichRecMoniToolBase
 *
 *  CVS Log :-
 *  $Id: RichRecMoniToolBase.cpp,v 1.3 2005-06-23 15:13:05 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2005/01/13
 */
//-----------------------------------------------------------------------------

// local
#include "RichRecBase/RichRecMoniToolBase.h"

// Standard constructor, initializes variables
RichRecMoniToolBase::RichRecMoniToolBase( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : RichMoniToolBase ( type, name, parent ),
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

StatusCode RichRecMoniToolBase::initialize()
{
  // Initialise base class
  const StatusCode sc = RichMoniToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // Common initialisation
  #include "RichRecInitOptions.icpp"

  return sc;
}

StatusCode RichRecMoniToolBase::finalize()
{
  // base class finalize
  return RichMoniToolBase::finalize();
}
