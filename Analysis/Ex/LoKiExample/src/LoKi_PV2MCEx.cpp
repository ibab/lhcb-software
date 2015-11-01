// $Id: 
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
// ============================================================================
// DaVinciMCKernel
// ============================================================================
#include "Kernel/IPV2MC.h"
#include "Kernel/PV2MC.h"
// ============================================================================
// LoKi 
// ============================================================================
#include "LoKi/Interface.h"
#include "LoKi/AlgoMC.h"
#include "LoKi/VertexCuts.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Example 
  {
    // ========================================================================
    /** @class PV2MCEx LoKi_PV2MCEx 
     *  Example of accession MCtruth for primary vertices 
     *  @see IPV2MC 
     *  @see  PV2MC 
     *  @author Vanya BELYAEV Ivan.Belayev@nikhef.nl
     *  @date 2008-11-03
     */
    class PV2MCEx : public LoKi::AlgoMC 
    {
      // ======================================================================
      /// the friend factory for instantiation
      friend class AlgFactory<LoKi::Example::PV2MCEx> ;
      // ======================================================================
    public:
      // ======================================================================
      /** the only one essential method 
       *  @see LoKi::Algo
       *  @see LoKi::Algo::analyse
       */
      virtual StatusCode analyse  () ;
      /// finalzation of the algorithm 
      virtual StatusCode finalize () 
      {
        m_pv2mc = (IPV2MC*) 0 ;
        return LoKi::AlgoMC::finalize () ;
      }
      // ======================================================================
    protected:
      // ======================================================================
      /** standard constructor 
       *  @param name the name of the algorithm instance 
       *  @param pSvc pointer to Service Locator 
       */
      PV2MCEx 
      ( const std::string& name ,    // name the name of the algorithm instance 
        ISvcLocator*       pSvc )                 // pointer to Service Locator 
        : LoKi::AlgoMC ( name , pSvc )
        , m_pv2mc ( 0 )
      {}
      // ======================================================================
      // virtual and protected destructor 
      virtual ~PV2MCEx () {}
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled 
      PV2MCEx() ;                            // default constructor is disabled 
      /// copy constructor is disabled 
      PV2MCEx ( const PV2MCEx& ) ;              // copy constructor is disabled 
      /// assignement operator is disabled 
      PV2MCEx& operator=( const PV2MCEx& ) ;         // no assignement operator
      // ======================================================================
    private:
      // ======================================================================
      /// get the tool (aquire on demand)
      const IPV2MC* pvmc() const 
      {
        if   ( !m_pv2mc )
        { m_pv2mc = tool<IPV2MC> ( "LoKi::PV2MC/PV2MC" , this ) ; }
        return m_pv2mc ;
      }
      // ======================================================================
    private:
      // ======================================================================      
      /// the tool itself 
      mutable LoKi::Interface<IPV2MC> m_pv2mc ;              // the tool itself 
      // ======================================================================      
    };
    // ========================================================================
  } // end of namespace LoKi::Example 
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
/*  the only one essential method 
 *  @see LoKi::Algo
 *  @see LoKi::Algo::analyse
 */
// ============================================================================
StatusCode LoKi::Example::PV2MCEx::analyse  ()
{
  using namespace LoKi        ;
  using namespace LoKi::Types ;
  using namespace LoKi::Cuts  ;
  
  /// get all primary vertices 
  VRange primaries = vselect ( "primaries" , PRIMARY ) ;
  
  always() << "Number of PVs: " << primaries.size() << endmsg ;
  
  const IPV2MC::PV2MC*        t1 = pvmc () -> pv2MC        () ;
  const IPV2MC::PV2Collision* t2 = pvmc () -> pv2Collision () ;
  
  Assert ( 0 != t1 && 0 != 2 , "Invalid tables from IPC2Mc tool" ) ;
  
  MsgStream& log = always() ;
  
  log << "Number of PVs: " << primaries.size() << endmsg ;
  for ( VRange::iterator ipv = primaries.begin() ; 
        primaries.end() != ipv ; ++ipv ) 
  {
    const LHCb::VertexBase* _pv = *ipv ; 
    if ( 0 == _pv ) { continue ; }
    const LHCb::RecVertex*   pv = dynamic_cast<const LHCb::RecVertex*> ( _pv ) ;
    if ( 0 ==  pv ) { continue ; }
    
    /// get all related monte -carlo vertices 
    IPV2MC::PV2MC::Range        r1 = t1->relations ( pv ) ;
    /// get all related collisions
    IPV2MC::PV2Collision::Range r2 = t2->relations ( pv ) ;
    
    log
      << std::endl 
      << " PV#"   << (ipv-primaries.begin()+1)  
      << " #key "      <<  pv->key() 
      << " \t#tracks " <<  pv->tracks().size()  
      << " related LHCb::MCVertices/LHCb::GenCollisions are " 
      << r1.size() << "/" << r2.size() 
      << std::endl ;
    
    // loop over related Monte Carlo vertices 
    for ( IPV2MC::PV2MC::Range::iterator i1 = r1.begin() ; 
          r1.end() != i1 ; ++i1 ) 
    {
      const LHCb::MCVertex* mc = i1->to() ;
      if ( 0 == mc ) { continue ; }
      log 
        << " \tMCVertex  #"  << (i1-r1.begin()+1) 
        << " \tkey "       <<  mc->key() 
        << " \t#tracks "   <<  i1->weight().first
        << " \t#chi2   "   <<  i1->weight().second 
        << " \t position"  <<  mc->position() 
        << std::endl ;
    }
    
    // loop over related Generator Collisions 
    for ( IPV2MC::PV2Collision::Range::iterator i2 = r2.begin() ; 
          r2.end() != i2 ; ++i2 ) 
    {
      const LHCb::GenCollision* gc = i2->to() ;
      if ( 0 == gc ) { continue ; }
      log
        << " \tCollision #"  << (i2-r2.begin()+1) 
        << " \tkey "       <<  gc->key() 
        << " \t#tracks "   <<  i2->weight().first
        << " \t#chi2   "   <<  i2->weight().second 
        << std::endl ;
    }
  }
  log << endmsg ;
  
  return StatusCode::SUCCESS ;
}
// ============================================================================
/// declare a factory
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LoKi::Example,PV2MCEx)
// ============================================================================
// The END 
// ============================================================================
