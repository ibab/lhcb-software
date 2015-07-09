#ifndef TESLABRUNELMONITOR_H 
#define TESLABRUNELMONITOR_H 1

// Include files 
// from DaVinci.
#include "Kernel/DaVinciHistoAlgorithm.h"

// =================================
// LoKi
// =================================
#include "LoKi/PhysTypes.h"
#include "LoKi/IHybridFactory.h"
//
// TeslaTools
#include "TeslaTools/ITeslaMatcher.h"


/** @class TeslaBrunelMonitor TeslaBrunelMonitor.h
x
 *  
 *
 *  @author Lucio Anderlini
 *  @date   2013-09-04
 */
class TeslaBrunelMonitor : public DaVinciHistoAlgorithm {
public: 
  /// Standard constructor
  TeslaBrunelMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TeslaBrunelMonitor( ); ///< Destructor

private:
  struct Variable {
    Variable() 
    : m_fun  ( LoKi::BasicFunctors<const LHCb::Particle*>::Constant ( -1.e+10 ) ) 
    , m_cut  ( LoKi::BasicFunctors<const LHCb::Particle*>::Constant (    0    ) ) 
    {}

    double m_max;
    std::string m_name, m_code;
    LoKi::PhysTypes::Fun  m_fun, m_cut  ;   
  };

  typedef std::vector<Variable> Variables;

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  std::vector <double> m_max;
  std::vector <std::string>  m_code;
  std::vector<std::string> m_particle, m_cut, m_name;
  Variables m_vars;

  virtual StatusCode initVariables();
  virtual StatusCode initTESlocations();
 
  std::string m_factory;
  std::string m_preambulo;
  std::string m_folder;

  std::string m_histTitle, m_histName, m_teslaMatcherName;

  size_t m_nVariables;

  ITeslaMatcher* m_matcher;

  std::map <std::string, std::string> m_matching_locations;
  std::map <int, std::string>         m_parsed_locations;


};
#endif // TESLABRUNELMONITOR_H

