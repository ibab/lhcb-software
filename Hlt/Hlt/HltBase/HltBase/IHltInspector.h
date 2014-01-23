// $Id: IHltInspector.h,v 1.1 2009-12-23 10:26:44 graven Exp $
// ============================================================================
#ifndef IHLTINSPECTOR_H
#define IHLTINSPECTOR_H 1
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
namespace Hlt {
class Selection;
}
class IAlgorithm;
// ============================================================================
namespace Hlt {
// ==========================================================================
/** @class IInspector HltBase/IHltInspector.h
 *  An abstract interface, whch allow the inspection of Hlt-selection(s)
 *  @attention the implementation is not supposed to be fast & efficient!
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2009-03-16
 */
class IInspector : virtual public IInterface {
    // ========================================================================
  public:
    // ========================================================================
    /// the actual type of alg-list
    typedef std::vector<const IAlgorithm *> AlgList; // the type of alg-list
    /// the actual type of selection-list
    typedef std::vector<const Hlt::Selection *> SelList; // the type of alg-list
    /// the actual type of list of keys
    typedef std::vector<std::string> KeyList; // the type of key-list
    // ========================================================================
  public: // ispection by selection: producer & consumers
    // ========================================================================
    /** get the producer for the given selection
     *  @param selection the selection
     *  @return the producer
     */
    virtual const IAlgorithm *producer( const Hlt::Selection *selection ) const = 0;
    /** get the producer for the given selection
     *  @param selection the selection
     *  @return the producer
     */
    virtual const IAlgorithm *producer( const std::string &selection ) const = 0;
    // ========================================================================
    /** get all nominal consumers for the given selection
     *  @param selection the selection
     *  @param alglist (OUTPUT) list of consumers
     *  @param number of nominal consumers
     */
    virtual size_t consumers( const Hlt::Selection *selection,
                              AlgList &alglist ) const = 0;
    /** get all nominal consumers for the given selection
     *  @param selection the selection
     *  @param alglist (OUTPUT) list of consumers
     *  @param number of nominal consumers
     */
    virtual size_t consumers( const std::string &selection,
                              AlgList &alglist ) const = 0;
    // ========================================================================
  public: // ispection by algorithms: inputs/outputs
    // ========================================================================
    /** get the inputs for the given algorithm
     *  @param algorithm the algorithm (consumer)
     *  @param selections (OUTPUT) the list of input selections
     *  @return number of input selections
     */
    virtual size_t inputs( const IAlgorithm *algorithm,
                           SelList &selections ) const = 0;
    /** get the inputs for the given algorithm
     *  @param algorithm the algorithm (consumer)
     *  @param selections (OUTPUT) the list of input selections
     *  @return number of input selections
     */
    virtual size_t inputs( const IAlgorithm *algorithm,
                           KeyList &selections ) const = 0;
    /** get the inputs for the given algorithm
     *  @param algorithm the algorithm (consumer)
     *  @param selections (OUTPUT) the list of input selections
     *  @return number of input selections
     */
    virtual size_t inputs( const std::string &algorithm,
                           SelList &selections ) const = 0;
    /** get the inputs for the given algorithm
     *  @param algorithm the algorithm (consumer)
     *  @param selections (OUTPUT) the list of input selections
     *  @return number of input selections
     */
    virtual size_t inputs( const std::string &algorithm,
                           KeyList &selections ) const = 0;
    // ========================================================================
    /** get the outputs for the given algorithm
     *  @param algorithm the algorithm (producer)
     *  @param selections (OUTPUT) the list of input selections
     *  @return number of output selections
     */
    virtual size_t outputs( const IAlgorithm *algorithm,
                            SelList &selections ) const = 0;
    /** get the outputs for the given algorithm
     *  @param algorithm the algorithm (producer)
     *  @param selections (OUTPUT) the list of input selections
     *  @return number of output selections
     */
    virtual size_t outputs( const IAlgorithm *algorithm,
                            KeyList &selections ) const = 0;
    /** get the outputs for the given algorithm
     *  @param algorithm the algorithm (producer)
     *  @param selections (OUTPUT) the list of input selections
     *  @return number of output selections
     */
    virtual size_t outputs( const std::string &algorithm,
                            SelList &selections ) const = 0;
    /** get the outputs for the given algorithm
     *  @param algorithm the algorithm (producer)
     *  @param selections (OUTPUT) the list of input selections
     *  @return number of output selections
     */
    virtual size_t outputs( const std::string &algorithm,
                            KeyList &selections ) const = 0;
    // ========================================================================
  public: // TES-selections
          // ========================================================================
          /** get the "readers" for the given TES-location
     *  @param location TES-location
     *  @param alglist (OUTPUT) the list of readers
     *  @return number of readers
     */
    virtual size_t readers( const std::string &location,
                            AlgList &alglist ) const = 0;
    /** get all readers
     *  @param alglist (OUTPUT) the list of readers
     *  @return number of readers
     */
    virtual size_t allReaders( AlgList &alglist ) const = 0;
    /** get the input TES locations for the given reader
     *  @param reader the algorithm
     *  @param locations (OUTPUT) the list of input TES locations
     *  @return numebr of locations
     */
    virtual size_t readTES( const IAlgorithm *reader, KeyList &locations ) const = 0;
    /** get the input TES locations for the given reader
     *  @param reader the algorithm
     *  @param locations (OUTPUT) the list of input TES locations
     *  @return numebr of locations
     */
    virtual size_t readTES( const std::string &reader,
                            KeyList &locations ) const = 0;
    /** get all TES-input locations
     *  @param locations (OUTPUT) the list of input TES locations
     *  @return numebr of locations
     */
    virtual size_t allTES( KeyList &locations ) const = 0;
    // ========================================================================
  public:
    // ========================================================================
    /** get all algorithms
     *  @param algs (OUTPUT) the list of algorithms
     *  @return number of algorithms
     */
    virtual size_t algorithms( AlgList &algs ) const = 0;
    /** get all selections
     *  @param sels (OUTPUT) the list of selections
     *  @return number of selections
     */
    virtual size_t selections( KeyList &sels ) const = 0;
    // ========================================================================
  public: //  some general stuff
    // ========================================================================
    /// registered algorithm?
    virtual bool hasAlgorithm( const IAlgorithm *alg ) const = 0;
    /// registered algorithm?
    virtual bool hasAlgorithm( const std::string &alg ) const = 0;
    /// registered producer?
    virtual bool hasProducer( const IAlgorithm *alg ) const = 0;
    /// registered producer?
    virtual bool hasProducer( const std::string &alg ) const = 0;
    /// registered consumer
    virtual bool hasConsumer( const IAlgorithm *alg ) const = 0;
    /// registered producer?
    virtual bool hasConsumer( const std::string &alg ) const = 0;
    /// registered selection ?
    virtual bool hasSelection( const std::string &key ) const = 0;
    /// registered selection ?
    virtual bool hasSelection( const Hlt::Selection *selection ) const = 0;
    // ========================================================================
  public: // boring stuff from the framework
    // ========================================================================
    /// Return the unique interface ID
    static const InterfaceID &interfaceID(); // the unique interface ID
    // ========================================================================
  protected:
    // ========================================================================
    /// virtual & protected destructor
    virtual ~IInspector(); // virtual & protected destructor
    // ========================================================================
};
// ==========================================================================
} // end of namespace Hlt
// ============================================================================
// The END
// ============================================================================
#endif // IHLTINSPECTOR_H
// ============================================================================
