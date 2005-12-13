
//-----------------------------------------------------------------------------
/** @file RichTools_load.cpp
 *
 * Implementation file for classes in the RichTools Component Library
 *
 * CVS Log :-
 * $Id: RichTools_load.cpp,v 1.6 2005-12-13 15:07:11 jonrob Exp $
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
  // for all spherical mirrors
  DECLARE_TOOL( RichRayTracingAllSph );  
  
  // RichSmartID manipulation 
  DECLARE_TOOL( RichSmartIDTool );

  // new tools
  DECLARE_TOOL( RichMirrorSegFinder );

  // refractive index tools
  DECLARE_TOOL( RichTabulatedRefractiveIndex );

  // detector parameters
  DECLARE_TOOL( RichDetParameters );

  // particle properties tool  
  DECLARE_TOOL( RichParticleProperties );

}
