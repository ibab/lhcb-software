// $Id: ISTReadoutTool.h,v 1.1 2006-02-10 08:59:31 mneedham Exp $
#ifndef _ISTReadoutTool_H
#define _ISTReadoutTool_H

#include "GaudiKernel/IAlgTool.h"

#include <string>
#include "STDAQDefinitions.h"

class STTell1Board;

namespace LHCb{
 class STChannelID;
};

/** @class ISTReadoutTool ISTReadoutTool.h  STDAQ/ISTReadoutTool
*
*  Interface Class for things related to the Readout - Tell1 Board
*
*  @author M.Needham
*  @date   13/3/2002
*/

// Declaration of the interface ID ( interface id, major version, minor version)

static const InterfaceID IID_ISTReadoutTool("ISTReadoutTool", 0 , 0);


class ISTReadoutTool: virtual public IAlgTool{

public:

  /// Static access to interface id
  static const InterfaceID& interfaceID() { return IID_ISTReadoutTool; }

  /// number of boards
  virtual unsigned int nBoard() const = 0;

  /// convert STChannelID to DAQ ChannelID
  virtual STDAQ::chanPair offlineChanToDAQ(const LHCb::STChannelID aOfflineChan) const = 0;

  /// find the Tell1 board given a board ID
  virtual STTell1Board* findByBoardID(const STTell1ID aBoardID) const = 0;

  /// find a Tell board by storage order
  virtual STTell1Board* findByOrder(const unsigned int aValue) const = 0;

  /// get region
  virtual unsigned int region(const LHCb::STChannelID aChan) const = 0;

  /// printout
  virtual void printMapping() const = 0;
 
};


#endif // _ISTReadoutTool_H
