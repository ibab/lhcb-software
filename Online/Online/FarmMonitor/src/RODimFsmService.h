// $Id: RODimFsmService.h,v 1.1 2009-04-03 09:16:40 ocallot Exp $
#ifndef RODIMFSMSERVICE_H 
#define RODIMFSMSERVICE_H 1

// Include files
//#include "GaudiKernel/MsgStream.h"   // Get default definitions from Gaudi
#include <string>
#include "dim/dis.hxx"

/** @class RODimFsmService RODimFsmService.h
 *  Holds a Dim service for one line of the Farm Monitoring
 *
 *  @author Olivier Callot
 *  @date   2007-12-21
 */
class RODimFsmService {
public: 
  /// Standard constructor
  RODimFsmService( std::string part, int nb ); 

  virtual ~RODimFsmService( ); ///< Destructor

  void update( int ii, std::string name, std::string message );

  std::string& part() { return m_part; }
  
protected:

private:
  std::string m_part;
  DimService* m_service;
  struct dimBuffer {
    int number;
    int severity;
    char state[12];
    char message[128];
  };  
  dimBuffer m_dimBuffer;
};
#endif // RODIMFSMSERVICE_H
