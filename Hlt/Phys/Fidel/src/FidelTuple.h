// $Id: FidelTuple.h,v 1.1 2008-05-13 14:30:22 sfurcas Exp $
#ifndef FIDELTUPLE_H 
#define FIDELTUPLE_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"
#include "Kernel/Particle2MCLinker.h"

class IRelatedPV;

/** @class FidelTuple FidelTuple.h
 *  
 *
 *  @author Sara Furcas
 *  @date   2008-03-26
 */
class FidelTuple : public DVAlgorithm {
public: 
  /// Standard constructor
  FidelTuple( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FidelTuple( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization


protected:

private:
 Particle2MCLinker*   m_pLinker;
IRelatedPV* m_relatedPV;

};
#endif // FIDELTUPLE_H
