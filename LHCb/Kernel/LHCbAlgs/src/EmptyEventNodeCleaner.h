// $Id: $
#ifndef EMPTYEVENTNODECLEANER_H 
#define EMPTYEVENTNODECLEANER_H 1

#include "GaudiAlg/GaudiAlgorithm.h"

#include "GaudiKernel/AlgFactory.h" 
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

  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  void cleanNodes( DataObject * obj,
                   const std::string & location,
                   unsigned int nRecCount = 0 );

private:

  std::string m_inputStream; ///< Input stream root

};
#endif // EMPTYEVENTNODECLEANER_H
