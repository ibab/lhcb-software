// $Id: TupleToolTwoParticleMatching.cpp,v 1.0 2015-02-27 08:59:11 ikomarov $

#include "TupleToolTwoParticleMatching.h"
#include "Kernel/HashIDs.h"

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
  toolNames_default.push_back  (  "TupleToolPID"        );
  //toolNames_default.push_back  (  "TupleToolGeometry"   );
  toolNames_default.push_back  (  "TupleToolMCTruth"   );
  toolNames_default.push_back  (  "TupleToolMCBackgroundInfo"   );

  declareProperty( "MatchWith"            , m_matching_location = "/Event/Hlt2DiMuonJPsi/Particles" );
  declareProperty( "TupleTools", m_toolNames = toolNames_default );
  declareInterface<IParticleTupleTool>(this);
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
  return sc;
}

//=============================================================================

StatusCode TupleToolTwoParticleMatching::fill( const LHCb::Particle* mother
                                          , const LHCb::Particle* P
                                          , const std::string&  head
                                          , Tuples::Tuple& tuple )
{
  StatusCode sc = StatusCode::SUCCESS;

  const std::string prefix = "Matched_"+fullName(head);

  if (!P->isBasicParticle())
  {
    const LHCb::Particle* best_particle = P;

    double best_overlap_original = 0;
    double best_overlap_loaded = 0;
    double m_counter = 0;
    if (exist<LHCb::Particles>(m_matching_location))
    {
      LHCb::Particle::Range candidates_for_matching = get<LHCb::Particle::Range>(m_matching_location);
      for(LHCb::Particle::Range::const_iterator iCand=candidates_for_matching.begin(); iCand!= candidates_for_matching.end(); ++iCand){
        if ((P->isBasicParticle()==(*iCand)->isBasicParticle())&&(P->charge()==(*iCand)->charge())){
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