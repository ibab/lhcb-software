// $Id: HepMCJets2Jets.h,v 1.1 2009-12-14 12:34:33 cocov Exp $
// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// DaVinci Kernel 
// ============================================================================
#include "Kernel/IJets2Jets.h"            // Interface
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
#include "LoKi/GenParticleCuts.h" 
#include "LoKi/GenExtract.h" 
#include "LoKi/AlgoMC.h" 
// ============================================================================

# include <limits>
namespace LoKi 
{
  class IReporter;
}
namespace LoKi
{
  class HepMCJets2Jets
    : public virtual IJets2Jets
    , public virtual IIncidentListener
    , public         GaudiTool
  {
    // ========================================================================
    /// friend factory for instantiation 
    friend class ToolFactory<LoKi::HepMCJets2Jets> ;
  public: 

    // ========================================================================
    /// standard initialization of the tool
    StatusCode initialize() ;
    StatusCode finalize() ;
    virtual void makeRelation( const IJets2Jets::Jets& StdPrimaryJets 
                                          , const IJets2Jets::Jets& StdSecondaryJets ,IJets2Jets::Table &table ) const ;

    virtual void handle( const Incident& );

  protected:
    // ========================================================================
    /// standard constructor
    HepMCJets2Jets 
    ( const std::string& type   , ///< tool type ??? 
      const std::string& name   , ///< tool name 
      const IInterface*  parent ) 
      : GaudiTool ( type, name , parent )
      , m_tableHepMC2StdHepMC ( "Relations/Phys/StdHepMCRef2HepMC" )
      , m_tables()
      , m_reporter(0)
      , m_matcher(0)
      , m_MatchingMethod(uninitialised)	
      , m_p2mcAssoc(0)
      //      , m_p2mcAssocType("DaVinciSmartAssociator")
      , m_p2mcAssocType("MCMatchObjP2MCRelator")
    , m_matchingP(0.999)
  
    {
      //
      declareInterface<IJets2Jets>(this);
      //
      declareProperty ( "StdHepMC2HepMCTable"            , m_tableHepMC2StdHepMC   ,
                        "Relation table between StdHepMCParticles and corresponding HepMC::GenParticle"      ) ;  
      m_tables.push_back ( "Relations/" + LHCb::ProtoParticleLocation::Charged  ) ;
      m_tables.push_back ( "Relations/" + LHCb::ProtoParticleLocation::Upstream ) ;
      m_tables.push_back ( "Relations/" + LHCb::ProtoParticleLocation::Neutrals ) ;
      declareProperty("RelTableLocations",  m_tables );
      declareProperty("MatchingMethod", m_MatchingMethodString = "measured",
		      "Possible values : 'measured', and 'MCtrue'");
      declareProperty("MatchingMomentum", m_matchingP = 0.999,
		      "Require the HepMC particle to that close to the MC particle. 1 for stricly equality negative to disable this cut.");
      if (m_matchingP<=0)
	m_matchingP = std::numeric_limits<double>::min();
    } 
    /// virtual protected destructor 
  virtual ~HepMCJets2Jets() {} 
    // ========================================================================
  private:
    // ========================================================================
    // default constructor is disabled 
    HepMCJets2Jets() ;
    // copy constructor is disabled 
    HepMCJets2Jets( const HepMCJets2Jets& ) ;
    // assignement operator is disabled 
    HepMCJets2Jets& operator=( const HepMCJets2Jets& ) ;

    
    inline LoKi::MCMatch matcher() const 
    {
      return LoKi::MCMatch( m_matcher );
    }
    void addTables(LoKi::MCMatchObj* matcher) const;

    // ========================================================================
  private:
    // ======================================================================== 
    std::string       m_tableHepMC2StdHepMC ;///< Location of relation table between HepMC and StdHepMC
    LoKi::ILoKiSvc* m_loki ;

    // for the mcmatching
    typedef std::vector<std::string> Addresses ;
    Addresses m_tables;
    LoKi::IReporter* m_reporter;
    LoKi::MCMatchObj* m_matcher;
    IIncidentSvc* m_incSvc;
    
    mutable bool m_loaded ;
    std::string m_MatchingMethodString;
    enum MatchingMethod { uninitialised = false,
			  measured,
			  MCtrue};
    MatchingMethod m_MatchingMethod;
    bool initMethod();
    IParticle2MCAssociator* m_p2mcAssoc;
    std::string m_p2mcAssocType;
    double m_matchingP;

    // ========================================================================
    // ========================================================================
  };
}
 // end of namespace LoKi 
