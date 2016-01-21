#ifndef Run2GhostId_H
#define Run2GhostId_H 1

// Include files
#include "TrackInterfaces/IHitExpectation.h"
//#include "TrackInterfaces/IVPExpectation.h"
#include "TrackInterfaces/IVeloExpectation.h"
#include "OTDAQ/IOTRawBankDecoder.h"
#include "TrackInterfaces/IGhostProbability.h"
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "Event/Track.h"



class IClassifierReader;
namespace Rich{
  class TabulatedFunction1D;
}


/** @class Run2GhostId Run2GhostId.h
 *
 *  @author Paul Seyfert
 *  @date   06-02-2015
 */
class Run2GhostId : public GaudiTool,
                       virtual public IGhostProbability {

typedef std::vector<std::string> (Run2GhostId::* varNameMethod)() const;

public:
  /// Standard constructor
  Run2GhostId( const std::string& type,
                  const std::string& name,
                  const IInterface* parent);



  virtual ~Run2GhostId(){}; ///< Destructor

  virtual StatusCode finalize(); ///< Algorithm initialization
  virtual StatusCode initialize(); ///< Algorithm initialization
  virtual StatusCode execute(LHCb::Track& aTrack) const; 
  virtual std::vector<float> netInputs(LHCb::Track& aTrack) const;  /// TODO inline
  virtual std::vector<std::string> variableNames(LHCb::Track::Types type) const;
  virtual StatusCode beginEvent() {return countHits();}; 

protected:
  StatusCode countHits();

private:
  IOTRawBankDecoder* m_otdecoder;
  IVeloExpectation   *m_veloExpectation;
  std::vector<IClassifierReader*> m_readers;
  std::vector<Rich::TabulatedFunction1D*> m_flatters;

  std::vector<varNameMethod> m_varNameMethods;

  std::vector<IHitExpectation*> m_Expectations;
  std::vector<std::string> m_inNames;
  std::vector<std::string> m_expectorNames;
  std::vector<int>* m_expectedHits;
  int* m_vectorsizes;

  bool m_DaVinci;

  int m_veloHits;
  int m_ttHits;
  //int m_utHits;
  //int m_ftHits;
  int m_itHits;
  int m_otHits;

  std::vector<std::string> veloVars() const ;
  std::vector<std::string> upstreamVars() const ;
  std::vector<std::string> downstreamVars() const ;
  std::vector<std::string> longVars() const ;
  std::vector<std::string> ttrackVars() const ;

};

#endif // Run2GhostId_H
