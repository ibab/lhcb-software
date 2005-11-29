// $Id: Generation.h,v 1.3 2005-11-29 15:55:02 robbep Exp $
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

  unsigned int m_nEvents ; ///< Number of generated events
  unsigned int m_nAcceptedEvents ; ///< Number of accepted events
  unsigned int m_nInteractions ; ///< Number of generated interactions
  unsigned int m_nAcceptedInteractions ; 
  ///< Number of interactions in accepted events
  unsigned int m_n1b ;
  unsigned int m_n3b ;
  unsigned int m_nPromptB ;
  unsigned int m_n1c ;
  unsigned int m_n3c ;
  unsigned int m_nPromptC ;
  unsigned int m_nbc ;

  unsigned int m_n1bAccepted ;
  unsigned int m_n3bAccepted ;
  unsigned int m_nPromptBAccepted ;
  unsigned int m_n1cAccepted ;
  unsigned int m_n3cAccepted ;
  unsigned int m_nPromptCAccepted ;
  unsigned int m_nbcAccepted ;
  
  unsigned int m_nBeforeFullEvent ;
  unsigned int m_nAfterFullEvent ;


  void updateInteractionCounters( unsigned int & n1b , unsigned int & n3b ,
                                  unsigned int & nPromptB , 
                                  unsigned int & n1c , unsigned int & n3c , 
                                  unsigned int & nPromptC , 
                                  unsigned int & nbc , 
                                  const HepMCEvent * theEvent ) ;

  inline double fraction( const unsigned int A , const unsigned int B ) const
  { return ( (double) A / (double) B ) ; }
  inline double err_fraction( const unsigned int A , const unsigned int B ) 
    const 
  { return sqrt( A * ( 1. - ( (double) A / (double) B ) ) ) / 
      ( (double) B ) ; }
};
#endif // GENERATORS_GENERATION_H
