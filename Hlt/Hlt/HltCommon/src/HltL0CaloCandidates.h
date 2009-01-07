// $Id: HltL0CaloCandidates.h,v 1.4 2009-01-07 15:33:46 graven Exp $
#ifndef HLTCOMMON_HLTLOCALOPREPARE_H 
#define HLTCOMMON_HLTL0CALOPREPARE_H 1

// Include files
// from Gaudi
#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltSelectionContainer.h"
#include "HltBase/ICaloSeedTool.h"
#include "GaudiKernel/Property.h"

/** @class HltHadAlleyPreTrigger HltHadAlleyPreTrigger.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2006-07-28
 */
class HltL0CaloCandidates : public HltAlgorithm {
public: 
  /// Standard constructor
  HltL0CaloCandidates( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltL0CaloCandidates( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution


private:

  void makeTrack(const LHCb::L0CaloCandidate& calo, LHCb::Track& track);
  void addExtras(const LHCb::L0CaloCandidate& calo, LHCb::Track& track);

    class L0CaloCandidateCut {
    public:
        L0CaloCandidateCut( L0DUBase::CaloType::Type type ) : m_type(type), m_hasThreshold(false), m_threshold(-1) {}
        L0CaloCandidateCut( L0DUBase::CaloType::Type type, int threshold ) : m_type(type), m_hasThreshold(true), m_threshold(threshold) {}
        bool operator()(const LHCb::L0CaloCandidate* calo) const {
            return ( calo != 0 ) 
                && ( calo->type() == m_type ) 
                && ( !m_hasThreshold || calo->etCode() > m_threshold );
        }
    private:
        L0DUBase::CaloType::Type m_type;
        bool m_hasThreshold;
        int  m_threshold;
    };

  typedef std::vector< L0CaloCandidateCut > CutList_t;
  CutList_t generateCutList(const LHCb::L0DUChannel& channel);
  CutList_t generateCutList(const std::string& whichType);

  Hlt::SelectionContainer2<LHCb::Track,LHCb::L0CaloCandidate> m_selection; 

  std::string m_l0Location;  
  std::string m_l0Channel;  
  std::string m_caloMakerName;  
  ICaloSeedTool* m_caloMaker;

  AIDA::IHistogram1D* m_et;
  AIDA::IHistogram1D* m_etMax;

  //std::map< unsigned int, CutList_t > m_l0config;


};
#endif // HLTHADALLEYPRETRIGGER_H
