// $Id: ReadHltReport.h,v 1.2 2009-10-16 11:29:32 graven Exp $
#ifndef READHLTSUMMARY_H 
#define READHLTSUMMARY_H 1

// Include files
// from HLT, this is a specialized DVAlgorithm
#include "Kernel/DVAlgorithm.h"
/** @class ReadHltReport ReadHltReport.h
 *
 *  Reads HltSummary class and prints out various things
 *
 *  @author Patrick Koppenburg
 *  @date   2007-03-29
 */
class IParticlePropertySvc;

class ReadHltReport : public DVAlgorithm {
public: 
  /// Standard constructor
  ReadHltReport( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ReadHltReport( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  StatusCode readHltReport(const LHCb::HltDecReports*);
  StatusCode printParticles(const LHCb::Particle::ConstVector&)const;

private:
  bool m_printParticles ; ///< print particles
  unsigned int m_maxCandidatesPrinted ; ///< maximum number of candidates per selection printed out
  std::string m_decisionString ; ///< "Decision", following Tomasz' guidelines
  std::string m_hltDecReportsLocation  ; ///< location
  std::string m_hltSelReportsLocation  ; ///< location
  
};
#endif // READHLTSUMMARY_H
