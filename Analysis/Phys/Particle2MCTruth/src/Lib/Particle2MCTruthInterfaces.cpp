// $Id: Particle2MCTruthInterfaces.cpp,v 1.3 2009-10-15 10:09:08 jpalac Exp $
// Include files 

#include "P2MCP/IP2MCP.h"

namespace 
{
  const InterfaceID IID_IP2MCP ( "IP2MCP", 1, 0 );
}
const InterfaceID& IP2MCP::interfaceID() { return IID_IP2MCP; }
IP2MCP::~IP2MCP() { }
