// $Id: TrackLikelihood.h,v 1.3 2009-08-06 18:19:10 smenzeme Exp $
#ifndef _TrackLikelihood_H
#define _TrackLikelihood_H

#include "TrackInterfaces/ITrackManipulator.h"
#include "GaudiAlg/GaudiTool.h"

/** @class TrackLikelihood TrackLikelihood.h
 *
 * Implementation of TrackLikelihood tool
 * see interface header for description
 *
 *  @author M.Needham
 *  @date   22/5/2007
 */

class IHitExpectation;
class IVeloExpectation;

namespace LHCb{
  class Track;
}

class TrackLikelihood: public GaudiTool,
                       virtual public ITrackManipulator  {

 public:

  /** constructer */
  TrackLikelihood(const std::string& type,
                  const std::string& name,
                  const IInterface* parent);  

  /** destructer */
  virtual ~TrackLikelihood();

  StatusCode initialize();

  StatusCode execute(LHCb::Track& aTrack) const;

 private:

  double binomialTerm(const unsigned int& found, 
                      const unsigned int& expected, 
                      const double& eff) const;

  double addVelo(LHCb::Track& aTrack) const;
  double addTT(LHCb::Track& aTrack) const;
  double addUT(LHCb::Track& aTrack) const;
  double addOT(LHCb::Track& aTrack) const;
  double addIT(LHCb::Track& aTrack) const;

  IVeloExpectation* m_veloExpectation;
  IHitExpectation* m_ttExpectation;
  IHitExpectation* m_utExpectation;
  IHitExpectation* m_itExpectation;
  IHitExpectation* m_otExpectation;
 

  double m_veloREff;
  double m_veloPhiEff;
  double m_otEff;
  double m_itEff;
  double m_ttEff;
  double m_utEff;
  double m_veloHighEff1;
  double m_veloHighEff2;
  double m_itHighEff;
  double m_ttHighEff;
  double m_utHighEff;

  bool m_useVelo;
  bool m_useOT;
  bool m_useIT;
  bool m_useTT;
  bool m_useUT;
  double m_chiWeight;

 
};

#endif
