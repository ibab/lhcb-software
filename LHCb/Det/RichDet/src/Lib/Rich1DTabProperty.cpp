
//-----------------------------------------------------------------------------
/** @file Rich1DTabProperty.cpp
 *
 *  Implementation file for class : Rich1DTabProperty
 *
 *  CVS Log :-
 *  $Id: Rich1DTabProperty.cpp,v 1.2 2006-03-13 17:47:43 jonrob Exp $
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

//============================================================================
Rich1DTabProperty::~Rich1DTabProperty( ) 
{ 
  clearInterpolator();   
  //if (NULL != m_svcLocator) m_svcLocator->release();
  //if (NULL != m_msgSvc)     m_msgSvc->release();
  //if (NULL != m_updMgrSvc)  m_updMgrSvc->release();
}
//============================================================================

// Constructor from a tabulated property pointer
Rich1DTabProperty::Rich1DTabProperty( const TabulatedProperty * tab,
                                      const bool registerUMS,
                                      const gsl_interp_type * interType )
  : Rich1DTabFunc ( interType ),
    m_tabProp     ( tab       ),
    m_svcLocator  ( NULL ),
    m_msgSvc      ( NULL ),
    m_updMgrSvc   ( NULL )
{

  // Check the data is valid
  if ( !tab ) throw GaudiException("Null TabulatedProperty",
                                   "*Rich1DTabProperty*", StatusCode::FAILURE );

  // copy data to internal container
  for ( TabulatedProperty::Table::const_iterator it = tab->table().begin();
        it != tab->table().end(); ++it )
  {
    m_data[ (*it).first ] = (*it).second;
  }

  if ( registerUMS )
  {
    MsgStream msg( msgSvc(), "Rich1DTabProperty" );
    msg << MSG::DEBUG << "Registry UMS dependency for " 
        << tabProperty()->name() << endreq;

    // register update method
    try
    {
      TabulatedProperty * nonconsttab = const_cast<TabulatedProperty*>(tab);
      updMgrSvc()->registerCondition( this, nonconsttab, 
                                      &Rich1DTabProperty::updateTabProp );
    }
    catch ( const GaudiException & excp )
    {
      msg << MSG::WARNING 
          << tabProperty()->name() << " '" << excp.message() << "'" << endreq;
    }

  }

  // initialise the underlying GSL interpolator
  m_OK = initInterpolator( interType );

}

StatusCode Rich1DTabProperty::updateTabProp()
{
  MsgStream msg( msgSvc(), "Rich1DTabProperty" );
  msg << MSG::INFO << "Update triggered for " << tabProperty()->name() << endreq;
  m_OK = initInterpolator( interType() );
  return StatusCode::SUCCESS;
}

// Services ------------------------------

ISvcLocator* Rich1DTabProperty::svcLocator()
{
  // get the Gaudi service locator
  if ( !m_svcLocator )
  {
    m_svcLocator = Gaudi::svcLocator();
  }
  return m_svcLocator;
}

IUpdateManagerSvc* Rich1DTabProperty::updMgrSvc()
{
  if ( !m_updMgrSvc )
  {
    const StatusCode sc = svcLocator()->service("UpdateManagerSvc" , m_updMgrSvc);
    if ( sc.isFailure() )
    {
      throw GaudiException ( "Could not locate UpdateManagerSvc",
                             "*Rich1DTabProperty*" , StatusCode::FAILURE);
    }
  }
  return m_updMgrSvc;
}

IMessageSvc* Rich1DTabProperty::msgSvc()
{
  if ( !m_msgSvc )
  {
    const StatusCode sc = svcLocator()->service("MessageSvc" , m_msgSvc);
    if ( sc.isFailure() )
    {
      throw GaudiException ( "Could not locate MessageSvc",
                             "*Rich1DTabProperty*" , StatusCode::FAILURE);
    }
  }
  return m_msgSvc;
}
