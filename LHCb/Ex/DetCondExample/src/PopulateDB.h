//$Id: PopulateDB.h,v 1.4 2005-02-09 09:06:25 marcocle Exp $
#ifndef DETCONDEXAMPLE_POPULATEDB_H
#define DETCONDEXAMPLE_POPULATEDB_H 1

// Base class
#include "GaudiKernel/Algorithm.h"

// Forward declarations
class IConditionsDBCnvSvc;
class IConditionsDBGate;
class ICondDBObject;

#include "ConditionsDB/CondDBException.h"
#include "ConditionsDB/CondDBKey.h"

///---------------------------------------------------------------------------
/** @class PopulateDB PopulateDB.h DetCondExample/PopulateDB.h

    Simple algorithm to populate the ConditionsDB.

    @author Andrea Valassi 
    @date August 2001
*///--------------------------------------------------------------------------

class PopulateDB : public Algorithm {

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

 private:

  /// Handle to the ConditionsDBCnvSvc (encode data to be written)
  IConditionsDBCnvSvc* m_conditionsDBCnvSvc;

  /// Handle to the ConditionsDBGate (write the data in the CondDB)
  IConditionsDBGate* m_conditionsDBGate;

  ICondDBObject* createCondDBObject(const CondDBKey& since,
                                    const CondDBKey& till,
                                    const std::string& data,
                                    const std::string& description)
    throw(CondDBException);
  void destroyCondDBObject(ICondDBObject* CondObj)
    throw(CondDBException);
};

#endif    // DETCONDEXAMPLE_POPULATEDB_H


