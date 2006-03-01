// $Id: VeloDigiMoni.h,v 1.2 2006-03-01 13:48:55 szumlat Exp $
#ifndef VELODIGIMONI_H 
#define VELODIGIMONI_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "Event/VeloDigit.h"
#include "Linker/LinkerTool.h"

/** @class VeloDigiMoni VeloDigiMoni.h
 *  
 *
 *  @author Tomasz Szumlak
 *  @date   2005-11-18
 */

class VeloDigit;

class VeloDigiMoni : public GaudiTupleAlg {
public: 

  /// Standard constructor
  VeloDigiMoni( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~VeloDigiMoni( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  //
  StatusCode getData();
  StatusCode testVeloDigit();

protected:

  typedef LinkerTool<LHCb::VeloDigit, LHCb::MCHit> asctTool;
  typedef asctTool::DirectType Table;
  typedef Table::Range Range;
  typedef Table::iterator iterator;
  
private:

  std::string m_digitContainer;
  bool m_printInfo;
  bool m_testDigits;
  LHCb::VeloDigits* m_veloDigits;
  double m_numberOfEvents;
  double m_nVeloDigits;
  double m_nVeloDigits2;
  
};
#endif // VELODIGIMONI_H
