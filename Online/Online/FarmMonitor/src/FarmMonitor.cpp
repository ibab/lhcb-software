// $Id: FarmMonitor.cpp,v 1.1.1.1 2008-02-22 15:06:07 ocallot Exp $
// Include files
#include <netdb.h>
#include <arpa/inet.h>
#include "GaudiKernel/MsgStream.h"
#include "dim/dic.hxx"

// local
#include "FarmMonitor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FarmMonitor
//
// 2007-11-14 : Olivier Callot
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FarmMonitor::FarmMonitor( std::string partition ) : DimClient() {

  std::cout << "Starting FarmMonitor for partition " << partition;
  char* dnsNode = getDnsNode();
  std::cout << " DNS node " << dnsNode << std::endl;

  // Get all the services from teh specified counter -> identifies the streams.
  DimBrowser browser;
  std::string request = "ECS*";
  int nServ = browser.getServices( request.c_str() );
  std::cout << "-- found " << nServ << " ECS* services" << std::endl;
  
  // Extract the node and address form the service, create the counter.
  char* svcName;
  char* fmt;  
  while ( 0 != browser.getNextService( svcName, fmt ) ) {
    std::string fullName( svcName );
    std::cout << "Found service '" << fullName << "'   fmt " << fmt << std::endl;
  }

  // Get the list of nodes in the partition  
  std::string nodeList = "ECS_RunInfo/" + partition + "/nodeList";
  DimInfo nodes( nodeList.c_str(), "", this );
  
  std::string total( "Total" );  
  m_total = new NodeCounter( total, false );

  // Declare a service with the number of nodes to display
  m_numberOfNodes = 0;
  std::string serviceName = "FarmMonitor/NumberOfNodes";
  char* format="I";
  m_service = new DimService( serviceName.c_str(), format, (void*) &m_numberOfNodes, 4 );

  //-- Create counters, just with the number, up to 20
  for ( int kk = 0; 20 >= kk ; ++kk ) {
    DimNodeService* dim = new DimNodeService( kk );
    m_services.push_back( dim );
  }

  // Start the DimServer providing the services
  m_dimServer = new DimServer();
  std::string serverName = "FarmMonitor";
  m_dimServer->start( serverName.c_str() );
  std::cout << "FarmMonitor initialized" << std::endl;
}
//=============================================================================
// Destructor
//=============================================================================
FarmMonitor::~FarmMonitor() {}
//=========================================================================
//  Receive the list of nodes, when it changes
//=========================================================================
void FarmMonitor::infoHandler ( ) {
  DimInfo* curr = getInfo();
  char* cNodes = curr->getString();
  int lNodes   = curr->getSize();

  std::cout << "lNodes = " << lNodes << std::endl;

  m_nodes.clear();
  m_numberOfNodes = 0;
  
  while ( 1 < lNodes ) {
    int ll = strlen( cNodes );

    // from the numeric address, get the node name
    std::string nodeAddr( cNodes );
    char node[100];
    int nodeLen = 99;
    struct sockaddr_in sa;
    int saLen = sizeof(struct sockaddr);
    sa.sin_family = AF_INET;
    inet_aton( cNodes, &sa.sin_addr );
    getnameinfo( (const sockaddr*)&sa, saLen, node, nodeLen, 0, 0, 0 );
    lNodes = lNodes - ll - 1;
    cNodes = cNodes + ll + 1;

    std::string address(node);
    address = address.substr( 0, address.find( "-" ) );

    NodeCounter* nodeCntr = new NodeCounter( address );
    std::cout << "Added node " << address << std::endl;
    
    m_nodes.push_back( nodeCntr );
  }
}
//=========================================================================
//  Update the counters uppon request
//=========================================================================
void FarmMonitor::update ( ) { 
  m_total->reset();

  std::vector<NodeCounter*>::iterator it;
  for ( it = m_nodes.begin(); m_nodes.end() != it; ++it ) {
    (*it)->update();
    m_total->increment( *it );
  }
  m_total->update();

  //== Publish the node. All of them...
  std::vector<DimNodeService*>::iterator itS = m_services.begin();  
  for ( it = m_nodes.begin(); m_nodes.end() != it; ++it ) {
    if ( m_services.end() != itS ) {
      (*itS)->update( *it );
      itS++;
    }
  }
  
  if ( m_services.end() != itS ) {
    (*itS)->update( m_total );
    itS++;
  }
  bool mustUpdate = false;
  int nLines = itS - m_services.begin();
  if ( m_numberOfNodes != nLines ) {
    mustUpdate = true;
    m_numberOfNodes = nLines;
  }
  if ( mustUpdate ) m_service->updateService();
}

//=========================================================================
//  Display on the current screem
//=========================================================================
void FarmMonitor::display ( ) { 
  char clear=12;
  std::cout << clear << "-- " << printableDate() << " --" << std::endl;
  std::cout << "        Node   MEP reqstd    MEP recvd"
            << "   Incomplete      Produced      Written" << std::endl;

  std::vector<NodeCounter*>::iterator it;
  for ( it = m_nodes.begin(); m_nodes.end() != it; ++it ) {
    std::cout << (*it)->statusLine() << std::endl;
  }
  std::cout << std::endl << m_total->statusLine() << std::endl;
}
//=============================================================================
