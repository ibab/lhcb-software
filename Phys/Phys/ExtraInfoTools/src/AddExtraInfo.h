#ifndef ADDEXTRAINFO_H 
#define ADDEXTRAINFO_H 1

#include <algorithm>
#include <string>
#include <sstream>

// Include files 
#include "GaudiKernel/Property.h"

#include "Kernel/IExtraInfoTool.h"

#include "Kernel/DaVinciAlgorithm.h"

/** @class AddExtraInfo AddExtraInfo.h
 *
 *  Algorithm to add Particle isolation variables
 *  calculated by ConeVariables tool to particle extra info
 *
 *  @author Anton Poluektov
 *  @date   19/02/2012
 */

class AddExtraInfo : public DaVinciAlgorithm
{

public:

  /// Standard constructor
  AddExtraInfo( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~AddExtraInfo( ); ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  std::vector<std::string> m_toolNames;
  std::list<IExtraInfoTool*> m_tools;
  int m_maxLevel;
  std::vector<std::string> m_daughterLocations;
  
  void fill(const LHCb::Particle* top, LHCb::Particle* c, int level);

};

//=======================================================================//
#endif // AddExtraInfo_H
