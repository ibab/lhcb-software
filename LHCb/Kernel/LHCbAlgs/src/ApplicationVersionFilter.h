
#ifndef ApplicationVersionFilter_H 
#define ApplicationVersionFilter_H 1

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// Event
#include "Event/ProcessHeader.h"

// boost
#include <boost/regex.hpp>

/** @class ApplicationVersionFilter ApplicationVersionFilter.h
 *  
 *  Checks the application version from the given ProcessHeader,
 *  comparing it to a given regex pattern to pass/reject the event.
 *
 *  @author Chris Jones
 *  @date   18-04-2013
 */
class ApplicationVersionFilter : public GaudiAlgorithm 
{

public: 

  /// Standard constructor
  ApplicationVersionFilter( const std::string& name, 
                            ISvcLocator* pSvcLocator );

  virtual ~ApplicationVersionFilter( ); ///< Destructor

  virtual StatusCode execute();    ///< Algorithm execution

private:

  std::string m_regex;    ///< String match regex
  std::string m_loc;      ///< DataObject location in TES

};

#endif // DATAOBJECTVERSIONFILTER_H
