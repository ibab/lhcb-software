// $Id: CheatedSelection.h,v 1.6 2005-01-31 10:07:46 pkoppenb Exp $
#ifndef USER_CheatedSelection_H 
#define USER_CheatedSelection_H 1

// from STL
#include <fstream>
#include <string>
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "DaVinciMCTools/IDebugTool.h"
#include "Kernel/DVAlgorithm.h"
// from Event 
#include "Event/EventHeader.h"
#include "Event/GenMCLink.h"
#include "Event/SelResult.h"

// from Associators
#include "DaVinciAssociators/Particle2MCLinksAsct.h"

/** @class CheatedSelection CheatedSelection.h 
 *  
 *  @author Marco Musy
 *  @date   16/11/2004
 */

typedef std::vector<MCParticle*> MCParticleVector;

// Forward declarations
class DataObject;
class IDebugTool;
class IParticlePropertySvc;

class CheatedSelection : public DVAlgorithm {

  const MCParticle* originof( const MCParticle* ) ;
  void SignalTree( const MCParticle*, MCParticleVector& , ParticleVector& );

public: 
  /// Standard constructor
  CheatedSelection( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CheatedSelection( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:
  double m_BHEPm;
  IDebugTool* m_debug;
  Particle2MCLinksAsct::IAsct* m_pAsctLinks;

  //properties
  double m_BMassWindow;
};
//===========================================================================//
#endif // USER_CheatedSelection_H
