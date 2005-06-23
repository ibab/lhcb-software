
//-----------------------------------------------------------------------------
/** @file RichRecAlgBase.cpp
 *
 *  Implementation file for RICH reconstruction algorithm base class : RichRecAlgBase
 *
 *  CVS Log :-
 *  $Id: RichRecAlgBase.cpp,v 1.22 2005-06-23 15:13:05 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2003-05-10
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "RichRecBase/RichRecAlgBase.h"

// Standard constructor
RichRecAlgBase::RichRecAlgBase( const std::string& name,
                                ISvcLocator* pSvcLocator )
  : RichAlgBase ( name, pSvcLocator ),
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
RichRecAlgBase::~RichRecAlgBase() {};

// Initialise
StatusCode RichRecAlgBase::initialize()
{
  // Initialise base class
  const StatusCode sc = RichAlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  // Common initialisation
  #include "RichRecInitOptions.icpp"

  return sc;
}

// Main execute method
StatusCode RichRecAlgBase::execute()
{
  // All algorithms should re-implement this method
  return Error ( "Default RichRecAlgBase::execute() called !!" );
}

// Finalize
StatusCode RichRecAlgBase::finalize()
{
  //
  // Leave space to do something here later on if needed
  //

  // Finalize base class
  return RichAlgBase::finalize();
}
