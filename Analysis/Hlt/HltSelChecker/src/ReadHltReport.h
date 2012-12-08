// $Id: ReadHltReport.h,v 1.2 2009-10-16 11:29:32 graven Exp $
#ifndef READHLTSUMMARY_H
#define READHLTSUMMARY_H 1

#include "GaudiAlg/GaudiAlgorithm.h"

/** @class ReadHltReport ReadHltReport.h
 *
 *  Reads HltSummary class and prints out various things
 *
 *  @author Patrick Koppenburg
 *  @date   2007-03-29
 */
class IParticlePropertySvc;

class ReadHltReport : public GaudiAlgorithm
{

public:

  /// Standard constructor
  ReadHltReport( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ReadHltReport( ); ///< Destructor

  virtual StatusCode execute(); ///< Algorithm execution

private:

  StatusCode readHltReport(const LHCb::HltDecReports*); ///< main method
  StatusCode printObject(const LHCb::HltObjectSummary*, std::string increment = "");  ///< print summary object (recursive)

private:
  bool m_printParticles ; ///< print particles
  bool m_printDecisions ; ///< print decisions
  std::string m_decisionString ; ///< "Decision", following Tomasz' guidelines
  std::string m_hltDecReportsLocation  ; ///< location
  std::string m_hltSelReportsLocation  ; ///< location
  std::map<std::string,std::string> m_infos ; ///< all known info fields

};
#endif // READHLTSUMMARY_H
