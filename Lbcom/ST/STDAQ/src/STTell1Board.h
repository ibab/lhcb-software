// $Id: STTell1Board.h,v 1.2 2006-03-15 16:38:51 mneedham Exp $
#ifndef _STTell1Board_H
#define _STTell1Board_H 1

#include "STTell1ID.h"
#include "Kernel/STChannelID.h"
#include <vector>
#include <iostream>

/** @class STTell1Board STTell1Board.h "STDAQ/STTell1Board.h"
 *
 *  Class for converting from STChannelID to daq channel/STTell1ID
 *
 *  @author M.Needham
 *  @date   5/01/2004
 */


class STTell1Board{

public:

  /// constructer
  STTell1Board(const STTell1ID aBoard, 
               const unsigned int stripsPerHybrid);

  /// destructer
  virtual ~STTell1Board();

  /// add wafer
  void addSector(LHCb::STChannelID aOfflineChan, unsigned int orientation);

  /// board indentifer
  STTell1ID boardID() const;

  /// channel is in this board
  bool isInside(const LHCb::STChannelID aOfflineChan,
                unsigned int& sectorIndex) const;

  /// construct LHCb::STChannelID from DAQ Channel
  LHCb::STChannelID DAQToOffline(const unsigned int aDAQChan) const;

  /// construct DAQChannel from LHCb::STChannelID
  unsigned int offlineToDAQ(const LHCb::STChannelID aOfflineChan,
                            const unsigned int sectorIndex) const;


  /// Operator overloading for stringoutput
  friend std::ostream& operator<< (std::ostream& s, const STTell1Board& obj)
  {
    return obj.fillStream(s);
  }


  // Fill the ASCII output stream
  virtual std::ostream& fillStream(std::ostream& s) const;


  //std::ostream& STTell1Board::printOut( std::ostream& os ) const;

private:

  STTell1ID m_boardID;
  unsigned int m_nStripsPerHybrid;
  std::vector<LHCb::STChannelID> m_sectorsVector;
  std::vector<int> m_orientation;

};

inline STTell1ID STTell1Board::boardID() const{
  return m_boardID;
}


#endif // _STTell1Board_H
