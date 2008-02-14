// $Id: STReadoutTool.h,v 1.4 2008-02-14 10:02:24 mneedham Exp $
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

  bool STReadoutTool::ADCOfflineToDAQ(const LHCb::STChannelID aOfflineChan,
				      const STTell1ID aBoardID,
				      LHCb::STCluster::ADCVector& adcs) const;


  /// find the Tell1 board given a board ID
  virtual STTell1Board* findByBoardID(const STTell1ID aBoardID) const;

  /// find Tell1 board by storage order
  virtual STTell1Board* findByOrder(const unsigned int aValue) const;

protected:

  void clear();

  void printMapping() const;
  
  unsigned int m_hybridsPerBoard;
  unsigned int m_nBoard;
  std::vector<STTell1Board*> m_boards;
  std::vector<unsigned int> m_firstBoardInRegion;

  bool m_printMapping;

};

#endif // _STReadoutTool_H
