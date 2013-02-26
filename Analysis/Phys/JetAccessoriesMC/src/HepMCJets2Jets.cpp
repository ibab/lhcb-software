// $Id: HepMCJets2Jets.cpp,v 1.1 2009-12-14 12:34:33 cocov Exp $
// Include files
#include "HepMCJets2Jets.h"


// ============================================================================
// standard initialization of the tool 
// ============================================================================
StatusCode LoKi::HepMCJets2Jets::initialize() 
{
  StatusCode sc = GaudiTool::initialize();
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
 
  if  (m_MatchingMethodString == "MCtrue") // have to use string here because initMethod() hasn't been called yet
    m_p2mcAssoc = tool<IParticle2MCAssociator>(m_p2mcAssocType, this);


  return (0!=m_reporter && 0!=m_matcher && 0!= m_incSvc && 0!= m_loki&&initMethod()) ? sc : StatusCode::FAILURE;

}

void LoKi::HepMCJets2Jets::handle(const Incident&) 
{
  if ( 0 != m_matcher ) { m_matcher->clear(); }
  m_loaded = false ;

}

StatusCode LoKi::HepMCJets2Jets::finalize()
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

void LoKi::HepMCJets2Jets::makeRelation( const IJets2Jets::Jets& StdPrimaryJets  
                                         , const IJets2Jets::Jets& StdSecondaryJets  ,IJets2Jets::Table &table ) const 
{
  using namespace LoKi        ;
  using namespace LoKi::Types ;
  using namespace LoKi::Cuts  ;
  typedef std::vector<const LHCb::Particle*> Parts;
 
  if ( !m_loaded ) { addTables ( m_matcher ) ; m_loaded = true ; }


  // Table to get the HepMCParticle from the StdHepMCParticle
	typedef LHCb::Relation2D<LHCb::Particle,HepMC::GenParticle*>  Table2DHepMC2Part ;

 
  LHCb::HepMC2MC2D* tableHepMC2MC = 
    get<LHCb::HepMC2MC2D> ( LHCb::HepMC2MCLocation::Default ) ;
    
  LoKi::MCMatch mc = this->matcher();

  Table2DHepMC2Part* table_togetHepMC = get<Table2DHepMC2Part>(m_tableHepMC2StdHepMC);
  // Loop over the StdJets
  for( IJets2Jets::Jets::const_iterator secjet =StdSecondaryJets.begin() ; StdSecondaryJets.end()!= secjet ; secjet++ )
  { 
    // Loop over the StdHepMCJets
    Parts daug_secjet;    
    LoKi::Extract::getParticles (*secjet,std::back_inserter (daug_secjet), PID!="CELLjet");

    for(IJets2Jets::Jets::const_iterator primjet =StdPrimaryJets.begin() ; StdPrimaryJets.end()!= primjet ; primjet++)
    {
      
      Parts daug_primjet;
      LoKi::Extract::getParticles (*primjet,std::back_inserter (daug_primjet), PID!="CELLjet");
      double weight_jetsec_jetprim = 0.;

      // Object to store the HepMCParticles of the jet
      LoKi::GenTypes::GenContainer TheHepMCVect;

      //Loop over the StdHepMC Particles of the HepMCjet
      for(Parts::iterator idaug_primjet = daug_primjet.begin() ; daug_primjet.end()!= idaug_primjet  ; idaug_primjet++ )
      {
        Table2DHepMC2Part::Range links = table_togetHepMC ->relations (*idaug_primjet ) ;
        for ( Table2DHepMC2Part::iterator ilink = links.begin() ;links.end() != ilink ; ++ilink )
        {
          const HepMC::GenParticle* gp  = ilink->to() ;
          if ( 0 == gp ) { continue ;}
          TheHepMCVect.push_back(gp);
        }
      }
      Cut StdMC_fromGen = GMCTRUTH ( TheHepMCVect , tableHepMC2MC , mc ) ;
      
      //Loop over the Std Particles of the jet
      for(Parts::iterator idaug_secjet = daug_secjet.begin() ; daug_secjet.end() != idaug_secjet   ; idaug_secjet++ )
      {
        
        if (0 ==(*idaug_secjet)) { continue ;}
        if(StdMC_fromGen(*idaug_secjet))
	  {
	  if (m_MatchingMethod==measured)
           weight_jetsec_jetprim += E(*idaug_secjet)/E(*primjet);
	  if (m_MatchingMethod==MCtrue)
	    {
	      debug()<< "Matching reco particle to MC thruth." << endmsg;
	      Assert( m_p2mcAssoc, "The DaVinci smart associator hasn't been initialized!");
	      for(Parts::iterator idaug_primjet = daug_primjet.begin() ; daug_primjet.end()!= idaug_primjet  ; idaug_primjet++ )
		{ 
		  if (msgLevel(MSG::DEBUG)) 
		    {
		      debug () << "Matching Rec Particle:";
		      debug () << (*idaug_primjet);
		      debug () << (*idaug_primjet)->momentum();
		      debug () << (*idaug_primjet)->momentum().E()<< endmsg;
		      
		    }
		  const LHCb::MCParticle* AssocMPC  = m_p2mcAssoc->relatedMCP(*idaug_secjet);
		  if (AssocMPC)
		    {
		      if (msgLevel(MSG::DEBUG)) 
			{ 
			  debug () << "Associated MC Particle";
			  debug () << AssocMPC << endmsg;
			  debug () << AssocMPC->momentum() << endmsg;
			  debug () << AssocMPC->momentum().E() << endmsg;
			  debug() << "Associated MCParticle found for truth matched rec Particle " << *idaug_primjet << " of type " << (*idaug_primjet)->particleID()  << endmsg;
			  
			}
		      if  ( AssocMPC->momentum().E()/(*idaug_primjet)->momentum().E()>	m_matchingP&&
			    AssocMPC->momentum().E()/(*idaug_primjet)->momentum().E()<(1.0/	m_matchingP))
			{
			  weight_jetsec_jetprim += m_p2mcAssoc->relatedMCP(*idaug_secjet)->momentum().E()/E(*primjet);
			  break; 
			}
		    }
		  else 
		    error() << "No Associated MCParticle found for truth matched rec Particle " << *idaug_primjet << " of type " << (*idaug_primjet)->particleID()  << endmsg;
		   
		}
	    }
	  }
        
      }
      
      if(weight_jetsec_jetprim>0) table.relate(*primjet,*secjet,weight_jetsec_jetprim);
    }
  }

}
void LoKi::HepMCJets2Jets::addTables(LoKi::MCMatchObj* matcher) const 
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
bool LoKi::HepMCJets2Jets::initMethod()
{
  if (m_MatchingMethodString =="measured")
    {
      m_MatchingMethod = measured;
      return true;
    }
   if (m_MatchingMethodString =="MCtrue")
     {
      m_MatchingMethod = MCtrue;
      return true;
    }
    m_MatchingMethod = uninitialised;
   return false;
}


// ============================================================================
/// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,HepMCJets2Jets)

// ============================================================================
// The END 
// ============================================================================
