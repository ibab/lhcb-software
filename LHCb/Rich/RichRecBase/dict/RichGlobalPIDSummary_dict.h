

//   **************************************************************************
//   *                                                                        *
//   *                      ! ! ! A T T E N T I O N ! ! !                     *
//   *                                                                        *
//   *  This file was created automatically by GaudiObjDesc, please do not    *
//   *  delete it or edit it by hand.                                         *
//   *                                                                        *
//   *  If you want to change this file, first change the corresponding       *
//   *  xml-file and rerun the tools from GaudiObjDesc (or run make if you    *
//   *  are using it from inside a Gaudi-package).                            *
//   *                                                                        *
//   **************************************************************************



#ifndef RichRecBase_RichGlobalPIDSummary_H
#define RichRecBase_RichGlobalPIDSummary_H 1

// Include files
#include <vector>
#include "Kernel/CLHEPStreams.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ObjectList.h"
#include "GaudiKernel/ObjectVector.h"
#include "Kernel/SerializeStl.h"
#include "GaudiKernel/StreamBuffer.h"


// Class ID definition
  static const CLID& CLID_RichGlobalPIDSummary = 12431;

// Namespace for locations in TDS
namespace RichGlobalPIDSummaryLocation {
  static const std::string& Default = "Rec/Rich/GlobalPIDSummary";
}


/** @class RichGlobalPIDSummary RichGlobalPIDSummary.h 
 *
 *  Summary object for RICH Global PID analysis
 *
 *  @author Chris Jones    (Christopher.Rob.Jones@cern.ch)
 *  created Sun Mar  2 19:00:05 2003
 *
 */

class RichGlobalPIDSummary: public DataObject
{

public: 

  friend class RichGlobalPIDSummary_dict;

  /// Default Constructor 
  RichGlobalPIDSummary() 
    : m_usedTracks(0),
    m_usedVeloTracks(0),
    m_usedSeedTracks(0),
    m_usedMatchTracks(0),
    m_usedForwardTracks(0),
    m_usedUpstreamTracks(0),
    m_usedVeloTTTracks(0),
    m_totalTracks(0),
    m_totalVeloTracks(0),
    m_totalSeedTracks(0),
    m_totalMatchTracks(0),
    m_totalForwardTracks(0),
    m_totalUpstreamTracks(0),
    m_totalVeloTTTracks(0),
    m_nRich1HitPixels(0),
    m_nRich2HitPixels(0),
    m_eventLL(),
    m_eventProcessedOK(),
    m_eventIteration(0),
    m_rich1BckEstimate(0.0),
    m_rich2BckEstimate(0.0) {}

  /// Destructor 
  virtual ~RichGlobalPIDSummary() {}

  /// Retrieve pointer to class definition structure
  virtual const CLID& clID() const; 
  static const CLID& classID(); 

  /// Push back this value in event LL vector
  void addToEventLL(double logL);

  /// Retrieve Total number of used tracks
  int usedTracks() const; 

  /// Update Total number of used tracks
  void setUsedTracks(int value);

  /// Retrieve Number of velo tracks used in global PID analysis
  int usedVeloTracks() const; 

  /// Update Number of velo tracks used in global PID analysis
  void setUsedVeloTracks(int value);

  /// Retrieve Number of seed tracks used in global PID analysis
  int usedSeedTracks() const; 

  /// Update Number of seed tracks used in global PID analysis
  void setUsedSeedTracks(int value);

  /// Retrieve Number of match tracks used in global PID analysis
  int usedMatchTracks() const; 

  /// Update Number of match tracks used in global PID analysis
  void setUsedMatchTracks(int value);

  /// Retrieve Number of forward tracks used in global PID analysis
  int usedForwardTracks() const; 

  /// Update Number of forward tracks used in global PID analysis
  void setUsedForwardTracks(int value);

  /// Retrieve Number of upstream tracks used in global PID analysis
  int usedUpstreamTracks() const; 

  /// Update Number of upstream tracks used in global PID analysis
  void setUsedUpstreamTracks(int value);

  /// Retrieve Number of Velo-TT tracks used in global PID analysis
  int usedVeloTTTracks() const; 

  /// Update Number of Velo-TT tracks used in global PID analysis
  void setUsedVeloTTTracks(int value);

  /// Retrieve Total number of tracks in the event
  int totalTracks() const; 

  /// Update Total number of tracks in the event
  void setTotalTracks(int value);

  /// Retrieve Total number of velo tracks in event
  int totalVeloTracks() const; 

  /// Update Total number of velo tracks in event
  void setTotalVeloTracks(int value);

  /// Retrieve Total number of seed tracks in event
  int totalSeedTracks() const; 

