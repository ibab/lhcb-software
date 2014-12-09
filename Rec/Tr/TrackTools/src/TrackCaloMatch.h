#ifndef _TrackCaloMatch_H
#define _TrackCaloMatch_H

/** @class TrackCaloMatch TrackCaloMatch.h
 *
 * Implementation of TrackCaloMatch tool
 * see interface header for description
 *
 *  @author M.Needham
 *  @date   30/12/2005
 */
  
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentListener.h"

#include "TrackInterfaces/ITrackCaloMatch.h"

#include <string>
#include "Relations/IRelation.h"

namespace LHCb{
  class Track;
}
 
class TrackCaloMatch: public GaudiTool, 
                      virtual public ITrackCaloMatch,
                      virtual public IIncidentListener  {
 
public:

  /// constructer
  TrackCaloMatch(const std::string& type,
                      const std::string& name,
                      const IInterface* parent);
                                                                                
  virtual ~TrackCaloMatch();
             
  StatusCode initialize();
                                                           
  /// the method
  double energy(const LHCb::Track& aTrack) const;


  /** Implement the handle method for the Incident service.
  *  This is used to inform the tool of software incidents.
  *
  *  @param incident The incident identifier
  */
  void handle( const Incident& incident );

private: 

  void initEvent() const;

  typedef IRelation<LHCb::Track,float> Table ;
  double energy(const LHCb::Track& aTrack, const TrackCaloMatch::Table* table ) const;

  mutable Table* m_ecalE;                 
  mutable Table* m_hcalE;     
  mutable Table* m_psE;   
  std::string m_ecalLocation;
  std::string m_hcalLocation;
  std::string m_prsLocation;               
                                        
  double m_alpha;
  double m_beta;
  double m_gamma;

  mutable bool m_configured;

};

#endif
