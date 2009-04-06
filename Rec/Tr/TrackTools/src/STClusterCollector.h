// $Id: STClusterCollector.h,v 1.1 2009-04-06 07:55:39 mneedham Exp $
#ifndef _STClusterCollector_H
#define _STClusterCollector_H 1

#include <map>
#include <string>

// ST tool base class
#include "Kernel/STToolBase.h"

// LHCbKernel
#include "TrackInterfaces/ISTClusterCollector.h"

/** @class STClusterCollector STClusterCollector.h
 *
 *  Class for collecting STClusters around a track
 *  @author M.Needham
 *  @date   28/3/2009
 */

#include "GaudiKernel/IIncidentListener.h"

#include "Event/STCluster.h"

namespace LHCb{
  class Track;
}

class ITrackExtrapolator;
class ITrajPoca;

class STClusterCollector: public ST::ToolBase, 
                                virtual public ISTClusterCollector,
                                virtual public IIncidentListener  {

 public: 
   
  /// constructer
  STClusterCollector( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

  /// destructer
  virtual ~STClusterCollector();


  /// initialize method
  StatusCode initialize();

  /// collect the hits
  virtual StatusCode execute(const LHCb::Track& track,
                             ISTClusterCollector::Hits& outputCont) const; 


  /** Implement the handle method for the Incident service.
  *  This is used to inform the tool of software incidents.
  *
  *  @param incident The incident identifier
  */
  virtual void handle( const Incident& incident );

 private:
 
  double m_xTol;
  double m_yTol;
  double m_refZ;
  double m_windowSize; 
  std::string m_dataLocation;
  mutable LHCb::STClusters*  m_dataCont;
  mutable bool m_configured;

  std::string m_extrapolatorName;
  ITrackExtrapolator* m_extrapolator;

  ITrajPoca * m_trajPoca;

};

#endif // STClusterCollector_H
