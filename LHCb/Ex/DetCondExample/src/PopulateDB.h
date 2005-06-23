// $Id: PopulateDB.h,v 1.8 2005-06-23 09:33:37 marcocle Exp $
#ifndef POPULATEDB_H 
#define POPULATEDB_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// Forward declarations
class ICondDBAccessSvc;
class ICondDBObject;
class Condition;

/** @class PopulateDB PopulateDB.h
 *  
 *  Simple algorithm to populate the ConditionsDB.
 *
 *  @author Marco Clemencic
 *  @date   2005-06-22
 */
class PopulateDB : public GaudiAlgorithm {
public: 
  /// Standard constructor
  PopulateDB( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PopulateDB( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  /// Store sample data
  StatusCode i_condDBStoreSampleData();

  /// Dump sample data
  StatusCode i_condDBDumpSampleData();

  /// Encode XML temperature data
  std::string i_encodeXmlTemperature( const double temperature,
                                      const std::string& objName );
  
  /// Encode XML paramVector
  std::string i_encodeXmlParamVector( const double par[3],
                               const std::string& objName,
                               const std::string& parName );
  
  /// Dump the contents of a CondDBFolder
  StatusCode i_dumpFolder( const std::string& folderName,
                           const std::string& tagName );
  
  /// Handle to the CondDBAccessSvc (to write the data in the CondDB)
  ICondDBAccessSvc* m_dbAccSvc;

};
#endif // POPULATEDB_H
