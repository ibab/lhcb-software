// $Id: ProductionAsymmetry.h,v 1.1 2007-05-16 17:33:29 gcorti Exp $
#ifndef PRODUCTIONASYMMETRY_H 
#define PRODUCTIONASYMMETRY_H 1

// Include files
// from STL
#include <string>
#include "boost/lexical_cast.hpp"

// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"

// from AIDA

/** @class ProductionAsymmetry ProductionAsymmetry.h 
 *  
 *  Based on Generator analysis algorithm
 *  Examines the production asymmetry of a list of input PIDs
 *  plots a few useful histograms known to strongly affect this
 *  asymmetry
 *  
 *  
 *  @author R. Lambert  
 *  @date   2007-04-24
 */
class ProductionAsymmetry : public GaudiHistoAlg {
public:
  /// Standard constructor
  ProductionAsymmetry( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ProductionAsymmetry( ); ///< Destructor

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
  
  std::string    m_generatorName;
  std::string    m_signalName;   ///< name of plots/graphs

  std::vector<int>  m_hepPID;       ///< an option for the PID, which particles to look for.
  std::vector<LHCb::ParticleID> m_sPID;       ///<PIDs of signal particles filled from the option m_hepPID  

  std::vector<int> m_counters;  ///counters of number of particles of signal types
  std::vector<int> m_asymcounters;  //counters of differences between particles and antiparticles
  std::vector<int> m_partcounters; //counters of particles and antiparticles
  
   
   

  // Location where to find HepMC event
  std::string m_inputData ;

  //plots for a given particle
  StatusCode someplots(const HepMC::GenParticle* plotme);
  
  //plots arrive with the name of the particle they were for


};
#endif // PRODUCTIONASYMMETRY_H
