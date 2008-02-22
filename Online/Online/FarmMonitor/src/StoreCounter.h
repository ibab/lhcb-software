// $Id: StoreCounter.h,v 1.1.1.1 2008-02-22 15:06:06 ocallot Exp $
#ifndef STORECOUNTER_H 
#define STORECOUNTER_H 1

// Include files
#include "GaudiKernel/MsgStream.h"   // Get default definitions from Gaudi
#include "dim/dis.hxx"
#include "dim/dic.hxx"

/** @class StoreCounter StoreCounter.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2007-12-05
 */
class StoreCounter {
public: 
  /// Standard constructor
  StoreCounter(  std::string& name, std::string& address ); 

  virtual ~StoreCounter( ); ///< Destructor
 void update();

  std::string statusLine();

  void createCounters();

  void reset() {
    m_rcv = 0;
    m_wri = 0;
  }
  
  void increment( const StoreCounter* cnt ) {    
    m_rcv += cnt->nReceived();
    m_wri += cnt->nWritten();
  }  

  int nReceived()   const { return m_rcv; }
  int nWritten()    const { return m_wri; }

protected:

private:
  std::string m_name;
  std::string m_address;
  DimInfo*    m_evtRcv;
  DimInfo*    m_evtWri;
  
  int m_rcv;
  int m_wri;
  
  DimService* m_service;
  struct dimBuffer {
    int rcv;
    int wri;
  };  
  dimBuffer m_dimBuffer;
};
#endif // STORECOUNTER_H
