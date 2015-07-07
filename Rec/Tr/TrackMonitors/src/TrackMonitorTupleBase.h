// $Id: TrackMonitorTupleBase.h,v 1.4 2010-04-07 21:30:23 wouter Exp $
#ifndef TRACKMONITORTUPLEBASE_H
#define TRACKMONITORTUPLEBASE_H 1
 
// Include files
 
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"

#include <string>
#include <map>

// interfaces
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "Kernel/ITrajPoca.h"
#include "GaudiKernel/IMagneticFieldSvc.h"

#include "Event/Track.h"


/** @class TrackMonitorTupleBase TrackMonitorTupleBase.h "TrackCheckers/TrackMonitorTupleBase"
 * 
 *  Base class for track monitoring: essentially a 'box' of common tools

 *  @author Ch. Elsasser
 *  @date   16-7-2013
 */                 
                                                           
class TrackMonitorTupleBase : public GaudiTupleAlg {
                                                                             
 public:
                                                                             
  /** Standard construtor */
  TrackMonitorTupleBase( const std::string& name, ISvcLocator* pSvcLocator );
                                                                             
  /** Destructor */
  virtual ~TrackMonitorTupleBase();

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

  /** Get a pointer to the track extrapolator
  *  @return extrapolator
  */
  const ITrackExtrapolator* extrapolator() const;

  /** Input track container location 
  *  @return location
  */
  const std::string& inputContainer() const; 

  /** Whether to split by algorithm
  *  @return splitByAlgorithm true or false
  */ 
  bool splitByAlgorithm() const;
  bool splitByType() const { return  m_splitByType; }
  
  /** To avoid hard coding...
  *  @return all string
  */ 
  const std::string& all() const;

 protected:
  void setSplitByType(bool b) { m_splitByType = b ; }

  std::string histoDirName( const LHCb::Track& track) const ;

 private:
  std::string m_extrapolatorName;
  std::string m_tracksInContainer;    ///< Input Tracks container location
 
  ITrajPoca*         m_poca;          ///< Pointer to the ITrajPoca interface  
  IMagneticFieldSvc* m_pIMF;          ///< Pointer to the magn. field service
  const ITrackExtrapolator* m_extrapolator; ///< Pointer to extrapolator

  bool m_splitByAlgorithm;
  bool m_splitByType;
  std::string m_allString;  
                                                                       
};

inline IMagneticFieldSvc* TrackMonitorTupleBase::fieldSvc() const{
  return m_pIMF;
}

inline ITrajPoca* TrackMonitorTupleBase::pocaTool() const{
  return m_poca;
}

inline const ITrackExtrapolator* TrackMonitorTupleBase::extrapolator() const{
  return m_extrapolator;
}

inline const std::string& TrackMonitorTupleBase::inputContainer() const{
  return m_tracksInContainer;
}

inline bool TrackMonitorTupleBase::splitByAlgorithm() const{
  return m_splitByAlgorithm;
}


inline const std::string& TrackMonitorTupleBase::all() const{
  return m_allString;
}

#endif // TRACKMONITORTUPLEBASE_H
