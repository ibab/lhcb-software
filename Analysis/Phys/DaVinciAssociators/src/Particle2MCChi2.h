// $Id: Particle2MCChi2.h,v 1.7 2006-06-23 14:54:39 phicharp Exp $
#ifndef Particle2MCChi2_H 
#define Particle2MCChi2_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IToolSvc.h"

// local
#include "DaVinciAssociators/Particle2MCLinker.h"
#include "AsctAlgorithm.h"

/** @class Particle2MCChi2 Particle2MCChi2.h
 *  
 *
 *  @author Philippe Charpentier
 *  @date   17/05/2002
 */
class Particle2MCChi2 : public AsctAlgorithm {
public:
  /// Standard constructor
  Particle2MCChi2( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~Particle2MCChi2( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  double                m_chi2;         ///< Chi2 maximum
  Object2MCLinker*        m_p2MCLink;     ///< Pointer to a P2MCPLink object
};
#endif // Particle2MCChi2_H
