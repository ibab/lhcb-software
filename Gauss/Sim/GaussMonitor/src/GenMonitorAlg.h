// $Id: GenMonitorAlg.h,v 1.3 2005-06-23 17:57:16 gcorti Exp $
#ifndef GAUSSMONITOR_GENMONITORALG_H 
#define GAUSSMONITOR_GENMONITORALG_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"

/** @class GenMonitorAlg GenMonitorAlg.h Algorithms/GenMonitorAlg.h
 *  
 *  Monitoring algorithms for the generator sequences
 *  
 *  @author Patrick Robbe (modified G.Corti)
 *  @date   2005-04-11
 */
class GenMonitorAlg : public GaudiHistoAlg {
public:
  /// Standard constructor
  GenMonitorAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~GenMonitorAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  std::string    m_dataPath;            ///< location of input data
  double         m_minEta;              ///< Min pseudo rapidity acceptance
  double         m_maxEta;              ///< Max pseudo rapidity acceptance

  int            m_counter       , m_counterstable;
  int            m_counterCharged, m_counterChInEta;
  int            m_nEvents;
  
  std::string    m_generatorName;

  double lifetime ( HepMC::GenParticle * ) ;
};
#endif // GAUSSMONITOR_GENMONITORALG_H
