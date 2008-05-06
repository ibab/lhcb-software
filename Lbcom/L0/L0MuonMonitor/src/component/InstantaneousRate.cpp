// $Id: InstantaneousRate.cpp,v 1.1 2008-05-06 12:18:20 jucogan Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

#include "Event/RawEvent.h"
#include "Event/ODIN.h"

// local
#include "InstantaneousRate.h"

//-----------------------------------------------------------------------------
// Implementation file for class : InstantaneousRate
//
// 2008-04-16 : 
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( InstantaneousRate );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
InstantaneousRate::InstantaneousRate( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : GaudiHistoTool ( type, name , parent )
{
  declareInterface<InstantaneousRate>(this);

  declareProperty( "MaxRate"  , m_max = 5);

  m_previous_orbit=0;
  m_previous_event=0;  
  m_hrate=NULL;
  m_hrate2=NULL;
  m_hbid=NULL;
  m_hbid2=NULL;

}
//=============================================================================
// Destructor
//=============================================================================
InstantaneousRate::~InstantaneousRate() {} 
//=============================================================================

void InstantaneousRate::bookHistos()
{
  std::string hname;
  hname  = name()+"_Instantaneous Rate(Hz)";
  m_hrate = book1D(hname,hname,0.,m_max,100);
  hname  = name()+"_Instantaneous Rate (Hz)";
  m_hrate2 = book1D(hname,hname,0.,50,500);
  hname  = name()+"_L0_B_Id";
  m_hbid  = book1D(hname,hname,-0.5,-0.5+3564,3564);
  hname  = name()+"_L0_B_Id_coarse";
  m_hbid2  = book1D(hname,hname,-0.5,-0.5+3564,89);
  hname  = name()+"_DeltaT (ms)/20ms";
  m_hdt = book1D(hname,hname,-0.5,10000.-0.5,500);
}

void InstantaneousRate::fillHistos()
{
  double freq= rate();
  fill(m_hrate,freq,1);
  fill(m_hrate2,freq,1);

  int bid=bunchId();
  fill(m_hbid,bid,1);
  fill(m_hbid2,bid,1);

  int dt=deltaT();
  fill(m_hdt,dt,1);
}

int InstantaneousRate::deltaT(){

  int dt=-1;

  if (exist<LHCb::RawEvent> (LHCb::RawEventLocation::Default)) {
    LHCb::RawEvent* rawEvt = get<LHCb::RawEvent>( LHCb::RawEventLocation::Default );
    const std::vector<LHCb::RawBank*>& banks = rawEvt->banks( LHCb::RawBank::ODIN );
    if (banks.size()==1){
      std::vector<LHCb::RawBank*>::const_iterator itBnk = banks.begin();
      LHCb::ODIN odin;
      odin.set(*itBnk);
      ulonglong event =odin.eventNumber() ;
      unsigned int orbit =odin.orbitNumber();
      //       debug()<<"Current event is "<<event<<" occuring @ orbit# "<<orbit
      //              <<"  previous was "<<m_previous_event<<" @ "<<m_previous_orbit<<endmsg;
      dt = (orbit-m_previous_orbit)*(25.e-6*3564.);
      m_previous_orbit = orbit;
      m_previous_event = event;
    } else {
      error()<<"More than 1 ODIN bank  ("<<banks.size()<<")"<<endmsg;
    }
  } else {
    error()<<LHCb::RawEventLocation::Default<<"not found @"<<rootInTES()<<endmsg;
  }
  return dt;
}

double InstantaneousRate::rate(){

  double instantaneousRate=-1;

  if (exist<LHCb::RawEvent> (LHCb::RawEventLocation::Default)) {
    LHCb::RawEvent* rawEvt = get<LHCb::RawEvent>( LHCb::RawEventLocation::Default );
    const std::vector<LHCb::RawBank*>& banks = rawEvt->banks( LHCb::RawBank::ODIN );
    if (banks.size()==1){
      std::vector<LHCb::RawBank*>::const_iterator itBnk = banks.begin();
      LHCb::ODIN odin;
      odin.set(*itBnk);
      ulonglong event =odin.eventNumber() ;
      unsigned int orbit =odin.orbitNumber();
      //       debug()<<"Current event is "<<event<<" occuring @ orbit# "<<orbit
      //              <<"  previous was "<<m_previous_event<<" @ "<<m_previous_orbit<<endmsg;
      unsigned int delta_orbit=orbit-m_previous_orbit;
      if (delta_orbit==0) delta_orbit=1;
      instantaneousRate = (1.e9/(25.*3564.))/(delta_orbit);
      m_previous_orbit = orbit;
      m_previous_event = event;
    } else {
      error()<<"More than 1 ODIN bank  ("<<banks.size()<<")"<<endmsg;
    }
  } else {
    error()<<LHCb::RawEventLocation::Default<<"not found @"<<rootInTES()<<endmsg;
  }
  return instantaneousRate;
}

int InstantaneousRate::bunchId(){

  int bunch=-1;

  if (exist<LHCb::RawEvent> (LHCb::RawEventLocation::Default)) {
    LHCb::RawEvent* rawEvt = get<LHCb::RawEvent>( LHCb::RawEventLocation::Default );
    const std::vector<LHCb::RawBank*>& banks = rawEvt->banks( LHCb::RawBank::ODIN );
    if (banks.size()==1){
      std::vector<LHCb::RawBank*>::const_iterator itBnk = banks.begin();
      LHCb::ODIN odin;
      odin.set(*itBnk);
      bunch =odin.bunchId() ;
    } else {
      error()<<"More than 1 ODIN bank  ("<<banks.size()<<")"<<endmsg;
    }
  } else {
    error()<<LHCb::RawEventLocation::Default<<"not found @"<<rootInTES()<<endmsg;
  }
  return bunch;
}
