// $Id: HltL0CaloPrepare.h,v 1.7 2009-01-06 12:18:25 graven Exp $
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
class HltL0CaloPrepare : public HltAlgorithm {
public: 
  /// Standard constructor
  HltL0CaloPrepare( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltL0CaloPrepare( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  void makeTrack(const LHCb::L0CaloCandidate& calo, LHCb::Track& track);
  void addExtras(const LHCb::L0CaloCandidate& calo, LHCb::Track& track);

private:

  Hlt::SelectionContainer2<LHCb::Track,LHCb::L0CaloCandidate> m_selection; 

  std::string m_caloMakerName;  
  ICaloSeedTool* m_caloMaker;
  
  int m_L0ETID;

  AIDA::IHistogram1D* m_histoEt;
  AIDA::IHistogram1D* m_histoEtBest;

  class caloTypeProperty {
  public:
    caloTypeProperty(caloTypeProperty& rhs) ;
    caloTypeProperty(const std::string& s) ;
    caloTypeProperty& operator=(const std::string& s) ;
    L0DUBase::CaloType::Type operator()() const { return m_val;}
    StringProperty& property() { return m_prop;}
    void updateHandler(Property&);
  private:
    StringProperty m_prop;
    L0DUBase::CaloType::Type m_val;
  };

  double m_minEt;
  HltL0CaloPrepare::caloTypeProperty m_caloType;

};
#endif // HLTHADALLEYPRETRIGGER_H
