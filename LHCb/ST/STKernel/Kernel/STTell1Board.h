// $Id: STTell1Board.h,v 1.13 2009-01-22 11:43:44 mneedham Exp $
#ifndef _STTell1Board_H
#define _STTell1Board_H 1

#include "Kernel/STTell1ID.h"
#include "Kernel/STChannelID.h"
#include "Kernel/STDAQDefinitions.h"
#include "Kernel/StripRepresentation.h"
#include <vector>
#include <iostream>

#include "Event/STCluster.h"

/** @class STTell1Board STTell1Board.h "STDAQ/STTell1Board.h"
 *
 *  Class for converting from STChannelID to daq channel/STTell1ID
 *
 *  @author M.Needham
 *  @date   5/01/2004
 */


class STTell1Board{

public:

  typedef std::pair<LHCb::STChannelID,int> chanPair;

  /// constructer
  STTell1Board(const STTell1ID aBoard, 
               const unsigned int stripsPerHybrid, 
               const std::string& type);

  /// destructer
  virtual ~STTell1Board();

  /// add wafer
  void addSector(LHCb::STChannelID aOfflineChan, unsigned int orientation, const std::string& serviceBox);

  /// board indentifer
  STTell1ID boardID() const;

  /// same board id
  bool sameID(const STTell1ID& id) const;

  /// flat tell1 number as in itell1XXX etc
  unsigned int flatTell1Number() const;
 
  /// channel is in this board
  bool isInside(const LHCb::STChannelID aOfflineChan,
                unsigned int& sectorIndex) const;

  /// construct LHCb::STChannelID from DAQ Channel
  chanPair DAQToOffline(const unsigned int fracStrip,
			const STDAQ::version& version,
                        const STDAQ::StripRepresentation aDAQChan) const;

  /// fill adc values offline 
  void ADCToOffline(const unsigned int aDAQChan,
		      LHCb::STCluster::ADCVector& adcs,
		      const int version,
		      const unsigned int offset,
		      const double interStripPos) const;
  

  /// construct DAQChannel from LHCb::STChannelID
  unsigned int offlineToDAQ(const LHCb::STChannelID aOfflineChan,
                            const unsigned int sectorIndex,
			    double isf) const;

  /// check channel is valid
  bool validChannel(const unsigned int daqChan) const;

  /// vector of sectors on the board
  const std::vector<LHCb::STChannelID>& sectorIDs() const;
  
  /// vector of hybrid orientations
  const std::vector<int>& orientation() const;

  /// vector of service boxes
  const std::vector<std::string>& serviceBoxes() const;

  /// service box
  std::string serviceBox(const LHCb::STChannelID& chan) const; 

  /// service box
  std::string serviceBox(const STDAQ::StripRepresentation& tell1Chan) const;

  /// number of readout sectors
  unsigned int nSectors() const;

  /// Operator overloading for stringoutput
  friend std::ostream& operator<< (std::ostream& s, const STTell1Board& obj)
  {
    return obj.fillStream(s);
  }

  // Fill the ASCII output stream
  virtual std::ostream& fillStream(std::ostream& s) const;

  /** print method for python Not needed in C++ */
  std::string toString() const;

private:

  /// service box
  std::string serviceBox(const unsigned int& waferIndex) const; 

  STTell1ID m_boardID;
  unsigned int m_nStripsPerHybrid;
  std::vector<LHCb::STChannelID> m_sectorsVector;
  std::vector<int> m_orientation;
  std::vector<std::string> m_serviceBoxVector;
  std::string m_detType;

};

#include <sstream>
#include <string>

inline std::string STTell1Board::toString() const{
  std::ostringstream o;
  fillStream(o);
  return o.str();
}

inline STTell1ID STTell1Board::boardID() const{
  return m_boardID;
}

inline bool STTell1Board::sameID(const STTell1ID& id) const{
  return m_boardID == id ? true : false;
}

inline const std::vector<int>& STTell1Board::orientation() const{
  return m_orientation;
}

inline const std::vector<std::string>& STTell1Board::serviceBoxes() const{
  return m_serviceBoxVector;
}

inline const std::vector<LHCb::STChannelID>& STTell1Board::sectorIDs() const{
  return m_sectorsVector;
}

inline std::string STTell1Board::serviceBox(const unsigned int& waferIndex) const {
  return (waferIndex < m_serviceBoxVector.size() ? m_serviceBoxVector[waferIndex] : std::string("Unknown"));
}

inline std::string STTell1Board::serviceBox(const LHCb::STChannelID& chan) const{
  unsigned int waferIndex;
  isInside(chan, waferIndex);
  return serviceBox(waferIndex);
}

inline bool STTell1Board::validChannel(const unsigned int daqChan) const{
  if (daqChan > m_nStripsPerHybrid*m_sectorsVector.size()) return false;
  const int index = daqChan/m_nStripsPerHybrid; 
  if (m_sectorsVector[index].sector() == 0 ) return false;
  return true; 
}
  
inline unsigned int STTell1Board::nSectors() const{
  return m_sectorsVector.size();
}

#endif // _STTell1Board_H
