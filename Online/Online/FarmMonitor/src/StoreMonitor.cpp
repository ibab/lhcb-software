// $Id: StoreMonitor.cpp,v 1.1.1.1 2008-02-22 15:06:06 ocallot Exp $
// Include files 

#include "GaudiKernel/MsgStream.h"
#include "dim/dic.hxx"

// local
#include "StoreMonitor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : StoreMonitor
//
// 2007-12-05 : Olivier Callot
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
StoreMonitor::StoreMonitor( std::string& part ) {

  std::string node;
  std::string addr;
 
  // Get all the services from teh specified counter -> identifies the streams.
  DimBrowser browser;
  std::string request = part + "_STORESTRM*_RCV*/Receiver/EventsIn";
  int nServ = browser.getServices( request.c_str() );
  
  // Extract the node and address form the service, create the counter.
  char* serviceName;
  char* format;  
  while ( 0 != browser.getNextService( serviceName, format ) ) {
    std::string fullName( serviceName );
    node = fullName.substr( part.length()+1 );
    node = node.substr(0, node.find( "_" ) );
    addr = fullName.substr( fullName.find("_RCV")+4 );
    addr = addr.substr(0, addr.find("/Rece"));
    StoreCounter* storeCntr = new StoreCounter( node, addr );
    m_nodes.push_back( storeCntr );
    std::cout << "Add counter node " << node << " addr " << addr << std::endl;
  }
  node = "";
  addr = "StoreTotal";
  m_total = new StoreCounter( node, addr );

  sleep( 1 );  // Need to wait before the first access

  // Start the DimServer providing the services on node ecs01
  m_dimServer = new DimServer();
  node = "hlt01";
  m_dimServer->setDnsNode( node.c_str() );
  
  std::string serverName = "StoreMonitor";
  m_dimServer->start( serverName.c_str() );

}
//=============================================================================
// Destructor
//=============================================================================
StoreMonitor::~StoreMonitor() {} 

//=========================================================================
//  Update the counters uppon request
//=========================================================================
void StoreMonitor::update ( ) { 
  m_total->reset();

  std::vector<StoreCounter*>::iterator it;
  for ( it = m_nodes.begin(); m_nodes.end() != it; ++it ) {
    (*it)->update();
    m_total->increment( *it );
  }
  m_total->update();
}

//=========================================================================
//  Display on the current screem
//=========================================================================
void StoreMonitor::display ( ) { 
  char clear=12;
  std::cout << clear << "-- " << printableDate() << " --" << std::endl;
  std::cout << "        Node   Evt received  Evt Written" << std::endl;

  std::vector<StoreCounter*>::iterator it;
  for ( it = m_nodes.begin(); m_nodes.end() != it; ++it ) {
    std::cout << (*it)->statusLine() << std::endl;
  }
  std::cout << std::endl << m_total->statusLine() << std::endl;
}
//=============================================================================
