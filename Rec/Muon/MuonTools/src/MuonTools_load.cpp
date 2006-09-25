// $Id: MuonTools_load.cpp,v 1.5 2006-09-25 15:53:26 asatta Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2004/12/06 12:01:34  asatta
// v3r8 add a new tool
//
// Revision 1.3  2003/06/18 15:39:33  asatta
// v3r5 add MuonGeyInfoTool
//
// Revision 1.2  2002/05/10 12:47:09  dhcroft
// Added new interface to get size of pads and regions
//
// Revision 1.1.1.1  2002/03/15 15:58:17  dhcroft
// First version of the MuonTools package, incomplete
//
// ============================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(MuonTools) {
  
  DECLARE_TOOL( MuonPosTool  );
  
};


// ============================================================================
// The End 
// ============================================================================
