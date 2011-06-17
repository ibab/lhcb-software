// $Id: RootAddress.h,v 1.8 2010-09-17 09:40:02 frankb Exp $
//====================================================================
//	RootAddress.h
//--------------------------------------------------------------------
//
//	Author     : M.Frank
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/RootCnv/src/RootAddress.h,v 1.8 2010-09-17 09:40:02 frankb Exp $
#ifndef GAUDIROOTCNV_ROOTADDRESS_H
#define GAUDIROOTCNV_ROOTADDRESS_H

// Framework include files
#include "GaudiKernel/GenericAddress.h"
#include "TTreeFormula.h"

// Forward declaration
class TTree;
class TTreeFormula;

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
  class GAUDI_API RootAddress : virtual public GenericAddress {
  public:
    /// Pointer to ROOT select statement (filled for N-tuples only)
    TTreeFormula*       select;
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
      : GenericAddress(svc,clid,p1,p2,ip1,ip2), select(0), section(0) { }
      /// Standard Destructor
      virtual ~RootAddress() {  if ( select ) delete select; select = 0; }
  };
}

#endif // GAUDIROOTCNV_ROOTADDRESS_H
