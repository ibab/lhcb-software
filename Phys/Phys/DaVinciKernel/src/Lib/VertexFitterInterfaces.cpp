// $Id: VertexFitterInterfaces.cpp,v 1.3 2007-02-06 09:57:56 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.3 $
// ============================================================================
// 
// ============================================================================
// include files 
// ============================================================================
#include "Kernel/IParticleCombiner.h"
#include "Kernel/IParticleReFitter.h"
#include "Kernel/IVertexFit.h"
#include "Kernel/IMassVertexFit.h"
#include "Kernel/IDirectionFit.h"
// ============================================================================

// ============================================================================
/** @file 
 *
 *  impleemnattion of few mandatory methofd for interfaces 
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
  const InterfaceID IID_IVertexFitter     ( "IVertexFit"     , 1 , 0 ) ;
  const InterfaceID IID_IMassVertexFitter ( "IMassVertexFit" , 1 , 0 ) ;
  const InterfaceID IID_IDirectionFitter  ( "IDirectionFit"  , 1 , 0 ) ; 
};

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

// ============================================================================
// The END 
// ============================================================================


