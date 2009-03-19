// $Id: IHltUnit.h,v 1.4 2009-03-19 20:11:55 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_IHLTUNIT_H 
#define LOKI_IHLTUNIT_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
// #include <vector>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/IAlgorithm.h"
// ============================================================================
// HltBase 
// ============================================================================
#include "HltBase/stringKey.h"
#include "HltBase/HltSelection.h"
// ============================================================================
namespace Hlt 
{
  class IRegister  ;
  class IData      ; 
}
// ============================================================================
namespace LoKi
{
  // =========================================================================
  // the base for all functors 
  class AuxFunBase ; // the base for all functors 
  // ==========================================================================
  /** @class IHltUnit LoKi/IHltUnit.h
   *  Simple interface, suitable to act as facade unit for the Hlt Algorithm
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date   2008-11-10
   */
  class IHltUnit : virtual public IInterface 
  {
  public:
    // ========================================================================
    /// the actual type of functor 
    typedef LoKi::AuxFunBase Client ;             // the actual type of functor 
    /// the actual type of Key
    typedef stringKey        Key    ;       // the actual type of selection key 
    // ========================================================================
  public:
    // ========================================================================
    /** declare (the templated) selection
     *  @param key the key to be used for selection 
     *  @return pointer to the registered seelction (or NULL)
     */
    template <class T>
    Hlt::TSelection<T>* 
    declareOutput 
    ( const Key&    key    , 
      const Client& client ) const ;
    // ========================================================================
    /** register the selection 
     *  @param selection the selection to be registered 
     *  @param client the client 
     *  @return status code 
     */
    virtual StatusCode 
    registerOutput 
    ( Hlt::Selection* selection , 
      const Client&   client    ) const = 0 ;
    // ========================================================================
    /** declare the input selection 
     *  @param key the selection key 
     *  @param client the client 
     */
    virtual const Hlt::Selection* 
    declareInput 
    ( const Key&      key      , 
      const Client&   client   ) const = 0 ;
    // ========================================================================
    /** get the (const) selection by key 
     *  @param key the key 
     *  @return pointer to the selection 
     */
    virtual const Hlt::Selection* 
    selection 
    ( const Key&      key      , 
      const Client&   cleint   ) const = 0 ;
    // =========================================================================
  public:
    // =========================================================================
    /** get the (const) selection by key  (anonymous)  
     *  @param key the key 
     *  @return pointer to the selection 
     */
    virtual const Hlt::Selection* 
    selection ( const Key& key ) const = 0 ;
    // =========================================================================
  public:
    // =========================================================================
    /** get the selection by key (non-const)
     *  @param key the key 
     *  @return pointer to the selection 
     */
    virtual Hlt::Selection* 
    retrieve  
    ( const Client& client , 
      const Key&    key    ) const = 0 ;
    // ========================================================================
  public: // IInterface related stuff 
    // ========================================================================
    /// the unique interface identifier 
    static const InterfaceID& interfaceID() ;
    // ========================================================================
  protected:
    // ========================================================================
    /// virtual & protected destructor 
    virtual ~IHltUnit () ;                    // virtual & protected destructor 
    // ========================================================================
  } ;
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
/*  declare (the templated) selection
 *  @param key the key to be used for selection 
 *  @return pointer to the registered seelction (or NULL)
 */
// ============================================================================
template <class T>
Hlt::TSelection<T>* 
LoKi::IHltUnit::declareOutput 
( const LoKi::IHltUnit::Key&    key    , 
  const LoKi::IHltUnit::Client& client ) const 
{
  Hlt::TSelection<T>* selection = new Hlt::TSelection<T>( key ) ;
  StatusCode sc = this->registerOutput ( selection , client ) ;
  if ( sc.isSuccess() ) { return selection ; }                    // RETURN 
  delete selection ;
  return 0 ;
}
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_IHLTFACADE_H
// ============================================================================
