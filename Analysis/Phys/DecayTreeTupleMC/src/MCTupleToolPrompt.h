// $Id: $
#ifndef MCTUPLETOOLPROMPT_H
#define MCTUPLETOOLPROMPT_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/PhysicalConstants.h"

#include "Kernel/IParticleTupleTool.h"
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

#include "DecayTreeTupleBase/TupleToolBase.h"

// from GaudiAlg
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"

// from Event
#include "Event/Particle.h"
#include "Event/MCParticle.h"

// from Kernel
#include "Kernel/IParticle2MCAssociator.h"

/** @class MCTupleToolPrompt MCTupleToolPrompt.h
 *
 * \brief Tool to determine whether a decay originates from prompt charm based on the true lifetime of its ancestors.
 * Based on code written by Patrick Spradlin.
 *
 * Tuple columns:
 * - MC_ISPROMPT, MC_LONGLIVED_ID, MC_LONGLIVED_KEY
 *
 *  @author Philip John Hunt
 *  @date   2011-06-13
 */

//forward declarations
class IParticlePropertySvc;
class IParticle2MCAssociator;

class MCTupleToolPrompt : public TupleToolBase,
                          virtual public IParticleTupleTool
{

public:

  /// Standard constructor
  MCTupleToolPrompt( const std::string& type,
                     const std::string& name,
                     const IInterface* parent);

  virtual ~MCTupleToolPrompt( ); ///< Destructor

  virtual StatusCode initialize();

  virtual StatusCode fill( const LHCb::Particle*
                           , const LHCb::Particle*
                           , const std::string&
                           , Tuples::Tuple& );

protected:

  double m_maxLifetime;
  bool m_storeLongLivedPid;

private:

  LHCb::IParticlePropertySvc *m_ppSvc;
  IParticle2MCAssociator *m_p2mcAssoc;
  std::string m_p2mcAssocType;

};

#endif // MCTUPLETOOLPROMPT_H
