// $Id: $
#ifndef VBVERTEXREQUIREMENT_H 
#define VBVERTEXREQUIREMENT_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DaVinciAlgorithm.h"


/** @class VBVertexRequirement VBVertexRequirement.h
 *  Asks for same PV 
 *
 *  @author Albert Bursche
 *  @date   2012-01-27
 */
class VBVertexRequirement : public DaVinciAlgorithm {
public: 
  /// Standard constructor
  VBVertexRequirement( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~VBVertexRequirement( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

};
#endif // VBVERTEXREQUIREMENT_H
