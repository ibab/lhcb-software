// $Id: TestLinker.h,v 1.1 2004-06-11 15:26:18 phicharp Exp $
#ifndef TestLinker_H
#define TestLinker_H 1

// Include files
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Relations/IAssociatorWeighted.h"

// from Associators
#include "DaVinciAssociators/Particle2MCLink.h"
#include "AsctAlgorithm.h"

/** @class TestLinker TestLinker.h
 *  
 *  Example of use of various DaVinci features
 *
 *  @author Philippe Charpentier
 *  @date   26/04/2002
 */

class TrStoredTrack;

#define TR_EFFICIENCY

class TestLinker : public AsctAlgorithm {

public:
  typedef IAssociatorWeighted<TrStoredTrack,MCParticle,double> Tr2MCPartAsct;
  /// Standard constructor 
  TestLinker(const std::string& name, ISvcLocator* pSvcLocator); 

  virtual ~TestLinker( ); ///< Standard destructor
  
  StatusCode initialize();    ///< Algorithm initialization
  StatusCode execute   ();    ///< Algorithm execution
  StatusCode finalize  ();    ///< Algorithm finalization

  void prTable( MsgStream& log, MSG::Level level, const std::string title, 
                const std::vector<int>& table, const int width);
  int trType( TrStoredTrack* tr );

private:

  // Define the name of the associator to be used, and define a pointer
  std::string m_nameMCAsct;  ///< Name of tool for Part to MCPart Association
  Particle2MCLink* m_linkChi2;  ///< Pointer to associator using Chi2
  Particle2MCLink* 
    m_linkLinks; ///< Pointer to associator using links
  Particle2MCLink* 
    m_linkComp; ///< Pointer to composite particle associator using links
  Particle2MCLink* 
    m_linkWithChi2; ///< Pointer to associator with chi2 as weight
  Particle2MCLink* 
    m_linkCharged;  ///< Pointer to associator of ProtoParticles
  Particle2MCLink* 
    m_linkNeutral;  ///< Pointer to associator of ProtoParticles
  ProtoParticle2MCLink*
    m_linkChargedPP;
  ProtoParticle2MCLink*
    m_linkNeutralPP;
  Tr2MCPartAsct* m_pAsctTrack;

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
