// $Id: Generation.h,v 1.6 2005-12-31 17:32:39 robbep Exp $
#ifndef GENERATORS_GENERATION_H 
#define GENERATORS_GENERATION_H 1

#include "GaudiAlg/GaudiAlgorithm.h"

// Forward declarations
class ISampleGenerationTool ;
class IPileUpTool ;
class IDecayTool ;
class IVertexSmearingTool ;
class IFullGenEventCutTool ;

namespace HepMC {
  class GenParticle ;
}

namespace LHCb {
  class HepMCEvent ;
}

/** @class Generation Generation.h "Generation.h"
 *  
 *  Main algorithm to generate Monte Carlo events.
 *
 *  @author Patrick Robbe
 *  @date   2005-08-11
 */
class Generation : public GaudiAlgorithm {
 public:
  /// Standard constructor
  Generation( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~Generation( ); ///< Destructor

  /** Algorithm initialization.
   *  -# Initializes the common Gaudi random number generator used in all 
   *     generators,
   *  -# Retrieve Pile-up tool, sample generation tool, decay tool,
   *     vertex smearing tool and full event cut tool used in the generation 
   *     of events.
   */
  virtual StatusCode initialize();

  /** Algorithm execution.
   *  Repeat the following sequence until a good set of interactions is 
   *  generated.
   *  -# Compute the number of interactions to generate in the event, using
   *     the IPileUpTool.
   *  -# Generate this number of interactions using the IProductionTool. 
   *     The IProductionTool indicates if a good event has been generated
   *     (containing for example a B hadron).
   *  -# Decay with the IDecayTool all particles in the event, if this is
   *     a good event.
   *  -# Apply the full event generator level cut (using IFullEventGenCutTool)
   *     and accept or reject the event.
   *  -# Store in event store the accepted event.
   */
  virtual StatusCode execute   ();

  /** Algorithm finalization.
   *  Print generation counters.
   */
  virtual StatusCode finalize  ();

protected:
  /// Decay the event with the IDecayTool.
  StatusCode decayEvent( LHCb::HepMCEvent * theEvent ) ;

private:
  int          m_eventType ; ///< Event type (set by options)

  /// Location where to store generator events (set by options)
  std::string  m_hepMCEventLocation ; 

  /// Location where to store the Header of the events (set by options)
  std::string  m_genHeaderLocation ;

  /// Location where to store HardInfo (set by options)
  std::string  m_genCollisionLocation ;

  ISampleGenerationTool    * m_sampleGenerationTool   ; ///< Sample tool

  IPileUpTool              * m_pileUpTool             ; ///< Pile-up tool

  IDecayTool               * m_decayTool              ; ///< Decay tool

  IVertexSmearingTool      * m_vertexSmearingTool     ; ///< Smearing tool

  IFullGenEventCutTool     * m_fullGenEventCutTool    ; ///< Cut tool

  /// Name of the IPileUpTool (set by options).
  std::string m_pileUpToolName           ;

  /// Name of the ISampleGenerationTool - MinimumBias, ... (set by options)
  std::string m_sampleGenerationToolName ;

  /// Name of the IDecayTool (set by options)
  std::string m_decayToolName            ;

  /// Name of the IVertexSmearingTool (set by options)
  std::string m_vertexSmearingToolName   ;

  /// Name of the IFullGenEventCutTool (set by options)
  std::string m_fullGenEventCutToolName  ;

  unsigned int m_nEvents ; ///< Number of generated events

  unsigned int m_nAcceptedEvents ; ///< Number of accepted events

  unsigned int m_nInteractions ; ///< Number of generated interactions

  /// Number of interactions in accepted events
  unsigned int m_nAcceptedInteractions ;

  /// Counter of generated interactions with more than 1 b quark
  unsigned int m_n1b ;

  /// Counter of generated interactions with more than 3 b quark
  unsigned int m_n3b ;

  /// Counter of generated interactions with at least one prompt B hadron
  unsigned int m_nPromptB ;

  /// Counter of generated interactions with more than 1 c quark
  unsigned int m_n1c ;

  /// Counter of generated interactions with more than 3 c quarks
  unsigned int m_n3c ;

  /// Counter of generated interactions with at least one prompt charm
  unsigned int m_nPromptC ;

  /// Counter of generated interactions with primary b and primary c quarks
  unsigned int m_nbc ;

  /// Counter of accepted interactions with more than 1 b quark
  unsigned int m_n1bAccepted ;

  /// Counter of accepted interactions with more than 3 b quark
  unsigned int m_n3bAccepted ;

  /// Counter of accepted interactions with at least one prompt B hadron
  unsigned int m_nPromptBAccepted ;

  /// Counter of accepted interactions with more than 1 c quark
  unsigned int m_n1cAccepted ;

  /// Counter of accepted interactions with more than 3 c quarks
  unsigned int m_n3cAccepted ;

  /// Counter of accepted interactions with at least one prompt charm
  unsigned int m_nPromptCAccepted ;

  /// Counter of accepted interactions with primary b and primary c quarks
  unsigned int m_nbcAccepted ;

  /// Counter of events before the full event generator level cut  
  unsigned int m_nBeforeFullEvent ;

  /// Counter of events after the full event generator level cut
  unsigned int m_nAfterFullEvent ;


  /** Update the counters counting on interactions.
   *  @param[in,out] n1b       Counter of events with at least 1 b quark
   *  @param[in,out] n3b       Counter of events with at least 3 b quarks
   *  @param[in,out] nPromptB  Counter of events with at least one prompt B
   *  @param[in,out] n1c       Counter of events with at least 1 c quark
   *  @param[in,out] n3c       Counter of events with at least 3 c quarks
   *  @param[in,out] nPromptC  Counter of events with at least one prompt D
   *  @param[in]     theEvent  The event to study
   */
  void updateInteractionCounters( unsigned int & n1b , unsigned int & n3b ,
                                  unsigned int & nPromptB , 
                                  unsigned int & n1c , unsigned int & n3c , 
                                  unsigned int & nPromptC , 
                                  unsigned int & nbc , 
                                  const LHCb::HepMCEvent * theEvent ) ;
};
#endif // GENERATORS_GENERATION_H
