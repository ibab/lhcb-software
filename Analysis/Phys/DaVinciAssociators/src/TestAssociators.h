// $Id: TestAssociators.h,v 1.3 2003-05-26 11:38:38 phicharp Exp $
#ifndef TestAssociators_H
#define TestAssociators_H 1

// Include files
#include "GaudiKernel/Algorithm.h"

// from Associators
#include "DaVinciAssociators/Particle2MCAsct.h"
#include "DaVinciAssociators/Particle2MCLinksAsct.h"
#include "DaVinciAssociators/Particle2MCWithChi2Asct.h"
#include "DaVinciAssociators/ProtoParticle2MCAsct.h"
#include "TrAssociators/ITrReconstructible.h"

/** @class TestAssociators TestAssociators.h
 *  
 *  Example of use of various DaVinci features
 *
 *  @author Philippe Charpentier
 *  @date   26/04/2002
 */

class TestAssociators : public Algorithm {

public:
  /// Standard constructor 
  TestAssociators(const std::string& name, ISvcLocator* pSvcLocator); 

  virtual ~TestAssociators( ); ///< Standard destructor
  
  StatusCode initialize();    ///< Algorithm initialization
  StatusCode execute   ();    ///< Algorithm execution
  StatusCode finalize  ();    ///< Algorithm finalization

  void prTable( MsgStream& log, MSG::Level level, const std::string title, 
                const std::vector<int>& table, const int width);
  int trType( TrStoredTrack* tr );

private:

  // Define the name of the associator to be used, and define a pointer
  std::string m_nameMCAsct;  ///< Name of tool for Part to MCPart Association
  Particle2MCAsct::IAsct* m_pAsctChi2;  ///< Pointer to associator using Chi2
  Particle2MCLinksAsct::IAsct* 
    m_pAsctLinks; ///< Pointer to associator using links
  Particle2MCAsct::IAsct* 
    m_pAsctComp; ///< Pointer to composite particle associator using links
  Particle2MCWithChi2Asct::IAsct* 
    m_pAsctWithChi2; ///< Pointer to associator with chi2 as weight
  ProtoParticle2MCAsct::IAsct* 
    m_pAsctProto;  ///< Pointer to associator of ProtoParticles
  Tr2MCPartAsct* m_pAsctTrack;
  ITrReconstructible* 
    m_pTrRecons; ///< Pointer to the tool to check if an MC is reconstructible
  std::vector<std::string> m_inputData; ///< List of input set

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
};


#endif    // TestAssociators_H
