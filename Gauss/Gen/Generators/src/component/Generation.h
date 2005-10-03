// $Id: Generation.h,v 1.1 2005-10-03 10:28:25 robbep Exp $
#ifndef GENERATORS_GENERATION_H 
#define GENERATORS_GENERATION_H 1

#include "GaudiAlg/GaudiAlgorithm.h"

class HepMCEvent ;
class HardInfo ;
class ISampleGenerationTool ;
class IPileUpTool ;
class IDecayTool ;
class IVertexSmearingTool ;

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

  std::string m_pileUpToolName , m_sampleGenerationToolName , m_decayToolName ,
    m_vertexSmearingToolName ;

  int m_nEvents ; ///< Number of generated events
  int m_nAcceptedEvents ; ///< Number of accepted events
  int m_nInteractions ; ///< Number of generated interactions
  int m_nAcceptedInteractions ; ///< Number of interactions in accepted events

};
#endif // GENERATORS_GENERATION_H
