
//-----------------------------------------------------------------------------
/** @file Rich1DTabProperty.cpp
 *
 *  Implementation file for class : Rich::TabulatedProperty1D
 *
 *  CVS Log :-
 *  $Id: Rich1DTabProperty.cpp,v 1.12 2009-08-03 09:22:37 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-26
 */
//-----------------------------------------------------------------------------

// local
#include "RichDet/Rich1DTabProperty.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

using namespace Rich;

//============================================================================
TabulatedProperty1D::~TabulatedProperty1D( )
{
  clearInterpolator();
  // Following lines cause a crash. ??
  //if (NULL != m_svcLocator) m_svcLocator->release();
  //if (NULL != m_msgSvc)     m_msgSvc->release();
  //if (NULL != m_updMgrSvc)  m_updMgrSvc->release();
}
//============================================================================

// Constructor from a tabulated property pointer
TabulatedProperty1D::TabulatedProperty1D( const TabulatedProperty * tab,
                                          const bool registerUMS,
                                          const gsl_interp_type * interType )
  : TabulatedFunction1D ( interType ),
    m_tabProp     ( tab  ),
    m_svcLocator  ( NULL ),
    m_msgSvc      ( NULL ),
    m_updMgrSvc   ( NULL )
{

  if ( registerUMS )
  {
    MsgStream msg( msgSvc(), "Rich::TabulatedProperty1D" );
    msg << MSG::DEBUG << "Registering UMS dependency for "
        << tabProperty()->name() << endmsg;

    // register update method
    try
    {
      TabulatedProperty * nonconsttab = const_cast<TabulatedProperty*>(tab);
      updMgrSvc()->registerCondition( this,
                                      nonconsttab,
                                      &TabulatedProperty1D::updateTabProp );
    }
    catch ( const GaudiException & excp )
    {
      msg << MSG::WARNING
          << tabProperty()->name() << " '" << excp.message() << "'" << endmsg;
    }

  }

  // initialise the underlying GSL interpolator
  m_OK = initInterpolator( tab, interType );

}

bool
TabulatedProperty1D::initInterpolator( const TabulatedProperty * tab,
                                       const gsl_interp_type * interType )
{
  // Check the data is valid
  if ( !tab ) throw GaudiException("Null Rich::TabulatedProperty",
                                   "*TabulatedProperty1D*", StatusCode::FAILURE );

  // set interpolator type
  if ( NULL != interType ) m_interType = interType;

  // copy data to internal container
  m_data.clear();
  for ( TabulatedProperty::Table::const_iterator it = tab->table().begin();
        it != tab->table().end(); ++it )
  {
    m_data[ (*it).first ] = (*it).second;
  }

  // init the underlying GSL interpolator
  return m_OK = ( this->TabulatedFunction1D::initInterpolator(interType) );
}

StatusCode TabulatedProperty1D::updateTabProp()
{
  // make a message object
  MsgStream msg( msgSvc(), "Rich::TabulatedProperty1D" );
  msg << MSG::INFO << "Update triggered for " << tabProperty()->name() << endmsg;

  // run the update
  m_OK = initInterpolator( tabProperty(), interType() );

  // check status of update
  if ( !m_OK )
  {
    msg << MSG::ERROR << "Update FAILED for " << tabProperty()->name() << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

// Services ------------------------------

ISvcLocator* TabulatedProperty1D::svcLocator()
{
  // get the Gaudi service locator
  if ( !m_svcLocator )
  {
    m_svcLocator = Gaudi::svcLocator();
    if ( 0 == m_svcLocator )
    {
      throw GaudiException( "ISvcLocator* points to NULL!",
                            "*TabulatedProperty1D*", StatusCode::FAILURE );
    }
  }
  return m_svcLocator;
}

IUpdateManagerSvc* TabulatedProperty1D::updMgrSvc()
{
  if ( !m_updMgrSvc )
  {
    const StatusCode sc = svcLocator()->service("UpdateManagerSvc", m_updMgrSvc);
    if ( sc.isFailure() )
    {
      throw GaudiException( "Could not locate UpdateManagerSvc",
                            "*TabulatedProperty1D*", StatusCode::FAILURE );
    }
  }
  return m_updMgrSvc;
}

IMessageSvc* TabulatedProperty1D::msgSvc()
{
  if ( !m_msgSvc )
  {
    const StatusCode sc = svcLocator()->service("MessageSvc", m_msgSvc);
    if ( sc.isFailure() )
    {
      throw GaudiException( "Could not locate MessageSvc",
                            "*TabulatedProperty1D*", StatusCode::FAILURE );
    }
  }
  return m_msgSvc;
}
