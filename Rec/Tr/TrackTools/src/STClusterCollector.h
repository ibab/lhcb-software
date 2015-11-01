// $Id: STClusterCollector.h,v 1.3 2010-01-21 13:51:18 jluisier Exp $
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

#include <memory>


namespace LHCb{
  class Track;
  class STChannelID;
}

class ITrackExtrapolator;
class ITrajPoca;
class ISTChannelIDSelector;
class IMagneticFieldSvc;

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

  
  typedef std::pair<LHCb::STCluster*, std::shared_ptr<LHCb::Trajectory> >
    STClusterTrajectory;
  typedef std::vector<STClusterTrajectory> STClusterTrajectories;

 
 private:

  void initEvent() const;

  bool select(const LHCb::STChannelID& chan) const;
 
  double m_xTol;
  double m_yTol;
  double m_refZ;
  double m_windowSize; 
  std::string m_dataLocation;
  mutable STClusterTrajectories  m_dataCont;
  mutable bool m_configured;
  bool m_ignoreHitsOnTrack;

  std::string m_extrapolatorName;
  ITrackExtrapolator* m_extrapolator;

  ISTChannelIDSelector* m_selector;
  std::string m_selectorType;
  std::string m_selectorName;

  ITrajPoca * m_trajPoca;

  IMagneticFieldSvc *m_magFieldSvc;
  bool m_magneticField;
};

#endif // STClusterCollector_H
