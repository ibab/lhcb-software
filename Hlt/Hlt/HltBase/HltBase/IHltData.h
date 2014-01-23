// $Id: IHltData.h,v 1.1 2009-12-23 10:26:44 graven Exp $
// ============================================================================
#ifndef IHLTDATA_H
#define IHLTDATA_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StringKey.h"
// ============================================================================
// forward declarations
// ============================================================================
namespace Hlt
{
class Selection;
}
namespace Hlt
{
template <class TYPE>
class TSelection;
}
class IAlgorithm;
class DataObject;
// ============================================================================
namespace Hlt
{
// ==========================================================================
/** @class IData LoKi/IHltData.h
 *  An abstract interface, which allow to get data from Hlt service
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2009-03-16
 */
class IData : virtual public IInterface
{
    // =========================================================================
  public:
    // =========================================================================
    /// the actual type of the key
    typedef Gaudi::StringKey Key;
    // =========================================================================
  public:
    // =========================================================================
    enum Errors {
        // ====================================================================
        Retrieve_Invalid_Producer = 600,
        Retrieve_Unknown_Producer,
        Retrieve_Unknown_Selection,
        Selection_Unknown_Selection,
        Selection_Unknown_Consumer,
        TES_Invalid_Reader,
        TES_Unknown_Reader,
        TES_Unknown_Location
        // ====================================================================
    };
    // ========================================================================
  public: // (const-access)
    // ========================================================================
    /// existing selection ?
    virtual bool hasSelection( const Key& key ) const = 0;
    // ========================================================================
  public: // (const-access)
          // ========================================================================
          /** retrieve the selection by name  (conts-access by consumer)
     *  @param key the selection key/name
     *  @param alg the consumer
     *  @return pointer to the selection (0 for missing selections)
     */
    virtual const Hlt::Selection*
    selection( const Key& key,                             // selection key/name
               const IAlgorithm* consumer = 0 ) const = 0; //       the consumer
    // ========================================================================
    /** retrieve the selection by name  (non-const access, only by producer)
     *  @param alg the producer
     *  @param key the selection key/name
     *  @return pointer to the selection (0 for missing selections)
     */
    virtual Hlt::Selection*
    retrieve( const IAlgorithm* producer, //        the producer
              const Key& key ) const = 0; //  selection key/name
    // ========================================================================
  public: // TES locations
          // ========================================================================
          /** retrieve the object from TES
     *  @param reader the algorithm
     *  @param location TES-location
     *  @return object for TES
     */
    virtual const DataObject* tes( const IAlgorithm* reader,        //       reader
                                   const Key& location ) const = 0; // TES-location
    // ========================================================================
  public: // helper method
          // ========================================================================
          /** helper method to get the concrete selection
     *  @param key the selection key/name
     *  @param alg the consumer
     */
    template <class TYPE>
    const Hlt::TSelection<TYPE>* get( const Key& key, // selection key/name
                                      const IAlgorithm* alg =
                                          0 ) const //        the consumer
    {
        const Hlt::Selection* sel = this->selection( key, alg );
        if ( 0 == sel ) {
            return 0;
        }
        return dynamic_cast<const Hlt::TSelection<TYPE>*>( sel );
    }
    /** helper method to get the concrete selection (non-const)
     *  @param alg the producer
     *  @param key the selection key/name
     */
    template <class TYPE>
    Hlt::TSelection<TYPE>* get( const IAlgorithm* producer, //        the producer
                                const Key& key ) const      //  selection key/name
    {
        Hlt::Selection* sel = this->retrieve( producer, key );
        if ( 0 == sel ) {
            return 0;
        }
        return dynamic_cast<Hlt::TSelection<TYPE>*>( sel );
    }
    // ========================================================================
  public: // mandatory stuff from the framework
    // ========================================================================
    /// Return the unique interface ID
    static const InterfaceID& interfaceID(); // the unique interface ID
    // ========================================================================
  protected:
    // ========================================================================
    /// virtual & protected destructor
    virtual ~IData(); // virtual & protected destructor
    // ========================================================================
};
// ==========================================================================
} // end of namespace Hlt
// ============================================================================
// The END
// ============================================================================
#endif // IHLTDATA_H
// ============================================================================
