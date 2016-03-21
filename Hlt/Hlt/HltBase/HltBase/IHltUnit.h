// $Id: IHltUnit.h,v 1.1 2010-01-25 09:28:01 graven Exp $
// ============================================================================
#ifndef IHLTUNIT_H
#define IHLTUNIT_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <memory>
#ifdef __GCCXML__
// gccxml doesn't know about any C++11 constructions...  presumably it reads
// from an older, pre-C++11 version of  <memory>.
// note: it's explicitly forbidden to fwd declare this in std.. but it is the
//       only way to use a unique_ptr as argument in a method seen by gccxml
namespace std { template <typename T> struct unique_ptr ; }
#endif
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/StringKey.h"
// ============================================================================
// HltBase
// ============================================================================
#include "HltBase/HltSelection.h"
// ============================================================================
class DataObject;
// ============================================================================
namespace Hlt
{
// ==========================================================================
class IRegister;
class IData;
// ==========================================================================
}
// ============================================================================
namespace LoKi
{
// ==========================================================================
class AuxFunBase; // the base for all functors
// ==========================================================================
}
// ============================================================================
namespace Hlt
{
// ==========================================================================
/** @class IHltUnit LoKi/IHltUnit.h
 *  Simple interface, suitable to act as facade unit for the Hlt Algorithm
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2008-11-10
 */
class IUnit : virtual public IInterface
{
  public:
    // ========================================================================
    /// the actual type of functor
    typedef LoKi::AuxFunBase Client; // the actual type of functor
    /// the actual type of Key
    typedef Gaudi::StringKey Key; // the actual type of selection key
    // ========================================================================
  public:
    // ========================================================================
    /** declare (the templated) selection
     *  @param key the key to be used for selection
     *  @return pointer to the registered seelction (or NULL)
     */
    template <class T>
    Hlt::TSelection<T>* declareOutput( const Key& key, const Client& client ) const;
    // ========================================================================
    /** register the selection
     *  @param selection the selection to be registered
     *                   NOTE: assumes ownership
     *  @param client    the client
     *  @return status code
     */
    virtual StatusCode registerOutput( std::unique_ptr<Hlt::Selection> selection,
                                       const Client& client ) const = 0;
    // ========================================================================
    /** declare the input selection
     *  @param key the selection key
     *  @param client the client
     */
    virtual const Hlt::Selection* declareInputSelection( const Key& key,
                                                         const Client& client ) const = 0;
    // ========================================================================
    /** register the query  to TES-selection
     *  @param location TES location to be registered
     *  @param consumer algorithm/consumer
     *  @return Status Code
     */
    virtual StatusCode
    registerTESInput( const Key& location,              //          TES location
                      const Client& client ) const = 0; //                client
    // ========================================================================
  public:
    // ========================================================================
    /** get the (const) selection by key
     *  @param key the key
     *  @return pointer to the selection
     */
    virtual const Hlt::Selection* selection( const Key& key,
                                             const Client& client ) const = 0;
    // =========================================================================
  public:
    // =========================================================================
    /** get the (const) selection by key  (anonymous)
     *  @param key the key
     *  @return pointer to the selection
     */
    virtual const Hlt::Selection* selection( const Key& key ) const = 0;
    // =========================================================================
  public:
    // =========================================================================
    /** get the object form TES
     *  @param client the client
     *  @param location the TES location
     *  @return the object
     */
    virtual const DataObject* tes( const Client& client,
                                   const Key& location ) const = 0;
    // =========================================================================
  public:
    // =========================================================================
    /** get the data form TES
     *  @param client the client
     *  @param TES -location
     *  @return the data
     */
    template <class TYPE>
    const TYPE* tesData( const Client& client, const Key& location ) const;
    // ========================================================================
  public:
    // =========================================================================
    /** get the selection by key (non-const)
     *  @param key the key
     *  @return pointer to the selection
     */
    virtual Hlt::Selection* retrieve( const Client& client,
                                      const Key& key ) const = 0;
    // ========================================================================
  public: // IInterface related stuff
    // ========================================================================
    /// the unique interface identifier
    static const InterfaceID& interfaceID();
    // ========================================================================
  protected:
    // ========================================================================
    /// virtual & protected destructor
    virtual ~IUnit(); // virtual & protected destructor
    // ========================================================================
};
// ==========================================================================
} // end of namespace LoKi
#ifndef __GCCXML__
  // ============================================================================
  /*  declare (the templated) selection
*  @param key the key to be used for selection
*  @return pointer to the registered seelction (or NULL)
*/
// ============================================================================
template <class T>
Hlt::TSelection<T>*
Hlt::IUnit::declareOutput( const Hlt::IUnit::Key& key,
                           const Hlt::IUnit::Client& client ) const
{
    std::unique_ptr<Hlt::TSelection<T>> selection{ new Hlt::TSelection<T>( key ) };
    auto ret = selection.get();
    StatusCode sc = this->registerOutput( std::move(selection), client );
    return sc.isSuccess() ? ret : nullptr ;   // RETURN
}
// ============================================================================
/*  get the data form TES
 *  @param client the client
 *  @param TES -location
 *  @return the data
 */
// ============================================================================
template <class TYPE>
const TYPE* Hlt::IUnit::tesData( const Hlt::IUnit::Client& client,
                                 const Hlt::IUnit::Key& location ) const
{
    const auto* obj = this->tes( client, location );
    return obj ? dynamic_cast<const TYPE*>( obj ) : nullptr;
}
#endif
// ============================================================================
// The END
// ============================================================================
#endif // IHLTUNIT_H
// ============================================================================
