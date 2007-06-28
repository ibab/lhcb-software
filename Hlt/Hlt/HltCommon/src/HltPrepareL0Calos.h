// $Id: HltPrepareL0Calos.h,v 1.2 2007-06-28 22:07:39 hernando Exp $
#ifndef HLTPREPAREL0CALOS_H 
#define HLTPREPAREL0CALOS_H 1

// Include files
// from Gaudi
#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltFunctions.h"
#include "Event/L0CaloCandidate.h"

/** @class HltHadAlleyPreTrigger HltHadAlleyPreTrigger.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2006-07-28
 */
class HltPrepareL0Calos : public HltAlgorithm {
public: 
  /// Standard constructor
  HltPrepareL0Calos( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltPrepareL0Calos( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  class checkType : public Estd::filter<LHCb::L0CaloCandidate> {
  public:
    explicit checkType(int type): m_type(type) {}
    bool operator() (const LHCb::L0CaloCandidate& c) const 
    {return (c.type() == m_type);} 
    Estd::filter<LHCb::L0CaloCandidate>* clone() const 
    {return new checkType(m_type);}
  public:
    int m_type;
  };

  LHCb::Track* makeTrack(const LHCb::L0CaloCandidate& calo);

protected:

  int m_caloType;
  double m_etMin;

  std::string m_caloCandidatesLocation;
  std::string m_outputL0CaloCandidatesName;

  HltHisto m_histoEt;
  HltHisto m_histoEt1;

protected:

  std::vector<LHCb::L0CaloCandidate*> m_calos;
  std::vector<LHCb::L0CaloCandidate*> m_ocalos;

  Estd::function<LHCb::L0CaloCandidate>* _etFun;

  Estd::filter<LHCb::Track>* _matchFilter;
  Estd::filter<LHCb::L0CaloCandidate>* _typeFilter;
  Estd::filter<LHCb::L0CaloCandidate>* _etFilter;

};
#endif // HLTHADALLEYPRETRIGGER_H
