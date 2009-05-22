// $Id: DecayFinderBase.h,v 1.1 2009-05-22 18:12:36 ibelyaev Exp $
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
#include "LoKi/IDecayNodeFactory.h"
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
  /** @class DecayFinderBase 
   *  Base class forimplementation of various decay finders 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-05-22
   */
  class DecayFinderBase 
    : public GaudiTool 
    , public Decays::IDecayNodeFactory
  {
  public:
    // ========================================================================
    /** build the node ftrom description 
     *  @see Decays::DecanNodeFactory
     */
    StatusCode node 
    ( const std::string& descriptor , 
      Decays::Node&      node       ) const 
    { return _parse ( node , descriptor ) ; }
    //  =======================================================================
  public:
    // ========================================================================
    /// the specific finalization 
    virtual StatusCode finalize () ;
    // ========================================================================
  protected:
    // ========================================================================
    /// constructor 
    DecayFinderBase 
    ( const std::string& type   ,                   // the actual tool type (?)
      const std::string& name   ,                   //     the actual tool name 
      const IInterface*  parent ) ;                 //               the parent
    /// virtual protected destructor 
    virtual ~DecayFinderBase() ;
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
    const LHCb::IParticlePropertySvc* ppSvc () const 
    {
      if ( 0 != m_ppSvc ) { return m_ppSvc ; }                        // RETURN 
      m_ppSvc = svc<LHCb::IParticlePropertySvc>
        ( "LHCb::ParticlePropertySvc" , true ) ;
      return m_ppSvc ;                                                // RETURN 
    }
    // ========================================================================
  private:
    // ========================================================================  
    /// the default constructor is disabled 
    DecayFinderBase () ;                 // the default constructor is disabled 
    /// copy constructor is disabled 
    DecayFinderBase ( const DecayFinderBase& ) ;          // no copy consructor 
    /// assignement operator is disabled 
    DecayFinderBase& operator=( const DecayFinderBase& );     // no assignement
    // ========================================================================
  private:
    // ========================================================================
    /// the particle proeprty service 
    mutable const LHCb::IParticlePropertySvc* m_ppSvc ; //  particle properties
    // ========================================================================    
  };
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_DECAYFINDERBASE_H
// ============================================================================