  /// Update Total number of seed tracks in event
  void setTotalSeedTracks(int value);

  /// Retrieve Total number of match tracks in event
  int totalMatchTracks() const; 

  /// Update Total number of match tracks in event
  void setTotalMatchTracks(int value);

  /// Retrieve Total number of forward tracks in event
  int totalForwardTracks() const; 

  /// Update Total number of forward tracks in event
  void setTotalForwardTracks(int value);

  /// Retrieve Total number of upstream tracks in event
  int totalUpstreamTracks() const; 

  /// Update Total number of upstream tracks in event
  void setTotalUpstreamTracks(int value);

  /// Retrieve Total number of velo-TT tracks in event
  int totalVeloTTTracks() const; 

  /// Update Total number of velo-TT tracks in event
  void setTotalVeloTTTracks(int value);

  /// Retrieve Number of hit pixels in Rich1
  int nRich1HitPixels() const; 

  /// Update Number of hit pixels in Rich1
  void setNRich1HitPixels(int value);

  /// Retrieve Number of hit pixels in Rich2
  int nRich2HitPixels() const; 

  /// Update Number of hit pixels in Rich2
  void setNRich2HitPixels(int value);

  /// Retrieve History of overall event LogLikelihood values
  const std::vector<float>& eventLL() const; 

  /// Retrieve History of overall event LogLikelihood values (non-const)
  std::vector<float>& eventLL();

  /// Update History of overall event LogLikelihood values
  void setEventLL(const std::vector<float>& value);

  /// Retrieve Boolean to indicate if the event was successfully processed
  bool eventProcessedOK() const; 

  /// Update Boolean to indicate if the event was successfully processed
  void setEventProcessedOK(bool value);

  /// Retrieve Current event iteration
  int eventIteration() const; 

  /// Update Current event iteration
  void setEventIteration(int value);

  /// Retrieve Esitamte of overall background in Rich1
  double rich1BckEstimate() const; 

  /// Update Esitamte of overall background in Rich1
  void setRich1BckEstimate(double value);

  /// Retrieve Esitamte of overall background in Rich2
  double rich2BckEstimate() const; 

  /// Update Esitamte of overall background in Rich2
  void setRich2BckEstimate(double value);

  /// Serialize the object for writing
  virtual StreamBuffer& serialize(StreamBuffer& s) const;

  /// Serialize the object for reading
  virtual StreamBuffer& serialize(StreamBuffer& s);

  /// Fill the ASCII output stream
  virtual std::ostream& fillStream(std::ostream& s) const;

protected: 

private: 

  int m_usedTracks; ///< Total number of used tracks
  int m_usedVeloTracks; ///< Number of velo tracks used in global PID analysis
  int m_usedSeedTracks; ///< Number of seed tracks used in global PID analysis
  int m_usedMatchTracks; ///< Number of match tracks used in global PID analysis
  int m_usedForwardTracks; ///< Number of forward tracks used in global PID analysis
  int m_usedUpstreamTracks; ///< Number of upstream tracks used in global PID analysis
  int m_usedVeloTTTracks; ///< Number of Velo-TT tracks used in global PID analysis
  int m_totalTracks; ///< Total number of tracks in the event
  int m_totalVeloTracks; ///< Total number of velo tracks in event
  int m_totalSeedTracks; ///< Total number of seed tracks in event
  int m_totalMatchTracks; ///< Total number of match tracks in event
  int m_totalForwardTracks; ///< Total number of forward tracks in event
  int m_totalUpstreamTracks; ///< Total number of upstream tracks in event
  int m_totalVeloTTTracks; ///< Total number of velo-TT tracks in event
  int m_nRich1HitPixels; ///< Number of hit pixels in Rich1
  int m_nRich2HitPixels; ///< Number of hit pixels in Rich2
  std::vector<float> m_eventLL; ///< History of overall event LogLikelihood values
  bool m_eventProcessedOK; ///< Boolean to indicate if the event was successfully processed
  int m_eventIteration; ///< Current event iteration
  double m_rich1BckEstimate; ///< Esitamte of overall background in Rich1
  double m_rich2BckEstimate; ///< Esitamte of overall background in Rich2

};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------


inline const CLID& RichGlobalPIDSummary::clID() const 
{
  return RichGlobalPIDSummary::classID();
}

inline const CLID& RichGlobalPIDSummary::classID()
{
  return CLID_RichGlobalPIDSummary;
}

inline void RichGlobalPIDSummary::addToEventLL(double logL)
{
   m_eventLL.push_back( logL );
}

inline int RichGlobalPIDSummary::usedTracks() const 
{
  return m_usedTracks;
}

