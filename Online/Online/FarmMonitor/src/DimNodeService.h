// $Id: DimNodeService.h,v 1.1.1.1 2008-02-22 15:06:06 ocallot Exp $
#ifndef DIMNODESERVICE_H 
#define DIMNODESERVICE_H 1

// Include files
#include "GaudiKernel/MsgStream.h"   // Get default definitions from Gaudi
#include "dim/dis.hxx"
#include "NodeCounter.h"

/** @class DimNodeService DimNodeService.h
 *  Holds a Dim service for one line of the Farm Monitoring
 *
 *  @author Olivier Callot
 *  @date   2007-12-21
 */
class DimNodeService {
public: 
  /// Standard constructor
  DimNodeService( int nb ); 

  virtual ~DimNodeService( ); ///< Destructor

  void update( NodeCounter* counter );
  
protected:

private:
  DimService* m_service;
  struct dimBuffer {
    int number;
    char name[12];
    char time[12];
    int rqd;
    int rcv;
    int inc;
    int pro;
    int wri;
  };  
  dimBuffer m_dimBuffer;
};
#endif // DIMNODESERVICE_H
