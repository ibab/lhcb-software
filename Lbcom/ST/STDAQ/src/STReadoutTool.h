// $Id: STReadoutTool.h,v 1.8 2008-11-06 12:57:20 cattanem Exp $
#ifndef _STReadoutTool_H
#define _STReadoutTool_H

// STL
#include <vector>

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// STDAQ
#include "Kernel/ISTReadoutTool.h"
#include "Kernel/STTell1ID.h"
#include "Kernel/STDAQDefinitions.h"

// ST
#include "Kernel/STChannelID.h"


/** @class STReadoutTool STReadoutTool.h  STDAQ/STReadoutTool
 *
 *  Concret Class for things related to the Readout - Tell1 Board
 *
 *  @author M.Needham
 *  @date   13/3/2002
*/

class DeSTDetector;

class STReadoutTool: public GaudiTool, virtual public ISTReadoutTool{

public:

  /// Constructer
  STReadoutTool(const std::string& type,
                const std::string& name,
                const IInterface* parent);

  /// destructer
  virtual ~STReadoutTool();

  /// init
  virtual StatusCode initialize();

  /// nBoard
  virtual unsigned int nBoard() const;

  ///return vector of Tell1IDs
  virtual std::vector<STTell1ID> boardIDs() const;

  /// convert ITChannelID to DAQ ChannelID
  virtual STDAQ::chanPair offlineChanToDAQ(const LHCb::STChannelID aOfflineChan,					   double isf) const;

/// convert offline interStripFraction to DAQ interStripFraction
  virtual double interStripToDAQ(const LHCb::STChannelID aOfflineChan,
                                 const STTell1ID aBoardID,
                                 const double isf) const;

  bool ADCOfflineToDAQ(const LHCb::STChannelID aOfflineChan,
                       const STTell1ID aBoardID,
                       LHCb::STCluster::ADCVector& adcs) const;


  /// find the Tell1 board given a board ID
  virtual STTell1Board* findByBoardID(const STTell1ID aBoardID) const;

  /// find Tell1 board by storage order
  virtual STTell1Board* findByOrder(const unsigned int aValue) const;

  /// list of the readout sector ids on the board
  virtual std::vector<LHCb::STChannelID> sectorIDs(const STTell1ID board) const;
  
  /// list of the readout sectors
  virtual std::vector<DeSTSector*> sectors(const STTell1ID board) const;

  /// service box
  virtual unsigned int nServiceBox() const;

  /// service box number
  virtual std::string serviceBox(const LHCb::STChannelID& aChan) const; 

  /// list of the readout sectors ids in a service box 
  virtual std::vector<LHCb::STChannelID> sectorIDsOnServiceBox(const std::string& serviceBox) const;
  
  /// list of the readout sectors in a service box
  virtual std::vector<DeSTSector*> sectorsOnServiceBox(const std::string& serviceBox) const;

  /// list of service boxes
  virtual const std::vector<std::string>& serviceBoxes() const;

  /// print mapping
  void printMapping() const;

protected:

  void clear();

  StatusCode validate() const;

  unsigned int m_hybridsPerBoard;
  unsigned int m_nBoard;
  unsigned int m_nServiceBox;
  std::vector<STTell1Board*> m_boards;
  std::vector<std::string> m_serviceBoxes;
  std::vector<unsigned int> m_firstBoardInRegion;

  bool m_printMapping;
  DeSTDetector* m_tracker;
  std::string m_detType;

};

#endif // _STReadoutTool_H
