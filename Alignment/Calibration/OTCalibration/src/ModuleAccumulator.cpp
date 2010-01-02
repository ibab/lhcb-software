
#include "ModuleAccumulator.h"
#include "OTCalibrationAlg.h"
#include "OTDet/DeOTModule.h"

#include <TH1.h>
#include <TH2.h>

namespace OTCal {

  ModuleAccumulator::ModuleAccumulator( const DeOTModule& module, OTCalibrationAlg& parent) 
    : 
    m_module(&module),
    m_parent(&parent),
    m_straws(module.nChannels()),
    m_nevents(0),
    m_timeh1(0),m_resh1(0),
    m_timeresh1(0),m_timevsdistanceh2(0) 
  { 
  }

  void ModuleAccumulator::reset() 
  {
    m_nevents = 0 ;
    for( StrawContainer::iterator it=m_straws.begin(); it!=m_straws.end(); ++it) it->reset() ;
    if(m_timeh1)           m_timeh1->Reset() ;
    if(m_resh1)            m_resh1->Reset() ;
    if(m_timeresh1)        m_timeresh1->Reset() ;
    if(m_trkerrh1)         m_trkerrh1->Reset() ;
    if(m_respullh1)        m_respullh1->Reset() ;
    if(m_timevsdistanceh2) m_timevsdistanceh2->Reset() ;
    if(m_ttodcalibh2)      m_ttodcalibh2->Reset() ;
  }

  std::string ModuleAccumulator::name() const
  {
    char path[256] ;
    sprintf(path,"Station%d/Layer%d/Quadrant%d/Module%d",
	    m_module->stationID(), m_module->layerID(), m_module->quarterID(), m_module->moduleID()) ;
    return std::string(path) ;
  }

  void ModuleAccumulator::book() 
  {
    std::string path = name() ;
    //const double tbinsize = 25./16 ;
    double dmax = m_parent->maxDistance() ;
    double tmin = m_parent->minTime() ;
    double tmax = tmin + 75. ;
    m_parent->registerhist(path,m_timeh1    = new TH1F("timeh1","",96,tmin,tmax)) ;
    m_parent->registerhist(path,m_resh1     = new TH1F("resh1","",100,-1,1)) ;
    m_parent->registerhist(path,m_timeresh1 = new TH1F("timeresh1","",100,-25,25)) ;
    m_parent->registerhist(path,m_trkerrh1  = new TH1F("trkerrh1","",100,0,1)) ;
    m_parent->registerhist(path,m_trkdisth1 = new TH1F("trkdisth1","",100,-10,10)) ;
    m_parent->registerhist(path,m_respullh1  = new TH1F("respullh1","",100,-10,10)) ;
    
    m_parent->registerhist(path,m_timevsdistanceh2 = new TH2F("timevsdistanceh2","",80,-dmax,dmax,48,tmin,tmax)) ;
    m_parent->registerhist(path,m_ttodcalibh2  = new TH2F("ttodcalibh2","",25,0,dmax,48,tmin,tmax)) ;
    m_parent->registerhist(path,m_timeresvsdistanceh2 = new TH2F("timeresvsdistanceh2","",80,-dmax,dmax,50,-25,25)) ;
    m_parent->registerhist(path,m_resvsdistanceh2 = new TH2F("resvsdistanceh2","",80,-dmax,dmax,50,-1.0,1.0)) ;
    m_parent->registerhist(path,m_resvstimeh2 = new TH2F("resvstimeh2","",48,tmin,tmax,50,0,dmax)) ;
  }
  
  void ModuleAccumulator::add(const StrawHit& hit) {
    if( m_timeh1==0 ) book() ;
    ++m_nevents ;
    // update the relevant straw accumulator
    //std::cout << "ModuleAcc: " << m_straws.size() << " " << hit.straw()-1 << std::endl ;
    m_straws[hit.straw()-1].add(hit) ;
    m_average.add(hit) ;
    m_timeh1->Fill(hit.driftTime()) ;
    m_resh1->Fill(hit.residual()) ;
    m_timeresh1->Fill(hit.timeResidual() ) ;
    m_trkerrh1->Fill(std::sqrt(hit.trkVariance())) ;
    m_respullh1->Fill(hit.residualPull()) ;
    m_timevsdistanceh2->Fill(hit.trkDistance(),hit.driftTime()) ;
    m_ttodcalibh2->Fill(std::abs(hit.trkDistance()),hit.driftTime()) ;
    m_trkdisth1->Fill(hit.trkDistance()) ;
    m_timeresvsdistanceh2->Fill(hit.trkDistance(),hit.residualScaleFactor() * hit.timeResidual()) ;
    m_resvsdistanceh2->Fill(hit.trkDistance(),hit.residualScaleFactor() * hit.residual()) ;
    m_resvstimeh2->Fill(hit.driftTime(),std::abs(hit.residualScaleFactor() * hit.residual())) ;
  }
  
}
