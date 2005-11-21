// $Id: Generation.h,v 1.2 2005-11-21 16:17:19 robbep Exp $
#ifndef GENERATORS_GENERATION_H 
#define GENERATORS_GENERATION_H 1

#include "GaudiAlg/GaudiAlgorithm.h"

class HepMCEvent ;
class HardInfo ;
class ISampleGenerationTool ;
class IPileUpTool ;
class IDecayTool ;
class IVertexSmearingTool ;
class IFullGenEventCutTool ;

namespace HepMC {
  class GenParticle ;
}

/** @class Generation Generation.h 
 *  
 *  Generic algorithm to generate Monte Carlo events
 *
 *  @author Patrick Robbe
 *  @date   2005-08-11
 */
class Generation : public GaudiAlgorithm {
public:
  typedef std::vector< int >                  PIDs        ;
  typedef std::vector< HepMCEvent * >         EventVector ;
  typedef std::vector< HardInfo * >           HardVector  ;
  typedef std::vector< HepMC::GenParticle * > GenParticles ;
    
  /// Standard constructor
  Generation( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~Generation( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  StatusCode decayEvent( HepMCEvent * theEvent ) ;

private:
  int          m_eventType ;
  std::string  m_hepMCEventLocation ;
  std::string  m_genHeaderLocation ;
  std::string  m_hardInfoLocation ;
  PIDs         m_pidList ;
  
  ISampleGenerationTool    * m_sampleGenerationTool   ;
  IPileUpTool              * m_pileUpTool             ;
  IDecayTool               * m_decayTool              ;
  IVertexSmearingTool      * m_vertexSmearingTool     ;
  IFullGenEventCutTool     * m_fullGenEventCutTool    ;

  std::string m_pileUpToolName           ;
  std::string m_sampleGenerationToolName ;
  std::string m_decayToolName            ;
  std::string m_vertexSmearingToolName   ;
  std::string m_fullGenEventCutToolName  ;

  int m_nEvents ; ///< Number of generated events
  int m_nAcceptedEvents ; ///< Number of accepted events
  int m_nInteractions ; ///< Number of generated interactions
  int m_nAcceptedInteractions ; ///< Number of interactions in accepted events
  int m_nBeforeFullEvent ;
  int m_nAfterFullEvent ;
};
#endif // GENERATORS_GENERATION_H
