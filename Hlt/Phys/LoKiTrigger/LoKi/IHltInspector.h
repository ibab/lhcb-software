// $Id: IHltInspector.h,v 1.1 2009-03-19 13:16:12 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_IHLTINSPECTOR_H 
#define LOKI_IHLTINSPECTOR_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IInterface.h"
// ============================================================================
// HltBase 
// ============================================================================
#include "HltBase/stringKey.h"
// ============================================================================
// forward declarations
// ============================================================================
namespace Hlt { class Selection ; }
class IAlgorithm ;
// ============================================================================
namespace Hlt
{
  // ==========================================================================
  /** @class IInspector LoKi/IHltInspector.h
   *  An abstract interface, whch allow the inspection of Hlt-selection(s)
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date   2009-03-16
   */
  class IInspector : virtual public IInterface
  {      
    // ========================================================================
  public:
    // ========================================================================
    /// the actual type of the selection key/name
    typedef stringKey Key ;                  // the actual type of the key/name 
    /// the actual type of alg-list
    typedef std::vector<const IAlgorithm*>     AlgList ;// the type of alg-list
    /// the actual type of selection-list
    typedef std::vector<const Hlt::Selection*> SelList ;// the type of alg-list
    /// the actual type of list of keys 
    typedef std::vector<Key>                   KeyList ;// the type of key-list
    // ========================================================================
  public:   // ispection by selection: producer & consumers 
    // ========================================================================
    /** get the producer for the given selection 
     *  @param selection the selection 
     *  @return the producer 
     */
    virtual const IAlgorithm*  producer 
    ( const Hlt::Selection*    selection ) const = 0 ;
    /** get the producer for the given selection 
     *  @param selection the selection 
     *  @return the producer 
     */
    virtual const IAlgorithm* producer 
    ( const Key&              selection ) const = 0 ;
    // ========================================================================
    /** get all nominal consumers for the given selection 
     *  @param selection the selection 
     *  @param alglist (OUTPUT) list of consumers 
     *  @param number of nominal consumers 
     */
    virtual size_t consumers 
    ( const Hlt::Selection* selection , 
      AlgList&              alglist   ) const  = 0 ;  
    /** get all nominal consumers for the given selection 
     *  @param selection the selection 
     *  @param alglist (OUTPUT) list of consumers 
     *  @param number of nominal consumers 
     */
    virtual size_t consumers 
    ( const Key&            selection , 
      AlgList&              alglist   ) const  = 0 ;
    // ========================================================================
  public:   // ispection by algorithms: inputs/outputs  
    // ========================================================================
    /** get the inputs for the given algorithm 
     *  @param algorithm the algorithm (consumer) 
     *  @param selections (OUTPUT) the list of input selections 
     *  @return number of input selections 
     */
    virtual size_t inputs 
    ( const IAlgorithm*  algorithm , 
      SelList&           selections ) const = 0 ;
    /** get the inputs for the given algorithm 
     *  @param algorithm the algorithm (consumer) 
     *  @param selections (OUTPUT) the list of input selections 
     *  @return number of input selections 
     */
    virtual size_t inputs 
    ( const IAlgorithm*  algorithm , 
      KeyList&           selections ) const = 0 ;
    /** get the inputs for the given algorithm 
     *  @param algorithm the algorithm (consumer) 
     *  @param selections (OUTPUT) the list of input selections 
     *  @return number of input selections 
     */
    virtual size_t inputs 
    ( const std::string& algorithm , 
      SelList&           selections ) const = 0 ;
    /** get the inputs for the given algorithm 
     *  @param algorithm the algorithm (consumer) 
     *  @param selections (OUTPUT) the list of input selections 
     *  @return number of input selections 
     */
    virtual size_t inputs 
    ( const std::string& algorithm , 
      KeyList&           selections ) const = 0 ;
    // ========================================================================    
    /** get the outputs for the given algorithm 
     *  @param algorithm the algorithm (producer) 
     *  @param selections (OUTPUT) the list of input selections 
     *  @return number of output selections 
     */
    virtual size_t outputs 
    ( const IAlgorithm*  algorithm , 
      SelList&           selections ) const = 0 ;
    /** get the outputs for the given algorithm 
     *  @param algorithm the algorithm (producer) 
     *  @param selections (OUTPUT) the list of input selections 
     *  @return number of output selections 
     */
    virtual size_t outputs 
    ( const IAlgorithm*  algorithm , 
      KeyList&           selections ) const = 0 ;
    /** get the outputs for the given algorithm 
     *  @param algorithm the algorithm (producer) 
     *  @param selections (OUTPUT) the list of input selections 
     *  @return number of output selections 
     */
    virtual size_t outputs
    ( const std::string& algorithm , 
      SelList&           selections ) const = 0 ;
    /** get the outputs for the given algorithm 
     *  @param algorithm the algorithm (producer) 
     *  @param selections (OUTPUT) the list of input selections 
     *  @return number of output selections 
     */
    virtual size_t outputs 
    ( const std::string& algorithm , 
      KeyList&           selections ) const = 0 ;
    // ========================================================================
  public: //  some general stuff
    // ========================================================================
    /// registered algorithm?
    virtual bool hasAlgorithm ( const IAlgorithm*  alg ) const = 0 ;
    /// registered algorithm?
    virtual bool hasAlgorithm ( const std::string& alg ) const = 0 ;
    /// registered producer?
    virtual bool hasProducer  ( const IAlgorithm*  alg ) const = 0 ;
    /// registered producer?
    virtual bool hasProducer  ( const std::string& alg ) const = 0 ;
    /// registered consumer
    virtual bool hasConsumer  ( const IAlgorithm*  alg ) const = 0 ;
    /// registered producer?
    virtual bool hasConsumer  ( const std::string& alg ) const = 0 ;
    /// registered selection ?
    virtual bool hasSelection ( const Key&         key ) const = 0 ;
    /// registered selection ?
    virtual bool hasSelection ( const Hlt::Selection* selection ) const = 0 ;
    // ========================================================================
  public: // boring stuff from the framework
    // ========================================================================
    /// Return the unique interface ID 
    static const InterfaceID& interfaceID() ;        // the unique interface ID 
    // ========================================================================
  protected:
    // ========================================================================
    /// virtual & protected destructor 
    virtual ~IInspector() ;                   // virtual & protected destructor 
    // ========================================================================
  };
  // ==========================================================================
} // end of namespace Hlt 
// ============================================================================
// The END
// ============================================================================
#endif // LOKI_IHLTINSPECTOR_H
// ============================================================================
