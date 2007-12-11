// $Id: CopyPrimaryVertices.h,v 1.5 2007-12-11 17:37:12 jpalac Exp $
#ifndef COPYPRIMARYVERTICES_H 
#define COPYPRIMARYVERTICES_H 1

// Include files
// from Gaudi
#include "MicroDST/MicroDSTAlgorithm.h"
#include "Event/RecVertex.h"

/** @class CopyPrimaryVertices CopyPrimaryVertices.h
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
