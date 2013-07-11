// $Id: $
#ifndef _OldTrackSmearState_H_
#define _OldTrackSmearState_H_

/** @class TrackSmearState TrackSmearState.h
 *
 * Smear the state
 *
 *  @author M.Needham
 *  @date   30/05/2006
 */

#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IRndmGen.h"
#include "GaudiAlg/GaudiAlgorithm.h"

#include <string>
#include <vector>

class TGraphErrors;


class OldTrackSmearState: public GaudiAlgorithm {

public:

  // Constructors and destructor
  OldTrackSmearState(const std::string& name,
              ISvcLocator* pSvcLocator);
  virtual ~OldTrackSmearState();

  virtual StatusCode initialize();

  virtual StatusCode execute();

private:


  std::string m_inputRootFile;
  std::string m_inputLocation;
  std::string m_year;


  double m_smearAngle;  
  double m_fudgeFactor;
  TGraphErrors* m_graph;

 SmartIF<IRndmGen> m_gaussDist; 


};

#endif
