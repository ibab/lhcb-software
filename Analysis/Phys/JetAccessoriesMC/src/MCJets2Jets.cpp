// $Id: HepMCJets2Jets.cpp,v 1.1 2009-12-14 12:34:33 cocov Exp $
// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// DaVinci Kernel
// ============================================================================
#include "Kernel/IRelateJets.h"            // Interface
// ============================================================================
// Event
// ============================================================================
#include "Event/Particle.h"
#include "LoKi/Kinematics.h"
#include "LoKi/Geometry.h"

#include "Relations/Relation1D.h"
#include "Relations/Relation2D.h"

#include "LoKi/ParticleCuts.h"
#include "LoKi/MCParticleCuts.h"
#include "LoKi/PhysMCParticleCuts.h"

#include "LoKi/ILoKiSvc.h"

#include "LoKi/MCMatch.h"
#include "LoKi/MCTypes.h"
#include "LoKi/MCMatch.h"
#include "LoKi/MCMatchObj.h"

#include <GaudiKernel/IIncidentListener.h>
#include <GaudiKernel/IIncidentSvc.h>
#include "Kernel/IParticle2MCAssociator.h"

// ============================================================================
// LoKiGen
// ============================================================================
#include "LoKi/MCParticleCuts.h"
#include "LoKi/AlgoMC.h"
// ============================================================================

# include <limits>
namespace LoKi
{
  class IReporter;
}
namespace LoKi
{
  class MCJets2Jets
    : public virtual IRelateJets
    , public virtual IIncidentListener
    , public         GaudiTool
  {
    // ========================================================================
    /// friend factory for instantiation
    friend class ToolFactory<LoKi::MCJets2Jets> ;
  public:

    // ========================================================================
    /// standard initialization of the tool
    StatusCode initialize() ;
    StatusCode finalize() ;
    virtual void makeRelation( const IRelateJets::Jets& StdPrimaryJets,
                               const IRelateJets::Jets& StdSecondaryJets ,
                               IRelateJets::Table &tableMC2R ,
                               IRelateJets::Table &tableR2MC ) const ;

    virtual void handle( const Incident& );

  protected:
    // ========================================================================
    /// standard constructor
    MCJets2Jets
    ( const std::string& type   , ///< tool type ???
      const std::string& name   , ///< tool name
      const IInterface*  parent )
      : GaudiTool ( type, name , parent )
      , m_mcJet2MCP ( "Relations/Phys/MCJets2MCParticles" )
      , m_tables()
      , m_reporter(0)
      , m_matcher(0)
      , m_minR (0.2)

    {
      //
      declareInterface<IRelateJets>(this);
      //
      declareProperty ( "MCJet2MCP"            , m_mcJet2MCP   ,
                        "Relation table between StdHepMCParticles and corresponding HepMC::GenParticle"      ) ;
      m_tables.push_back ( "Relations/" + LHCb::ProtoParticleLocation::Charged  ) ;
      m_tables.push_back ( "Relations/" + LHCb::ProtoParticleLocation::Upstream ) ;
      m_tables.push_back ( "Relations/" + LHCb::ProtoParticleLocation::Neutrals ) ;
      declareProperty("RelTableLocations",  m_tables );
      declareProperty("MinRForP2MC", m_minR,
                      "Minimal R value to consider a MC 2 Rec particle match (avoid bad match from hadronic interaction or soft decay)");
    }
    /// virtual protected destructor
    virtual ~MCJets2Jets() {}
    // ========================================================================
  private:
    // ========================================================================
    // default constructor is disabled
    MCJets2Jets() ;
    // copy constructor is disabled
    MCJets2Jets( const MCJets2Jets& ) ;
    // assignement operator is disabled
    MCJets2Jets& operator=( const MCJets2Jets& ) ;


    inline LoKi::MCMatch matcher() const
    {
      return LoKi::MCMatch( m_matcher );
    }
    void addTables(LoKi::MCMatchObj* matcher) const;

    // ========================================================================
  private:
    // ========================================================================
    std::string     m_mcJet2MCP ;///< Location of relation table between HepMC and StdHepMC
    LoKi::ILoKiSvc* m_loki ;

    // for the mcmatching
    typedef std::vector<std::string> Addresses ;
    Addresses m_tables;
    LoKi::IReporter* m_reporter;
    LoKi::MCMatchObj* m_matcher;
    IIncidentSvc* m_incSvc;
    mutable bool m_loaded ;
    double m_minR;
    // ========================================================================
    // ========================================================================
  };
}
// end of namespace LoKi


// ============================================================================
// standard initialization of the tool
// ============================================================================
StatusCode LoKi::MCJets2Jets::initialize()
{
  const StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  // locate LoKi service (needed for some functions)
  m_loki = svc<LoKi::ILoKiSvc>( "LoKiSvc" , true ) ;

  m_reporter = tool<LoKi::IReporter>( "LoKi::Reporter", this ) ;

  if (msgLevel(MSG::VERBOSE)) {
    verbose() << "RelTableLocations: " << endmsg;
    for (Addresses::const_iterator iAddr = m_tables.begin(); iAddr!= m_tables.end(); ++iAddr) {
      verbose() << "\t" << *iAddr << endmsg;
    }
  }

  m_incSvc = svc<IIncidentSvc>("IncidentSvc", true);
  if (0!=m_incSvc) m_incSvc->addListener( this, IncidentType::EndEvent);

  // create the new matcher
  m_matcher = new LoKi::MCMatchObj( "P2MCRelator" , m_reporter ) ;
  // increment the reference counter
  m_matcher->addRef() ;

  m_loaded = false ;

  return (0!=m_reporter && 0!=m_matcher && 0!= m_incSvc && 0!=m_loki) ? sc : StatusCode::FAILURE;

}

