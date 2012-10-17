// ============================================================================
#ifndef LOKI_DECAYFINDERBASE_H 
#define LOKI_DECAYFINDERBASE_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/Nodes.h"
// ============================================================================
// local
// ============================================================================
// forward decalrations 
// ============================================================================
/** @file LoKi/DecayBase.h
 *
 *  This file is a part of LoKi project -
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 */
namespace Decays 
{
  class Node ;
  namespace Parsers { class Tree ; }
} 
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @class DecayBase 
   *  Base class forimplementation of various decay finders 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-05-22
   */
  class DecayBase : public GaudiTool 
  {
    // ========================================================================
  public:
    // ========================================================================
    /// the specific finalization 
    virtual StatusCode finalize () ;
    // ========================================================================
  protected:
    // ========================================================================
    /// constructor 
    DecayBase 
    ( const std::string& type   ,                   // the actual tool type (?)
      const std::string& name   ,                   //     the actual tool name 
      const IInterface*  parent ) ;                 //               the parent
    /// virtual protected destructor 
    virtual ~DecayBase () ;
    // ========================================================================
  protected:
    // ========================================================================
    /** parse the node 
     *  @param node  (OUTPUT) parsed node 
     *  @param input (INPUT) input string
     *  @return status code
     */
    StatusCode _parse 
    ( Decays::Node& node , std::string input ) const ;
    // ========================================================================
    /** parse the tree
     *  @param tree  (OUTPUT) parsed node 
     *  @param input (INPUT) input string
     *  @return status code
     */
    StatusCode _parse 
    ( Decays::Parsers::Tree& tree , std::string input ) const ;
    // ========================================================================
    /** convert the substring ' [ a ]cc ' into ' [ a , aCC ] '  
     *  The lines are coded by Sascha Mazurov
     */
    std::string _makeCC ( std::string input ) const ;
    // ========================================================================
  protected:
    // ========================================================================
    Decays::Node node ( const std::string& descriptor ) const ;  
    // ========================================================================
  protected:
    // ========================================================================
    const LHCb::IParticlePropertySvc* ppSvc () const 
    {
      if ( 0 != m_ppSvc ) { return m_ppSvc ; }                        // RETURN 
      m_ppSvc = svc<LHCb::IParticlePropertySvc>
        ( "LHCb::ParticlePropertySvc" , true ) ;
      return m_ppSvc ;                                                // RETURN 
    }
    // ========================================================================
    const std::string& defaultNode () const { return m_default_node ; }
    const std::string& defaultTree () const { return m_default_tree ; }      
    // ========================================================================
  private:
    // ========================================================================  
    /// the default constructor is disabled 
    DecayBase () ;                       // the default constructor is disabled 
    /// copy constructor is disabled 
    DecayBase ( const DecayBase& ) ;                      // no copy consructor 
    /// assignement operator is disabled 
    DecayBase& operator=( const DecayBase& ) ;                // no assignement
    // ========================================================================
  private:
    // ========================================================================
    /// the particle proeprty service 
    mutable const LHCb::IParticlePropertySvc* m_ppSvc ; //  particle properties
    // ========================================================================    
    /// the default node  
    std::string m_default_node ;                            // the default node  
    /// the default tree 
    std::string m_default_tree ;                            // the default tree
    // ========================================================================    
  };
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_DECAYFINDERBASE_H
// ============================================================================