inline void RichGlobalPIDSummary::setUsedTracks(int value)
{
  m_usedTracks = value; 
}

inline int RichGlobalPIDSummary::usedVeloTracks() const 
{
  return m_usedVeloTracks;
}

inline void RichGlobalPIDSummary::setUsedVeloTracks(int value)
{
  m_usedVeloTracks = value; 
}

inline int RichGlobalPIDSummary::usedSeedTracks() const 
{
  return m_usedSeedTracks;
}

inline void RichGlobalPIDSummary::setUsedSeedTracks(int value)
{
  m_usedSeedTracks = value; 
}

inline int RichGlobalPIDSummary::usedMatchTracks() const 
{
  return m_usedMatchTracks;
}

inline void RichGlobalPIDSummary::setUsedMatchTracks(int value)
{
  m_usedMatchTracks = value; 
}

inline int RichGlobalPIDSummary::usedForwardTracks() const 
{
  return m_usedForwardTracks;
}

inline void RichGlobalPIDSummary::setUsedForwardTracks(int value)
{
  m_usedForwardTracks = value; 
}

inline int RichGlobalPIDSummary::usedUpstreamTracks() const 
{
  return m_usedUpstreamTracks;
}

inline void RichGlobalPIDSummary::setUsedUpstreamTracks(int value)
{
  m_usedUpstreamTracks = value; 
}

inline int RichGlobalPIDSummary::usedVeloTTTracks() const 
{
  return m_usedVeloTTTracks;
}

inline void RichGlobalPIDSummary::setUsedVeloTTTracks(int value)
{
  m_usedVeloTTTracks = value; 
}

inline int RichGlobalPIDSummary::totalTracks() const 
{
  return m_totalTracks;
}

inline void RichGlobalPIDSummary::setTotalTracks(int value)
{
  m_totalTracks = value; 
}

inline int RichGlobalPIDSummary::totalVeloTracks() const 
{
  return m_totalVeloTracks;
}

inline void RichGlobalPIDSummary::setTotalVeloTracks(int value)
{
  m_totalVeloTracks = value; 
}

inline int RichGlobalPIDSummary::totalSeedTracks() const 
{
  return m_totalSeedTracks;
}

inline void RichGlobalPIDSummary::setTotalSeedTracks(int value)
{
  m_totalSeedTracks = value; 
}

inline int RichGlobalPIDSummary::totalMatchTracks() const 
{
  return m_totalMatchTracks;
}

inline void RichGlobalPIDSummary::setTotalMatchTracks(int value)
{
  m_totalMatchTracks = value; 
}

inline int RichGlobalPIDSummary::totalForwardTracks() const 
{
  return m_totalForwardTracks;
}

inline void RichGlobalPIDSummary::setTotalForwardTracks(int value)
{
  m_totalForwardTracks = value; 
}

inline int RichGlobalPIDSummary::totalUpstreamTracks() const 
{
  return m_totalUpstreamTracks;
}

inline void RichGlobalPIDSummary::setTotalUpstreamTracks(int value)
{
  m_totalUpstreamTracks = value; 
}

inline int RichGlobalPIDSummary::totalVeloTTTracks() const 
{
  return m_totalVeloTTTracks;
}

inline void RichGlobalPIDSummary::setTotalVeloTTTracks(int value)
{
  m_totalVeloTTTracks = value; 
}

inline int RichGlobalPIDSummary::nRich1HitPixels() const 
{
  return m_nRich1HitPixels;
}

inline void RichGlobalPIDSummary::setNRich1HitPixels(int value)
{
  m_nRich1HitPixels = value; 
}

inline int RichGlobalPIDSummary::nRich2HitPixels() const 
{
  return m_nRich2HitPixels;
}

inline void RichGlobalPIDSummary::setNRich2HitPixels(int value)
{
  m_nRich2HitPixels = value; 
}

inline const std::vector<float>& RichGlobalPIDSummary::eventLL() const 
{
  return m_eventLL;
}

inline std::vector<float>& RichGlobalPIDSummary::eventLL()
{
  return m_eventLL;
}

inline void RichGlobalPIDSummary::setEventLL(const std::vector<float>& value)
{
  m_eventLL = value; 
}

inline bool RichGlobalPIDSummary::eventProcessedOK() const 
{
  return m_eventProcessedOK;
}

inline void RichGlobalPIDSummary::setEventProcessedOK(bool value)
{
  m_eventProcessedOK = value; 
}

inline int RichGlobalPIDSummary::eventIteration() const 
{
  return m_eventIteration;
}

inline void RichGlobalPIDSummary::setEventIteration(int value)
{
  m_eventIteration = value; 
}

