//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetCond/DetCond/IConditionsDBGate.h,v 1.1.1.1 2001-09-14 15:07:22 andreav Exp $
#ifndef DETCOND_ICONDITIONSDBGATE_H
#define DETCOND_ICONDITIONSDBGATE_H 1

// Base class
#include "GaudiKernel/IInterface.h"

// External constants
#include "GaudiKernel/ClassID.h"

// IConditionsDBGate service ID (interface id, major version, minor version) 
static const InterfaceID IID_IConditionsDBGate (19703, 1, 0);

// Forward declarations
class ConditionData;
class ICondDBMgr;
class ICondDBObject;
class ITime;

///---------------------------------------------------------------------------
/** @class IConditionsDBGate IConditionsDBGate.h Det/DetCond/IConditionsDBGate.h

    Abstract interface to a low-level service for CERN-IT CondDB manipulation.

    This is the low level interface to the CondDB. It takes care of:
    <UL>
    <LI> Database management (open, init, close, transactions)
    <LI> Folder management
    <LI> Tag management
    <LI> Data access
    </UL>

    @author Andrea Valassi 
    @date August 2001
*///--------------------------------------------------------------------------

class IConditionsDBGate : virtual public IInterface
{

 public:

  // Reimplemented from IInterface

  /// Interface ID of this class
  static const InterfaceID& interfaceID() { return IID_IConditionsDBGate; }

 public:
  
  // CondDB data access

  /// Read range and data of a CondDBObject by folder name, tag and time
  virtual StatusCode 
    readCondDBObject      ( ITime&              refValidSince,
			    ITime&              refValidTill,
			    std::string&        data,
			    const std::string&  folderName,
			    const std::string&  tagName,
			    const ITime&        time            ) = 0;
  
 public:

  // CondDB folder management

  /// Read the description of a folder in the CondDB
  virtual StatusCode 
    readCondDBFolder      ( std::string&        description,
		            const std::string&  folderName      ) = 0;
  
 public:

  /// (TEMPORARY?) Handle to the CondDB manager for arbitrary manipulations.
  virtual ICondDBMgr* condDBManager ( ) = 0;

};

///---------------------------------------------------------------------------

#endif // DETCOND_ICONDITIONSDBGATE_H
