// $Id: HltUnit.h,v 1.2 2008-11-13 13:14:42 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_HLTUNIT_H 
#define LOKI_HLTUNIT_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/VectorMap.h"
// ============================================================================
// HltBase 
// ============================================================================
#include "HltBase/IHltDataSvc.h"
#include "Kernel/IANNSvc.h"
// ============================================================================
// LoKi 
// ============================================================================
#include "LoKi/FilterAlg.h"
#include "LoKi/IHltUnit.h"
#include "LoKi/CoreTypes.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @class HltUnit HltUnit.h LoKi/HltUnit.h
   *  Simple algorithm with pretends to be a replacement of
   *  the major part of the python' HltConf.Hlt1Line stuff
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date   2008-11-10
   */
  class HltUnit 
    : public LoKi::FilterAlg
    , public virtual LoKi::IHltUnit
  {
    // ========================================================================
    // the friend factory for instantiation 
    friend class AlgFactory<LoKi::HltUnit> ;
    // ========================================================================
  public:                                                          // Algorithm 
    // ========================================================================
    /// execute the algorithm
    virtual StatusCode execute () ;
    // ========================================================================
  public:                                                    // LoKi::FilterAlg 
    // ========================================================================
    /** decode the functor 
     *  @see LoKi::FilterAlg
     *  @see LoKi::ICoreFactory 
     */
    virtual StatusCode decode() ;
    // ========================================================================
  public:                                                // LoKi::IHltUnit part 
    // ========================================================================
    /** register the selection 
     *  (internal method, should not be invoked directly) 
     *  @see LoKi::IhltUnit::registerSelection
     *  @param selection the seelction to be registered 
     *  @return status code 
     */
    virtual StatusCode registerSelection ( Hlt::Selection* selection ) ;
    // ========================================================================
    /** get the selection by key 
     *  @param key the key 
     *  @see LoKi::IhltUnit::selection
     *  @return pointer to the selection 
     */
    virtual Hlt::Selection* selection ( const stringKey& key ) const ;
    // ========================================================================
  public:                                                  // IInterface tricks  
    // ========================================================================
    /** query interface trick (please do not abuse it!)
     *  @see IInterface::queryInterface 
     *  @param iid  the unique interface ID 
     *  @param ppvi pointer to the interface 
     *  @return status code 
     */
    virtual StatusCode queryInterface 
    (const InterfaceID& iid, void** ppvi ) ;
    // ========================================================================
  protected:
    // ========================================================================
    /// get access to Hlt data service 
    inline IHltDataSvc* hltSvc() const 
    {
      if ( 0 == m_hltSvc ) 
      { m_hltSvc = svc<IHltDataSvc>( "HtlDataSvc" , true ) ; }
      return m_hltSvc ;
    }
    /// get access to Hlt data service 
    inline IANNSvc* annSvc() const 
    {
      if ( 0 == m_annSvc ) 
      { m_annSvc = svc<IANNSvc>( "HtlANNSvc" , true ) ; }
      return m_annSvc ;
    }
    // ========================================================================
    /// monitor the selections? 
    inline bool monitor() const { return m_monitor || msgLevel ( MSG::DEBUG ) ; }
    // ========================================================================
    inline Hlt::Selection*  i_selection ( const stringKey& key ) const 
    {
      IHltDataSvc* hlt = hltSvc() ;
      const IAlgorithm* a_ = this ;
      IAlgorithm* a = const_cast<IAlgorithm*> (  a_ ) ;
      Hlt::Selection& s = hlt->selection ( key , a ) ;
      return &s ;
    }
    // ========================================================================
  protected:                                            // allowed constructors  
    // ========================================================================
    /** standard constructor 
     *  @param name the algorithm instance name 
     *  @param pSvc pointer to Service Locator 
     */
    HltUnit 
    ( const std::string& name ,                   //    algorithm instance name 
      ISvcLocator*       pSvc ) ;                 // pointer to Service Locator
    /// virtual & protected destructor 
    virtual ~HltUnit() ;                      // virtual & protected destructor 
    // ========================================================================
  private:                                                     // private stuff 
    // ========================================================================
    /// the default constructor is disabled 
    HltUnit () ;                         // the default constructor is disabled 
    /// the copy constructor is disabled 
    HltUnit ( const HltUnit& ) ;            // the copy constructor is disabled 
    /// the assignemet opeartor is disabled 
    HltUnit& operator=( const HltUnit& ) ;            // no assignemet operator
    // ========================================================================
  private:
    // ========================================================================
    /// ANN-service 
    mutable IANNSvc*     m_annSvc  ;                             // ANN-service 
    // ========================================================================
    /// the pointer to HLT data service 
    mutable IHltDataSvc* m_hltSvc  ;         // the pointer to HLT data service 
    /// the flag to switch on-of monitoring 
    bool                 m_monitor ;     // the flag to switch on-of monitoring 
    // ========================================================================
  private:
    // ========================================================================
    /// container of keys 
    typedef GaudiUtils::VectorMap<stringKey,      Hlt::Selection*>  Map;
    typedef GaudiUtils::VectorMap<stringKey,const Hlt::Selection*> CMap;
    /// keys for all selections 
    mutable CMap  m_all ;
    /// keys for all "output" selections 
    mutable  Map  m_out ;
    /// the functor itself
    LoKi::Types::FCut    m_cut ;                        // the functor itself
    // ======================================================================== 
  } ;
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
#endif // LOKI_HLTUNIT_H
// ============================================================================