void LoKi::MCJets2Jets::handle(const Incident&)
{
  if ( 0 != m_matcher ) { m_matcher->clear(); }
  m_loaded = false ;

}

StatusCode LoKi::MCJets2Jets::finalize()
{
  m_incSvc=0;

  if ( 0 != m_matcher )
  {
    m_matcher -> clear () ;
    long count = m_matcher->refCount();
    while ( 0 < count ) { count = m_matcher->release() ; }
    m_matcher = 0 ;
  }

  m_loaded = false ;

  return GaudiTool::finalize() ;
}

void LoKi::MCJets2Jets::makeRelation( const IRelateJets::Jets& StdPrimaryJets,
                                      const IRelateJets::Jets& StdSecondaryJets  ,
                                      IRelateJets::Table &tableMC2R , 
                                      IRelateJets::Table &tableR2MC ) const
{
  using namespace LoKi        ;
  using namespace LoKi::Types ;
  using namespace LoKi::Cuts  ;

  if ( !m_loaded ) { addTables ( m_matcher ) ; m_loaded = true ; }


  typedef LHCb::Relation1D<LHCb::Particle, LHCb::MCParticle> MCJets2MCP ;
  MCJets2MCP* MCJ2MCP =  get<MCJets2MCP> ( m_mcJet2MCP ) ;

  LoKi::MCMatch mc = this->matcher();

  // Loop over the MC jets and create the function for matching
  for(IRelateJets::Jets::const_iterator primjet =StdPrimaryJets.begin() ; 
      StdPrimaryJets.end()!= primjet ; primjet++)
  {
    MCJets2MCP::Range range =  MCJ2MCP->relations(*primjet);
    Cut matchedMC = PFALSE ;
    std::vector< LHCb::Particle * > ps_tmp;
    for ( MCJets2MCP::Range::const_iterator i_mcp = range.begin() ;
          range.end()!= i_mcp ; ++i_mcp ){
      LHCb::MCParticle *mcp = (*i_mcp).to();
      if (!mcp)continue;
      LHCb::Particle* p_tmp = new LHCb::Particle();
      p_tmp->setMomentum(mcp->momentum());
      Fun dr2 = DR2(p_tmp);
      ps_tmp.push_back(p_tmp);
      matchedMC = ( matchedMC || ( MCTRUTH(mc,mcp) && dr2  < m_minR*m_minR ) );
    }
    Fun fun = SUMTREE( E , matchedMC , 0.0 ) ;

    // Loop over the StdJets
    for( IRelateJets::Jets::const_iterator secjet =StdSecondaryJets.begin() ; 
         StdSecondaryJets.end()!= secjet ; secjet++ )
    {
      double matchedE = fun(*secjet);
      if ( fun(*secjet)<1e-2 ) continue;
      double recoEMatched = matchedE / E(*secjet);
      double mcEMatched = 0.;
      MCCut matchedRC = MCFALSE;
      std::vector< LHCb::MCParticle * > mcps_tmp;
      for ( SmartRefVector< LHCb::Particle >::const_iterator ip = 
              (*secjet)->daughters().begin();
            (*secjet)->daughters().end()!=ip;++ip )
      {
        LHCb::MCParticle* mcp_tmp = new LHCb::MCParticle;
        mcp_tmp->setMomentum((*ip)->momentum());
        mcps_tmp.push_back(mcp_tmp);
        MCFun dr2     = MCDR2(mcp_tmp);
        matchedRC = ( matchedRC || ( RCTRUTH(mc,*ip) && dr2  < m_minR*m_minR ) );
      }
      for ( MCJets2MCP::Range::const_iterator i_mcp = range.begin() ; 
            range.end()!= i_mcp ; ++i_mcp ){
        LHCb::MCParticle *mcp = (*i_mcp).to();
        if(!mcp) continue;
        if ( matchedRC(mcp) ) {
          mcEMatched += MCE(mcp);
        }
      }
      mcEMatched = mcEMatched/E(*primjet);
      tableMC2R.relate(*primjet,*secjet,mcEMatched);
      tableR2MC.relate(*secjet,*primjet,recoEMatched);
      while( !mcps_tmp.empty())
      {
        LHCb::MCParticle* p = mcps_tmp.back() ;
        mcps_tmp.pop_back() ;
        delete p ;
      }
    }
    while( !ps_tmp.empty())
    {
      LHCb::Particle* p = ps_tmp.back() ;
      ps_tmp.pop_back() ;
      delete p ;
    }
    //always()<<"g"<<endmsg;
  }
}
void LoKi::MCJets2Jets::addTables(LoKi::MCMatchObj* matcher) const
{
  for (Addresses::const_iterator item = m_tables.begin(); item!=m_tables.end(); ++item) {
    const std::string& address = *item;
    if (exist<LoKi::Types::TablePP2MC>(address) ) {
      LoKi::Types::TablePP2MC* table = get<LoKi::Types::TablePP2MC>(address);
      matcher->addMatchInfo(table);
    } else if (exist<LoKi::Types::TableP2MC>(address) ) {
      LoKi::Types::TableP2MC* table = get<LoKi::Types::TableP2MC>(address);
      matcher->addMatchInfo(table);
    } else {
      Error ( " There is no valid data at '" + address + "'" ).ignore() ;
    }
  }
  m_loaded = true ;
}

// ============================================================================
/// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,MCJets2Jets)

// ============================================================================
// The END
// ============================================================================
