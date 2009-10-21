// $Id: TrackNNGhostId.h,v 1.2 2009-10-21 07:14:16 albrecht Exp $
#ifndef TRACKNNGHOSTID_H 
#define TRACKNNGHOSTID_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "OTDAQ/IOTRawBankDecoder.h"

#include "TrackInterfaces/ITrackManipulator.h"            // Interface


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


protected:

private:
	IOTRawBankDecoder* m_otdecoder;
  IClassifierReader* m_classReader;
  std::vector<double>* m_inputVec;
  std::vector<std::string> m_inputVars;
};
#endif // TRACKNNGHOSTID_H
