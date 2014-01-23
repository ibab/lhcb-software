// $Id: IHltRegister.h,v 1.1 2009-12-23 10:26:44 graven Exp $
// ============================================================================
#ifndef IHLTREGISTER_H
#define IHLTREGISTER_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/StringKey.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Interface.h"
// ============================================================================
// HltBase
// ============================================================================
// ============================================================================
// forward declarations
// ============================================================================
namespace Hlt {
class Selection;
}
namespace Hlt {
template <class TYPE> class TSelection;
}
class IAlgorithm;
// ============================================================================
namespace Hlt {
// ==========================================================================
/** @class IRegister LoKi/IHltRegister.h
 *  An abstract interface, whch allow register the Hlt-selection(s)
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2009-03-16
 */
class IRegister : virtual public IInterface {
  public:
    // ========================================================================
    /// the actual type of the key
    typedef Gaudi::StringKey Key; // the actual type of the key
    // ========================================================================
  public: // error codes
    // ========================================================================
    enum Errors {
        // ====================================================================
        Lock_Invalid_Algorithm = 500,
        Lock_Transactions_Disabled,
        Lock_Service_Already_Locked,
        Lock_Double_Registration,
        Lock_Invalid_Lock,
        // ====================================================================
        Register_Invalid_Producer,
        Register_Invalid_Consumer,
        Register_Invalid_Selection,
        Register_Invalid_Lock,
        Register_Double_Registration,
        Register_Unknown_Selection
        // ====================================================================
    };
    // ========================================================================
  public: // helper class to lock/unlock the transactions
    // ========================================================================
    /** helper class to lock properly the transactions
     *
     *  @code
     *
     *  // open the scope
     *  {
     *   Hlt::IRegister* svc = ... ;   // get the servce
     *
     *   // lock the service & open the transactions
     *   Hlt::IRegister::Lock lock ( svc , this ) ; // 'this' is an algorithm
     *
     *   StatusCodde sc = lock -> registerInput  ( ... ) ;
     *   ...
     *   sc             = lock -> registerInput  ( ... ) ;
     *   ...
     *   sc             = lock -> registerOutput ( ... ) ;
     *   ...
     *   sc             = lock -> registerOutput ( ... ) ;
     *
     *  } // end of the scope, unlock the service, close transactions
     *
     *  @endcode
     *
     *  @see Hlt::IRegister
     *  @see Hlt::IRegister::lock
     *  @see Hlt::IRegister::unlock
     *  @see Hlt::IRegister::registerInput
     *  @see Hlt::IRegister::registerOutput
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-03-16
     */
    class Lock {
        // ====================================================================
      public:
        // ====================================================================
        /** constructor from the registrator and algorithm
         *  open&lock  the transaction
         *  @exception throw GaudiException for invalid Hlt::IRrgister::lock
         *  @exception throw GaudiException for invalid arguments
         *  @param reg pointer to Hlt::IRegister interface
         *  @param alg the algorithm
         */
        Lock( IRegister *reg, const IAlgorithm *alg );
        /** destructor (close & unlock transaction)
         *  @exception throw exception for invalid Hlt::IRrgister::unlock
         */
        ~Lock();
        // ====================================================================
      public:
        // ====================================================================
        /// get the access to the registrator  (optional)
        Hlt::IRegister *operator->() const { return m_register; }
        // ====================================================================
      private:
        // ====================================================================
        /// default constructor is disabled
        Lock(); // default constructor is disabled
        /// copy constructor is disabled
        Lock( const Lock & ); // copy constructor is disabled
        /// assignement operator is disabled
        Lock &operator=( const Lock & ); // assignement operator is disabled
        // ====================================================================
      private:
        // ====================================================================
        /// the service
        LoKi::Interface<IRegister> m_register; //    the service
        /// the algorithm
        LoKi::Interface<IAlgorithm> m_algorithm; //  the algorithm
        // ====================================================================
    };
    // ========================================================================
    /// Lock is a friend!
    friend class Hlt::IRegister::Lock; // Lock is a friend
    // ========================================================================
  public: // the only essential methods:
    // ========================================================================
    /** register the output selection during the allowed transaction for
     *  locked service
     *  @attention the service much be locked properly during the transaction!
     *  @param selection the selection to be registered
     *  @param producer  the algorithm/producer
     *  @return status code
     */
    virtual StatusCode
    registerOutput( Hlt::Selection *selection,        // the output selection
                    const IAlgorithm *producer ) = 0; //             producer
    /** register the input selection  dirung the allower transactions for
     *  locked service
     *  @attention the service much be locked properly during the transaction!
     *  @param selection the selection to be registered
     *  @param consumer the algorithm/consumer
     *  @return status code
     */
    virtual StatusCode
    registerInput( const Key &selection,             // the input selection
                   const IAlgorithm *consumer ) = 0; //            consumer
    // =========================================================================
    /** register the query to TES-selection
     *  @attention the service much be locked properly during the transaction!
     *  @param location TES location to be registered
     *  @param consumer algorithm/consumer
     *  @return Status Code
     */
    virtual StatusCode
    registerTESInput( const Key &location,              //        TES location
                      const IAlgorithm *consumer ) = 0; //            consumer
    // =========================================================================
  protected: // lets be a bit paranoic about the registration
    // =========================================================================
    /** start the transaction/lock the registrator
     *  @see  Hlt::IRegister::Lock
     *  @param alg the algorithm which starts the transaction
     *  @param status code
     */
    virtual StatusCode lock( const IAlgorithm *alg ) = 0;
    /** end the transaction/unlock the registrator
     *  @see  Hlt::IRegister::Lock
     *  @param alg the algorithm which ends the transaction
     *  @param status code
     */
    virtual StatusCode unlock( const IAlgorithm *alg ) = 0;
    // =========================================================================
  public: // mandatory stuff from the framework
    // ========================================================================
    /// Return the unique interface ID
    static const InterfaceID &interfaceID(); // the unique interface ID
    // ========================================================================
  protected:
    // ========================================================================
    /// virtual & protected destructor
    virtual ~IRegister(); // virtual & protected destructor
    // ========================================================================
};
// ==========================================================================
} // end of namespace Hlt
// ============================================================================
// The END
// ============================================================================
#endif // LOKI_IHLTREGISTER_H
// ============================================================================
