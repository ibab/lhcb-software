// $Id: CheatedSelection.h,v 1.3 2004-05-03 13:54:10 pkoppenb Exp $
#ifndef USER_CheatedSelection_H 
#define USER_CheatedSelection_H 1

// from STL
#include <fstream>
#include <string>
#include <math.h>
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IToolSvc.h"
#include "AIDA/IHistogram1D.h"
#include "DaVinciMCTools/IDebugTool.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/GenericAddress.h"
#include "DaVinciTools/DVAlgorithm.h"
// from Event 
#include "Event/EventHeader.h"
#include "Event/GenMCLink.h"
#include "Event/Vertex.h"
#include "Event/Particle.h"
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Vector/LorentzVector.h"

// from Associators
#include "DaVinciAssociators/Particle2MCLinksAsct.h"

/** @class CheatedSelection CheatedSelection.h 
 *  
 *  @author Marco Musy
 *  @date   21/01/2004
 */

typedef std::vector<MCParticle*> MCParticleVector;

// Forward declarations
class DataObject;
class IDebugTool;
class IParticlePropertySvc;

class CheatedSelection : public DVAlgorithm {

  const MCParticle* originof( const MCParticle* ) ;
  void SignalTree( MCParticleVector& , ParticleVector& );

public: 
  /// Standard constructor
  CheatedSelection( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CheatedSelection( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:
 
  long m_BID;
  double m_BHEPm;
  IDebugTool* m_debug;
  Particle2MCLinksAsct::IAsct* m_pAsctLinks;

  //properties
  double m_BMassWindow;

};
//===========================================================================//
#endif // USER_CheatedSelection_H
