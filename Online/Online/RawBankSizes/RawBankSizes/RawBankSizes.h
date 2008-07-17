// $Id: RawBankSizes.h,v 1.3 2008-07-17 13:24:07 jost Exp $
#ifndef COMPONENT_RawBankSizes_H 
#define COMPONENT_RawBankSizes_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
// from LHCb
#include "Event/RawEvent.h"
#include "math.h"
namespace LHCb 
{
  class BankDescr
  {
  public:
    bool isError;
    std::string name;
    int min_source;
    int max_source;
    int rootbankidx;
    int smin;
    int smax;
    double sum;
    double sum2;
    int nentries;

    BankDescr()
    {
      smin  = 999999999;
      smax  = -1;
      min_source = 999999999;
      max_source = -1;
      nentries = 0;
      sum=0;
      sum2=0;
    }
    void fillstat (int siz)
    {
      if (siz < smin) smin = siz;
      if (siz > smax) smax = siz;
      sum += siz;
      sum2 += siz*siz;
      nentries++;
    }
    void stat(double &mean, double &rms, double &s_min, double &s_max)
    {
      if (nentries > 0)
      {
        mean = sum/nentries;
        rms = sum2/nentries - mean*mean;
        rms = sqrt(rms);
        s_min = smin;
        s_max = smax;
      }
      else
      {
        mean = 0;
        rms = 1000;
        s_min = 0;
        s_max = 1000;
      }
    }
    void setSID(int s)
    {
      if (s<min_source)
      {
        min_source = s;
      }
      if (s>max_source)
      {
        max_source = s;
      }
    }

    void init(int bn)
    {
      switch (bn)
      {
      case LHCb::RawBank::ITError:
        {
          isError = true;
          rootbankidx=LHCb::RawBank::IT;
          break;
        }
      case LHCb::RawBank::TTError:  
        {
          isError = true;
          rootbankidx=LHCb::RawBank::TT;
          break;
        }
      case LHCb::RawBank::VeloError:   
        {
          isError = true;
          rootbankidx=LHCb::RawBank::Velo;
          break;
        }
      case LHCb::RawBank::OTError:
        {
          isError = true;
          rootbankidx=LHCb::RawBank::OT;
          break;
        }
      case LHCb::RawBank::EcalPackedError:
        {
          isError = true;
          rootbankidx=LHCb::RawBank::EcalPacked;
          break;
        }
      case LHCb::RawBank::HcalPackedError:
        {
          isError = true;
          rootbankidx=LHCb::RawBank::HcalPacked;
          break;
        }
      case LHCb::RawBank::PrsPackedError:
        {
          isError = true;
          rootbankidx=LHCb::RawBank::PrsPacked;
          break;
        }
      case LHCb::RawBank::L0CaloError:
        {
          isError = true;
          rootbankidx=LHCb::RawBank::L0Calo;
          break;
        }
      case LHCb::RawBank::L0MuonError:
        {
          isError = true;
          rootbankidx=LHCb::RawBank::L0Muon;
          break;
        }
      case LHCb::RawBank::MuonError:
        {
          isError = true;
          rootbankidx=LHCb::RawBank::Muon;
          break;
        }
      case LHCb::RawBank::L0DUError:
        {
          isError = true;
          rootbankidx=LHCb::RawBank::L0DU;
          break;
        }
      default:
        {
          isError=false;
          rootbankidx = -1;
          break;
        }
      }
      name=LHCb::RawBank::typeName( (LHCb::RawBank::BankType) bn );;
    }
  };

  /** @class RawBankSizes RawBankSizes.h component/RawBankSizes.h
   *  
   *
   *  @author Olivier Deschamps
   *  @date   2008-03-27
   */
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
    LHCb::RawEvent* m_rawEvt;
    //
    std::vector<std::string> m_bankNames;
    std::vector<LHCb::RawBank::BankType> m_bankTypes;
    std::map<std::string,int> m_max;
    int m_def;
    int m_bin;
    bool m_prof;
    bool m_firstevent;
    BankDescr Banks[LHCb::RawBank::LastType];
    BankDescr totsize;
    int n_ev;
    bool first_few;
    int m_nRMS;
  };
}
#endif // COMPONENT_RawBankSizes_H
