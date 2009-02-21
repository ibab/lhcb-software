#ifndef Alignment_MisAlignAlg_H
#define Alignment_MisAlignAlg_H

//-----------------------------------------------------------------------------
/** @file MisAlignAlg.h
 *
 *  Header file for generating mis-alignments : MisAlignAlg
 *
 *  CVS Log :-
 *  $Id: MisAlignAlg.h,v 1.1 2009-02-21 22:21:17 jblouw Exp $
 *
 *  @author J. Blouw johan.blouw@cern.ch
 *  @date   18/07/2008
 */
//-----------------------------------------------------------------------------

// geometry
#include "DetDesc/IDetectorElement.h"

//from DetDesc
#include "DetDesc/DetectorElement.h"
#include "VeloDet/DeVelo.h"
#include "OTDet/DeOTDetector.h"
#include "STDet/DeSTDetector.h"
#include "STDet/DeITDetector.h"
#include "STDet/DeTTDetector.h"
#include "MuonDet/DeMuonDetector.h"

// GaudiAlg
#include "GaudiAlg/GaudiAlgorithm.h"

// local

#include "MisalignmentParser.h"

using namespace std;


class MisAlignAlg : public GaudiAlgorithm {
  
 public:
  
  
  MisAlignAlg( const std::string &name,
	       ISvcLocator* pSvcLocator);
  
  virtual ~MisAlignAlg();
  
  virtual StatusCode initialize();
  
  virtual StatusCode execute();
  
  virtual StatusCode finalize();
  
 private:

  StatusCode CacheDetElements( std::vector<std::string> & );
  void Config( IDetectorElement *);
  IDetectorElement *m_deit;
  IDetectorElement *m_dett;
  IDetectorElement *m_deot;
  IDetectorElement *m_develo;
  IDetectorElement *m_demuon;

  DeVelo *m_velo; 
  DeSTDetector *m_tt;
  DeSTDetector *m_it;
  DeOTDetector *m_ot;
  DeMuonDetector *m_muon;

  std::vector<std::string> m_detectors;
  std::string m_VeLofile;
  std::string m_TTfile;
  std::string m_OTfile;
  std::string m_ITfile;
  std::string m_MUONfile;

  bool tt_detector, ot_detector, it_detector, muon_detector, velo_detector;

  MisalignmentParser *parse;
  
};

#endif
