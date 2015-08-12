#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiAlg/GaudiHistoAlg.h"
#include "Event/TwoProngVertex.h"
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
#include "TrackInterfaces/ITrackVertexer.h"
#include "gsl/gsl_cdf.h"

class TrackV0Monitor : public GaudiHistoAlg 
{
public:

   /** Standard construtor */
  TrackV0Monitor( const std::string& name, ISvcLocator* pSvcLocator );

  /** Destructor */
  virtual ~TrackV0Monitor();

  /** Algorithm initialize */
  virtual StatusCode initialize();

  /** Algorithm initialize */
  virtual StatusCode finalize();

  /** Algorithm execute */
  virtual StatusCode execute();

private:
  void process( const LHCb::TwoProngVertex& vertex,
		const LHCb::RecVertex::Range& pvs, 
		const std::string& dir) const ;
  void computeDecayLength(const LHCb::TwoProngVertex& vertex,
			  const LHCb::RecVertex::Range& pvs,
			  double& ipchi2,
			  double& decaylength,
			  double& decaylengtherr) const ;
private:
  ToolHandle<ITrackVertexer> m_vertexer ;
  std::string m_v0Location ;
  double m_pionmass ;
  double m_protonmass ;
  double m_maxIPChi2 ;
} ;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TrackV0Monitor )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackV0Monitor::TrackV0Monitor( const std::string& name,
				ISvcLocator* pSvcLocator)
  : GaudiHistoAlg( name , pSvcLocator ), 
  m_vertexer("TrackVertexer"),
  m_pionmass(0.),
  m_protonmass(0.),
  m_maxIPChi2(25.0)
{
  declareProperty( "V0Location", m_v0Location = LHCb::TwoProngVertexLocation::Default ) ;
  declareProperty( "MaxIPChi2", m_maxIPChi2 ) ;
}

//=============================================================================
// Destructor
//=============================================================================
TrackV0Monitor::~TrackV0Monitor() 
{
}

StatusCode TrackV0Monitor::initialize()
{
  StatusCode sc = GaudiHistoAlg::initialize() ;
  LHCb::IParticlePropertySvc* propertysvc = 
    svc<LHCb::IParticlePropertySvc>("LHCb::ParticlePropertySvc",true) ;
  if( propertysvc ) {
    const LHCb::ParticleProperty* pion = propertysvc->find( "pi+" ) ;
    m_pionmass   = pion ? pion->mass() : 0 ;
    const LHCb::ParticleProperty* proton = propertysvc->find( "p+" ) ;
    m_protonmass = proton ? proton->mass() : 0 ;
    // const LHCb::ParticleProperty* kshort = propertysvc->find( "KS0" ) ;
    //     m_k0pid = kshort ? LHCb::ParticleID(kshort->pdgID()) : 0 ;
    //     const LHCb::ParticleProperty* lambda = propertysvc->find( "Lambda0" );
    //     m_lambdapid     = lambda ? LHCb::ParticleID(lambda->pdgID()) : 0 ;
    //     m_lambdabarpid = lambda ? LHCb::ParticleID(lambda->antiParticle()->pdgID()) : 0 ;
  } else {
    sc = StatusCode::FAILURE ;
  }
  
  if( sc.isSuccess() ) 
    sc = m_vertexer.retrieve() ;

  return sc ;
}

StatusCode TrackV0Monitor::finalize()
{
  m_vertexer.release().ignore() ;
  return GaudiHistoAlg::finalize() ;
}
  
