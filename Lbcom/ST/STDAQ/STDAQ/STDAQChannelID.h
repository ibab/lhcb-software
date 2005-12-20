#ifndef _STDAQChannelID_H
#define _STDAQChannelID_H 1

/** @class STDAQChannelID STDAQChannelID.h "STDAQ/STDAQChannelID.h"
 *
 *  Class to describe a STDAQChannelID
 *
 *  @author M.Needham
 *  @date   7/01/2004
 */

#include <iostream>
#include "STDAQ/STDAQDefinitions.h"


class STDAQChannelID{

public:

  /// constructer with strip and sectorIndex
  STDAQChannelID(unsigned int sectorIndex,
                 unsigned int strip):
  m_channelID(0u){

    m_channelID = (sectorIndex<<sectorBits) +
                  (strip<<stripBits);                  
  }
  
  explicit STDAQChannelID(int id):
  m_channelID(id){}

  /// Default Constructor
  STDAQChannelID()
    : m_channelID(0) {}

  /// Destructor
  virtual ~STDAQChannelID() {}

  /// sector 
  unsigned int sector() const; 

  /// strip
  unsigned int strip() const;

  /// cast
  operator int() const;
 
  /// Retrieve IT Channel ID
  unsigned int channelID() const;
 
  // ppx
  unsigned int ppx() const;
 
  /// Operator overloading for stringoutput
  friend std::ostream& operator<< (std::ostream& s, const STDAQChannelID& obj)
  {
    return obj.fillStream(s);
  }

  // Fill the ASCII output stream
  virtual std::ostream& fillStream(std::ostream& s) const;

private:

  enum bits { stripBits = 0, sectorBits = 9}; /// Enumeration to store the bit packing offsets
  enum masks {stripMask = 0x000001ff, sectorMask = 0x00000e00};

  unsigned int m_channelID; /// STDAQChannelID
};

inline  STDAQChannelID::operator int() const
{
   return m_channelID;
}

inline unsigned int STDAQChannelID::channelID() const
{
  return m_channelID;
}

inline unsigned int STDAQChannelID::sector() const 
{
  return ((m_channelID & sectorMask) >> sectorBits);
}

inline unsigned int STDAQChannelID::strip() const
{
  return ((m_channelID & stripMask) >> stripBits);
}

inline unsigned int STDAQChannelID::ppx() const
{
  return m_channelID/STDAQ::nStripPerPPx;
}

inline std::ostream& STDAQChannelID::fillStream(std::ostream& s) const
{
  s << "{ "
    << " channelID:\t" << std::hex << channelID() << std::endl
    << std::dec << " sector:\t"     << sector() << std::endl
    << " strip:\t"     << strip() << " } ";

  return s;
}

#endif // _STDAQChannelID_H


