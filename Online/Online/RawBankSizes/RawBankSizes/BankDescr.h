#ifdef RAWSIZE_BANKDESCR_H
#else
#define RAWSIZE_BANKDESCR_H
namespace LHCb 
{
  class BankDescr
  {
  public:
    bool isError;
    std::string name;
    int rootbankidx;
    double sum;
    double siz;
    double xmin,xmax,binw;
    int nbin;
    int nentries;
    IHistogram1D *h;
    std::string h_name;
    IProfile1D *p;
    std::string p_name;
    bool noprofile;
    bool nohist;

    BankDescr()
    {
      nentries = 0;
      sum=0;
      isError = false;
    }
    void init(std::string &nam, bool noprof)
    {
      name = nam;
      noprofile = noprof;
      h_name = "h"+name;
      p_name = "p"+name;
      
    };
    void init(int bn, bool noprof)
    {
      noprofile = noprof;
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
      case LHCb::RawBank::L0PUError:
        {
          isError = true;
          rootbankidx=LHCb::RawBank::L0PU;
          break;
        }
      default:
        {
          isError=false;
          rootbankidx = -1;
          break;
        }
      }
      name=LHCb::RawBank::typeName( (LHCb::RawBank::BankType) bn );
      h_name = "h"+name;
      p_name = "p"+name;
    }
  };
}
#endif
