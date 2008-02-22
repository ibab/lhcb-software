// $Id: FarmMonitor.h,v 1.1.1.1 2008-02-22 15:06:07 ocallot Exp $
#ifndef FARMMONITOR_H 
#define FARMMONITOR_H 1

// Include files
#include "dim/dis.hxx"
#include "dim/dic.hxx"
#include "NodeCounter.h"
#include "DimNodeService.h"

/** @class FarmMonitor FarmMonitor.h
 *  Monitor the farm, get the list of tasks to process
 *
 *  @author Olivier Callot
 *  @date   2007-11-14
 */
class FarmMonitor : public DimClient {
public: 
  /// Standard constructor
  FarmMonitor( std::string partition ); 

  virtual ~FarmMonitor( ); ///< Destructor

  void update();
  
  void display();
  
  virtual void infoHandler();
  
protected:
  std::string printableDate() {
    char date[30];
    char format[30]="%d-%b-%Y %H:%M:%S";
    time_t tt = time( 0 );
    const struct tm* myTime =  localtime( &tt );  
    strftime( date, 25, format, myTime );
    return std::string( date );
  }
  
private:
  std::vector<NodeCounter*>    m_nodes;
  std::vector<DimNodeService*> m_services;
  DimServer*                   m_dimServer;
  DimService*                  m_service;
  NodeCounter*                 m_total;
  int                          m_numberOfNodes;
};
#endif // FARMMONITOR_H
