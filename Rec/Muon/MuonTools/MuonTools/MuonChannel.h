//$Id: MuonChannel.h,v 1.1 2002-03-21 09:41:33 dhcroft Exp $
#ifndef MuonTools_MuonChannel_H
#define MuonTools_MuonChannel_H 1

// Include files

// From MuonDet
#include "MuonDet/MuonParameters.h"


/** @class MuonChannel MuonChannel.h 
 *
 *  Stores the channel offsets within a chamber + readout and channel size
 *
 *  @author David Hutchcroft
 *
 */

class MuonChannel{

public: 

  /// Constructor taking all values as arguments
  MuonChannel(const int& xPos, const int& yPos,  
              const MuonParameters::ReadoutType& type)
    : m_xPos(xPos), m_yPos(yPos), m_readout(type)
  {}

  /// Default Constructor 
  MuonChannel() 
    : m_xPos(0),
    m_yPos(0),
    m_readout(MuonParameters::Anode)
  {}

  /// Destructor 
  virtual ~MuonChannel() {}

  /// Retrieve x position of the         readout channel within the chamber
  int xPos() const; 

  /// Update x position of the         readout channel within the chamber
  void setXPos(int value);

  /// Retrieve y position of the         readout channel within the chamber
  int yPos() const; 

  /// Update y position of the         readout channel within the chamber
  void setYPos(int value);

  /// Retrieve Readout number of         channel
  MuonParameters::ReadoutType readout() const; 

  /// Update Readout number of         channel
  void setReadout(MuonParameters::ReadoutType value);

private: 

  int m_xPos; ///<         x position of the readout channel within the chamber
  int m_yPos; ///<         y position of the readout channel within the chamber
  MuonParameters::ReadoutType m_readout; ///<      Readout type of channel

};

// ---------------------------------------------------------------------------
//   end of class
// ---------------------------------------------------------------------------


inline int MuonChannel::xPos() const 
{
  return m_xPos;
}

inline void MuonChannel::setXPos(int value)
{
  m_xPos = value; 
}

inline int MuonChannel::yPos() const 
{
  return m_yPos;
}

inline void MuonChannel::setYPos(int value)
{
  m_yPos = value; 
}

inline MuonParameters::ReadoutType MuonChannel::readout() const 
{
  return m_readout;
}

inline void MuonChannel::setReadout(MuonParameters::ReadoutType value)
{
  m_readout = value; 
}

#endif   ///MuonTools_MuonChannel_H
