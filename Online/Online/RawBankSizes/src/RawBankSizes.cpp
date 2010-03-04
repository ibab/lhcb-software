// $Id: RawBankSizes.cpp,v 1.13 2010-03-04 08:35:44 frankb Exp $
// Include files 

// from Gaudi
#include <vector>
#include <map>
#include <string>
#include <utility>
#include "RawBankSizes.h"
#include "GaudiOnline/OnlineService.h"
#include "MDF/OnlineRunInfo.h"
//#include "Event/ODIN.h"

// local

//-----------------------------------------------------------------------------
// Implementation file for class : RawBankSizes
//
// 2008-03-27 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_NAMESPACE_ALGORITHM_FACTORY( LHCb,RawBankSizes);

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RawBankSizes::RawBankSizes( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
{
//	memset(m_hparams,0,sizeof(m_hparams));
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
StatusCode RawBankSizes::initialize() 
{
  int i;
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
       
       Banks[i].h	= histoSvc()->book( Banks[i].h_name,Banks[i].name+" Size in Bytes",m_hparams[i].n_bin+2,
    		m_hparams[i].xmin-binw,m_hparams[i].xmax+binw);
       declareInfo(Banks[i].h_name, Banks[i].h,Banks[i].name+" Size in Bytes");
    }
    else
    {
       printf("INFO: No Histogram booked for Bank %s\n",Banks[i].name.c_str());
    }
    if (!Banks[i].noprofile)
    {
       Banks[i].p	= histoSvc()->bookProf( Banks[i].p_name,Banks[i].name+" Size vs. SourceID",
    		m_hparams[idx].s_max-m_hparams[idx].s_min+1,
    		(float)m_hparams[idx].s_min,(float)m_hparams[idx].s_max);
       declareInfo(Banks[i].p_name, Banks[i].p,Banks[i].name+" Size vs. SourceID");
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
    printf("Binwidth for %s not a multiple of 4. %f\n",Banks[i].name.c_str(),totsize.binw);
   }
   h_totev = histoSvc()->book( "TotSize","Total Event Size in Bytes",m_totevpar.n_bin+2,
		   m_totevpar.xmin-totsize.binw,m_totevpar.xmax+totsize.binw);
   declareInfo("TotSize", h_totev,"Total Event Size in Bytes");
   p_banks = histoSvc()->bookProf( "Banksp","Bank Size vs. Bank Number",
    		 LHCb::RawBank::LastType+1, -1.0, (float)LHCb::RawBank::LastType);
   declareInfo("Banksp",p_banks,"Bank Size vs. Bank Number");
   for (dethmiter j=m_dethparams.begin();j!=m_dethparams.end();j++)
   {
      detbmiter bd = m_detectors.find(j->first);
      bd->second->xmin = j->second->xmin; 
      bd->second->xmax = j->second->xmax; 
      bd->second->nbin = j->second->n_bin; 
      bd->second->binw = (j->second->xmax-j->second->xmin)/j->second->n_bin; 
      if (((int)bd->second->binw % 4) != 0)
      {
       printf("Binwidth for %s not a multiple of 4. %f\n",Banks[i].name.c_str(),bd->second->binw);
      }
      bd->second->h = histoSvc()->book("Total_"+j->first,"Total Size for "+j->first, j->second->n_bin+2,
        j->second->xmin-bd->second->binw,j->second->xmax+bd->second->binw);
      declareInfo("Total_"+j->first, bd->second->h,"Total Size for "+j->first);
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
    
    std::string bname = LHCb::RawBank::typeName( *it );
    Banks[*it].siz = 0.0;
  }
  for(std::vector<LHCb::RawBank::BankType>::const_iterator it = m_bankTypes.begin();it!=m_bankTypes.end();it++)
  {
    const std::vector<LHCb::RawBank*>* banks= &m_rawEvt->banks(  *it );
	  for(std::vector<LHCb::RawBank*>::const_iterator ib = banks->begin();ib!=banks->end();ib++)
	  {
	    bnkid = (*ib)->type();
		  if (bnkid == LHCb::RawBank::ODIN)
		  {
		    //unsigned int *dat = (*ib)->data();
		    //LHCb::ODIN::TriggerType tt = (LHCb::ODIN::TriggerType)((dat[LHCb::ODIN::Word8] & LHCb::ODIN::TriggerTypeMask)>>16);
		    //nolumi = (tt != LHCb::ODIN::LumiTrigger);
		    const OnlineRunInfo* dat = (*ib)->begin<OnlineRunInfo>();
		    nolumi = (dat->triggerType != 2); // 2 = LumiTrigger
		  }
		  int  id;
		  id = bnkid;
		  tots += (*ib)->size();
		  Banks[id].siz += (*ib)->size();
      if (!Banks[id].noprofile)
      {
        Banks[id].p->fill( (double) (*ib)->sourceID() , (double) (*ib)->size());
      }
	  }

	  if (Banks[*it].siz > 0.0)
	  {
      if (!Banks[*it].nohist)
      {
        if (Banks[*it].siz <Banks[*it].xmin)
        {
          Banks[*it].h->fill(Banks[*it].xmin-Banks[*it].binw/2.0);
        }
        else if(Banks[*it].siz > Banks[*it].xmax)
        {
          Banks[*it].h->fill(Banks[*it].xmax+Banks[*it].binw/2.0);
        }
        else
        {
		      Banks[*it].h->fill(Banks[*it].siz);
        }
      }
      p_banks->fill((double)(*it),Banks[*it].siz);
	  }
  }
  if (nolumi)
  {
    for(std::vector<LHCb::RawBank::BankType>::const_iterator it = m_bankTypes.begin();it!=m_bankTypes.end();it++)
    {
      std::string d = m_hparams[*it].det;
      detbmiter j = m_detectors.find(d);
      j->second->sum += Banks[*it].siz; 
    }
    for (detbmiter j=m_detectors.begin();j!=m_detectors.end();j++)
    {
      if (j->second->sum < j->second->xmin)
      {
        j->second->h->fill(j->second->xmin-j->second->binw/2.0);
      }
      else if(j->second->sum > j->second->xmax)
      {
        j->second->h->fill(j->second->xmax+j->second->binw/2.0);
      }
      else
      {
		    j->second->h->fill(j->second->sum);
      }
    }
  }

  
  double total = tots;
  if (nolumi)
  {
	  h_totev->fill(total);
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
