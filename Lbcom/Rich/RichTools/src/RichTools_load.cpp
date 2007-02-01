
//-----------------------------------------------------------------------------
/** @file RichTools_load.cpp
 *
 * Implementation file for classes in the RichTools Component Library
 *
 * CVS Log :-
 * $Id: RichTools_load.cpp,v 1.9 2007-02-01 17:51:11 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 14/01/2002
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( RichTools )
{

  // The tool registry
  DECLARE_NAMESPACE_TOOL( Rich, ToolRegistry );

  // Ray tracing
  DECLARE_NAMESPACE_TOOL( Rich, RayTracing );

  // RichSmartID manipulation
  DECLARE_NAMESPACE_TOOL( Rich, SmartIDTool );

  // new tools
  DECLARE_NAMESPACE_TOOL( Rich, MirrorSegFinder );

  // refractive index tools
  DECLARE_NAMESPACE_TOOL( Rich, TabulatedRefractiveIndex );

  // detector parameters
  DECLARE_NAMESPACE_TOOL( Rich, DetParameters );

  // particle properties tool
  DECLARE_NAMESPACE_TOOL( Rich, ParticleProperties );

  // radiator tool
  DECLARE_NAMESPACE_TOOL( Rich, RadiatorTool );

}
