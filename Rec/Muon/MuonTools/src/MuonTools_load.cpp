// $Id: MuonTools_load.cpp,v 1.3 2003-06-18 15:39:33 asatta Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2002/05/10 12:47:09  dhcroft
// Added new interface to get size of pads and regions
//
// Revision 1.1.1.1  2002/03/15 15:58:17  dhcroft
// First version of the MuonTools package, incomplete
//
// ============================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(MuonTools) {
  
  DECLARE_TOOL( MuonTileIDXYZ  );
  DECLARE_TOOL( MuonGeometryTool  );
  DECLARE_TOOL( MuonGetInfoTool  );
  
};


// ============================================================================
// The End 
// ============================================================================
