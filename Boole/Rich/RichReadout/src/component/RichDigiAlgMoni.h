// $Id: RichDigiAlgMoni.h,v 1.8 2006-02-22 19:29:30 jonrob Exp $
#ifndef RICHMONITOR_RICHDIGIALGMONI_H
#define RICHMONITOR_RICHDIGIALGMONI_H 1

// STD
#include <sstream>

// base class
#include "RichKernel/RichHistoAlgBase.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

// Event model
#include "Event/MCRichDigit.h"
#include "Event/MCRichDeposit.h"
#include "Event/MCRichSummedDeposit.h"
#include "Event/MCRichHit.h"
#include "Kernel/RichParticleIDType.h"

// Kernel
#include "RichKernel/RichMap.h"
#include "RichKernel/RichHashMap.h"
#include "Kernel/RichDetectorType.h"
#include "Kernel/RichRadiatorType.h"
#include "Kernel/RichParticleIDType.h"
#include "Kernel/Vector3DTypes.h"

// GSL
#include "gsl/gsl_math.h"

// interfaces
#include "RichKernel/IRichSmartIDTool.h"
#include "RichKernel/IRichMCTruthTool.h"

// boost
#include "boost/lexical_cast.hpp"

// temporary histogramming numbers
#include "RichDetParams.h"

// namespaces
using namespace LHCb; ///< General LHCb namespace

/** @class RichDigiAlgMoni RichDigiAlgMoni.h RichDigiQC/RichDigiAlgMoni.h
 *
 *  Monitor for Rich Digitisation algorithm performance
 *
 *  @author Chris Jones   (Christopher.Rob.Jones@cern.ch)
 *  @date   2003-09-08
 */

class RichDigiAlgMoni : public RichHistoAlgBase 
{

public:

  /// Standard constructor
  RichDigiAlgMoni( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RichDigiAlgMoni( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private: // definitions

  /// key for HPD+MCParticle hits
  typedef std::pair<const LHCb::RichSmartID,const LHCb::MCParticle*>   PartKey;
  /// mapping given hits for each HPD+MCParticle Pair
  typedef std::map< PartKey, std::vector<const LHCb::MCRichDeposit*> > PartMap;

private: // methods

  // Map to count cherenkov photons for each radiator
  typedef std::pair<const MCParticle*,Rich::RadiatorType> PhotPair;
  typedef Rich::Map< PhotPair, int >                      PhotMap;

  // PD occupancies
  typedef Rich::Map<RichSmartID,int> PDMulti;

  /// Particle mass
  double mass( const MCParticle * mcPart );

  /// Returns beta for a given MCParticle
  double mcBeta( const MCParticle * mcPart );

  /// Returns the momentum for a given MCParticle
  double momentum( const MCParticle * mcPart );

  /// Count the number of photo electrons
  void countNPE( PhotMap & photMap,
                 const MCRichHit * hit );

  /// IS this a true cherenkov signal hit ?
  bool trueCKHit(  const MCRichHit * hit );

  /// Fill histograms for each HPD silicon wafer background plots
  void fillHPDPlots( const PartMap & pmap, 
                     const std::string & plotsDir,
                     const std::string & plotsName );

private: // data

  /// Pointer to RichSmartID tool
  const IRichSmartIDTool * m_smartIDTool;

  /// Pointer to MC truth tool
  const IRichMCTruthTool * m_mcTool;

  // job options
  std::string m_mcdigitTES;     ///< Location of MCRichDigits in TES
  std::string m_mcdepTES;       ///< Location of MCRichDeposits in TES
  std::string m_mchitTES;       ///< Location of MCRichHits in TES

  /// Particle masses
  Rich::Map<Rich::ParticleIDType,double> m_particleMass;

  // histo ID
  unsigned int m_ID;

  // max number of Quartz CK HPD event histos to make
  unsigned int m_maxID;

};

inline double RichDigiAlgMoni::mass( const MCParticle * mcPart )
{
  return m_particleMass[ m_mcTool->mcParticleType(mcPart) ];
}

inline double RichDigiAlgMoni::momentum( const MCParticle * mcPart )
{
  return ( mcPart ? mcPart->momentum().P() : 0 );
}

inline double RichDigiAlgMoni::mcBeta( const MCParticle * mcPart )
{
  if ( !mcPart ) return 0;
  const double pTot = momentum( mcPart );
  const double Esquare = pTot*pTot + gsl_pow_2(mass(mcPart));
  return ( Esquare > 0 ? pTot/sqrt(Esquare) : 0 );
}

inline bool RichDigiAlgMoni::trueCKHit(  const MCRichHit * hit ) 
{

  // Which radiator
  const Rich::RadiatorType rad = (Rich::RadiatorType)( hit->radiator() );

  // Is this a true hit
  return ( !hit->scatteredPhoton() &&
           !hit->chargedTrack()
           && ( rad == Rich::Aerogel ||
                rad == Rich::Rich1Gas ||
                rad == Rich::Rich2Gas ) );
}

#endif // RICHMONITOR_RICHDIGIALGMONI_H
