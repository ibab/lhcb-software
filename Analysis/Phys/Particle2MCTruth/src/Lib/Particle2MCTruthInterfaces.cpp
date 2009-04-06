// $Id: Particle2MCTruthInterfaces.cpp,v 1.1 2009-04-06 20:13:38 jpalac Exp $
// Include files 

#include "P2MCP/IP2MCP.h"

namespace 
{
  const InterfaceID IID_IP2MCP ( "IP2MCP", 1, 0 );
}
const InterfaceID& IP2MCP::interfaceID() { return IID_IP2MCP; }
IP2MCP::~IP2MCP() { }
