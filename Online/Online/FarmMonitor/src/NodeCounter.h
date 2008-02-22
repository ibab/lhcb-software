// $Id: NodeCounter.h,v 1.1.1.1 2008-02-22 15:06:06 ocallot Exp $
#ifndef NODECOUNTER_H 
#define NODECOUNTER_H 1

// Include files
#include "GaudiKernel/MsgStream.h"   // Get default definitions from Gaudi
#include "dim/dis.hxx"
#include "dim/dic.hxx"

/** @class NodeCounter NodeCounter.h
 *  This is the counters for a single node
 *
 *  @author Olivier Callot
 *  @date   2007-11-15
 */
class NodeCounter {
public: 
  /// Standard constructor
  NodeCounter( std::string& name, bool isNode=true ); 

  virtual ~NodeCounter( ); ///< Destructor

  void update();

  std::string statusLine();

  void createCounters();

  void reset() {
    m_rcv = 0;
    m_rqd = 0;
    m_inc = 0;
    m_pro = 0;
    m_wri = 0;
  }
  
  void increment( const NodeCounter* cnt ) {    
    m_rcv += cnt->nReceived();
    m_rqd += cnt->nRequested();
    m_inc += cnt->nIncomplete();
    m_pro += cnt->nProduced();
    m_wri += cnt->nWritten();
  }  

  int nReceived()   const { return m_rcv; }
  int nRequested()  const { return m_rqd; }
  int nIncomplete() const { return m_inc; }
  int nProduced()   const { return m_pro; }
  int nWritten()    const { return m_wri; }

  std::string name() const { return m_name; }

protected:

private:
  std::string m_name;
  std::string m_address;
  DimInfo*    m_mepRqd;
  DimInfo*    m_mepRcv;
  DimInfo*    m_incomp;
  DimInfo*    m_evtPro;
  DimInfo*    m_evtWri;
  
  int m_rcv;
  int m_rqd;
  int m_inc;
  int m_pro;
  int m_wri;
  
};
#endif // NODECOUNTER_H
