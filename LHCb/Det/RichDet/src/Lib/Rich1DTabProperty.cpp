
//-----------------------------------------------------------------------------
/** @file Rich1DTabProperty.cpp
 *
 *  Implementation file for class : Rich::TabulatedProperty1D
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
  // Unregister any UMS dependencies first
  if ( m_registedUMS ) updMgrSvc()->unregister(this);
  // clean up interpolators
  clearInterpolator();
  // Following lines cause a crash. ??
  //if (nullptr != m_svcLocator) m_svcLocator->release();
  //if (nullptr != m_msgSvc)     m_msgSvc->release();
  //if (nullptr != m_updMgrSvc)  m_updMgrSvc->release();
}
//============================================================================

// Constructor from a tabulated property pointer
TabulatedProperty1D::TabulatedProperty1D( const TabulatedProperty * tab,
                                          const bool registerUMS,
                                          const gsl_interp_type * interType )
  : TabulatedFunction1D ( interType ),
    m_tabProp           ( tab       )
{
  // initialise the underlying GSL interpolator
  m_OK = initInterpolator( tab, registerUMS, interType );
}

bool TabulatedProperty1D::configureUMS( const TabulatedProperty * tab )
{
  try
  {
    // Unregister any UMS dependencies first
    updMgrSvc()->unregister(this);
    m_registedUMS = false;
    
    // try registering updates
    auto * nonconsttab = const_cast<TabulatedProperty*>(tab);
    updMgrSvc()->registerCondition( this,
                                    nonconsttab,
                                    &TabulatedProperty1D::updateTabProp );

    // flag we correctly registered
    m_registedUMS = true;

  }
  catch ( const GaudiException & excp )
  {
    MsgStream msg( msgSvc(), "Rich::TabulatedProperty1D" );
    msg << MSG::WARNING
        << tabProperty()->name() << " '" << excp.message() << "'" << endmsg;
  }

  return m_registedUMS;
}

bool
TabulatedProperty1D::initInterpolator( const TabulatedProperty * tab,
                                       const bool registerUMS,
                                       const gsl_interp_type * interType )
{
  // Check the data is valid
  if ( !tab ) throw GaudiException("Null Rich::TabulatedProperty",
                                   "*TabulatedProperty1D*", StatusCode::FAILURE );

  // set interpolator type
  if ( nullptr != interType ) m_interType = interType;

  // UMS
  m_OK = ( registerUMS ? configureUMS(tab) : true );
  if ( !m_OK ) return m_OK;

  // copy data to internal container
  m_data.clear();
  for ( const auto & t : tab->table() ) { m_data[t.first] = t.second; }

  // init the underlying GSL interpolator
  m_OK = this->TabulatedFunction1D::initInterpolator(interType);

  // return
  return m_OK;
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
    if ( nullptr == m_svcLocator )
    {
      throw GaudiException( "ISvcLocator* points to nullptr!",
                            "*TabulatedProperty1D*", StatusCode::FAILURE );
    }
  }
  return m_svcLocator;
}

IUpdateManagerSvc* TabulatedProperty1D::updMgrSvc()
{
  if ( !m_updMgrSvc )
  {
    const auto sc = svcLocator()->service("UpdateManagerSvc", m_updMgrSvc);
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
    const auto sc = svcLocator()->service("MessageSvc", m_msgSvc);
    if ( sc.isFailure() )
    {
      throw GaudiException( "Could not locate MessageSvc",
                            "*TabulatedProperty1D*", StatusCode::FAILURE );
    }
  }
  return m_msgSvc;
}

double 
TabulatedProperty1D::rangeWarning( const double x, const double retx ) const
{
  std::cerr << "Rich::TabulatedProperty1D "
            << tabProperty()->name()
            << " : WARNING : Out-Of-Range x = " << x
            << " Valid Range = " << minX() << " to " << maxX() << std::endl;
  return retx;
}