inline double RichGlobalPIDSummary::rich1BckEstimate() const 
{
  return m_rich1BckEstimate;
}

inline void RichGlobalPIDSummary::setRich1BckEstimate(double value)
{
  m_rich1BckEstimate = value; 
}

inline double RichGlobalPIDSummary::rich2BckEstimate() const 
{
  return m_rich2BckEstimate;
}

inline void RichGlobalPIDSummary::setRich2BckEstimate(double value)
{
  m_rich2BckEstimate = value; 
}

inline StreamBuffer& RichGlobalPIDSummary::serialize(StreamBuffer& s) const 
{
  unsigned char l_eventProcessedOK = (m_eventProcessedOK) ? 1 : 0;
  s << m_usedTracks
    << m_usedVeloTracks
    << m_usedSeedTracks
    << m_usedMatchTracks
    << m_usedForwardTracks
    << m_usedUpstreamTracks
    << m_usedVeloTTTracks
    << m_totalTracks
    << m_totalVeloTracks
    << m_totalSeedTracks
    << m_totalMatchTracks
    << m_totalForwardTracks
    << m_totalUpstreamTracks
    << m_totalVeloTTTracks
    << m_nRich1HitPixels
    << m_nRich2HitPixels
    << m_eventLL
    << l_eventProcessedOK
    << m_eventIteration
    << (float)m_rich1BckEstimate
    << (float)m_rich2BckEstimate;
  return s;
}

inline StreamBuffer& RichGlobalPIDSummary::serialize(StreamBuffer& s)
{
  unsigned char l_eventProcessedOK;
  float l_rich1BckEstimate, l_rich2BckEstimate;
  s >> m_usedTracks
    >> m_usedVeloTracks
    >> m_usedSeedTracks
    >> m_usedMatchTracks
    >> m_usedForwardTracks
    >> m_usedUpstreamTracks
    >> m_usedVeloTTTracks
    >> m_totalTracks
    >> m_totalVeloTracks
    >> m_totalSeedTracks
    >> m_totalMatchTracks
    >> m_totalForwardTracks
    >> m_totalUpstreamTracks
    >> m_totalVeloTTTracks
    >> m_nRich1HitPixels
    >> m_nRich2HitPixels
    >> m_eventLL
    >> l_eventProcessedOK
    >> m_eventIteration
    >> l_rich1BckEstimate
    >> l_rich2BckEstimate;
  m_eventProcessedOK = (l_eventProcessedOK) ? true : false;
  m_rich1BckEstimate = l_rich1BckEstimate;
  m_rich2BckEstimate = l_rich2BckEstimate;
  return s;
}

inline std::ostream& RichGlobalPIDSummary::fillStream(std::ostream& s) const
{
  char l_eventProcessedOK = (m_eventProcessedOK) ? 'T' : 'F';
  s << "{ "
    << " usedTracks:\t" << m_usedTracks << std::endl
    << "   usedVeloTracks:\t" << m_usedVeloTracks << std::endl
    << "   usedSeedTracks:\t" << m_usedSeedTracks << std::endl
    << "   usedMatchTracks:\t" << m_usedMatchTracks << std::endl
    << "   usedForwardTracks:\t" << m_usedForwardTracks << std::endl
    << "   usedUpstreamTracks:\t" << m_usedUpstreamTracks << std::endl
    << "   usedVeloTTTracks:\t" << m_usedVeloTTTracks << std::endl
    << "   totalTracks:\t" << m_totalTracks << std::endl
    << "   totalVeloTracks:\t" << m_totalVeloTracks << std::endl
    << "   totalSeedTracks:\t" << m_totalSeedTracks << std::endl
    << "   totalMatchTracks:\t" << m_totalMatchTracks << std::endl
    << "   totalForwardTracks:\t" << m_totalForwardTracks << std::endl
    << "   totalUpstreamTracks:\t" << m_totalUpstreamTracks << std::endl
    << "   totalVeloTTTracks:\t" << m_totalVeloTTTracks << std::endl
    << "   nRich1HitPixels:\t" << m_nRich1HitPixels << std::endl
    << "   nRich2HitPixels:\t" << m_nRich2HitPixels << std::endl
    << "   eventLL:\t" << m_eventLL << std::endl
    << "   eventProcessedOK:\t" << l_eventProcessedOK << std::endl
    << "   eventIteration:\t" << m_eventIteration << std::endl
    << "   rich1BckEstimate:\t" << (float)m_rich1BckEstimate << std::endl
    << "   rich2BckEstimate:\t" << (float)m_rich2BckEstimate << " } ";
  return s;
}


#endif   ///RichRecBase_RichGlobalPIDSummary_H
