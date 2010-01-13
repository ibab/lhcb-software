// $Id: RootAddress.h,v 1.2 2010-01-13 18:34:21 frankb Exp $
//====================================================================
//	RootAddress.h
//--------------------------------------------------------------------
//
//	Author     : M.Frank
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/RootCnv/src/RootAddress.h,v 1.2 2010-01-13 18:34:21 frankb Exp $
#ifndef ROOTCNV_RootAddress_H
#define ROOTCNV_RootAddress_H

// Framework include files
#include "GaudiKernel/GenericAddress.h"
#include "RootSelect.h"

class TTree;

namespace Gaudi {

  class RootDataConnection;

  /** @class RootAddress RootAddress.h GaudiRoot/RootAddress.h
   *
   * Description:
   *
   * Definition of a transient link which is capable of locating
   * an object in the persistent storage.
   *
   * @author  M.Frank
   * @version 1.0
   */
  class RootAddress : virtual public GenericAddress {
  protected:
  public:
    RootSelect*         select;
    RootDataConnection* connection;
    TTree*              section;
  public:
    /// Full constructor
    RootAddress( long svc,
		 const CLID& clid,
		 const std::string& p1="", 
		 const std::string& p2="",
		 unsigned long ip1=0,
		 unsigned long ip2=0)
      : GenericAddress(svc,clid,p1,p2,ip1,ip2), select(0), connection(0), section(0)    {}
  
    /// Standard Destructor
    virtual ~RootAddress() {
      if ( select ) delete select;
      select = 0;
    }
  };
}

#endif // ROOTCNV_RootAddress_H
