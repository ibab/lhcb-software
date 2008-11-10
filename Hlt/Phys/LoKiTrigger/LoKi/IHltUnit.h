// $Id: IHltUnit.h,v 1.1 2008-11-10 17:31:53 ibelyaev Exp $
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
// ============================================================================
// HltBase 
// ============================================================================
#include "HltBase/stringKey.h"
#include "HltBase/HltSelection.h"
// ============================================================================
namespace LoKi
{
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
    /** declare (the templated) selection
     *  @param key the key to be usef for selection 
     *  @return pointer to the registered seelction (or NULL)
     */
    template <class T>
    Hlt::Selection* declareSelection ( const stringKey& key ) 
    {
      Hlt::TSelection<T>* selection = new Hlt::TSelection<T>( key ) ;
      StatusCode sc = this->registerSelection ( selection ) ;
      if ( sc.isSuccess() ) { return selection ; }                    // RETURN 
      delete selection ;
      return 0 ;
    }
    // ========================================================================
    /** register the selection 
     *  (internal method, should not be invoked directly) 
     *  @param selection the seelction to be registered 
     *  @return status code 
     */
    virtual StatusCode registerSelection ( Hlt::Selection* selection ) = 0 ;
    // ========================================================================
    /** get the selection by key 
     *  @param key the key 
     *  @return pointer to the selection 
     */
    virtual Hlt::Selection* selection ( const stringKey& key ) const = 0 ;
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
// The END 
// ============================================================================
#endif // LOKI_IHLTFACADE_H
// ============================================================================
