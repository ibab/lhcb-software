// $Id: CopyPrimaryVertices.h,v 1.8 2008-09-01 17:12:57 jpalac Exp $
#ifndef COPYPRIMARYVERTICES_H 
#define COPYPRIMARYVERTICES_H 1

// Include files
// from MicroDST
#include "MicroDST/KeyedContainerClonerAlg.h"
#include <MicroDST/ICloneRecVertex.h>
#include "MicroDST/BindType2ClonerDef.h"
// from LHCb
#include <Event/RecVertex.h>
// local
#include "Defaults.h"
/** @class CopyPrimaryVertices CopyPrimaryVertices.h
 *  
 * MicroDSTAlgorithm to clone LHCb::RecVertex and related objects 
 * from one TES location to a parallel one.
 * It inherits the std::string properties InputLocation and OutputPrefix from
 * MicroDSTCommon. The LHCb::RecVertex are taken from the TES location 
 * defined by InputLocation, and are cloned and put in TES location 
 * "/Event" + OutputPrefix + InputLocation. 
 * If InputLocation already contains
 * a leading "/Event" it is removed. 
 * If InputLocation is not set, the RecVertices are taken from LHCb::RecVertexLocation::Primary.
 * The actual cloning of individual LHCb::RecVertex is performed by the 
 * ICloneRecVertex, the implementation of which is set by the property 
 * ClonerType, with default value "RecVertexCloner"
 * @see ICloneRecVertex
 * @see RecVertexCloner
 * 
 * <b>Example</b>: Clone RecVertices from "/Event/Rec/Vertex/V0" to 
 * "/Event/MyLocation/Rec/Vertex/V0" using a RecVertexCloner
 *  @code
 *
 *  // Add a CopyPrimaryVertices instance to a selection sequence
 *  SeqDC06selBd2Jpsi2MuMu_Kst2KPi.Members += {"MicroDST::KeyedContainerClonerAlg<LHCb::RecVertex>/CopyPrimaryVertices"};
 *  CopyPrimaryVertices.OutputPrefix = "MyLocation"; // Default.
 *  CopyParticles.InputLocation = "/Event/Rec/Vertex/V0";
 *  CopyParticles.ClonerType = "RecVertexCloner"

 *  @endcode
 * 
 *
 *  @author Juan PALACIOS juan.palacios@nikhef.nl
 *  @date   2007-10-15
 */
template <> struct BindType2Cloner<LHCb::RecVertex> 
{
  typedef LHCb::RecVertex type;
  typedef ICloneRecVertex cloner;
};
//=============================================================================
template<> struct Defaults<LHCb::RecVertex>
{
  const static std::string clonerType;
};
const std::string Defaults<LHCb::RecVertex>::clonerType = "RecVertexCloner";
//=============================================================================
template<> struct Location<LHCb::RecVertex>
{
  const static std::string Default;
};
const std::string Location<LHCb::RecVertex>::Default = LHCb::RecVertexLocation::Primary;
//=============================================================================
typedef MicroDST::KeyedContainerClonerAlg<LHCb::RecVertex> CopyPrimaryVertices;
DECLARE_ALGORITHM_FACTORY( CopyPrimaryVertices );
#endif // COPYPRIMARYVERTICES_H
