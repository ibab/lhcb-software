// $Id: TupleToolTwoParticleMatching.cpp,v 1.0 2015-02-27 08:59:11 ikomarov $

#include "TupleToolTwoParticleMatching.h"
#include "Kernel/HashIDs.h"
#include "Kernel/ParticleProperty.h"
#include "Kernel/IParticlePropertySvc.h"
#include <cstdlib>

//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolTwoParticleMatching
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TupleToolTwoParticleMatching )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolTwoParticleMatching::TupleToolTwoParticleMatching( const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent )
: TupleToolBase ( type, name, parent )
  //,m_tuple       ( NULL )
{
  std::vector<std::string> toolNames_default;
  
  toolNames_default.push_back  (  "TupleToolKinematic"  );
  toolNames_default.push_back  (  "TupleToolPid"        );
  //toolNames_default.push_back  (  "TupleToolGeometry"   );
  toolNames_default.push_back  (  "TupleToolMCTruth"   );
  toolNames_default.push_back  (  "TupleToolMCBackgroundInfo"   );

  declareProperty ( "MatchLocations" , m_matching_locations );                   
  declareProperty( "ToolList", m_toolNames = toolNames_default );
  declareProperty( "Prefix", m_headPrefix = "Matched_" );
  declareProperty( "Suffix", m_headSuffix = "");
  declareInterface<IParticleTupleTool>(this);
  
// PROPERTY INCLUDED TO PRESERVE BACKWARD COMPATIBILITY  
  declareProperty( "MatchWith",  m_matching_location = "OBSOLETE" );
  declareProperty( "TupleTools", m_toolNames);
}

//=============================================================================

StatusCode TupleToolTwoParticleMatching::initialize()
{
  const StatusCode sc = TupleToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  for(std::vector<std::string>::iterator iName = m_toolNames.begin(); iName != m_toolNames.end(); ++iName)
  {
    if ((*iName)=="TupleToolGeometry")
    {
      warning()<<"Geometry tuple tool can not be used in matching (yet). Please find a way how to include it by yourself."<<endmsg;
    }
    else
    {
      m_tuple.push_back(tool<IParticleTupleTool>( *iName, *iName+":PUBLIC", this ));
    }
  }

  if (m_matching_location != "OBSOLETE")
  {
    error() << "Option MatchWith is deprecated. Use MatchLocations instead." << endmsg;
    m_matching_locations ["J/psi(1S)"] = m_matching_location;
  }

  // Gets the ParticleProperty service
  LHCb::IParticlePropertySvc*  ppSvc = 
    this -> template svc<LHCb::IParticlePropertySvc> 
                            ( "LHCb::ParticlePropertySvc" , true) ;

  std::map<std::string, std::string>::const_iterator matchLocation;

  for (matchLocation = m_matching_locations.begin();
       matchLocation != m_matching_locations.end();
       matchLocation++)
  {
    const LHCb::ParticleProperty* property = ppSvc->find(matchLocation->first);
    if (property)
      m_parsed_locations [ abs(property->pdgID().pid()) ] = matchLocation->second;
  }

  return sc;
}

//=============================================================================

StatusCode TupleToolTwoParticleMatching::fill( const LHCb::Particle*
                                               , const LHCb::Particle* P
                                               , const std::string&  head
                                               , Tuples::Tuple& tuple )
{
  StatusCode sc = StatusCode::SUCCESS;

  const std::string prefix = m_headPrefix + fullName(head) + m_headSuffix;

  if (!P)
    return StatusCode ( true );
  
  const int pid = abs(P->particleID().pid());
  const int apid = abs(pid);

  if (m_parsed_locations.count(apid))
  {
    const LHCb::Particle* best_particle = P;
    const std::string matching_location ( m_parsed_locations [ apid ] );

    double best_overlap_original = 0;
    double best_overlap_loaded = 0;
    double m_counter = 0;
    if (exist<LHCb::Particles>(matching_location))
    {
      LHCb::Particle::Range candidates_for_matching = get<LHCb::Particle::Range>(matching_location);
      for(LHCb::Particle::Range::const_iterator iCand=candidates_for_matching.begin(); iCand!= candidates_for_matching.end(); ++iCand){
//        if ((P->isBasicParticle()==(*iCand)->isBasicParticle())&&(P->charge()==(*iCand)->charge())){
        if ((*iCand)->particleID().pid() == pid ){
          m_counter +=1;
          std::pair< double, double > c_overlap = LHCb::HashIDs::overlap((*iCand), P);
          double loaded_overlap = std::get<0>(c_overlap);
          double original_overlap = std::get<1>(c_overlap);
          if (std::min(loaded_overlap,original_overlap)>std::min(best_overlap_loaded,best_overlap_original))
          {
            best_particle = *iCand;
            best_overlap_loaded = loaded_overlap;
            best_overlap_original = original_overlap;
          }
        }
      }
    }
    /*
    const LHCb::Particle* clone_best_particle = best_particle->clone();
    const SmartRefVector < LHCb::Particle> matched_daughters = clone_best_particle->daughters();
    const SmartRefVector < LHCb::Particle> orig_daughters = P->daughters();
    for( SmartRefVector<LHCb::Particle>::const_iterator matched_d= matched_daughters.begin(); matched_d!= matched_daughters.end(); ++matched_d){
      //Here you can do something with daughters of matched particles
    }
    */
    bool test = true;
    test &= tuple->column( "NMatchingCand_"+fullName(head), m_counter);
    test &= tuple->column( "Overlap_original_"+fullName(head), best_overlap_original);
    test &= tuple->column( "Overlap_loaded_"+fullName(head), best_overlap_loaded);
    sc = sc && StatusCode(test);

    for(std::vector<IParticleTupleTool*>::const_iterator iTool = m_tuple.begin(); iTool != m_tuple.end(); ++iTool) 
    {
      sc = sc && (*iTool)->fill( NULL, best_particle, prefix, tuple);
    }
  }
  return sc;
}
