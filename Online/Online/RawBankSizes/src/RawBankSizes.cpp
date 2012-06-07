// $Id: RawBankSizes.cpp,v 1.18 2010-09-25 21:31:08 frankb Exp $
// Include files

// from Gaudi
#include <vector>
#include <map>
#include <string>
#include <utility>
#include "RawBankSizes.h"
#include "MDF/OnlineRunInfo.h"
#include "MDF/MDFHeader.h"
//#include "Event/ODIN.h"

// local

//-----------------------------------------------------------------------------
// Implementation file for class : RawBankSizes
//
// 2008-03-27 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_NAMESPACE_ALGORITHM_FACTORY( LHCb,RawBankSizes)

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RawBankSizes::RawBankSizes( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
{
//  memset(m_hparams,0,sizeof(m_hparams));
   m_DetectorNames.push_back("L0");
   m_DetectorNames.push_back("PRS");
   m_DetectorNames.push_back("ECal");
   m_DetectorNames.push_back("HCal");
   m_DetectorNames.push_back("Velo");
   m_DetectorNames.push_back("RICH");
   m_DetectorNames.push_back("IT");
   m_DetectorNames.push_back("TT");
   m_DetectorNames.push_back("OT");
   m_DetectorNames.push_back("Muon");
   m_DetectorNames.push_back("HLT");
   m_DetectorNames.push_back("Online");
   m_DetectorNames.push_back("LHCb");
   m_DetectorNames.push_back("TDET");
   m_DetectorNames.push_back("Unknown");

   declareProperty( "bankNames"   , m_bankNames );
   declareProperty( "MaxSizeMap"  , m_max);
   declareProperty( "MaxSizeDef"  , m_def=500);
   declareProperty( "Bins"        , m_bin=100);
   declareProperty( "Profile"     , m_prof=true);
   declareProperty( "number_ofRMS"     , m_nRMS=5);

   // default bank types list == all banks !
   for(int i = 0 ; i != (int) LHCb::RawBank::LastType; i++)
   {
     std::string bname = LHCb::RawBank::typeName( (LHCb::RawBank::BankType) i );
     m_bankNames.push_back( bname );
     declareProperty(bname,m_hparams[i]/*,"(0,0.0,0.0,0,-2)"*/);
   }
   declareProperty("TotEv",m_totevpar/*,"(0,0.0,0.0,0,-2)"*/);
   declareProperty("HLTRatio",m_HLTratiopar/*,"(0,0.0,0.0,0,-2)"*/);
   std::vector<std::string>::iterator detiter = m_DetectorNames.begin();
   for (detiter= m_DetectorNames.begin();detiter != m_DetectorNames.end();detiter++)
   {
     std::string d = *detiter;
     BankDescr *b = new BankDescr();
     b->init(d,true);
     HParam::HistParams *p = new HParam::HistParams();
     declareProperty("D_"+d,*p);
     m_detectors.insert(std::pair<std::string,BankDescr*>(d,b));
     m_dethparams.insert(std::pair<std::string,HParam::HistParams*>(d,p));
   }
   m_firstevent = true;
   first_few = true;
   n_ev = 0;
   std::string s="Total";
   totsize.init(s,false);
}
//=============================================================================
// Destructor
//=============================================================================
RawBankSizes::~RawBankSizes()
{
}

//=============================================================================
// Initialization
//=============================================================================
// StatusCode RawBankSizes::initialize()
// {
//   int i;
//   StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
//   if ( sc.isFailure() ) return sc;  // error printed already by GaudiHistoAlg
//
//   if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
//
// //   Banks = new BankDescr[(int) LHCb::RawBank::LastType];
//
//   // convert bankNames to bankTypes
//   for(std::vector<std::string>::iterator it = m_bankNames.begin();it!=m_bankNames.end();it++)
//   {
//     bool found = false;
//     for(i = 0 ; i != (int) LHCb::RawBank::LastType; i++)
//     {
//       std::string bname = LHCb::RawBank::typeName( (LHCb::RawBank::BankType) i );
//       if( bname == *it)
//       {
//         found = true;
//         m_bankTypes.push_back( (LHCb::RawBank::BankType) i );
//       }
//     }
//     if( !found) warning() << "Requested bank '" << *it << "' is not a valid name" << endreq;
//   }
//
//    for(i = 0 ; i != (int) LHCb::RawBank::LastType; i++)
//    {
// //     printf("%s %s\n",LHCb::RawBank::typeName( (LHCb::RawBank::BankType) i).c_str(),
// //     m_hparams[i].det.c_str());
//      Banks[i].init(i,(m_hparams[i].s_max < 0));
//      if (m_hparams[i].n_bin == -2)
//      {
//         printf("WARNING: No Histogram Options for Bank %s\n",LHCb::RawBank::typeName( (LHCb::RawBank::BankType) i
//         ).c_str());
//      }
//      Banks[i].nohist = (m_hparams[i].n_bin <= 0);
//    }
//    for(i = 0 ; i != (int) LHCb::RawBank::LastType; i++)
//    {
//     int idx;
//     idx = i;
//     if (Banks[i].isError)
//     {
//       idx = Banks[i].rootbankidx;
//     }
//     if (!Banks[i].nohist)
//     {
//        double binw = (m_hparams[i].xmax - m_hparams[i].xmin)/m_hparams[i].n_bin;
//        Banks[i].xmin = m_hparams[i].xmin;
//        Banks[i].xmax = m_hparams[i].xmax;
//        Banks[i].binw = binw;
//        if (((int)binw % 4) != 0)
//        {
//         printf("Binwidth for %s not a multiple of 4. %f\n",Banks[i].name.c_str(),binw);
//        }
//        Banks[i].nbin = m_hparams[i].n_bin;
//
//        Banks[i].h[0]  = histoSvc()->book( Banks[i].h_name[0],Banks[i].name+" Size in Bytes (all Triggers)",m_hparams[i].n_bin+2,
//         m_hparams[i].xmin-binw,m_hparams[i].xmax+binw);
//        declareInfo(Banks[i].h_name[0], Banks[i].h[0],Banks[i].name+" Size in Bytes (all Triggers)");
//        Banks[i].h[1]  = histoSvc()->book( Banks[i].h_name[1],Banks[i].name+" Size in Bytes (nonLumi)",m_hparams[i].n_bin+2,
//         m_hparams[i].xmin-binw,m_hparams[i].xmax+binw);
//        declareInfo(Banks[i].h_name[1], Banks[i].h[1],Banks[i].name+" Size in Bytes (nonLumi)");
//     }
//     else
//     {
//        printf("INFO: No Histogram booked for Bank %s\n",Banks[i].name.c_str());
//     }
//     if (!Banks[i].noprofile)
//     {
//        Banks[i].p[0]  = histoSvc()->bookProf( Banks[i].p_name[0],Banks[i].name+" Size vs. SourceID (all Triggers)",
//         m_hparams[idx].s_max-m_hparams[idx].s_min+1,
//         (float)m_hparams[idx].s_min,(float)m_hparams[idx].s_max);
//        declareInfo(Banks[i].p_name[0], Banks[i].p[0],Banks[i].name+" Size vs. SourceID (all Triggers)");
//        Banks[i].p[1]  = histoSvc()->bookProf( Banks[i].p_name[1],Banks[i].name+" Size vs. SourceID (nonLumi)",
//         m_hparams[idx].s_max-m_hparams[idx].s_min+1,
//         (float)m_hparams[idx].s_min,(float)m_hparams[idx].s_max);
//        declareInfo(Banks[i].p_name[1], Banks[i].p[1],Banks[i].name+" Size vs. SourceID (nonLumi)");
//     }
//     else
//     {
//        printf("INFO: No Profile booked for Bank %s\n",Banks[i].name.c_str());
//     }
//    }
//    totsize.xmin = m_totevpar.xmin;
//    totsize.xmax = m_totevpar.xmax;
//    totsize.nbin = m_totevpar.n_bin;
//    totsize.binw = (totsize.xmax-totsize.xmin)/totsize.nbin;
//    if (((int)totsize.binw % 4) != 0)
//    {
//     printf("Binwidth for %s not a multiple of 4. %f\n",Banks[i].name.c_str(),totsize.binw);
//    }
//    h_totev[0] = histoSvc()->book( "TotSize(all)","Total Event Size in Bytes (all Triggers)",m_totevpar.n_bin+2,
//        m_totevpar.xmin-totsize.binw,m_totevpar.xmax+totsize.binw);
//    declareInfo("TotSize(all)", h_totev[0],"Total Event Size in Bytes (all Triggers)");
//    h_totev[1] = histoSvc()->book( "TotSize(nonLumi)","Total Event Size in Bytes (NonLumi)",m_totevpar.n_bin+2,
//        m_totevpar.xmin-totsize.binw,m_totevpar.xmax+totsize.binw);
//    declareInfo("TotSize(nonLumi)", h_totev[1],"Total Event Size in Bytes (nonLumi)");
//    p_banks[0] = histoSvc()->bookProf( "Banksp(all)","Bank Size vs. Bank Number (all Triggers)",
//          LHCb::RawBank::LastType+1, -1.0, (float)LHCb::RawBank::LastType);
//    declareInfo("Banksp(all)",p_banks[0],"Bank Size vs. Bank Number (all Triggers)");
//    p_banks[1] = histoSvc()->bookProf( "Banksp(nonLumi)","Bank Size vs. Bank Number (nonLumi)",
//          LHCb::RawBank::LastType+1, -1.0, (float)LHCb::RawBank::LastType);
//    declareInfo("Banksp(nonLumi)",p_banks[1],"Bank Size vs. Bank Number (nonLumi)");
//    for (dethmiter j=m_dethparams.begin();j!=m_dethparams.end();j++)
//    {
//       detbmiter bd = m_detectors.find(j->first);
//       bd->second->xmin = j->second->xmin;
//       bd->second->xmax = j->second->xmax;
//       bd->second->nbin = j->second->n_bin;
//       bd->second->binw = (j->second->xmax-j->second->xmin)/j->second->n_bin;
//       if (((int)bd->second->binw % 4) != 0)
//       {
//        printf("Binwidth for %s not a multiple of 4. %f\n",Banks[i].name.c_str(),bd->second->binw);
//       }
//       bd->second->h[0] = histoSvc()->book("Total_"+j->first+"(all)","Total Size for "+j->first+" in Bytes (all Triggers)",
//        j->second->n_bin+2, j->second->xmin-bd->second->binw,j->second->xmax+bd->second->binw);
//       declareInfo("Total_"+j->first+"(all)", bd->second->h[0],"Total Size for "+j->first+" in Bytes (all Triggers)");
//       bd->second->h[1] = histoSvc()->book("Total_"+j->first+"(noLumi)","Total Size for "+j->first+" in Bytes (nonLumi)",
//        j->second->n_bin+2, j->second->xmin-bd->second->binw,j->second->xmax+bd->second->binw);
//       declareInfo("Total_"+j->first+"(noLumi)", bd->second->h[1],"Total Size for "+j->first+" in Bytes (nonLumi)");
//    }
//    m_firstevent = true;
//    n_ev = 0;
//   return StatusCode::SUCCESS;
// }

StatusCode RawBankSizes::initialize()
{
  int i;
  int hindx;

  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiHistoAlg

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

//   Banks = new BankDescr[(int) LHCb::RawBank::LastType];

  // convert bankNames to bankTypes
  for(std::vector<std::string>::iterator it = m_bankNames.begin();it!=m_bankNames.end();it++)
  {
    bool found = false;
    for(i = 0 ; i != (int) LHCb::RawBank::LastType; i++)
    {
      std::string bname = LHCb::RawBank::typeName( (LHCb::RawBank::BankType) i );
      if( bname == *it)
      {
        found = true;
        m_bankTypes.push_back( (LHCb::RawBank::BankType) i );
      }
    }
    if( !found) warning() << "Requested bank '" << *it << "' is not a valid name" << endreq;
  }

   for(i = 0 ; i != (int) LHCb::RawBank::LastType; i++)
   {
//     printf("%s %s\n",LHCb::RawBank::typeName( (LHCb::RawBank::BankType) i).c_str(),
//     m_hparams[i].det.c_str());
     Banks[i].init(i,(m_hparams[i].s_max < 0));
     if (m_hparams[i].n_bin == -2)
     {
        printf("WARNING: No Histogram Options for Bank %s\n",LHCb::RawBank::typeName( (LHCb::RawBank::BankType) i
        ).c_str());
     }
     Banks[i].nohist = (m_hparams[i].n_bin <= 0);
   }
   for(i = 0 ; i != (int) LHCb::RawBank::LastType; i++)
   {
    int idx;
    idx = i;
    if (Banks[i].isError)
    {
      idx = Banks[i].rootbankidx;
    }
    if (!Banks[i].nohist)
    {
       double binw = (m_hparams[i].xmax - m_hparams[i].xmin)/m_hparams[i].n_bin;
       Banks[i].xmin = m_hparams[i].xmin;
       Banks[i].xmax = m_hparams[i].xmax;
       Banks[i].binw = binw;
       if (((int)binw % 4) != 0)
       {
        printf("Binwidth for %s not a multiple of 4. %f\n",Banks[i].name.c_str(),binw);
       }
       Banks[i].nbin = m_hparams[i].n_bin;
       for (hindx = 0; hindx < HISTPERHIST;hindx++)
       {
         Banks[i].h[hindx]  = histoSvc()->book( Banks[i].h_name[hindx],Banks[i].name+" Size in Bytes"+titqual[hindx],m_hparams[i].n_bin,
          m_hparams[i].xmin,m_hparams[i].xmax);
         declareInfo(Banks[i].h_name[hindx], Banks[i].h[hindx],Banks[i].name+" Size in Bytes"+titqual[hindx]);
       }
    }
    else
    {
       printf("INFO: No Histogram booked for Bank %s\n",Banks[i].name.c_str());
    }
    if (!Banks[i].noprofile)
    {
      for (hindx=0;hindx<HISTPERHIST;hindx++)
      {
        Banks[i].p[hindx]  = histoSvc()->bookProf( Banks[i].p_name[hindx],Banks[i].name+" Size vs. SourceID"+titqual[hindx],
         m_hparams[idx].s_max-m_hparams[idx].s_min+1,
         (float)m_hparams[idx].s_min,(float)m_hparams[idx].s_max);
        declareInfo(Banks[i].p_name[hindx], Banks[i].p[hindx],Banks[i].name+" Size vs. SourceID"+titqual[hindx]);
      }
    }
    else
    {
       printf("INFO: No Profile booked for Bank %s\n",Banks[i].name.c_str());
    }
   }
   totsize.xmin = m_totevpar.xmin;
   totsize.xmax = m_totevpar.xmax;
   totsize.nbin = m_totevpar.n_bin;
   totsize.binw = (totsize.xmax-totsize.xmin)/totsize.nbin;
   if (((int)totsize.binw % 4) != 0)
   {
    // Beat: This is wrong and was Banks[i], which clearly accesses bad memory!
    printf("Binwidth for %s not a multiple of 4. %f\n",Banks[0].name.c_str(),totsize.binw);
   }
   for (hindx=0;hindx<HISTPERHIST;hindx++)
   {
     h_totev[hindx] = histoSvc()->book( "TotSize"+namqual[hindx],"Total Event Size in Bytes"+titqual[hindx],m_totevpar.n_bin,
         m_totevpar.xmin,m_totevpar.xmax);
     declareInfo("TotSize"+namqual[hindx], h_totev[hindx],"Total Event Size in Bytes"+titqual[hindx]);
     h_HLTratio[hindx] = histoSvc()->book( "HLTRatio"+namqual[hindx],"Ratio HLT Total Size"+titqual[hindx],m_HLTratiopar.n_bin,
         m_HLTratiopar.xmin,m_HLTratiopar.xmax);
     declareInfo("HLTRatio"+namqual[hindx], h_HLTratio[hindx],"Ratio HLT Total Size"+titqual[hindx]);
     p_banks[hindx] = histoSvc()->bookProf("Banksp"+namqual[hindx],"Bank Size vs. Bank Number"+titqual[hindx],
					   LHCb::RawBank::LastType+1, -1.0, (float)LHCb::RawBank::LastType);
     declareInfo("Banksp"+namqual[hindx],p_banks[hindx],"Bank Size vs. Bank Number"+titqual[hindx]);
   }
   for (dethmiter j=m_dethparams.begin();j!=m_dethparams.end();j++)
   {
      detbmiter bd = m_detectors.find(j->first);
      bd->second->xmin = j->second->xmin;
      bd->second->xmax = j->second->xmax;
      bd->second->nbin = j->second->n_bin;
      bd->second->binw = (j->second->xmax-j->second->xmin)/j->second->n_bin;
      if (((int)bd->second->binw % 4) != 0)
      {
        // Beat: This is wrong and was Banks[i], which clearly accesses bad memory!
        printf("Binwidth for %s not a multiple of 4. %f\n",Banks[0].name.c_str(),bd->second->binw);
      }
      for (hindx=0;hindx<HISTPERHIST;hindx++)
      {
        bd->second->h[hindx] = histoSvc()->book("Total_"+j->first+namqual[hindx],"Total Size for "+j->first+" in Bytes"+titqual[hindx],
         j->second->n_bin, j->second->xmin,j->second->xmax);
        declareInfo("Total_"+j->first+namqual[hindx], bd->second->h[hindx],"Total Size for "+j->first+" in Bytes"+titqual[hindx]);
      }
   }
   m_firstevent = true;
   n_ev = 0;
  return StatusCode::SUCCESS;
}


//=============================================================================
// Main execution
//=============================================================================
StatusCode RawBankSizes::execute()
{
//  int bsize[LHCb::RawBank::LastType];
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // get RawEvent
  if( exist<LHCb::RawEvent>( LHCb::RawEventLocation::Default ) )
  {
    m_rawEvt= get<LHCb::RawEvent>( LHCb::RawEventLocation::Default );
  }
  else
  {
    Warning( "rawEvent not found at location '" + rootInTES() + LHCb::RawEventLocation::Default ).ignore();
    return StatusCode::SUCCESS;
  }
  n_ev++;
  int tots;
  tots = 0;
  int bnkid=0;
  bool nolumi=false;
  for (detbmiter j=m_detectors.begin();j!=m_detectors.end();j++)
  {
     j->second->sum = 0.0;
  }

  for(std::vector<LHCb::RawBank::BankType>::const_iterator it = m_bankTypes.begin();it!=m_bankTypes.end();it++)
  {

//    std::string bname = LHCb::RawBank::typeName( *it );
    Banks[*it].siz = 0.0;
  }
  const std::vector<LHCb::RawBank*>& daq_banks= m_rawEvt->banks(RawBank::DAQ);
  nolumi = true;
  for(std::vector<LHCb::RawBank*>::const_iterator ib = daq_banks.begin();ib!=daq_banks.end();ib++)
  {
    RawBank *b = *ib;
    if (b->version() == DAQ_STATUS_BANK)
    {
      MDFHeader *h = b->begin<MDFHeader>();
      const unsigned int *trMask=h->subHeader().H1->triggerMask();
//      printf("Trigger Mask %0x %0x %0x %0x\n",trMask[0],trMask[1],trMask[2],trMask[3]);
      if ((trMask[1] & 0x10000) != 0)
      {
        nolumi = false;
      }
      break;
    }
  }

  for(std::vector<LHCb::RawBank::BankType>::const_iterator it = m_bankTypes.begin();it!=m_bankTypes.end();it++)
  {
    const std::vector<LHCb::RawBank*>* banks= &m_rawEvt->banks(  *it );
    for(std::vector<LHCb::RawBank*>::const_iterator ib = banks->begin();ib!=banks->end();ib++)
    {
      bnkid = (*ib)->type();
      int  id;
      id = bnkid;
      tots += (*ib)->size();
      Banks[id].siz += (*ib)->size();
      if (!Banks[id].noprofile)
      {
        Banks[id].p[0]->fill( (double) (*ib)->sourceID() , (double) (*ib)->size());
        if (nolumi)
        {
         Banks[id].p[1]->fill( (double) (*ib)->sourceID() , (double) (*ib)->size());
        }
        else
        {
         Banks[id].p[2]->fill( (double) (*ib)->sourceID() , (double) (*ib)->size());
        }
      }
    }

    if (Banks[*it].siz > 0.0)
    {
      double x = Banks[*it].siz;
      if (!Banks[*it].nohist)
      {
/*
        if (x <Banks[*it].xmin)
        {
          x = Banks[*it].xmin-Banks[*it].binw/2.0;
        }
        else if(x > Banks[*it].xmax)
        {
          x = Banks[*it].xmax+Banks[*it].binw/2.0;
        }
*/
        Banks[*it].h[0]->fill(x);
        if (nolumi)
        {
          Banks[*it].h[1]->fill(x);
        }
        else
        {
          Banks[*it].h[2]->fill(x);
        }
      }
      p_banks[0]->fill((double)(*it),Banks[*it].siz);
      if (nolumi)
      {
        p_banks[1]->fill((double)(*it),Banks[*it].siz);
      }
      else
      {
        p_banks[2]->fill((double)(*it),Banks[*it].siz);
      }

    }
  }
  for(std::vector<LHCb::RawBank::BankType>::const_iterator it = m_bankTypes.begin();it!=m_bankTypes.end();it++)
  {
    std::string d = m_hparams[*it].det;
    detbmiter j = m_detectors.find(d);
    j->second->sum += Banks[*it].siz;
  }
  for (detbmiter j=m_detectors.begin();j!=m_detectors.end();j++)
  {
    double x = j->second->sum;
    if (x>0)
    {
/*
      if (j->second->sum < j->second->xmin)
      {
        x = j->second->xmin+j->second->binw/2.0;
      }
      else if(j->second->sum > j->second->xmax)
      {
        x = j->second->xmax-j->second->binw/2.0;
      }
*/
      j->second->h[0]->fill(x);
      if (nolumi)
      {
        j->second->h[1]->fill(x);
      }
      else
      {
        j->second->h[2]->fill(x);
      }
    }
  }


  double total = tots;
  if (total >0)
  {
    detbmiter j = m_detectors.find("HLT");
    double rat = j->second->sum/total;
    h_totev[0]->fill(total);
    h_HLTratio[0]->fill(rat);
    if (nolumi)
    {
      h_totev[1]->fill(total);
      h_HLTratio[1]->fill(rat);
    }
    else
    {
      h_totev[2]->fill(total);
      h_HLTratio[2]->fill(rat);
    }
  }
  return StatusCode::SUCCESS;
}


//=============================================================================
//  Finalize
//=============================================================================
StatusCode RawBankSizes::finalize()
{
  printf("Number of Events Seen %d\n",n_ev);
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiHistoAlg::finalize();  // must be called after all other actions
}

//=============================================================================
