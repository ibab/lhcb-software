// $Id: TESCheck.cpp,v 1.7 2009-10-08 15:21:37 cattanem Exp $
// ============================================================================
// Include files
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/DataObject.h"
// ============================================================================

// ============================================================================
/** @file
 *
 *  Definition and implementation of class TESCheck
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2004-09-16
 */
// ============================================================================
/** @class TESCheck
 *  Simple class to check the existence of
 *  objects in Gaudi Transient stores
 *  The actual object type is not checked.
 *
 *  Also the algorithm  could be used to "preload" objects.
 *  For MSG::DEBUG level the printout of object location and the actula type
 *  is performed
 *
 *  Algorithm properties:
 *
 *   @li <tt>Inputs</tt>  vector of locations to be checked,
 *                        the default value is empty vector
 *
 *   @li <tt>Store</tt>   numerical identifier for Gaudi Transient store,
 *                        0 : Event Store ( default )
 *                        1 : Detector Store
 *                        2 : Histogram Store
 *
 *   @li <tt>Stop</tt>    flag which dictates to return error code if
 *                        data is not available (default) or just
 *                        print a warning message. The second options
 *                        can be useful for interactive environment
 *
 *  Usage:
 *
 *  @code
 *
 *  //
 *  ApplicationMgr.TopAlg += [ 'TESCheck/EvtCheck' ]
 *  EvtCheck.Inputs = { "MC/Calo" } ;
 *  EvtCheck.Inputs = { "MC/Velo" } ;
 *  //
 *  // check detector element
 *  ApplicationMgr.TopAlg += [ 'TESCheck/DetCheck' ]
 *  DetCheck.Inputs = { "/dd/Structure/LHCb" } ;
 *  // SWITCH to use Detector Data Store instead of Event Data Store
 *  DetCheck.DettectorStore = true ;
 *
 *  @endcode
 *
 *  Indeed it is a bit updated version of
 *     CaloCheckObjectsAlg from Calo/CaloReco package
 */
// ============================================================================
class TESCheck : public GaudiAlgorithm
{
  friend class AlgFactory<TESCheck>;
public:
  typedef StringArrayProperty       Inputs ;
  enum Stores
    {
      EvtStore = 0 ,
      DetStore     ,
      HstStore
    };

protected:
  typedef std::vector<std::string> _Inputs ;
public:

  /// execute the algorithm itself
  virtual StatusCode execute() ;

protected:

  /** constructor
   *  @param name algorithm instance name
   *  @param pSvc pointer to Service Locator
   */
  TESCheck ( const std::string& name ,
             ISvcLocator*       pSvc )
    : GaudiAlgorithm ( name , pSvc )
    , m_inputs       (          )
    , m_store        ( EvtStore )
    , m_stop         ( true     )
  {
    declareProperty  ( "Inputs" , m_inputs ) ;
    declareProperty  ( "Store"  , m_store  ) ;
    declareProperty  ( "Stop"   , m_stop   ) ;
    //
    m_inputs.declareUpdateHandler (&TESCheck::propHndl , this);
  };
  /// virtual destructor
  virtual ~TESCheck() {};

public:

  /// a technical - to be able to modify it interactively, e.g. in Python
  void propHndl ( Property&  /* p */ ) {};

private:

  // default constructor is disabled
  TESCheck() ;
  // copy constructor is disabled
  TESCheck           ( const TESCheck& ) ;
  // assignement operator is disabled
  TESCheck& operator=( const TESCheck& ) ;

private:
  Inputs  m_inputs ;
  int     m_store  ;
  bool    m_stop   ;
};
// ============================================================================

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TESCheck )

// ============================================================================
/// execute the algorithm itself
// ============================================================================
StatusCode TESCheck::execute()
{
  const _Inputs& inputs = m_inputs.value() ;

  for ( _Inputs::const_iterator i = inputs.begin() ; inputs.end() != i ; ++i )
  {
    const std::string& address = *i ;

    SmartIF<IDataProviderSvc> dp;
    switch(m_store) {
    case DetStore:
      dp = detSvc();
      break;
    case HstStore:
      dp = histoSvc();
      break;
    case EvtStore:
    default:
      dp = evtSvc();
    }

    SmartDataPtr<DataObject> obj( dp , address ) ;
    DataObject* o = obj ;
    if ( 0 == o  )
    {
      if ( m_stop ) { return Error ( "Check failed for '" + address + "'" ) ; }
      if ( msgLevel ( MSG::WARNING ) )
        Warning ( "Check failed for '" + address  + "'", StatusCode::SUCCESS, 1 ).ignore() ;
      setFilterPassed(false) ;
    }
    else {
      setFilterPassed(true) ;
      if ( msgLevel ( MSG::DEBUG ) )
      {
        debug() << "Object address/type : "
                << "'" << address << "'/'"
                << System::typeinfoName( typeid( *o ) ) << "'" << endmsg ;
      }
    }
    
  }

  return StatusCode::SUCCESS;
}
// ============================================================================

// ============================================================================
// The END
// ============================================================================
