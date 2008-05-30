// $Id: TrackMonitorBase.h,v 1.1 2008-05-30 13:50:08 wouter Exp $
#ifndef TRACKMONITORBASE_H
#define TRACKMONITORBASE_H 1
 
// Include files
 
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"

#include <string>
#include <map>

// interfaces
#include "TrackInterfaces/ITrackSelector.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "Kernel/ITrajPoca.h"
#include "GaudiKernel/IMagneticFieldSvc.h"

#include "Event/Track.h"


/** @class TrackMonitorBase TrackMonitorBase.h "TrackCheckers/TrackMonitorBase"
 * 
 *  Base class for track monitoring: essentially a 'box' of common tools

 *  @author M. Needham.
 *  @date   6-5-2007
 */                 
                                                           
class TrackMonitorBase : public GaudiHistoAlg {
                                                                             
 public:
                                                                             
  /** Standard construtor */
  TrackMonitorBase( const std::string& name, ISvcLocator* pSvcLocator );
                                                                             
  /** Destructor */
  virtual ~TrackMonitorBase();

  /** Algorithm initialization */
  virtual StatusCode initialize();

 protected:
   
  /** Get a pointer to Magnetic field service
  *  @return field service
  */
  IMagneticFieldSvc* fieldSvc() const;
 
  /** Get a pointer to the poca tool
  *  @return poca tool
  */
  ITrajPoca* pocaTool() const;

  /** Get a pointer to the track selection tool
  *  @return field service
  */
  ITrackSelector* selector(LHCb::Track::Types aType) const;

  /** Get a pointer to the track extrapolator
  *  @return extrapolator
  */
  ITrackExtrapolator* extrapolator() const;

  /** Input track container location 
  *  @return location
  */
  const std::string& inputContainer() const; 

  /** Whether to split by algorithm
  *  @return splitByAlgorithm true or false
  */ 
  bool splitByAlgorithm() const;
  
  typedef std::map<LHCb::Track::Types,ITrackSelector*> Selectors;

  /** To avoid hard coding...
  *  @return all string
  */ 
  const std::string& all() const;

 private:

  mutable Selectors m_selectors; 
    

  std::string m_extrapolatorName;
  std::string m_tracksInContainer;    ///< Input Tracks container location
 
  ITrajPoca*         m_poca;          ///< Pointer to the ITrajPoca interface  
  IMagneticFieldSvc* m_pIMF;          ///< Pointer to the magn. field service
  ITrackExtrapolator* m_extrapolator; ///< Pointer to extrapolator

  bool m_splitByAlgorithm;
  std::string m_allString;  
                                                                       
};

inline IMagneticFieldSvc* TrackMonitorBase::fieldSvc() const{
  return m_pIMF;
}

inline ITrajPoca* TrackMonitorBase::pocaTool() const{
  return m_poca;
}

inline ITrackSelector* TrackMonitorBase::selector(LHCb::Track::Types aType) const{
  return m_splitByAlgorithm == true ? m_selectors[aType] : 
                                      m_selectors[LHCb::Track::TypeUnknown];
}

inline ITrackExtrapolator* TrackMonitorBase::extrapolator() const{
  return m_extrapolator;
}

inline const std::string& TrackMonitorBase::inputContainer() const{
  return m_tracksInContainer;
}

inline bool TrackMonitorBase::splitByAlgorithm() const{
  return m_splitByAlgorithm;
}


inline const std::string& TrackMonitorBase::all() const{
  return m_allString;
}

#endif // TRACKMONITORBASE_H
