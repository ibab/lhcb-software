
//-----------------------------------------------------------------------------
/** @file RichTools_load.cpp
 *
 * Implementation file for classes in the RichTools Component Library
 *
 * CVS Log :-
 * $Id: RichTools_load.cpp,v 1.3 2004-07-26 18:03:05 jonrob Exp $
 * $Log: not supported by cvs2svn $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 14/01/2002
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( RichTools ) 
{

  // The tool registry
  DECLARE_TOOL( RichToolRegistry );

  // Ray tracing
  DECLARE_TOOL( RichRayTracing );  

  // photon reconstruction
  DECLARE_TOOL( RichPhotonReco );  
  
  // track segment makers
  DECLARE_TOOL( RichTrSegMakerFromTrStoredTracks ); 
  
  // RichSmartID manipulation 
  DECLARE_TOOL( RichSmartIDTool );

  // new tools
  DECLARE_TOOL( RichMirrorSegFinder );

  // refractive index tools
  DECLARE_TOOL( RichTabulatedRefractiveIndex );

  // detector parameters
  DECLARE_TOOL( RichDetParameters );

}
