// $Id: TrackNNGhostId.h,v 1.3 2009-10-23 12:20:26 albrecht Exp $
#ifndef TRACKNNGHOSTID_H
#define TRACKNNGHOSTID_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "OTDAQ/IOTRawBankDecoder.h"

#include "TrackInterfaces/ITrackManipulator.h"     

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
                       virtual public ITrackManipulator {
public:

  /// Standard constructor
  TrackNNGhostId( const std::string& type,
                  const std::string& name,
                  const IInterface* parent);

  virtual ~TrackNNGhostId( ); ///< Destructor

  StatusCode initialize();

  StatusCode finalize();

  StatusCode execute(LHCb::Track& aTrack) const;

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

  bool m_tuningMC12;

  const Rich::TabulatedFunction1D* m_FlattenLookupTableLongtrack;
  const Rich::TabulatedFunction1D* m_FlattenLookupTableVelo;
  const Rich::TabulatedFunction1D* m_FlattenLookupTableUpstream;
  const Rich::TabulatedFunction1D* m_FlattenLookupTableDownstream;
  const Rich::TabulatedFunction1D* m_FlattenLookupTableTtrack;

};
#endif // TRACKNNGHOSTID_H
