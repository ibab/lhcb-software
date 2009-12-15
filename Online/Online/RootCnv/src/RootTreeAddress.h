// $Id: RootTreeAddress.h,v 1.1 2009-12-15 15:37:25 frankb Exp $
//====================================================================
//	RootTreeAddress.h
//--------------------------------------------------------------------
//
//	Author     : M.Frank
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/RootCnv/src/RootTreeAddress.h,v 1.1 2009-12-15 15:37:25 frankb Exp $
#ifndef ROOT_RootTreeAddress_H
#define ROOT_RootTreeAddress_H

// Framework include files
#include "GaudiKernel/GenericAddress.h"

class TBranch;

/** @class RootTreeAddress RootTreeAddress.h GaudiRootTree/RootTreeAddress.h
  *
  * Description:
  *
  * Definition of a transient link which is capable of locating
  * an object in the persistent storage.
  *
  * @author  M.Frank
  * @version 1.0
  */
class RootTreeAddress : virtual public GenericAddress {
protected:
  /// TBranch object
  TBranch* m_branch;

public:
  /// Full constructor
  RootTreeAddress( long svc,
		   const CLID& clid,
		   const std::string& p1="", 
		   const std::string& p2="",
		   unsigned long ip1=0,
		   unsigned long ip2=0)
    : GenericAddress(svc,clid,p1,p2,ip1,ip2), m_branch(0)    {}

    /// Standard Destructor
    virtual ~RootTreeAddress() {
    }
    /// Access to TBranch object containing object data
    TBranch* buffer() const                  { return m_branch;     }
    /// Set buffer containing object data
    void setBuffer(TBranch* b)               {  m_branch = b;       }
};
#endif // ROOT_RootTreeAddress_H
