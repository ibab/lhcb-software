// $Id: RichDigiDataObjVerifier.h,v 1.1 2003-09-20 15:45:18 jonrob Exp $
#ifndef RICHDIGIQC_RICHDIGIDATOBJVERIFIER_H
#define RICHDIGIQC_RICHDIGIDATOBJVERIFIER_H 1

// from Gaudi
#include "GaudiKernel/Algorithm.h"

/** @class RichDigiDataObjVerifier RichDigiDataObjVerifier.h RichDigiQC/RichDigiDataObjVerifier.h
 *
 *  Low level printout of Rich Digitisation objects
 *
 *  @author Chris Jones   (Christopher.Rob.Jones@cern.ch)
 *  @date   2003-09-08
 */

class RichDigiDataObjVerifier : public Algorithm {

public:

  /// Standard constructor
  RichDigiDataObjVerifier( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RichDigiDataObjVerifier( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  // job options
  std::string m_digitTES;        ///< Location of RichDigits in TES
  std::string m_mcdigitTES;      ///< Location of MCRichDigits in TES
  std::string m_mcdepTES;        ///< Location of MCRichDeposits in TES
  std::string m_mcsumdepTES;     ///< Location of MCRichSummedDeposits in TES
  std::string m_mchitTES;        ///< Location of MCRichHits in TES

  bool m_bdDigits;        ///< Flag to turn on/off the printing of RichDigits
  bool m_bdMcDigits;      ///< Flag to turn on/off the printing of MCRichDigits
  bool m_bdMCDeposits;    ///< Flag to turn on/off the printing of MCRichDeposits
  bool m_bdMCSumDeposits; ///< Flag to turn on/off the printing of MCRichSummedDeposits
  bool m_bdMCHits;        ///< Flag to turn on/off the printing of MCRichHits

};
#endif // RICHDIGIQC_RICHDIGIDATOBJVERIFIER_H
