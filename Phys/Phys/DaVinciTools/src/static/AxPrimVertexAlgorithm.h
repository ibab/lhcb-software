// $Id: AxPrimVertexAlgorithm.h,v 1.1.1.1 2001-07-09 09:28:42 gcorti Exp $
#ifndef DAVINCITOOLS_AXPRIMVTXALGORITHM_H
#define DAVINCITOOLS_AXPRIMVTXALGORITHM_H 1

// Include files
#include "GaudiKernel/Algorithm.h"


/** @class AxPrimVertexAlgorithm AxPrimVertexAlgorithm.h DaVinciTools/AxPrimVertexAlgorithm.h
 *  
 * Find an offline primary vertex. Then store it in VtxCandidate class
 * under "/Event/Phys/PrimaryVertices"
 * Internally it uses the fortran routines of the axreclib
 *  @author Gloria Corti
 *  @date   20/05/2001
 */


class AxPrimVertexAlgorithm  : public Algorithm {

public:
  /// Standard constructor
  AxPrimVertexAlgorithm(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~AxPrimVertexAlgorithm(); ///< Standard destructor  

  StatusCode initialize();    ///< Algorithm initialization 
  StatusCode execute   ();    ///< Algorithm execution
  StatusCode finalize  ();    ///< Algorithm finalization

};


#endif    // DAVINCITOOLS_AXPRIMVTXALGORITHM_H
