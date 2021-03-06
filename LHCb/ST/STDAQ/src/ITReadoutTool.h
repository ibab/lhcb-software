// $Id: ITReadoutTool.h,v 1.4 2009-10-30 12:54:19 mtobin Exp $
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

  /** Add the mapping of source ID to board number for IT and TT */
  virtual const  std::map<unsigned int, unsigned int>& SourceIDToTELLNumberMap() const; 

  /** Add the mapping of board number to source ID for IT and TT */
  virtual const  std::map<unsigned int, unsigned int>& TELLNumberToSourceIDMap() const; 

private:

  StatusCode createBoards();

  unsigned int m_firstStation;
 

};






#endif // _ITReadoutTool_H
