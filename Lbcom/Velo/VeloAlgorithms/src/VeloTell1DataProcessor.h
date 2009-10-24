// $Id: VeloTell1DataProcessor.h,v 1.2 2009-10-24 13:48:23 szumlat Exp $
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

  enum sensors{
    PHI_SENSOR=1,
    R_SENSOR=2,
    PILE_UP=3
  };

  typedef LHCb::MCVeloFEs::const_iterator feConstIt;
  typedef LHCb::MCVeloFEs::iterator feIt;
  typedef std::vector<unsigned int> TOPO_CONT;
  typedef std::vector<unsigned int>::const_iterator TOPO_CONT_IT;
  typedef std::pair<unsigned int, unsigned int> BOUNDARIES;
  
  /// Standard constructor
  VeloTell1DataProcessor(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~VeloTell1DataProcessor( ); ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  virtual void orderStripsTopo(const VeloTELL1::sdataVec& inData,
                               const bool type);
  virtual void checkStripsOrdering(const unsigned int sens);
  virtual BOUNDARIES findBoundary(const unsigned int proc,
                                         const TOPO_CONT& cont);
  StatusCode getData();
  StatusCode writeDataForSensors();
  void writeOutput();

private:

  std::string m_fesContainer;        /// input simulated Front End objects
  LHCb::MCVeloFEs* m_fes;            /// container with FE objects
  std::string m_simADCs;             /// container's name for sim NZS data
  LHCb::VeloTELL1Datas* m_adcs;      /// data container
  VeloTELL1::sdataVec m_adcsCharge;  /// container with total sim charge
  VeloTELL1::sdataVec m_buffer;      /// memory buffer with reordered strips
  double m_adcScale;                 /// 10 bit bandwidth adc count scale
  double m_elScale;                  /// electron equiv. of the adc scale max
  bool m_isDebug;                    /// debug flag
  VeloTELL1::sdataVec m_innerBuffer; /// mem buffer for inner strips
  VeloTELL1::sdataVec m_outerBuffer; /// mem buffer for outer strips
  TOPO_CONT m_innerStrips;           /// number of inner str per FPGA
  TOPO_CONT m_outerStrips;           /// number of outer str per FPGA
  TOPO_CONT m_innerDummy;            /// inner dummy per FPGA
  TOPO_CONT m_outerDummy;            /// outer dummy per FPGA

};
#endif // VELOTELL1DATAPROCESSOR_H
