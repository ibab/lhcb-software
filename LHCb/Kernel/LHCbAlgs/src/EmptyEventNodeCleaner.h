// $Id: $
#ifndef EMPTYEVENTNODECLEANER_H 
#define EMPTYEVENTNODECLEANER_H 1

#include "GaudiAlg/GaudiAlgorithm.h"

#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/SmartIF.h"

/** @class EmptyEventNodeCleaner EmptyEventNodeCleaner.h
 *  
 *  Searches for and removes empty data nodes in the TES
 *
 *  @author Chris Jones
 *  @date   2012-01-31
 */
class EmptyEventNodeCleaner : public GaudiAlgorithm 
{

public: 

  /// Standard constructor
  EmptyEventNodeCleaner( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~EmptyEventNodeCleaner( ); ///< Destructor

  virtual StatusCode initialize ();   ///< Initialize
  virtual StatusCode execute   ();    ///< Execution

private:

  /// Performs a recursive clean of the given TES location
  void cleanNodes( DataObject * obj,
                   const std::string & location,
                   unsigned int nRecCount = 0 );

private:
  
  std::string m_inputStream;   ///< Input stream root
  std::string m_dataSvcName;   ///< name of the data service

  IDataProviderSvc* m_dataSvc; ///< The data service itself

};

#endif // EMPTYEVENTNODECLEANER_H
