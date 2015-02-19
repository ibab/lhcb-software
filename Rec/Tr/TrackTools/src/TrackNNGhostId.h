#ifndef TRACKNNGHOSTID_H
#define TRACKNNGHOSTID_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "OTDAQ/IOTRawBankDecoder.h"

#include "TrackInterfaces/IGhostProbability.h"     
#include "GaudiKernel/IIncidentListener.h" 

// For interpolator
#include "RichDet/Rich1DTabFunc.h"

class IHitExpectation;
class IVeloExpectation;


/** @class TrackNNGhostId TrackNNGhostId.h
 *
 *
 *  @author Johannes Albrecht
 *  @date   2009-10-06
 *
 *  Tool to calculate a track quality variable
 *  using a trained (TMVA) netork
 *
 */
class TrackNNGhostId : public GaudiTool,
                       virtual public IGhostProbability,
                       virtual public IIncidentListener {
public:

  /// Standard constructor
  TrackNNGhostId( const std::string& type,
                  const std::string& name,
                  const IInterface* parent);

  virtual ~TrackNNGhostId( ); ///< Destructor

  StatusCode initialize();

  StatusCode finalize();

  StatusCode execute(LHCb::Track& aTrack) const;

  StatusCode beginEvent() { return StatusCode::SUCCESS; }

  virtual void handle( const Incident& incident ); 

  /** reveal the variable names for a track type */
  virtual std::vector<std::string> variableNames(LHCb::Track::Types type) const {
    std::vector<std::string> vec;
    if (LHCb::Track::Velo==type) variableNames_Velo(vec);
    if (LHCb::Track::Long==type) variableNames_Longtrack(vec);
    if (LHCb::Track::Upstream==type) variableNames_Upstream(vec);
    if (LHCb::Track::Downstream==type) variableNames_Downstream(vec);
    if (LHCb::Track::Ttrack==type) variableNames_Ttrack(vec);
    return vec;
  }

  /** reveal the variable values for a track */
  virtual std::vector<float> netInputs(LHCb::Track& ) const {
    fatal() << "TrackNNGhostId::netInputs is NOT IMPLEMENTED" << endmsg;
    return std::vector<float>();
  }


private:

  IOTRawBankDecoder* m_otdecoder;
  std::vector<std::string> m_inNames;
  std::vector<double>* m_inputVec;

  IClassifierReader* m_readerVelo;
  IClassifierReader* m_readerUpstream;
  IClassifierReader* m_readerDownstream;
  IClassifierReader* m_readerTtrack;
  IClassifierReader* m_readerLong;

  IHitExpectation* m_ttExpectation;
  IHitExpectation* m_itExpectation;
  IHitExpectation* m_otExpectation;
  IVeloExpectation* m_veloExpectation;

  const Rich::TabulatedFunction1D* m_FlattenLookupTableLongtrack;
  const Rich::TabulatedFunction1D* m_FlattenLookupTableVelo;
  const Rich::TabulatedFunction1D* m_FlattenLookupTableUpstream;
  const Rich::TabulatedFunction1D* m_FlattenLookupTableDownstream;
  const Rich::TabulatedFunction1D* m_FlattenLookupTableTtrack;

  int m_nOTCont;
  bool m_configured;

  bool cutoff(double& unregularized) const;
  void variableNames_Longtrack(std::vector<std::string>& inNames) const;
  void variableNames_Velo(std::vector<std::string>& inNames) const;
  void variableNames_Upstream(std::vector<std::string>& inNames) const;
  void variableNames_Downstream(std::vector<std::string>& inNames) const;
  void variableNames_Ttrack(std::vector<std::string>& inNames) const;

  void debug_NN_input(const LHCb::Track::Types type) const;

  void initEvent(); 
};
#endif // TRACKNNGHOSTID_H
