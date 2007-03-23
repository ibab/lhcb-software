// $Id: ITReadoutTool.h,v 1.2 2007-03-23 14:36:53 cattanem Exp $
#ifndef _ITReadoutTool_H
#define _ITReadoutTool_H

#include <string>

// STDAQ
#include "STReadoutTool.h"
#include "Kernel/STChannelID.h"

/** @class ITReadoutTool ITReadoutTool.h
 *
 *  Concrete class for things related to the Readout - Tell1 Board
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
