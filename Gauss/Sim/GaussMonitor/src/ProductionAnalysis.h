// $Id: ProductionAnalysis.h,v 1.1 2007-05-16 17:33:29 gcorti Exp $
#ifndef PRODUCTIONANALYSIS_H 
#define PRODUCTIONANALYSIS_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"

/** @class ProductionAnalysis ProductionAnalysis.h 
 *  
 *  Based on the Generator Analysis Algorithm,
 *  Allows generator-level studies on particles with specific PID
 *  Analysis algorithms for the generator sequences with PID reference
 *  histograms for generators comparisons
 *  
 *  @author R. Lambert  
 *  @date   2007-04-24
 */
class ProductionAnalysis : public GaudiHistoAlg {
public:
  /// Standard constructor
  ProductionAnalysis( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ProductionAnalysis( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  
  
private:
  std::string    m_dataPath;            ///< location of input data
  double         m_minEta;              ///< Min pseudo rapidity acceptance
  double         m_maxEta;              ///< Max pseudo rapidity acceptance

  int            m_counter;             ///<counter of particles in acceptance
  int            m_nEvents;             ///<counter of events
  
  std::string    m_generatorName; ///< specifically apply to this generator
  std::string    m_signalName;   ///< which Particle to look for?

  int            m_hepPID;       ///< an option for the PID, which particle to look for.
  LHCb::ParticleID m_sPID;       ///< PID of signal particle  


  std::string m_inputData ;  ///< Location where to find HepMC event


};
#endif // PRODUCTIONANALYSIS_H
