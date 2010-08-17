// $Id: RootAddress.h,v 1.4 2010-08-17 17:16:24 frankb Exp $
//====================================================================
//	RootAddress.h
//--------------------------------------------------------------------
//
//	Author     : M.Frank
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/RootCnv/src/RootAddress.h,v 1.4 2010-08-17 17:16:24 frankb Exp $
#ifndef ROOTCNV_ROOTADDRESS_H
#define ROOTCNV_ROOTADDRESS_H

// Framework include files
#include "GaudiKernel/GenericAddress.h"
#include "RootDataConnection.h"
#include "RootSelect.h"

// Forward declaration
class TTree;

/*
 * Gaudi namespace declaration
 */
namespace Gaudi {

  // Forward declarations
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
    /// Pointer to ROOT select statement (filled for N-tuples only)
    RootSelect*         select;
    /// Pointer to ROOT data connection (filled for N-tuples only)
    RootDataConnection* connection;
    /// Pointer to ROOT TTree (filled for N-tuples only)
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

#endif // ROOTCNV_ROOTADDRESS_H
