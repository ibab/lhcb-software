#ifndef TESLAMONITOR_H 
#define TESLAMONITOR_H 1

// Include files 
// from DaVinci.
#include "Kernel/DaVinciHistoAlgorithm.h"

// =================================
// LoKi
// =================================
#include "LoKi/PhysTypes.h"
#include "LoKi/IHybridFactory.h"


/** @class TeslaMonitor TeslaMonitor.h
x
 *  
 *
 *  @author Lucio Anderlini
 *  @date   2013-09-04
 */
class TeslaMonitor : public DaVinciHistoAlgorithm {
public: 
  /// Standard constructor
  TeslaMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TeslaMonitor( ); ///< Destructor

private:
  struct Variable {
    Variable() 
    : m_fun  ( LoKi::BasicFunctors<const LHCb::Particle*>::Constant ( -1.e+10 ) ) 
    {}

    double m_min, m_max;
    std::string m_name, m_title, m_code;
    size_t m_nBins;
    LoKi::PhysTypes::Fun  m_fun  ;   
  };

  typedef std::vector<Variable> Variables;

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:


  std::vector <double> m_min, m_max;
  std::vector <std::string>  m_code;
  std::vector <size_t> m_nBins;
  std::string m_name, m_title, m_cut;
  Variables m_vars;

  virtual StatusCode initVariables();
 
  std::string m_factory;
  std::string m_preambulo;
  std::string m_folder;
  std::string m_particle;
  LoKi::PhysTypes::Fun m_decayDescFunc  ;   
};
#endif // TESLAMONITOR_H

