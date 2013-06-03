// $Id: RawBankSizes.h,v 1.7 2010-09-25 21:31:08 frankb Exp $
#ifndef COMPONENT_RawBankSizes_H
#define COMPONENT_RawBankSizes_H 1

// Include files
// from system
#include "math.h"
#include <map>
// from Gaudi
#include "Structure.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiAlg/GaudiHistoAlg.h"
// from LHCb
#include "AIDA/IHistogram1D.h"
#include "AIDA/IProfile1D.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "Event/RawEvent.h"
#include "BankDescr.h"

namespace LHCb
{
 typedef std::map<std::string,BankDescr*> detbmap;
 typedef detbmap::iterator detbmiter;
 typedef std::pair<detbmiter, bool> detbinsrtp;
 typedef std::map<std::string,HParam::HistParams*> dethmap;
 typedef dethmap::iterator dethmiter;
 typedef std::pair<dethmiter, bool> dethinsrtp;
}
namespace LHCb
{

  class RawBankSizes : public GaudiHistoAlg
  {
  public:
    /// Standard constructor
    RawBankSizes( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~RawBankSizes( ); ///< Destructor

    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization

  protected:

  private:
    std::vector<std::string> m_DetectorNames;
    LHCb::RawEvent* m_rawEvt;

    std::vector<std::string> m_bankNames;

    LHCb::detbmap m_detectors;
    LHCb::dethmap m_dethparams;

    std::vector<LHCb::RawBank::BankType> m_bankTypes;
    std::map<std::string,IHistogram1D *> m_dethist;
    std::map<int,double > m_detsums;
    std::map<std::string,int> m_max;
    int m_def;
    int m_bin;
    bool m_prof;
    bool m_firstevent;
    LHCb::BankDescr Banks[LHCb::RawBank::LastType];
    LHCb::BankDescr totsize;
    HParam::HistParams m_hparams[LHCb::RawBank::LastType];

    HParam::HistParams m_totevpar;
    HParam::HistParams m_HLTratiopar;

    dethmap dethparams;


    IHistogram1D *h_totev[HISTPERHIST];
    IProfile1D *p_banks[HISTPERHIST];
    IHistogram1D *h_HLTratio[HISTPERHIST];
    //IProfile1D *p_HLTratio[HISTPERHIST];
    int n_ev;
    bool first_few;
    int m_nRMS;
  };
}
#endif // COMPONENT_RawBankSizes_H
