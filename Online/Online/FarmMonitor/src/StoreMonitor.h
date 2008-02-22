// $Id: StoreMonitor.h,v 1.1.1.1 2008-02-22 15:06:06 ocallot Exp $
#ifndef STOREMONITOR_H 
#define STOREMONITOR_H 1

// Include files
#include "dim/dis.hxx"
#include "StoreCounter.h"

/** @class StoreMonitor StoreMonitor.h
 *  Monitor a storage node
 *
 *  @author Olivier Callot
 *  @date   2007-12-05
 */
class StoreMonitor {
public: 
  /// Standard constructor
  StoreMonitor( std::string& name ); 

  virtual ~StoreMonitor( ); ///< Destructor

  void update();
  
  void display();
  
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
  std::vector<StoreCounter*>  m_nodes;
  DimServer*                  m_dimServer;
  DimService*                 m_service;
  StoreCounter*               m_total;
};
#endif // STOREMONITOR_H
