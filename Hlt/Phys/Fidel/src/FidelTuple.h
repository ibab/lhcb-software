// $Id: FidelTuple.h,v 1.2 2008-06-19 17:37:44 sfurcas Exp $
#ifndef FIDELTUPLE_H 
#define FIDELTUPLE_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"
#include "Kernel/Particle2MCLinker.h"
#include "Kernel/ITriggerTisTos.h"

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

  ITriggerTisTos* m_TriggerTisTosTool;

};
#endif // FIDELTUPLE_H
