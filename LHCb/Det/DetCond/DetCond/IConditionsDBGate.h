//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetCond/DetCond/IConditionsDBGate.h,v 1.2 2004-12-08 17:12:07 marcocle Exp $
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
    @author Marco Clemencic
    @date November 2004
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

  /// Return a string stating the implementation used
  virtual const std::string & implementation() const = 0;

  /// Return the code of the implementation used
  virtual short implementationCode() const = 0;
  
 public:

  /// (TEMPORARY?) Handle to the CondDB manager for arbitrary manipulations.
  virtual ICondDBMgr* condDBManager ( ) = 0;

};

namespace ConditionsDBGateImplementation 
{
  /// Cannot define this as enum as there is no method to read enum properties
  static const short CONDDBNONE   = 0;
  static const short CONDDBOBJY   = 1;
  static const short CONDDBORACLE = 2;
  static const short CONDDBMYSQL  = 3;
};

///---------------------------------------------------------------------------

#endif // DETCOND_ICONDITIONSDBGATE_H
