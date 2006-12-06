// $Id: TsaCollector.h,v 1.7 2006-12-06 14:35:01 mneedham Exp $
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
#include "TsaKernel/Cluster.h"
#include "Event/State.h" 
#include "Kernel/LHCbID.h"

namespace Tsa{
  class Line;
  class Parabola;
}



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
                             Tsa::Clusters* clusters, const unsigned int nsigma) const;

  /// the method
  virtual StatusCode collect(const LHCb::State& aState, 
                             Tsa::STClusters* clusters, const unsigned int nsigma) const;

  /// the method
  virtual StatusCode collect(const LHCb::State& aState, 
                             Tsa::OTClusters* clusters, const unsigned int nsigma) const;
 
private:

  typedef std::pair<double, double> WindowSize;

  template<typename TYPE>
  void searchOT(const Tsa::Parabola& parab,
                const Tsa::Line& line,
                const WindowSize& win,
		ObjectVector<TYPE>*& clusters) const ;

  template<typename TYPE>
  void searchIT(const Tsa::Parabola& parab,
                const Tsa::Line& line,
                const WindowSize& win,
		ObjectVector<TYPE>*& clusters) const ;

  template <typename TYPE>
  std::vector<LHCb::LHCbID> channelList(ObjectVector<TYPE>*& clusters) const;

  void getClusters() const;

  void handle ( const Incident& incident );

  bool inContainer(LHCb::LHCbID testChan ,std::vector<LHCb::LHCbID>& ids ) const;
 
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

  std::string m_otClusterLocation;
  std::string m_itClusterLocation;
  bool m_correctDriftDist;

  double m_maxDriftRadius;
  double m_minDriftRadius;

};

#include "Event/State.h"
#include "TsaKernel/Parabola.h"
#include "TsaKernel/Line.h"

template<typename TYPE>
inline void TsaCollector::searchOT(const Tsa::Parabola& parab,
                            const Tsa::Line& line,
                            const WindowSize& win , 
                            ObjectVector<TYPE>*& clusters) const {


 // first make a list of already hit channels
 std::vector<LHCb::LHCbID> ids = channelList(clusters);

 for ( Tsa::OTClusters::iterator iter = m_otClusters->begin();  iter != m_otClusters->end() ; ++iter){
   if (inWindow(parab,line,*iter,win) == true) {
     double driftRadius = (*iter)->driftRadius(parab.value((*iter)->zMid()), line.value((*iter)->zMid()));
     if ( driftRadius < m_maxDriftRadius || driftRadius > m_minDriftRadius){
       if (m_correctDriftDist == true) {
         Tsa::OTCluster* newClus = (*iter)->clone();
         newClus->setDriftRadius(driftRadius);
         clusters->add(newClus);
       }
       else if (inContainer((*iter)->id(),ids) == false){
         Tsa::OTCluster* newClus = (*iter)->clone();
         clusters->add(newClus);
       } // correct drift distance
     } // drift cut
   } // in window
 } // iter          
}

template<typename TYPE>
inline void TsaCollector::searchIT(const Tsa::Parabola& parab,
                                   const Tsa::Line& line,
                                   const WindowSize& win , 
                                   ObjectVector<TYPE>*& clusters) const{


  // first make a list of already hit channels
  std::vector<LHCb::LHCbID> ids = channelList(clusters);

  // flat loop for now...nothing fancy
  for ( Tsa::STClusters::iterator iter = m_itClusters->begin();  iter != m_itClusters->end() ; ++iter){
    if ((inWindow(parab,line,*iter, win) == true)&& inContainer((*iter)->id(),ids) == false) {
     Tsa::STCluster* newClus = (*iter)->clone();
     clusters->add(newClus);
    } // window
  } // iter
}

template <typename TYPE>
inline std::vector<LHCb::LHCbID> TsaCollector::channelList( ObjectVector<TYPE>*& clusters) const{

  std::vector<LHCb::LHCbID> ids; ids.reserve(clusters->size());
  typename ObjectVector<TYPE>::iterator iterC = clusters->begin();
  for (; iterC != clusters->end(); ++iterC){
    ids.push_back((*iterC)->id());
   } // iterC

  return ids;
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
