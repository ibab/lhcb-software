//$Id: PopulateDB.h,v 1.7 2005-05-13 09:14:38 marcocle Exp $
#ifndef DETCONDEXAMPLE_POPULATEDB_H
#define DETCONDEXAMPLE_POPULATEDB_H 1

// Base class
#include "GaudiAlg/GaudiAlgorithm.h"

// Forward declarations
class ICondDBAccessSvc;
class ICondDBObject;
class Condition;

//#include "ConditionsDB/CondDBException.h"
//#include "ConditionsDB/CondDBKey.h"

///---------------------------------------------------------------------------
/** @class PopulateDB PopulateDB.h DetCondExample/PopulateDB.h

    Simple algorithm to populate the ConditionsDB.

    @author Marco Clemencic
    @date January 2005
*///--------------------------------------------------------------------------

class PopulateDB : public GaudiAlgorithm {

 public:

  /// Constructor
  PopulateDB ( const std::string& name, ISvcLocator* pSvcLocator ); 
  
  // Algorithm standard methods
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
 private:

  /// Store sample data
  StatusCode i_condDBStoreSampleData();

  /// Dump sample data
  StatusCode i_condDBDumpSampleData();

  /// Encode XML temperature data
  void i_encodeXmlTemperature( const double temperature,
			       const std::string& objName,
			       std::string& xmlString );
  
  /// Encode XML paramVector
  void i_encodeXmlParamVector( const double par[3],
			       const std::string& objName,
			       const std::string& parName,
			       std::string& xmlString );
  
  /// Dump the contents of a CondDBFolder
  StatusCode i_dumpFolder( const std::string& folderName,
			   const std::string& tagName );

  /// Tag the full tree.
  StatusCode i_tagDB(const std::string& tagName);

 private:

  /// Handle to the ConditionsDBGate (write the data in the CondDB)
  ICondDBAccessSvc* m_dbAccSvc;

};

#endif    // DETCONDEXAMPLE_POPULATEDB_H


