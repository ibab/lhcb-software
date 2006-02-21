// $Id: RichG4QwAnalysis.h,v 1.1 2006-02-21 17:05:27 seaso Exp $
#ifndef RICHANALYSIS_RICHG4QWANALYSIS_H 
#define RICHANALYSIS_RICHG4QWANALYSIS_H 1

// Include files

#include "GaudiKernel/NTuple.h"
#include "G4Track.hh" 

/** @class RichG4QwAnalysis RichG4QwAnalysis.h RichAnalysis/RichG4QwAnalysis.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2006-02-21
 */
class RichG4QwAnalysis {
public: 

  virtual ~RichG4QwAnalysis( ); ///< Destructor
  static RichG4QwAnalysis* getRichG4QwAnalysisInstance();
  void InitQwAnalysis();
  void FillQwAnalysisHisto(const G4Track& aChTrack);
  
  bool qwAnalysisNtupleBooked() {return m_qwAnalysisNtupleBooked;}

protected:

private:
  /// Standard constructor kept private.
  RichG4QwAnalysis( ); 
  static RichG4QwAnalysis* RichG4QwAnalysisInstance;

  INTuple *  m_qNtuple;
  int  m_qPartMax; // max number of particles for the ntuple.
  
  NTuple::Item <long> m_qPart; // number of tracks used 
  
  NTuple::Array <float> m_ChProdX;
  NTuple::Array <float> m_ChProdY;
  NTuple::Array <float> m_ChProdZ;
  NTuple::Array <float> m_RDetNum;
  NTuple::Array <float> m_ChPartType;
  NTuple::Array <float> m_ChProcType;
  NTuple::Array <float> m_ChTotEner;
  NTuple::Array <float> m_RadNum; 
  bool  m_qwAnalysisNtupleBooked; 
};
#endif // RICHANALYSIS_RICHG4QWANALYSIS_H
