
//-----------------------------------------------------------------------------
/** @file RichRecMoniAlgBase.cpp
 *
 *  Implementation file for RICH reconstruction monitor
 *  algorithm base class : RichRecMoniAlgBase
 *
 *  CVS Log :-
 *  $Id: RichRecMoniAlgBase.cpp,v 1.4 2005-06-23 15:13:05 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2005/01/13
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "RichRecBase/RichRecMoniAlgBase.h"

// Standard constructor
RichRecMoniAlgBase::RichRecMoniAlgBase( const std::string& name,
                                        ISvcLocator* pSvcLocator )
  : RichMoniAlgBase  ( name, pSvcLocator ),
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

// Destructor
RichRecMoniAlgBase::~RichRecMoniAlgBase() {};

// Initialise
StatusCode RichRecMoniAlgBase::initialize()
{
  // Initialise base class
  const StatusCode sc = RichMoniAlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  // Common initialisation
  #include "RichRecInitOptions.icpp"

  return sc;
}

// Main execute method
StatusCode RichRecMoniAlgBase::execute()
{
  // All algorithms should re-implement this method
  return Error ( "Default RichRecMoniAlgBase::execute() called !!" );
}

// Finalize
StatusCode RichRecMoniAlgBase::finalize()
{
  //
  // Leave space to do something here later on if needed
  //

  // Finalize base class
  return RichMoniAlgBase::finalize();
}
