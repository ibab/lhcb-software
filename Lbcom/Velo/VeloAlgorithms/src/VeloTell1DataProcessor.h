// $Id: VeloTell1DataProcessor.h,v 1.1 2009-08-10 08:48:02 szumlat Exp $
#ifndef VELOTELL1DATAPROCESSOR_H 
#define VELOTELL1DATAPROCESSOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// form TELL1
#include "VeloEvent/VeloTELL1Data.h"
#include "Tell1Kernel/VeloDecodeCore.h"
#include "Tell1Kernel/VeloDecodeConf.h"

// from LHCb event
#include "Event/MCVeloFE.h"

/** @class VeloTell1DataProcessor VeloTell1DataProcessor.h
 *  
 * the class implements emulator of the FADC cards on
 * TELL1 read-out board, input stream consists of container
 * with MCVeloFEs objects, output stream has both digitized
 * ADCs for each read-out sensor as well as digitized
 * pedestal values
 *
 *  @author Tomasz Szumlak
 *  @date   2009-07-30
 */

class VeloTell1DataProcessor : public GaudiAlgorithm {
public:

  typedef LHCb::MCVeloFEs::const_iterator feConstIt;
  typedef LHCb::MCVeloFEs::iterator feIt;
  /// Standard constructor
  VeloTell1DataProcessor(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~VeloTell1DataProcessor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  StatusCode getData();
  StatusCode writeDataForSensors();
  void writeOutput();

private:

  std::string m_fesContainer;        /// input simulated Front End objects
  LHCb::MCVeloFEs* m_fes;            /// container with FE objects
  std::string m_simADCs;             /// container's name for simulated NZS data
  LHCb::VeloTELL1Datas* m_adcs;            /// data container
  VeloTELL1::sdataVec m_adcsCharge;  /// container with total simulated charge
  double m_adcScale;                 /// 10 bit bandwidth adc count scale
  double m_elScale;                  /// electron equivalent of the adc scale max

};
#endif // VELOTELL1DATAPROCESSOR_H
