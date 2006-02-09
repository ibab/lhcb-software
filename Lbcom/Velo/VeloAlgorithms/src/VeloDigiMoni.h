// $Id: VeloDigiMoni.h,v 1.1 2006-02-09 12:20:41 szumlat Exp $
#ifndef VELODIGIMONI_H 
#define VELODIGIMONI_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "Event/VeloDigit.h"

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
