// $Id: CaloTools_load.cpp,v 1.3 2001-11-09 16:25:02 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2001/11/08 10:58:34  ibelyaev
//  new tools are added for selection of subclusters within the cluster
//
// Revision 1.1  2001/11/04 15:30:40  ibelyaev
// add concrete tool
// 
// ============================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_CONVERTER( MyConverter );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES(CaloTools) {

  DECLARE_TOOL( ClusterCovarianceMatrixTool  );
  DECLARE_TOOL( SubClusterSelectorAll        );
  DECLARE_TOOL( SubClusterSelector3x3        );
  DECLARE_TOOL( SubClusterSelectorMax2x2     );
  DECLARE_TOOL( SubClusterSelectorSwissCross );

};


// ============================================================================
// The End 
// ============================================================================
