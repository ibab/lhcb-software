#ifndef _UTREADOUTTOOL_H
#define _UTREADOUTTOOL_H

#include <string>

// STDAQ
#include "STReadoutTool.h"
#include "Kernel/STChannelID.h"

/** @class UTReadoutTool UTReadoutTool.h
 *
 *  
 *  @author Jianchun Wang
 *  @date   17/7/2012
*/

class UTReadoutTool: public STReadoutTool{

public:

  /// Constructer
  UTReadoutTool(const std::string& type,
                const std::string& name,
                const IInterface* parent);


  /// init
  virtual StatusCode initialize();

  /// get region
  virtual unsigned int region(const LHCb::STChannelID aChan) const;

  /** Add the mapping of source ID to board number for IT / TT / UT */
  virtual const  std::map<unsigned int, unsigned int>& SourceIDToTELLNumberMap() const; 

  /** Add the mapping of board number to source ID for IT / TT / UT */
  virtual const  std::map<unsigned int, unsigned int>& TELLNumberToSourceIDMap() const; 

private:

  StatusCode createBoards();


  unsigned int m_nRegionA;
  unsigned int m_firstStation;

};






#endif // _UTREADOUTTOOL_H
