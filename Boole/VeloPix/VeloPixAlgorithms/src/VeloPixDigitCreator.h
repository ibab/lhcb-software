#// $Id: VeloPixDigitCreator.h,v 1.1.1.1 2009-12-04 14:16:52 marcin Exp $
#ifndef VELOPIXDIGITCREATOR_H
#define VELOPIXDIGITCREATOR_H 1
// Include files
// from STL
#include <string>
// from Gaudi
// #include "GaudiAlg/GaudiAlgorithm.h"  // for an ordinary algorithm
#include "GaudiAlg/GaudiTupleAlg.h"    // for algorithm with histograms (setHistoTopDir, plot, plot2D)
// Event
#include "LHCbMath/LHCbMath.h"

/** @class VeloPixDigitCreator.h 
 *  VeloPixAlgorithms/VeloPixDigitCreator.h
 *
 *  @author Marcin Kucharczyk
 *  @date   2009/10/20
 */

// class VeloPixDigitCreator : public GaudiAlgorithm {  // for an ordinary algorithm
class VeloPixDigitCreator : public GaudiTupleAlg {    // for algorithm with histograms (setHistoTopDir, plot, plot2D)
public:
  /// Standard constructor
  VeloPixDigitCreator(const std::string& name,ISvcLocator* pSvcLocator);
  virtual ~VeloPixDigitCreator();     ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalise

protected:

private:

  void createDigits(const LHCb::MCVeloPixDigits* digitsMC, 
                    LHCb::VeloPixDigits* digitsCont);
  int convertToTDC(double charge);
  std::string m_inputLocation;
  std::string m_outputLocation;
  std::vector<std::string> m_sampleNames;
  std::vector<std::string> m_spillNames;
  std::vector<std::string> m_spillPaths;
  std::vector<std::string> m_outPaths;
  bool m_isDebug; 
  bool m_isVerbose;
};
#endif // VeloPixDigitCreator_H
