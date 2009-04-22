// $Id: PatSeedFit.h,v 1.2 2009-04-22 13:09:21 smenzeme Exp $
#ifndef INCLUDE_PATSEEDFIT_H
#define INCLUDE_PATSEEDFIT_H 1

#include "GaudiAlg/GaudiTool.h"
#include "TrackInterfaces/IPatSeedFit.h"


static const InterfaceID IID_PatSeedFit("PatSeedFit", 1, 0);

// forward declarations
class PatSeedTool;

/** @class PatSeedFit PatSeedFit.h
 *  
 * provide a convenient interface to the internal fit used in the PatSeeding
 * algorithm in the pattern reco
 *
 * @author Manuel Tobias Schiller <schiller@physi.uni-heidelberg.de>
 * @date   2009-01-28
 */
class PatSeedFit : public GaudiTool, virtual public IPatSeedFit {

public:

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_PatSeedFit; }

  /// Standard Constructor
  PatSeedFit(const std::string& type,
             const std::string& name,
             const IInterface* parent);

  virtual ~PatSeedFit(); ///< Destructor

  virtual StatusCode initialize(); ///< Tool initialization
  virtual StatusCode   finalize(); ///< Tool finalize


  virtual StatusCode fitSeed( const std::vector<LHCb::LHCbID> lhcbIDs, 
			      std::vector<LHCb::State> *states) const;
  /// fit a PatSeedTrack
  virtual StatusCode fitTrack( PatSeedTrack& track, double maxChi2,
		  unsigned minPlanes, bool xOnly, bool forceDebug ) const; 

private:
  PatSeedTool* m_patSeedTool;

  DeSTDetector* m_itDet ;
  const DeOTDetector* m_otDet;
  const IPatSeedFit* m_seedTool ;
  const IOTRawBankDecoder* m_otdecoder ;
  const ITrackMomentumEstimate *m_momentumTool ;
  ITrackFitter* m_veloFitter ;
  IPatSeedFit* m_seedFitter;

  double m_stateErrorX2;
  double m_stateErrorY2;
  double m_stateErrorTX2;
  double m_stateErrorTY2;
  double m_dRatio;
  double m_initialArrow;
  double m_maxChi2;
  std::map<LHCb::STChannelID, const LHCb::STLiteCluster*> m_clusterMap ;

};
#endif // INCLUDE_PATSEEDFIT_H

