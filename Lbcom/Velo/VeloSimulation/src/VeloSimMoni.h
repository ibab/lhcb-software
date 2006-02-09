// $Id: VeloSimMoni.h,v 1.1 2006-02-09 11:04:16 szumlat Exp $
#ifndef VELOSIMMONI_H 
#define VELOSIMMONI_H 1

// Include files
#include "Event/MCVeloFE.h"
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "VeloDet/DeVelo.h"

/** @class VeloSimMoni VeloSimMoni.h
 *  
 *
 *  @author Tomasz Szumlak & Chris Parkes
 *  @date   2005-11-03
 */

class MCVeloFE;
class IMCVeloFEType;

class VeloSimMoni : public GaudiTupleAlg {
public: 
  /// Standard constructor
  VeloSimMoni( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~VeloSimMoni( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  virtual StatusCode getData();
  virtual StatusCode VeloFEMonitor();

private:

  bool m_printInfo;
  bool m_detailedMonitor;
  LHCb::MCVeloFEs* m_mcVFEsCont;
  bool m_veloFEMoni;
  DeVelo* m_veloDet;
  IMCVeloFEType* m_feTypeTool;
  long m_nMCVeloFE;
  double m_nMCVeloFE2;
  long m_nMCVeloFEs;
  long m_nMCVeloFEn;
  long m_nMCVeloFEo;
  long m_NumberOfEvents;

};
#endif // VELOSIMMONI_H
