#ifndef TRT_ALIGNTOOLS__MODULEACCUMULATOR_H
#define TRT_ALIGNTOOLS__MODULEACCUMULATOR_H

#include "StrawAccumulator.h"
#include <vector>

class TH1 ;
class TH2F ;
class OTCalibrationAlg ;
class DeOTModule ;

namespace OTCal {
  
  class ModuleAccumulator
  {
  public:
    // typedefs
    typedef std::vector<StrawAccumulator> StrawContainer ;
    
    // constructors
    ModuleAccumulator() : m_module(0), m_parent(0) {}
    
    ModuleAccumulator( const DeOTModule& module, OTCalibrationAlg& parent) ;
    
    // methods (keep them inline for now)
    void add(const StrawHit& hit) ;
    void reset()  ;
    void book() ;
    std::string name() const ;
    
    const TH2F* timevsdistanceh2() const { return m_timevsdistanceh2 ; }
    const TH2F* ttodcalibh2() const { return m_ttodcalibh2 ; }
    const TH2F* timeresvsdistanceh2() const { return m_timeresvsdistanceh2 ; }
    const TH2F* resvsdistanceh2() const { return m_resvsdistanceh2 ; }
    const TH2F* resvstimeh2() const { return m_resvstimeh2 ; }
    
    double avtimeresidual() const { return m_average.avtimeresidual() ; }
    double avrawtimeresidual() const { return m_average.avrawtimeresidual() ; }
    double avrawtimeresidualerror() const { return m_average.avrawtimeresidualerror() ; }
    unsigned int nentries() const { return m_average.nentries() ; }
    const StrawContainer& straws() const { return m_straws ; }

    const DeOTModule& module() const { return *m_module ; }

  private:
    const DeOTModule* m_module ;
    OTCalibrationAlg* m_parent ;
    StrawContainer m_straws ;
    unsigned int m_nevents ;
    bool m_booked ;
    TH1* m_timeh1 ;
    TH1* m_resh1 ;
    TH1* m_timeresh1 ; 
    TH1* m_trkdisth1 ;
    TH1* m_trkerrh1 ;
    TH1* m_respullh1 ;
    TH2F* m_timevsdistanceh2 ;
    TH2F* m_ttodcalibh2 ;
    TH2F* m_timeresvsdistanceh2 ;
    TH2F* m_resvsdistanceh2 ;
    TH2F* m_resvstimeh2 ;
    StrawAccumulator m_average ; // store one 'average straw'
  } ;
  
}

#endif