void TrackV0Monitor::process( const LHCb::TwoProngVertex& vertex, 
			      const LHCb::RecVertex::Range& pvs,
			      const std::string& dir) const
{
  static double chi2max =  gsl_cdf_chisq_Qinv (1e-6,1); 
  double chi2 = vertex.chi2() ;
  double chi2prob = chi2<chi2max ?  gsl_cdf_chisq_Q(chi2,1) : 0 ;
  double pipimass = vertex.mass(m_pionmass,m_pionmass) ;
  double ppimass = vertex.mass(m_protonmass,m_pionmass) ;
  double pipmass = vertex.mass(m_pionmass,m_protonmass) ;
  if( vertex.trackA()->firstState().qOverP() < 0) std::swap(pipmass,ppimass) ;

  // compute the ipchi2 and the decaylength
  double ipchi2(-1), decaylength(0), decaylengtherr(1) ;
  computeDecayLength(vertex,pvs,ipchi2,decaylength,decaylengtherr) ; 
  plot( ipchi2 , dir + "/ipchi2", "ip chi2", 0, 100 ) ;
  if( ipchi2 < m_maxIPChi2 ) {
    plot( chi2prob,dir + "/chi2prob","chi2prob", 0,1 ) ;
    plot( vertex.position().z(), dir + "/z", "z", -100,2000) ;
    plot( pipimass, dir + "/pipimass","pi+ pi- mass", 450,550 ) ;
    plot( ppimass,  dir + "/ppimass","p+ pi- mass", 1090,1140 ) ;
    plot( pipmass,  dir + "/pipmass","p- pi+ mass", 1090,1140 ) ;
    plot( decaylength/decaylengtherr ,  dir + "/dls","decay length significance", -10, 100 ) ;
  }

  if( fullDetail() ) {
    plot( pipimass, dir + "/pipimassall","pi+ pi- mass", 450,550 ) ;
    plot( ppimass,  dir + "/ppimassall","p+ pi- mass", 1090,1140 ) ;
    plot( pipmass,  dir + "/pipmassall","p- pi+ mass", 1090,1140 ) ;
    plot( decaylength/decaylengtherr ,  dir + "/dlsall","decay length significance", -10, 100 ) ;
    plot( vertex.position().x(), dir + "/x", "x", -100,100) ;
    plot( vertex.position().y(), dir + "/y", "y", -100,100) ;
    plot( vertex.info( 1000, -1 ), dir + "/pvchi2", "pv chi2", 0, 100 ) ;
    plot( vertex.info( 1001, -1 ), dir + "/flightsign", "flight significance", 0, 100 ) ;
  }
}

StatusCode TrackV0Monitor::execute()
{ 
  setHistoTopDir("Track/") ;
  
  const LHCb::TwoProngVertices* v0container = 
    getIfExists<LHCb::TwoProngVertices>(  m_v0Location ) ;
  if ( NULL == v0container ) 
    return Warning( m_v0Location+" not found", StatusCode::SUCCESS, 0) ;

  plot(v0container->size(),"v0multiplicity","v0multiplicity",-0.5,20.5,21) ;
  
  // Get the PVs, to compute the IP chi2
  LHCb::RecVertex::Range pvs = get<LHCb::RecVertex::Range>(  LHCb::RecVertexLocation::Primary ) ;
  
  for( LHCb::TwoProngVertices::const_iterator iver = v0container->begin() ;
       iver != v0container->end(); ++iver ) {
    //
    const LHCb::TwoProngVertex* vertex = *iver ;

     // determine the type: Long-Long, DS-DS, or DS-Long
    const LHCb::Track::Types trackA = vertex->trackA()->type() ;
    const LHCb::Track::Types trackB = vertex->trackB()->type() ;
    
    std::string type = 
      trackA == LHCb::Track::Long && 
      trackB == LHCb::Track::Long ? 
      "LongLong" :
      trackA == LHCb::Track::Downstream && 
      trackB == LHCb::Track::Downstream ? 
      "DownstreamDownstream" : "Mixed" ;
    process(*vertex, pvs, type ) ;
  }
  
  return StatusCode::SUCCESS ;
}


void
TrackV0Monitor::computeDecayLength(const LHCb::TwoProngVertex& vertex,
				   const LHCb::RecVertex::Range& pvs,
				   double& ipchi2,
				   double& decaylength,
				   double& decaylengtherr) const
{
  // some default values
  ipchi2 = -1 ;
  decaylength = 0 ;
  decaylengtherr = 1 ;

  for( LHCb::RecVertex::Range::const_iterator ipv = pvs.begin() ;
       ipv != pvs.end(); ++ipv ) {
    double thisipchi2(-1), thisdecaylength(0), thisdecaylengtherr(1) ;
    bool success = m_vertexer->computeDecayLength( vertex, **ipv,
						   thisipchi2, thisdecaylength, thisdecaylengtherr) ;
    if( success &&
	(ipchi2<0 || thisipchi2<ipchi2) ) {
      ipchi2 = thisipchi2 ;
      decaylength = thisdecaylength ;
      decaylengtherr = thisdecaylengtherr ;
    }
  }
}
