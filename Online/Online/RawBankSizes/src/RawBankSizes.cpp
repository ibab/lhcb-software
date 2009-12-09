// $Id: RawBankSizes.cpp,v 1.8 2009-12-09 10:08:11 jost Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "RawBankSizes.h"

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
  //  printf("New Version with DOUBLE arithmetic...\n");
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
   }
   m_firstevent = true;
   first_few = true;
   n_ev = 0;
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
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiHistoAlg

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

//   Banks = new BankDescr[(int) LHCb::RawBank::LastType];

  // convert bankNames to bankTypes
  for(std::vector<std::string>::iterator it = m_bankNames.begin();it!=m_bankNames.end();it++)
  {
    bool found = false;
    for(int i = 0 ; i != (int) LHCb::RawBank::LastType; i++)
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
  
   for(int i = 0 ; i != (int) LHCb::RawBank::LastType; i++)
   {
     Banks[i].init(i);
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
  int bsize[LHCb::RawBank::LastType];
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
//
// First try finding out the scale of the histograms. taking the first few hundred events
//
  if (first_few)
  {
    //printf("Running through first few events...\n");
    int tots;
    tots = 0;
    for(int i = 0 ; i != (int) LHCb::RawBank::LastType; i++)
    {
      bsize[i] = 0;
    }
    for(std::vector<LHCb::RawBank::BankType>::const_iterator it = m_bankTypes.begin();it!=m_bankTypes.end();it++)
    {
      const std::vector<LHCb::RawBank*>* banks= &m_rawEvt->banks(  *it );
      for(std::vector<LHCb::RawBank*>::const_iterator ib = banks->begin();ib!=banks->end();ib++)
      {
        int source   = (*ib)->sourceID();
        int siz;
        Banks[(*ib)->type()].setSID(source);
        siz = (*ib)->size();
        bsize[(*ib)->type()] += siz;
        tots+=siz;
      }
    }
    for(int i = 0 ; i != (int) LHCb::RawBank::LastType; i++)
    {
      Banks[i].fillstat(bsize[i]);
    }
    //printf ("Filling totsize: %d\n",tots);
    totsize.fillstat(tots);
    n_ev++;
    if (n_ev >= 100) first_few=false;
    return StatusCode::SUCCESS;
  }
  //
  // Loop over banks
  //
  n_ev++;
  int tots;
  tots = 0;
  int bnkid=0;
  for(std::vector<LHCb::RawBank::BankType>::const_iterator it = m_bankTypes.begin();it!=m_bankTypes.end();it++)
  {
    
    std::string bname = LHCb::RawBank::typeName( *it );
    int bs = 0;
    const std::vector<LHCb::RawBank*>* banks= &m_rawEvt->banks(  *it );
    for(std::vector<LHCb::RawBank*>::const_iterator ib = banks->begin();ib!=banks->end();ib++)
    {
      bnkid = (*ib)->type();
      tots += (*ib)->size();
      bs += (*ib)->size();
      int s_min, s_max, id;
      id = bnkid;
      if (Banks[id].isError)
      {
        id= Banks[id].rootbankidx;
      }
      s_min = Banks[id].min_source;
      s_max = Banks[id].max_source;
      profile1D( (double) (*ib)->sourceID() , (double) (*ib)->size()
        , (*ib)->typeName( (*ib)->type()) + " size vs. sourceID"
        , s_min , s_max+1, s_max-s_min+1);
    }
    if (!banks->empty())
    {
      double mean;
      double rms;
      double smin,smax;
      Banks[bnkid].stat(mean,rms,smin,smax);
      if (rms < 1.0) rms=0.2*mean;
      double bmin = mean-m_nRMS*rms;
      if (bmin<0.0) bmin=0.0;
      plot1D((double)bs,bname+" Size in Bytes",bmin,mean+m_nRMS*rms,m_bin);
    }
  }
  double tmean,trms,tsmin,tsmax,total;
  totsize.stat(tmean,trms,tsmin,tsmax);
  total = tots;
  //printf ("totsize Class: %f %f \n", totsize.sum, totsize.sum2);
  //printf("Total Size: %f %f %f\n",total,tmean,trms);
  if (trms < 1.0) trms=100.0;
  double bmin = tmean-m_nRMS*trms;
  if (bmin<0.0) bmin=0.0;
  plot1D(total,"Total Event Size [Bytes]",bmin,tmean+m_nRMS*trms,m_bin);
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
