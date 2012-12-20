#ifndef STERRORMONITOR_H 
#define STERRORMONITOR_H 1

// Include files
// from STKernel
#include "Kernel/STHistoAlgBase.h"

/** @class STErrorMonitor STErrorMonitor.h
 *  
 *  Class to monitor the error banks. The output consists of:
 *  - One overview histograms that counts the number of error banks per Tell1.
 *  - One 2D histogram for each Tell1 with an error bank. It displays the error
 *    type versus the optical link number.
 *  There are three important job options:
 *  - \b DetType: to switch between "IT" and "TT"
 *  - \b ErrorLocation: to set the error location in case it is different from
 *     the default location.
 *  - \b ExpertHisto: Set this to false to turn of the 2D histograms.
 *
 *  @author Jeroen Van Tilburg
 *  @date   2009-03-20
 */
class STErrorMonitor : public ST::HistoAlgBase {
public: 
  /// Standard constructor
  STErrorMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  // The maximum number of Tell1s (used to determine number of bins)
  unsigned int m_maxTell1s;

  // Total number of active ports in detector
  unsigned int m_activePorts;
  
  // Job options
  bool m_expertHisto;         ///< Flag to turn off filling of expert histograms
  std::string m_errorLocation;///< Location of STTELL1BoardErrorBanks objects
  bool m_debug; ///< true if message service level is debug
  bool m_verbose; ///< true if message service level is verbose

  // Book histrograms for online monitoring
  AIDA::IHistogram1D* m_1d_evtCounter;
  AIDA::IHistogram1D* m_1d_errorBanks;
  AIDA::IHistogram2D* m_2d_errorTypes;

  /// Map of error histograms booked in initialize
  std::map<unsigned int, AIDA::IHistogram2D*> m_errorHistos;
  
  /// Label the y-axis with the error types
  void labelHistoErrorTypes( AIDA::IHistogram2D* histo );

  /// Fraction of ports which send error banks
  AIDA::IHistogram1D* m_1d_fracErrors;
  /// Fraction of ports which send error banks per TELL1
  AIDA::IProfile1D* m_prof_fracErrors;

  unsigned int m_nTELL1s; ///< Number of TELL1 boards expected.

  /// Store number of error banks/TELL1 (48 (42) Tell1s, 1->48 (42) for TT (IT))
  std::vector<unsigned int> m_nErrorBanksPerTELL1;


};
#endif // STERRORMONITOR_H
