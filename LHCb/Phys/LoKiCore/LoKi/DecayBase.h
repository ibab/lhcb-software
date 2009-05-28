// $Id: DecayBase.h,v 1.3 2009-05-28 08:55:34 ibelyaev Exp $
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
    ( Decays::Node&          node , const std::string& input ) const ;
    // ========================================================================
    /** parse the tree
     *  @param tree  (OUTPUT) parsed node 
     *  @param input (INPUT) input string
     *  @return status code
     */
    StatusCode _parse 
    ( Decays::Parsers::Tree& tree , const std::string& input ) const ;
    // ========================================================================
    /// convert the string ' [ a ]cc ' into 'a'
    bool _stripCC ( const std::string& input , 
                    std::string&       cc    ) const ;
    // ========================================================================
    /// convert the string ' [ a ]cc ' into ' ( a , aCC ) '  
    std::string _makeCC ( const std::string& input ) const ;
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
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_DECAYFINDERBASE_H
// ============================================================================
