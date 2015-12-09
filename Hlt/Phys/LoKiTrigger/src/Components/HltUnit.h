// $Id$
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
#include "GaudiKernel/HashMap.h"
#include "GaudiKernel/IIncidentListener.h"
// ============================================================================
// HltBase
// ============================================================================
#include "Kernel/IANNSvc.h"
// ============================================================================
// DaVinci Interfaces
// ============================================================================
#include "Kernel/IDVAlgorithm.h"
#include "Kernel/IVertexFit.h"
#include "Kernel/ILifetimeFitter.h"
#include "Kernel/IDistanceCalculator.h"
#include "Kernel/IRelatedPVFinder.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/FilterAlg.h"
#include "LoKi/ILoKiSvc.h"
#include "LoKi/CoreTypes.h"
// ============================================================================
#include "HltBase/IHltUnit.h"
#include "HltBase/IHltRegister.h"
#include "HltBase/IHltData.h"
#include "HltBase/IHltInspector.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/Particle.h"
#include "Event/RecVertex.h"
namespace LoKi
{
  // ==========================================================================
  /** @class HltUnit HltUnit.h LoKi/HltUnit.h
   *  Simple algorithm with pretends to be a replacement of
   *  the major part of the python HltConf.Hlt1Line stuff
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date   2008-11-10
   */
  class HltUnit
    :         public LoKi::FilterAlg
    , virtual public Hlt::IUnit
    , virtual public IIncidentListener
    , virtual public IDVAlgorithm
  {
    // ========================================================================
    // the friend factory for instantiation
    friend class AlgFactory<LoKi::HltUnit> ;
    // ========================================================================
  public:                                                          // Algorithm
    // ========================================================================
    /// initialize the algorithm
    virtual StatusCode initialize () override;
    // ========================================================================
    /// execute the algorithm
    StatusCode execute    () override;
    // ========================================================================
  public:                                                    // LoKi::FilterAlg
    // ========================================================================
    /** decode the functor
     *  @see LoKi::FilterAlg
     *  @see LoKi::ICoreFactory
     */
    StatusCode decode  () override;
    // ========================================================================
    /** define the code
     *  @see LoKi::FilterAlg
     *  @see LoKi::ICoreFactory
     */
    StatusCode defineCode () ;
    // ========================================================================
    /** define the stream
     *  @see LoKi::FilterAlg
     *  @see LoKi::ICoreFactory
     */
    StatusCode setCode ( const LoKi::Types::FCuts& cut ) ; // functor
    // ========================================================================
  public:                                                // Hlt::IUnit part
    // ========================================================================
    /** register the selection
     *  (internal method, should not be invoked directly)
     *  @param selection the selection to be registered
     *  @param client the client
     *  @return status code
     */
    StatusCode registerOutput
    ( std::unique_ptr<Hlt::Selection> selection ,
      const Client&   client    ) const override;
    // ========================================================================
    /** declare the input selection
     *  @param key the selection key
     *  @param client the client
     */
    const Hlt::Selection* declareInput
    ( const Key&      key       ,
      const Client&   client    ) const override;
    // ========================================================================
    /** get the (const) selection by key
     *  @attention only *local input* selections are available
     *  @param key the key
     *  @return pointer to the selection
     */
    const Hlt::Selection* selection
    ( const Key&    key    ,
      const Client& client ) const override;
    // =========================================================================
    /** get the (const) selection by key  (anonymous)
     *  @param key the key
     *  @return pointer to the selection
     */
    const Hlt::Selection*
    selection ( const Key& key ) const override;
    // =========================================================================
    /** get the object form TES
     *  @param client the client
     *  @param location the TES location
     *  @return the object
     */
    const DataObject* tes
    ( const Client& client   ,
      const Key&    location ) const override;
    // =========================================================================
    /** register the query  to TES-selection
     *  @param location TES location to be registered
     *  @param consumer algorithm/consumer
     *  @return Status Code
     */
    StatusCode   registerTESInput
    ( const Key&         location  ,                    //          TES location
      const Client&      client    ) const override;    //                client
    // =========================================================================
 public:
    // =========================================================================
    /** get the selection by key (non-const)
     *  @param key the key
     *  @return pointer to the selection
     */
    Hlt::Selection* retrieve
    ( const Client& /* client */ ,
      const Key&    /* key    */ ) const override
    {
      Error("retrieve(): not implemented ") ;
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
    StatusCode queryInterface
    (const InterfaceID& iid, void** ppvi ) override;
    // ========================================================================
  public:  // IDVAlgorithm stuff
    // ==========================================================================
    /// get the distance calculator tool
    virtual const IDistanceCalculator*
    distanceCalculator    ( const std::string& nickname = "DistanceCalculator" ) const override
    { return getTool<IDistanceCalculator>( nickname ); }
    /// get lifetime fitter tool
    virtual const ILifetimeFitter*
    lifetimeFitter        ( const std::string& nickname = "LifetimeFitter" ) const override
    { return getTool<ILifetimeFitter>    ( nickname ); }
    /// get the vertex fitter tool
    virtual const IVertexFit*
    vertexFitter          ( const std::string& nickname = "VertexFitter" ) const override
    { return getTool<IVertexFit>( nickname ); }
    /// get particle re-fitter tool
    virtual const IParticleReFitter*
    particleReFitter      ( const std::string& ) const override { return 0; }
    /// get particle filter tool
    virtual const IParticleFilter*
    particleFilter        ( const std::string& ) const override { return 0; }
    /// Accessor for decay-tree fitter
    virtual       IDecayTreeFit*
    decayTreeFitter       ( const std::string& ) const override { return 0; }
    /// Accessor for ParticleCombiner tool
    virtual const IParticleCombiner*
    particleCombiner      ( const std::string& nickname = "ParticleCombiner" ) const override
    { return getTool<IParticleCombiner>( nickname ); }
    /// Accessor for mass-fitter tool
    virtual const IMassFit*
    massFitter            ( const std::string& ) const override { return 0; }
    /// Accessor for direction-fitter tool
    virtual const IDirectionFit*
    directionFitter       ( const std::string& ) const override { return 0; }
    /// Accessor for primary vertex re-fitter tool
    virtual const IPVReFitter*
    primaryVertexReFitter ( const std::string& ) const override { return 0; }
    /// Access the Flavour tagging tool
    virtual IBTaggingTool* flavourTagging() const override { return 0; }
    // ==========================================================================
  public: // Incidents
    // ==========================================================================
    void handle( const Incident& incident ) override;
    // ==========================================================================
  public: // data
    // ==========================================================================
    /// Return a container of local LHCb::Particle*
    virtual const LHCb::Particle::Range
    particles       () const override { return LHCb::Particle::Range(); }
    /// Return a container of LHCb::RecVertex*, containing primary vertices.
    virtual const LHCb::RecVertex::Range
    primaryVertices () const override;
    /// Return the best primary vertex for a given LHCb::Particle.
    virtual const LHCb::VertexBase*
    bestVertex      ( const LHCb::Particle* ) const override;
    /// unrelated the related PV
    virtual void unRelatePV ( const LHCb::Particle* p ) const override
    { m_p2PVMap.erase ( p ) ; }
    /// Return gaudi algorithm
    virtual const GaudiAlgorithm* gaudiAlg ( ) const override { return this; }
    // ==========================================================================
  protected:
    // ========================================================================
    /// monitor the selections?
    inline bool monitor() const { return m_monitor || msgLevel ( MSG::DEBUG ) ; }
    // ========================================================================
  protected:                                            // allowed constructors
    // ========================================================================
    /// update handler for "Params"-property
    void updateParams ( Property& /* p */ ) ;
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
    ~HltUnit() override;                      // virtual & protected destructor
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
  private: // tool helper
    // ========================================================================
    typedef std::map<std::string, std::string> ToolMap;
    typedef std::map<std::string, IAlgTool*> Tools;
    typedef GaudiUtils::HashMap<const LHCb::Particle*, const LHCb::VertexBase*> P2PVMap;
    // ========================================================================
    template <class ToolInterface>
    const ToolInterface* getTool( const std::string& nickname ) const
    {
      if ( nickname == "" )
      {
        // incase noname return DistanceCalculator
        auto it_nick = m_toolNames.find("DistanceCalculator");
        if ( it_nick == m_toolNames.end() )
        {
          this->Exception(std::string("Default Tool DistanceCalulator tried and not found"));
        }
        auto tool = this->template tool<ToolInterface>(it_nick->second, this);
        if ( !tool )
        {
          this->Exception(std::string("Could not retrieve default tool of type DistanceCalulator"));
        }
        m_tools["DistanceCalulator"] = tool;
        return tool;
      }
      auto it_tool = m_tools.find(nickname);
      if ( it_tool != m_tools.end() )
      {
        return dynamic_cast<const ToolInterface*>(it_tool->second);
      }
      auto it_nick = m_toolNames.find(nickname);
      if ( it_nick == m_toolNames.end() )
      {
        this->Exception(std::string("No entry in Tools property for nickname ")
                        + nickname + std::string("."));
      }
      auto tool = this->template tool<ToolInterface>(it_nick->second, this);
      if ( !tool )
      {
        this->Exception(std::string("Could not retrieve tool of type ")
                        + it_nick->second + std::string("."));
      }
      m_tools[nickname] = tool;
      return tool;
    }
    // ========================================================================
    const IRelatedPVFinder* relatedPVFinder( const std::string& nickname = "RelatedPVFinder" ) const
    { return getTool<IRelatedPVFinder>( nickname ); }
    // ========================================================================
  private:
    // ========================================================================
    /// the flag to switch on-of monitoring
    bool                 m_monitor ;     // the flag to switch on-of monitoring
    // ========================================================================
    Gaudi::StringKey     m_pvSelection ; // HLT PV selection
    // ========================================================================
    ToolMap  m_toolNames ; // HLT PV selection
    // ========================================================================
  private:
    // ========================================================================
    std::vector<std::string>  m_preloadTools ; // Tools to preload
    bool m_decode;
    // Actual tools
    mutable Tools m_tools ;
    // ========================================================================
    // P2PV table
    mutable P2PVMap m_p2PVMap ;
    // ========================================================================
  private:
    // ========================================================================
    /// container of keys
    //FIXME:TODO: OMap should have a std::unique_ptr as value, but VectorMap
    //            does not support 'move' insertion...
    typedef GaudiUtils::VectorMap<Key,      Hlt::Selection*> OMap ;
    typedef GaudiUtils::VectorMap<Key,const Hlt::Selection*> IMap ;
    typedef std::vector<Key>                                 LVct ;
    /// keys for all "input"  selections
    mutable IMap      m_in  ;               // keys for all "input"  selections
    /// keys for all "output" selections
    mutable OMap      m_out ;               // keys for all "output" selections
    /// keys for all TES input locations
    mutable LVct      m_tes ;               // keys for all TES input locations
    /// the functor itself
    LoKi::Types::FCut m_cut ;                             // the functor itself
    // ========================================================================
  private: // parameters
    // ========================================================================
    // typedef GaudiUtils::VectorMap<std::string,double>  Params ;
    typedef std::map<std::string,double>  Params ;
    // ========================================================================
    Params   m_params     ;
    Params   m_params_old ;
    // ========================================================================
  } ; //
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
#endif // LOKI_HLTUNIT_H
// ============================================================================
