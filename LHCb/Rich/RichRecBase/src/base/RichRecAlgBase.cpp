// $Id: RichRecAlgBase.cpp,v 1.13 2004-06-10 14:14:49 jonesc Exp $

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "RichRecBase/RichRecAlgBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRecAlgBase
//
// 05/04/2002 : Chris Jones    Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Standard constructor
RichRecAlgBase::RichRecAlgBase( const std::string& name,
                                ISvcLocator* pSvcLocator )
  : RichAlgBase ( name, pSvcLocator ),
    m_pixTool      ( 0 ),
    m_tkTool       ( 0 ),
    m_segTool      ( 0 ),
    m_photTool     ( 0 ),
    m_statTool     ( 0 ) { }

// Destructor
RichRecAlgBase::~RichRecAlgBase() {};

// Initialise
StatusCode RichRecAlgBase::initialize()
{
  // Initialise base class
  StatusCode sc = RichAlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  //
  // Leave space to do something here later on if needed
  //

  return StatusCode::SUCCESS;
};

// Finalize
StatusCode RichRecAlgBase::finalize()
{
  //
  // Leave space to do something here later on if needed
  //

  // Finalize base class
  return RichAlgBase::finalize();
}
