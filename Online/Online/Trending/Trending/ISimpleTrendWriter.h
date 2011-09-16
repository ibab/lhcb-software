// $Id: $
#ifndef TRENDING_ISIMPLETRENDWRITER_H 
#define TRENDING_ISIMPLETRENDWRITER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_ISimpleTrendingTool ( "ISimpleTrendingTool", 1, 0 );

/** @class ISimpleTrendingTool ISimpleTrendingTool.h Trending/ISimpleTrendingTool.h
 *  Simplified interface for writing a trending
 *
 *  @author Olivier Callot
 *  @date   2011-03-24
 */
class ISimpleTrendWriter : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ISimpleTrendingTool; }

  virtual void setPartitionAndName( std::string& partition, std::string& name ) = 0;
  
  virtual void startEvent() = 0;
  
  virtual void addEntry( std::string tag, double value ) = 0;
  
  virtual void setAverageTime( unsigned int seconds ) = 0;

  virtual void setMaxTimeNoWrite( unsigned int seconds ) = 0;

  virtual void saveEvent() = 0;
  
  virtual void close() = 0;

};
#endif // TRENDING_ISIMPLETRENDWRITER_H
