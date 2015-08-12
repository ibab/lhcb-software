// $Id: $
#ifndef MUONINFORMATION_H 
#define MUONINFORMATION_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
//#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/GaudiAlgorithm.h"

#include "IMuonInformation.h"            // Interface

//#include "RichParticleSearchMain.h"
#include "Kernel/RichRadiatorType.h"
#include "Event/RichRecSegment.h"
// base class
#include "RichRecBase/RichRecTupleAlgBase.h"



#include "Kernel/RichRadiatorType.h"

// Event
#include "Event/RichRecStatus.h"
#include "Event/MCRichOpticalPhoton.h"
#include "Event/RichRecTrack.h"
#include "Event/Track.h"
#include "Event/Particle.h"

// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IProfile1D.h"

// Interfaces
#include "MCInterfaces/IRichRecMCTruthTool.h"
#include "RichKernel/IRichParticleProperties.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichTrackSelector.h"
#include "RichRecBase/IRichIsolatedTrack.h"
#include "RichKernel/IRichRefractiveIndex.h"
#include "RichRecBase/IRichRecGeomTool.h"
#include "CaloUtils/ICaloElectron.h"

// RichDet
#include "RichDet/DeRichSphMirror.h"
#include "RichDet/DeRichHPD.h"
#include "RichDet/DeRichLocations.h"
#include "RichDet/DeRich.h"

// Kernel
#include "RichKernel/BoostArray.h"
#include "RichKernel/RichSmartIDCnv.h"
#include "RichKernel/IRichSmartIDTool.h"
//#include <RichKernel/IRichRayTracing.h>

// boost
#include "boost/lexical_cast.hpp"
//#include <boost/foreach.hpp>

/** @class MuonInformation MuonInformation.h
 *  
 *
 *  @author Matt Coombes
 *  @date   2010-10-04
 */

using namespace LHCb;
class MuonInformation : public GaudiTool, virtual public IMuonInformation {
public: 
  /// Standard constructor
  MuonInformation( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  virtual ~MuonInformation( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization

  int HasMuonInformation(const LHCb::Track*);
  

protected:

private:
  ICaloElectron* m_caloElectron;

};
#endif // MUONINFORMATION_H
