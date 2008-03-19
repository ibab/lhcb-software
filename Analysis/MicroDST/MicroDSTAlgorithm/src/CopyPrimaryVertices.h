// $Id: CopyPrimaryVertices.h,v 1.6 2008-03-19 14:14:47 jpalac Exp $
#ifndef COPYPRIMARYVERTICES_H 
#define COPYPRIMARYVERTICES_H 1

// Include files
// from Gaudi
#include "MicroDST/MicroDSTAlgorithm.h"
// forward declarations
class ICloneRecVertex;

/** @class CopyPrimaryVertices CopyPrimaryVertices.h
 *  
 * Class to clone LHCb::RecVertex and related objects from one TES location
 * to a parallel one.
 * It inherits the std::string properties InputLocation and OutputPrefix from
 * MicroDSTCommon. The LHCb::RecVertex are taken from the TES location 
 * defined by InputLocation, and are cloned and put in TES location 
 * "/Event" + OutputPrefix + InputLocation. 
 * If InputLocation already contains
 * a leading "/Event" it is removed. 
 * If InputLocation is not set, the RecVertices are taken from LHCb::RecVertexLocation::Primary.
 * The actual cloning of individual LHCb::RecVertex is performed by the 
 * ICloneRecVertex, the implementation of which is set by the property 
 * ICloneRecVertex, with default value "RecVertexCloner"
 * @see ICloneRecVertex
 * @see RecVertexCloner
 * 
 * <b>Example</b>: Clone RecVertices from "/Event/Rec/Vertex/V0" to 
 * "/Event/MyLocation/Rec/Vertex/V0" using a RecVertexCloner
 *  @code
 *
 *  // Add a CopyPrimaryVertices instance to a selection sequence
 *  SeqDC06selBd2Jpsi2MuMu_Kst2KPi.Members += {"CopyPrimaryVertices"};
 *  CopyPrimaryVertices.OutputPrefix = "MyLocation"; // Default.
 *  CopyParticles.InputLocation = "/Event/Rec/Vertex/V0";
 *  CopyParticles.ICloneRecVertex = "RecVertexCloner"

 *  @endcode
 * 
 *
 *  @author Juan PALACIOS
 *  @date   2007-10-15
 */
class CopyPrimaryVertices : public MicroDSTAlgorithm {
public: 
  /// Standard constructor
  CopyPrimaryVertices( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CopyPrimaryVertices( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  typedef LHCb::RecVertex::Container Vertices;

private:

  ICloneRecVertex* m_vertexCloner;
  
  std::string m_vertexClonerName;

};
#endif // COPYPRIMARYVERTICES_H
