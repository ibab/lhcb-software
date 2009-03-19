// $Id: HltUnit.h,v 1.6 2009-03-19 13:16:12 ibelyaev Exp $
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
#include "Kernel/IANNSvc.h"
// ============================================================================
// LoKi 
// ============================================================================
#include "LoKi/FilterAlg.h"
#include "LoKi/IHltUnit.h"
#include "LoKi/ILoKiSvc.h"
#include "LoKi/CoreTypes.h"
// ============================================================================
#include "LoKi/IHltRegister.h"
#include "LoKi/IHltData.h"
#include "LoKi/IHltInspector.h"
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
    /// initialize the algorithm
    // virtual StatusCode initialize () ;
    // ========================================================================
    /// execute the algorithm
    virtual StatusCode execute    () ;
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
     *  @param selection the selection to be registered 
     *  @param client the client 
     *  @return status code 
     */
    virtual StatusCode 
    registerOutput 
    ( Hlt::Selection* selection , 
      const Client&   client    ) const ;
    // ========================================================================
    /** declare the input selection 
     *  @param key the selection key 
     *  @param client the client 
     */
    virtual const Hlt::Selection* 
    declareInput 
    ( const Key&      key       , 
      const Client&   client    ) const ;
    // ========================================================================
    /** get the (const) selection by key 
     *  @attention only *local input* selections are available 
     *  @param key the key 
     *  @return pointer to the selection 
     */
    virtual const Hlt::Selection* 
    selection 
    ( const Key&    key    , 
      const Client& client ) const ;
    // =========================================================================
    /** get the (const) selection by key  (anonymous)  
     *  @param key the key 
     *  @return pointer to the selection 
     */
    virtual const Hlt::Selection* 
    selection ( const Key& key ) const ;
    // =========================================================================
  public:
    // =========================================================================
    /** get the selection by key (non-const)
     *  @param key the key 
     *  @return pointer to the selection 
     */
    virtual Hlt::Selection* 
    retrieve  
    ( const Client& /* client */ , 
      const Key&    /* key    */ ) const 
    {
      Error("retrive(): not implemented ") ;
      return 0 ;
    }
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
    /// monitor the selections? 
    inline bool monitor() const { return m_monitor || msgLevel ( MSG::DEBUG ) ; }
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
  private: // protected:
    // ========================================================================
    /// "Assigned Numbers & Names" service 
    inline IANNSvc* annSvc () const       // "Assigned Numbers & Names" service 
    {
      if ( m_annSvc.validPointer() ) { return m_annSvc ; } 
      m_annSvc = svc<IANNSvc> ( "Hlt::Service" , true ) ;
      return m_annSvc ;
    }
    /// Hlt Register service 
    inline Hlt::IRegister* regSvc () const              // Hlt Register Service 
    {
      if ( m_regSvc.validPointer() ) { return m_regSvc; }
      m_regSvc = svc<Hlt::IRegister> ( "Hlt::Service" , true ) ;
      return m_regSvc ;
    }
    /// Hlt Data service 
    inline Hlt::IData* hltSvc     () const                  // Hlt Data Service
    {
      if ( m_hltSvc.validPointer() ) { return m_hltSvc; }
      m_hltSvc = svc<Hlt::IData> ( "Hlt::Service" , true ) ;
      return m_hltSvc ;
    }
    /// LoKi service 
    inline LoKi::ILoKiSvc* lokiSvc () const 
    {
      if ( m_lokiSvc.validPointer() ) { return m_lokiSvc; }
      m_lokiSvc = svc<LoKi::ILoKiSvc> ( "LoKiSvc" , true ) ;
      return m_lokiSvc ;
    } 
    // ========================================================================
  private: // services 
    // ========================================================================
    /// "Assigned Numbers & Names" service 
    mutable LoKi::Interface<IANNSvc>  m_annSvc  ;  // "Assigned Numbers & Names"
    /// Hlt Register service 
    mutable LoKi::Interface<Hlt::IRegister> m_regSvc ;        //   Hlt Register
    /// Hlt Data serrvice 
    mutable LoKi::Interface<Hlt::IData>     m_hltSvc ;     //  Hlt Data Service
    /// LoKi service 
    mutable LoKi::Interface<LoKi::ILoKiSvc> m_lokiSvc ;   // major LoKi service 
    // ========================================================================
  private: 
    // ========================================================================
    /// the flag to switch on-of monitoring 
    bool                 m_monitor ;     // the flag to switch on-of monitoring 
    // ========================================================================
  private:
    // ========================================================================
    /// container of keys 
    typedef GaudiUtils::VectorMap<Key,      Hlt::Selection*> OMap;
    typedef GaudiUtils::VectorMap<Key,const Hlt::Selection*> IMap;
    /// keys for all "input"  selections 
    mutable IMap      m_in  ;               // keys for all "input"  selections 
    /// keys for all "output" selections 
    mutable OMap      m_out ;               // keys for all "output" selections 
    /// the functor itself
    LoKi::Types::FCut m_cut ;                             // the functor itself
    // ======================================================================== 
  } ; // 
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
#endif // LOKI_HLTUNIT_H
// ============================================================================
