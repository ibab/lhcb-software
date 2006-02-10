// $Id: ITReadoutTool.h,v 1.1 2006-02-10 08:59:31 mneedham Exp $
#ifndef _ITReadoutTool_H
#define _ITReadoutTool_H

#include <string>

// STDAQ
#include "STReadoutTool.h"
#include "Kernel/STChannelID.h"

/** @class STReadoutTool STReadoutTool.h  STDAQ/STReadoutTool
 *
 *  Concret Class for things related to the Readout - Tell1 Board
 *
 *  @author M.Needham
 *  @date   13/3/2002
*/

class ITReadoutTool: public STReadoutTool{

public:

  /// Constructer
  ITReadoutTool(const std::string& type,
                const std::string& name,
                const IInterface* parent);


  /// init
  virtual StatusCode initialize();

  /// get region
  virtual unsigned int region(const LHCb::STChannelID aChan) const;

private:

  StatusCode createBoards();

  unsigned int m_firstStation;
  std::string m_conditionLocation;

};






#endif // _ITReadoutTool_H
