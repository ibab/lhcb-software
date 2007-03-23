// $Id: TTReadoutTool.h,v 1.2 2007-03-23 14:36:53 cattanem Exp $
#ifndef _TTReadoutTool_H
#define _TTReadoutTool_H

#include <string>

// STDAQ
#include "STReadoutTool.h"
#include "Kernel/STChannelID.h"

/** @class TTReadoutTool TTReadoutTool.h
 *
 *  Concrete class for things related to the Readout - Tell1 Board
 *
 *  @author M.Needham
 *  @date   13/3/2002
*/

class TTReadoutTool: public STReadoutTool{

public:

  /// Constructer
  TTReadoutTool(const std::string& type,
                const std::string& name,
                const IInterface* parent);


  /// init
  virtual StatusCode initialize();

  /// get region
  virtual unsigned int region(const LHCb::STChannelID aChan) const;

private:

  StatusCode createBoards();

  std::string m_conditionLocation;

  unsigned int m_nRegionA;
  unsigned int m_firstStation;

};






#endif // _TTReadoutTool_H
