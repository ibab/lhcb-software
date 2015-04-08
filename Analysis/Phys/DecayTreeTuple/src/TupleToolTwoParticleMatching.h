// $Id: TupleToolTwoParticleMatching.h,v 1.0 2015-02-27 08:59:11 ikomarov $
#ifndef _TupleToolTwoParticleMatching_H
#define _TupleToolTwoParticleMatching_H 1

// Gaudi
#include "GaudiKernel/ToolFactory.h"

// Base class
#include "DecayTreeTupleBase/TupleToolBase.h"
#include "TupleToolKinematic.h"

// Interfaces
#include "Kernel/IParticleTupleTool.h"
#include "RecInterfaces/IChargedProtoANNPIDTupleTool.h"

/** @class TupleToolTwoParticleMatching TupleToolTwoParticleMatching.h
 *
 * \brief Find the best matching particle from given location and wfill information about it to nTuple
 *
 * This tool should be applied as usual tuple tool for certain particle.
 *
 * As an input it takes locstion in dst, which contain paticles to compare with.
 * For example, one might run this tool for Tesla candidates and give location of 
 * stripping candidates to compare with.
 *
 * Having original particle (TupleTool was called for this particle in gaudi script) 
 * and a list of particles in the same event from given location, tool finds best matching candidate.
 * Best matching candidate (a.k.a. matched particle) has maximal overlap in LHCbIDs with original particle.
 *
 * Another input is the list of tuple tools which should be aplied to matched particle.
 * Pay attention that it takes just name of the tools, so at this moment it's impossible
 * to use pre-configured tool (unfortunately, LoKi TupleTool will not work)
 * Another restriction - since location of PVs should be the same for both particles,
 * tools like TupleToolGeometry will not work correctly and they are obsoleted in current version.
 * 
 * Leaves filled by the tool:
 *    - NMatchingCand_(head): Number of candidates in the same event in given location
 *    - Overlap_original_(head): output of HashIDs::overlap(original_particle, loaded_particle) for original particle
 *    - Overlap_loaded_(head): output of HashIDs::overlap(original_particle, loaded_particle) for matched particle
 *    - Matched_(head)_(Property_of_Matching_Particle): output of tuple tool aplied for best matching particle
 *
 * Example of usage in gaudi script:
 * @code
 * matching_tools = [ "TupleToolKinematic", "TupleToolMCTruth", "TupleToolMCBackgroundInfo"]
 * tm = tesla_tuple.addTupleTool(TupleToolTwoParticleMatching, name = "TupleToolTwoParticleMatching")
 * tm.MatchWith = "Phys/SelMyJPsi/Particles" #Locaion of particles to match with
 * tm.TupleTools = matching_tools
 * @endcode
 *
 * \sa DecayTreeTuple
 *
 *  @author Ilya Komarov
 *  @date   2015-02-27
 */
class TupleToolTwoParticleMatching : public TupleToolBase,
                                virtual public IParticleTupleTool
{

public:

  // Standard constructor
  TupleToolTwoParticleMatching( const std::string& type,
                           const std::string& name,
                           const IInterface* parent);

  virtual ~TupleToolTwoParticleMatching() {}

  StatusCode initialize(); 

public:

  virtual StatusCode fill(const LHCb::Particle*,
                          const LHCb::Particle*,
                          const std::string& ,
                          Tuples::Tuple& );

private:
  std::string m_matching_location;
  std::vector<std::string> m_toolNames;
  std::vector<IParticleTupleTool*> m_tuple;
  
};

#endif 