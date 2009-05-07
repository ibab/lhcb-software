// $Id: Symbols.h,v 1.2 2009-05-07 15:03:30 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $ 
// ============================================================================
#ifndef KERNEL_SYMBOLS_H 
#define KERNEL_SYMBOLS_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <vector>
#include <string>
// ============================================================================
// forward declarations 
// ============================================================================
namespace LHCb { class IParticlePropertySvc ; }
// ============================================================================
/** @file Kernel/Symbols.h
 *  Helper file with the definition of symbols, used for particles/nodes/decays
 *  @see LHCb::IParticlePropertySvc
 *  @see LHCb::ParticleProperty
 *  @see LHCb::ParticleID
 *  Many thanks to Antonio PELLEGRINO for the kind help with regex expressions 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-05-07
 */
// ============================================================================
namespace Decays 
{
  // ==========================================================================
  /** @class Symbols
   *  Helper class to obtain the information about valid 
   *  symbols for particles, decay descriptors and Nodes.
   *  @attention this class must be synchronized with development of new nodes!
   *  This class can be used also an interactive python help for the various 
   *  symbols
   *  
   *  @code 
   *
   *  >>> from PartProp.decorators import Decays 
   *  >>> s = Decays.Symbols 
   *  >>> print s.symbol ( ' JSpin(4) ' ) 
   *  >>> print s.validSymbol ( ' X+' ) 
   *  >>> print s.symbol ( 'Meson ' ) 
   *
   *  @endcode 
   *
   *  Many thanks to Antonio PELLEGRINO for the kind help with regex expressions 
   *
   *  @see Decays::iNode 
   *  @see Decays::Nodes 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-05-07
   */
  class Symbols 
  {
  public:
    // ========================================================================
    /// the actual type for the list of names 
    typedef std::vector<std::string> Names ;
    // ========================================================================
    /** get a list of particle names, known for Particle Properties Service 
     * 
     *  @code
     *  
     *   Decays::Names names ;
     *   const LHCb::IParticlePropertySvc* svc = ... ;
     *  
     *   StatusCode sc = Decays::particles ( svc  , names ) ;
     *   
     *  @endcode 
     *
     *  @see LHCb::IParticlePropertySvc 
     *  @param svc   (INPUT) pointer to particle property Service 
     *  @param name  (OUTPUT) list of particle names 
     *  @return status code
     */
    static StatusCode particles 
    ( const LHCb::IParticlePropertySvc* service , 
      Names&                            names   ) ;
    // ========================================================================
    /** get a list of particle names, known for Particle Properties Service 
     * 
     *  @code
     *  
     *   const LHCb::IParticlePropertySvc* svc = ... ;
     *  
     *   Decays::Names names = Decays::particles ( svc  ) ;
     *   
     *  @endcode 
     *
     *  @see LHCb::IParticlePropertySvc 
     *  @param svc   (INPUT) pointer to particle property Service 
     *  @return list of particle names 
     */
    static Names particles 
    ( const LHCb::IParticlePropertySvc* service ) ;
    // ========================================================================
    /** get a short description of the symbol (if known) 
     *  @param sym the symbol to be checked 
     *  @param service the pointer to Particle property service 
     *  @return short description (empty string if unknown)
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-05-07
     */
    static std::string symbol 
    ( const std::string&                sym     , 
      const LHCb::IParticlePropertySvc* service ) ;
    // ========================================================================
    /** get a short description of the embedded symbol
     *
     *  This is probably the most important method, 
     *  Please note the implementation - it costains the full
     *  list of "valid" symbols. The remaining symbols have a form of 
     *
     *  - " JSpin( i )"
     *  - " LSpin( i )"
     *  - " SSpin( i )"
     *  - " HasQuark( quark )"
     *  - " HasQuark( j )"
     *
     *  Where: 
     *   - <code>i</code> is integer positive number, 
     *   - <code>j</code> is a digit between 1 and 6, @see LHCb::ParticleID
     *   - <code>quark</code> is a quark name: <code>up</code>,
     *                   <code>down</code>, <code>strange</code>,
     *                   <code>charm</code>, <code>beauty</code>,
     *                   <code>bottom</code>, <code>top</code>
     *
     *  @see LHCb::ParticleID 
     *  @param sym the symbol to be checked 
     *  @return short description (empty string if unknown)
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-05-07
     */
    static const std::string& symbol ( const std::string& sym ) ;
    // ========================================================================
    /** check if the symbol/token is valid symbol 
     *  @param sym the symbol to be checked 
     *  @param service the pointer to Particle property service 
     *  @return true for valid/known symbols 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-05-07
     */
    static bool validSymbol 
    ( const std::string&                sym     , 
      const LHCb::IParticlePropertySvc* service ) ;
    // ========================================================================
    /** check if the symbol/token is valid embedded symbol 
     * 
     *  This is probably the most important method, 
     *  Please note the implementation - it costains the full
     *  list of "valid" symbols. The remaining symbols have a form of 
     *
     *  - " JSpin( i )"
     *  - " LSpin( i )"
     *  - " SSpin( i )"
     *  - " HasQuark( quark )"
     *  - " HasQuark( j )"
     *
     *  Where: 
     *   - <code>i</code> is integer positive number, 
     *   - <code>j</code> is a digit between 1 and 6, @see LHCb::ParticleID
     *   - <code>quark</code> is a quark name: <code>up</code>,
     *                   <code>down</code>, <code>strange</code>,
     *                   <code>charm</code>, <code>beauty</code>,
     *                   <code>bottom</code>, <code>top</code>
     *  @see LHCb::ParticleID 
     *  @param sym the symbol to be checked 
     *  @param service the pointer to Particle property service 
     *  @return truye for valid/known symbols 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-05-07
     */
    static bool validSymbol 
    ( const std::string&                sym     ) ;
    // ========================================================================
  } ;
  // ==========================================================================
} // end of namespace Decays
// ============================================================================
// The END 
// ============================================================================
#endif // KERNEL_SYMBOLS_H
// ============================================================================
