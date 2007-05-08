// $Id: TrackMonitorBase.h,v 1.1 2007-05-08 06:49:00 mneedham Exp $
#ifndef TRACKMONITORBASE_H
#define TRACKMONITORBASE_H 1
 
// Include files
 
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"

#include <string>

// interfaces
#include "TrackInterfaces/ITrackSelector.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "Kernel/ITrajPoca.h"
#include "GaudiKernel/IMagneticFieldSvc.h"

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
  ITrackSelector* selector() const;

  /** Get a pointer to the track extrapolator
  *  @return extrapolator
  */
  ITrackExtrapolator* extrapolator() const;

  /** Input track container location 
  *  @return location
  */
  const std::string& inputContainer() const; 


 private:

  std::string m_extrapolatorName; 
  std::string m_selectorName;
  std::string m_tracksInContainer;    ///< Input Tracks container location
 
  ITrajPoca*         m_poca;          ///< Pointer to the ITrajPoca interface  
  IMagneticFieldSvc* m_pIMF;          ///< Pointer to the magn. field service
  ITrackSelector* m_selector;         ///< Pointer to selector   
  ITrackExtrapolator* m_extrapolator; ///< Pointer to extrapolator
                                                                         
};

inline IMagneticFieldSvc* TrackMonitorBase::fieldSvc() const{
  return m_pIMF;
}

inline ITrajPoca* TrackMonitorBase::pocaTool() const{
  return m_poca;
}

inline ITrackSelector* TrackMonitorBase::selector() const{
  return m_selector;
}

inline ITrackExtrapolator* TrackMonitorBase::extrapolator() const{
  return m_extrapolator;
}

inline const std::string& TrackMonitorBase::inputContainer() const{
  return m_tracksInContainer;
}

#endif // TRACKMONITORBASE_H
