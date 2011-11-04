//====================================================================
//	DataSelector.h
//--------------------------------------------------------------------
//
//	Package    : DataSvc/Test_1 (The LHCb DataSvc service)
//
//  Description: Definition of example data selector object
//
//	Author     : M.Frank
//  Created    : 13/1/99
//
//====================================================================
#ifndef DATASVC_DATASELECTOR_H
#define DATASVC_DATASELECTOR_H 1

// STL includes
#include <string>
#include <iostream>
// Framework includes
#include "Gaudi/Interfaces/IDataDirectory.h"
#include "Gaudi/Interfaces/IDataStoreAgent.h"
#include "Gaudi/Interfaces/IDataSelector.h"

namespace Tests {

  /** @name The data selector class.
   * 
   *  Data selectors are used to traverse the transient data store
   *  in order to perform collections of objects.
   * 
   *  @author Markus Frank
   */
  class DataSelector : virtual public IDataStoreAgent, virtual public IDataSelector    {
  protected:
    /// Data member: do printout while seraching the store.
    bool m_print;
  public:
    /// Standard Constructor
    DataSelector(bool do_print)   {
      m_print = do_print;
    }
    /// Standard Destructor
    virtual ~DataSelector()   {
    }
    /// Analyse one object while traversing the store
    virtual bool analyse(IDataDirectory* dir)   {
      if ( m_print )    {
	std::cout << "Analysing-Select (" << dir->name() << ") Object:" 
		  << ((dir->object()==0) ? "UNLOADED" : "LOADED") << std::endl;
      }
      if ( 0 != dir->object() ) {
	push_back(dir->object());
      }
      return true;
    }
  };
}
#endif // DATASVC_DATASELECTOR_H
