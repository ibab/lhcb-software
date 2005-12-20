// $Id: STTell1Board.h,v 1.1.1.1 2005-12-20 12:47:27 mneedham Exp $
#ifndef _STTell1Board_H
#define _STTell1Board_H 1

#include "STDAQ/STTell1ID.h"
#include "Kernel/STChannelID.h"
#include <vector>
#include <iostream>

/** @class STTell1Board STTell1Board.h "STDAQ/STTell1Board.h"
 *
 *  Class for converting from STChannelID to STDAQChannelID/STTell1ID
 *
 *  @author M.Needham
 *  @date   5/01/2004
 */

class STDAQChannelID;

class STTell1Board{

public:

  /// constructer
  STTell1Board(const STTell1ID aBoard);

  /// destructer
  virtual ~STTell1Board();

  /// add wafer
  void addSector(LHCb::STChannelID aOfflineChan);

  /// board indentifer
  STTell1ID boardID() const;

  /// channel is in this board
  bool isInside(const LHCb::STChannelID aOfflineChan,
                unsigned int& sectorIndex) const;

  /// construct LHCb::STChannelID from STDAQChannelID
  LHCb::STChannelID DAQToOffline(const STDAQChannelID aDAQChan) const;

  /// construct STDAQChannelID from LHCb::STChannelID
  STDAQChannelID offlineToDAQ(const LHCb::STChannelID aOfflineChan,
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
  std::vector<LHCb::STChannelID> m_sectorsVector;

};

inline STTell1ID STTell1Board::boardID() const{
  return m_boardID;
}

#endif // _STTell1Board_H
