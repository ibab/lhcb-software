// $Id: MCParticleArrayFilterAlg.h,v 1.1 2008-04-14 15:23:46 jpalac Exp $
#ifndef MCPARTICLEARRAYFILTERALG_H 
#define MCPARTICLEARRAYFILTERALG_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

class IMCParticleArrayFilter;
class IPrintMCDecayTreeTool;
class IParticlePropertySvc;

/** @class MCParticleArrayFilterAlg MCParticleArrayFilterAlg.h
 *  
 *  Simple algorithm that takes a container of LHCb::MCParticle from the
 *  Transient Event Store (TES), applies an IMCParticleArrayFilter to produce 
 *  an LHCb::MCParticle container of filtered MCParticles, and places this in 
 *  the TES.
 *  <b>Options</b>:
 *  InputLocation: the TES location of the input LHCb::MCParticles 
 *  (default LHCb::MCParticleLocation::Default)
 *  OutputLocation: the TES location where the LHCb::MCParticle container of 
 *  filtered particles is placed.
 *  IMCParticleArrayFilter: Name of the implementation of the 
 *  IMCParticleArrayFilter interface to be used.
 *  @see IMCParticleArrayFilter
 *
 *  <b>Example:</b>: Take a container of MCParticles from 
 *  "/Event/MC/MyParticles", filter them according to a decay descriptor, 
 *  and place the container of filtered MCParticles in 
 *  "/Event/MC/MyDecayHeads".
 *
 *  @code
 *
 *  @endcode
 *
 *  @author Juan PALACIOS
 *  @date   2008-04-09
 */
class MCParticleArrayFilterAlg : public GaudiAlgorithm {
public: 
  /// Standard constructor
  MCParticleArrayFilterAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MCParticleArrayFilterAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  IMCParticleArrayFilter* m_filter;
  std::string m_filterType;
  std::string m_inputLocation;
  std::string m_outputLocation;
  IParticlePropertySvc* m_ppSvc;

};
#endif // MCPARTICLEARRAYFILTERALG_H
