// $Id: TestLinker.h,v 1.5 2010-02-05 08:20:04 jpalac Exp $
#ifndef TestLinker_H
#define TestLinker_H 1

// Include files
#include "GaudiAlg/GaudiAlgorithm.h"

// from Associators
#include "Kernel/Particle2MCLinker.h"
#include "AsctAlgorithm.h"

/** @class TestLinker TestLinker.h
 *
 *  Example of use of various DaVinci features
 *
 *  @author Philippe Charpentier
 *  @date   26/04/2002
 */

#define TR_EFFICIENCY

namespace LHCb {
  class Track;
}

class TestLinker : public AsctAlgorithm
{

public:

  /// Standard constructor
  TestLinker(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~TestLinker( ); ///< Standard destructor

  StatusCode initialize();    ///< Algorithm initialization
  StatusCode execute   ();    ///< Algorithm execution
  StatusCode finalize  ();    ///< Algorithm finalization

  void prTable( const MSG::Level level, const std::string title,
                const std::vector<int>& table, const int width);
  int trType( const LHCb::Track* tr );

private:

  // Define the name of the associator to be used, and define a pointer
  Particle2MCLinker* m_linkChi2;  ///< Pointer to associator using Chi2
  Particle2MCLinker* m_linkLinks; ///< Pointer to associator using links
  Particle2MCLinker* m_linkComp; ///< Pointer to composite particle associator using links
  Particle2MCLinker* m_linkWithChi2; ///< Pointer to associator with chi2 as weight
  ProtoParticle2MCLinker* m_linkChargedPP;
  ProtoParticle2MCLinker* m_linkNeutralPP;

  int m_matchLinks;
  int m_matchChi2;
  int m_matchFull;
  int m_matchLinksNotChi2;
  int m_matchLinksHighChi2;
  int m_matchChi2NotLinks;
  int m_matchDifferent;
  int m_matchLinksDiffComp;
  int m_matchMissedComp;
  int m_matchComp;
  int m_nbParts;
  int m_mcPartRecons;
  std::vector<int> m_mcPart2Track;
  std::vector<int> m_mcPart2Proto;
  std::vector<int> m_mcPart2Part;
  std::vector<int> m_mcPart2PartChi2;
  std::vector<int> m_mcPart2PartLink;
  int m_mcPartCount;
  int m_skippedEvts;
  int m_nbEvts;
  bool m_setInputData;
};


#endif    // TestLinker_H
