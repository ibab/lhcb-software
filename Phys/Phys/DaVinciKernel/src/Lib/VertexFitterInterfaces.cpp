// $Id: VertexFitterInterfaces.cpp,v 1.4 2008-01-15 18:16:53 ibelyaev Exp $
// ============================================================================
// include files 
// ============================================================================
#include "Kernel/IParticleCombiner.h"
#include "Kernel/IParticleReFitter.h"
#include "Kernel/IVertexFit.h"
#include "Kernel/IMassVertexFit.h"
#include "Kernel/IDirectionFit.h"
#include "Kernel/IMassFit.h"
// ============================================================================
/** @file 
 *
 *  implementation of few mandatory methods for interfaces 
 *  (Available through *.cpp filed to ensure the valdity of 
 *  dynamic_cast success independently on the loading 
 *  options of the shared libraries)
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2004-12-19
 */ 
// ============================================================================
namespace // unnamed namespace - "static components"
{
  const InterfaceID IID_IParticleCombiner ( "IParticleCombiner" , 1 , 0 ) ;
  const InterfaceID IID_IParticleReFitter ( "IParticleReFitter" , 1 , 0 ) ;
  const InterfaceID IID_IVertexFitter     ( "IVertexFit"        , 1 , 0 ) ;
  const InterfaceID IID_IMassVertexFitter ( "IMassVertexFit"    , 1 , 0 ) ;
  const InterfaceID IID_IDirectionFitter  ( "IDirectionFit"     , 1 , 0 ) ; 
  const InterfaceID IID_IMassFitter       ( "IMassFit"          , 1 , 0 ) ; 
}
// ============================================================================
const InterfaceID& IParticleCombiner:: interfaceID ()
{ return IID_IParticleCombiner ; }
IParticleCombiner::~IParticleCombiner () {};
// ============================================================================
const InterfaceID& IParticleReFitter:: interfaceID () 
{ return IID_IParticleReFitter ; }
IParticleReFitter::~IParticleReFitter () {};
// ============================================================================
const InterfaceID& IVertexFit::     interfaceID () 
{ return IID_IVertexFitter ; }
IVertexFit::~IVertexFit         () {} ;
// ============================================================================
const InterfaceID& IMassVertexFit:: interfaceID ()
{ return IID_IMassVertexFitter ; }
IMassVertexFit::~IMassVertexFit () {} ;
// ============================================================================
const InterfaceID& IDirectionFit::  interfaceID ()
{ return IID_IDirectionFitter ; }
IDirectionFit::~IDirectionFit   () {} ;
// ============================================================================
const InterfaceID& IMassFit::  interfaceID ()
{ return IID_IMassFitter ; }
IMassFit::~IMassFit   () {} ;
// ============================================================================

// ============================================================================
// The END 
// ============================================================================


