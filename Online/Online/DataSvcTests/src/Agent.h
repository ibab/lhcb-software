//====================================================================
//	Agent.h
//--------------------------------------------------------------------
//
//	Package    : PersistencySvc/Test_1 (The LHCb PersistencySvc service)
//
//  Description: Definition of Data Store test agent
//
//	Author     : M.Frank
//  Created    : 13/1/99
//	Changes    : 
//
//
//====================================================================
#ifndef DATASVC_AGENT_H
#define DATASVC_AGENT_H

// STL includes
#include <iostream>

// Framework includes
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IDataStoreAgent.h"

/** @name The Agent class.
 
  Example data store agent to traverse data store trees.
 
  @author Markus Frank
 */
class Agent : virtual public IDataStoreAgent  {
  bool m_debug;
public:
  /// Standard Constructor
  Agent(bool dbg)  : m_debug(dbg) {
  }
  /// Standard Destructor
  virtual ~Agent()  {
  }
  /// Analysis callback
  bool analyse(IRegistry* pReg, int /* level */ )   {
    DataSvcHelpers::RegistryEntry* dir = 
      dynamic_cast<DataSvcHelpers::RegistryEntry*>(pReg);
    if ( m_debug )    {
      DataObject* pObj = dir->object();
      std::cout << "Analysing "
                << " Object:" 
                << ((pObj==0)       ? "UNLOADED" : "LOADED  ") 
                << " Linkage:"
                << ((dir->isSoft()) ? "Symbolic" : "Hard    ")
                << " (" 
                << dir->identifier() 
                << ") ";
      if ( dir->isSoft() && 0 != pObj )    {
        std::cout <<  "  --->  " << dir->identifier();
      }
      if ( 0 != pObj )    {
        std::cout << "  " << typeid(*pObj).name();
      }
      std::cout << std::endl;
    }
    return true;
  }
};
#endif // DATASVC_AGENT_H
