// $Id: TsaCollector.h,v 1.2 2006-10-10 14:21:00 mneedham Exp $
#ifndef _TsaCollector_H
#define _TsaCollector_H

/** @class TsaTsaCollector TsaCollector Tsa/TsaCollector.h
 *
 *  Interface to isolation tool
 *
 *  @author M.Needham
 *  @date   30/04/2004
 */
#include "GaudiKernel/IIncidentListener.h"  
#include "GaudiAlg/GaudiTool.h"

#include "TsaKernel/ITsaCollector.h"
#include "TsaKernel/OTCluster.h"
#include "TsaKernel/STCluster.h"

namespace Tsa{

  class Cluster;
  class OTCluster;
  class STCluster;
  class Line;
  class Parabola;

}

#include "Event/State.h" 


class TsaCollector: public GaudiTool , 
                    virtual public ITsaCollector, 
                    virtual public IIncidentListener {

public:

  /// constructer
  TsaCollector(const std::string& type,
                      const std::string& name,
                      const IInterface* parent);
                                                                                
  virtual ~TsaCollector();

  StatusCode initialize();
			   
  /// the method
  virtual StatusCode collect(const LHCb::State& aState, 
                             Tsa::Clusters* clusters) const;

  /// the method
  virtual StatusCode collect(const LHCb::State& aState, 
                             Tsa::STClusters* clusters) const;

  /// the method
  virtual StatusCode collect(const LHCb::State& aState, 
                             Tsa::OTClusters* clusters) const;
 
private:

  typedef std::pair<double, double> WindowSize;

  template<typename TYPE>
  void searchOT(const Tsa::Parabola& parab,
                const Tsa::Line& line,
                const WindowSize& win,
		TYPE& clusters) const ;

  template<typename TYPE>
  void searchIT(const Tsa::Parabola& parab,
                const Tsa::Line& line,
                const WindowSize& win,
		TYPE& clusters) const ;

  void getClusters() const;

  void handle ( const Incident& incident );
 
  Tsa::Line yParam(const LHCb::State& aState) const;
  Tsa::Parabola xParam(const LHCb::State& aState) const;

  bool TsaCollector::inWindow(const Tsa::Parabola& parab,
                              const Tsa::Line& line, 
                              const Tsa::Cluster* cluster,
                              const TsaCollector::WindowSize& win) const;                                                                 

  mutable bool m_initEvent;

  mutable Tsa::STClusters* m_itClusters;
  mutable Tsa::OTClusters* m_otClusters;

  double m_curvFactor;
  bool m_searchIT;
  bool m_searchOT;
  double m_xWindowSigma;
  double m_yWindowSigma;
  std::string m_otClusterLocation;
  std::string m_itClusterLocation;
  bool m_correctDriftDist;

};

#include "Event/State.h"
#include "TsaKernel/Parabola.h"
#include "TsaKernel/Line.h"

template<typename TYPE>
inline void TsaCollector::searchOT(const Tsa::Parabola& parab,
                            const Tsa::Line& line,
                            const WindowSize& win , 
                            TYPE& clusters) const {

 // flat loop for now...nothing fancy
 for ( Tsa::OTClusters::iterator iter = m_otClusters->begin();  iter != m_otClusters->end() ; ++iter){
   if (inWindow(parab,line,*iter,win) == true) {
     Tsa::OTCluster* newClus = (*iter)->clone();
     if (m_correctDriftDist == true) newClus->setDriftRadius(parab.value((*iter)->zMid()), line.value((*iter)->zMid()));
     clusters->add(newClus);
   }
 } // iter
}

template<typename TYPE>
inline void TsaCollector::searchIT(const Tsa::Parabola& parab,
                                   const Tsa::Line& line,
                                   const WindowSize& win , 
                                   TYPE& clusters) const{

  // flat loop for now...nothing fancy
  for ( Tsa::STClusters::iterator iter = m_itClusters->begin();  iter != m_itClusters->end() ; ++iter){
    if (inWindow(parab,line,*iter, win) == true) {
     Tsa::STCluster* newClus = (*iter)->clone();
     clusters->add(newClus);
    }
  } // iter
}

inline Tsa::Line TsaCollector::yParam(const LHCb::State& aState) const{

  const double m = aState.ty();
  const double c = aState.y() - m*aState.z();
  return Tsa::Line(m,c);
}

inline Tsa::Parabola TsaCollector::xParam(const LHCb::State& aState) const{
 
  const double z = aState.z();
  const double a = aState.qOverP()/m_curvFactor;
  const double b = aState.tx() -2*z*a;  
  const double c = aState.x() - z*(b + a*z); 
  return Tsa::Parabola(a,b,c);
}

				       	       
#endif
