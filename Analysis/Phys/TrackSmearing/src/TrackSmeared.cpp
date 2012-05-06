// $Id: $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "TrackInterfaces/ITrackFitter.h"

// local
#include "TrackSmeared.h"
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/assign/list_of.hpp>
#include <TRandom3.h>


//-----------------------------------------------------------------------------
// Implementation file for class : TrackSmeared
//
// 2010-08-27 : Sascha Stahl
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TrackSmeared );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackSmeared::TrackSmeared( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputLocation",m_trackLocation);
  declareProperty( "OutputLocation",m_outputLocation);
  declareProperty( "Seed",m_seed = 4357);
  declareProperty( "xpar1_mc",m_xpar1_mc = 7.6);
  declareProperty( "xpar2_mc",m_xpar2_mc = 24.8);
  declareProperty( "xpar1_data",m_xpar1_data = 9.2 );
  declareProperty( "xpar2_data",m_xpar2_data = 28.3 );
  declareProperty( "ypar1_mc",m_ypar1_mc = 8.0);
  declareProperty( "ypar2_mc",m_ypar2_mc = 24.4);
  declareProperty( "ypar1_data",m_ypar1_data = 9.0 );
  declareProperty( "ypar2_data",m_ypar2_data = 27.6 );
  declareProperty( "makePlots",m_plots = false );
  declareProperty( "smear",m_smear = true);
  declareProperty( "Scale",m_scale = 1.0);
  declareProperty( "usePhi",m_usePhi = true );
  declareProperty( "smearBest", m_smearBest = false);
  declareProperty( "smearProto", m_smearProto = false );

  // Parameters for MC10 and Reco12
  /*
    

  declareProperty( "paramsx_data",m_paramsx_data=
                   boost::assign::list_of< std::pair<double,double> >
                   (7.39663,25.375)(11.9523,30.8468)(12.1657,31.0318)(6.80381,26.2239)
                   (6.31307,26.1924)(12.0715,30.4908)(13.968,29.4501)(8.58082,24.9248) );
  declareProperty( "paramsy_data",m_paramsy_data=
                   boost::assign::list_of< std::pair<double,double> >
                   (11.3659,23.9497)(9.61021,29.9749)(7.62483,31.652)(9.04329,26.1887) 
                   (10.1667,24.9057)(10.4476,28.463)(7.64104,30.8264)(12.5512,23.7025) );
  declareProperty( "paramsx_mc",m_paramsx_mc=
                   boost::assign::list_of< std::pair<double,double> >
                   (5.94179,19.298)(10.4385,29.5275)(9.38674,30.6793)(6.45098,19.2148)
                   (7.05916,18.8357)(9.35444,29.9623)(8.79601,30.6255)(6.60782,18.9007) );
  declareProperty( "paramsy_mc",m_paramsy_mc=
                   boost::assign::list_of< std::pair<double,double> >
                   (9.85327,19.1644)(6.61456,28.2261)(6.21135,29.2543)(10.0675,19.4316)
                   (9.74202,19.0929)(6.13729,28.6578)(5.31868,29.8177)(10.6302,18.7859) );
  */
  // Parameters for MC11 and Stripping17, don't mix them as the method to exctract them was changed.
  declareProperty( "paramsx_data",m_paramsx_data=
                   boost::assign::list_of< std::pair<double,double> >
                   (15.8007 , 21.5995 )(17.083 , 26.8743 )(17.188 , 26.9004 )(15.2431 , 21.8942 )
                   (15.3789 , 21.6944 )(17.3643 , 26.0733 )(18.1501 , 26.4945 )(15.9901 , 21.751 ));
  
  declareProperty( "paramsy_data",m_paramsy_data=
                   boost::assign::list_of< std::pair<double,double> >
                   (15.4397 , 22.0227 )(17.2131 , 25.725 )(17.3457 , 25.8134 )(15.1637 , 22.3395 )
                   (15.4863 , 21.9516 )(17.8457 , 24.7536 )(17.6028 , 25.6401 )(15.5212 , 22.0329 ) );
  declareProperty( "paramsx_mc",m_paramsx_mc=
                   boost::assign::list_of< std::pair<double,double> >
                   (12.241 , 16.793 )(15.4021 , 26.2156 )(15.0332 , 26.9101 )(12.4106 , 17.1177 )
                   (12.9271 , 16.7202 )(14.9721 , 26.2175 )(15.4448 , 26.4068 )(12.0711 , 17.2668 ) );
  declareProperty( "paramsy_mc",m_paramsy_mc=
                   boost::assign::list_of< std::pair<double,double> >
                   (14.7947 , 17.0342 )(12.9836 , 25.0856 )(12.7721 , 25.6032 )(15.0134 , 17.299 )
                   (14.652 , 17.3152 )(13.187 , 24.8611 )(12.8251 , 25.2981 )(14.0403 , 17.3576 ) );
  

  m_funcsy_mc.reserve(m_paramsy_mc.size());
  m_funcsx_mc.reserve(m_paramsx_mc.size());
  m_funcsy_data.reserve(m_paramsy_data.size());
  m_funcsx_data.reserve(m_paramsx_data.size());
  

}

//=============================================================================
// Destructor
//=============================================================================
TrackSmeared::~TrackSmeared() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackSmeared::initialize() {
  StatusCode sc = DVAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  info()<< "==> Initialize" << endmsg;
  info()<< "++++++++++++++++" << endmsg;
  info()<< "smear: "<< m_smear << endmsg;
  info()<< "scale factor: "<< m_scale << endmsg;
  info()<< "Random seed: "<< m_seed << endmsg;
  info()<< "use Phi dependency: " << m_usePhi << endmsg;
  info()<< "smear Tracks in Best Container: "<< m_smearBest << endmsg;
  info()<< "++++++++++++++++" << endmsg;

  //m_trackFitter = tool<ITrackFitter>("TrackInitFit");

  m_funcx_data = new TF1("funcx_data","[0]/1000.0+[1]*x",0.0,1.0);
  m_funcx_data->SetParameter(0,m_xpar1_data);
  m_funcx_data->SetParameter(1,m_xpar2_data);

  m_funcx_mc = new TF1("funcx_mc","[0]/1000.0+[1]*x",0.0,1.0);
  m_funcx_mc->SetParameter(0,m_xpar1_mc);
  m_funcx_mc->SetParameter(1,m_xpar2_mc);

  m_funcy_data = new TF1("funcy_data","[0]/1000.0+[1]*x",0.0,1.0);
  m_funcy_data->SetParameter(0,m_ypar1_data);
  m_funcy_data->SetParameter(1,m_ypar2_data);

  m_funcy_mc = new TF1("funcy_mc","[0]/1000.0+[1]*x",0.0,1.0);
  m_funcy_mc->SetParameter(0,m_ypar1_mc);
  m_funcy_mc->SetParameter(1,m_ypar2_mc);

  m_rnd=new TRandom3();
  m_rnd->SetSeed(m_seed);
  if(m_paramsx_data.size()!= m_paramsy_data.size() || m_paramsx_mc.size()!= m_paramsy_mc.size() || 
     m_paramsx_mc.size()!= m_paramsx_data.size() || m_paramsy_mc.size()!= m_paramsy_data.size() ||
     m_paramsx_data.size()!= m_paramsy_mc.size() || m_paramsx_mc.size()!= m_paramsy_data.size()  )
    return StatusCode::FAILURE;
  for(unsigned int i = 0; i< m_paramsx_data.size(); i++){
    m_funcsx_mc.push_back(new TF1(Form("funcx%d_mc",i),"[0]/1000.0+[1]*x",0.0,1.0));
    m_funcsx_mc[i]->SetParameter(0,m_paramsx_mc[i].first);
    m_funcsx_mc[i]->SetParameter(1,m_paramsx_mc[i].second);
  
    m_funcsy_mc.push_back( new TF1(Form("funcy%d_mc",i),"[0]/1000.0+[1]*x",0.0,1.0));
    m_funcsy_mc[i]->SetParameter(0,m_paramsy_mc[i].first);
    m_funcsy_mc[i]->SetParameter(1,m_paramsy_mc[i].second);
  
    m_funcsx_data.push_back( new TF1(Form("funcx%d_data",i),"[0]/1000.0+[1]*x",0.0,1.0));
    m_funcsx_data[i]->SetParameter(0,m_paramsx_data[i].first);
    m_funcsx_data[i]->SetParameter(1,m_paramsx_data[i].second);
    

    m_funcsy_data.push_back( new TF1(Form("funcy%d_data",i),"[0]/1000.0+[1]*x",0.0,1.0));
    m_funcsy_data[i]->SetParameter(0,m_paramsy_data[i].first);
    m_funcsy_data[i]->SetParameter(1,m_paramsy_data[i].second);
    
  }
    

  return StatusCode::SUCCESS;
}

double TrackSmeared::getSigmax( LHCb::State* state)
{
  double pt = state->pt();
  double inversept = pt > 0 ? 1.0/pt : 0.0;
  double sigma2 = m_funcx_data->Eval(inversept)*m_funcx_data->Eval(inversept)-
    m_funcx_mc->Eval(inversept)*m_funcx_mc->Eval(inversept);
  return sigma2>0 ? m_scale*sqrt(sigma2) : 0.0;

}

double TrackSmeared::getSigmay( LHCb::State* state)
{
  double pt = state->pt();
  double inversept = pt > 0 ? 1.0/pt : 0.0;
  double sigma2 = m_funcy_data->Eval(inversept)*m_funcy_data->Eval(inversept)-
    m_funcy_mc->Eval(inversept)*m_funcy_mc->Eval(inversept);
  return sigma2>0 ? m_scale*sqrt(sigma2) : 0.0;
}

double TrackSmeared::getSigma( LHCb::State* state,  std::vector<TF1*> funcs_data, std::vector<TF1*> funcs_mc)
{
  double pt = state->pt();
  double phi = state->momentum().phi() > 0 ?  state->momentum().phi() :  state->momentum().phi() + 2*M_PI;
  double inversept = pt > 0 ? 1.0/pt : 0.0;
  int phiBin = (int)(phi/(2*M_PI)*funcs_data.size());
  TF1* func_data = funcs_data[phiBin];
  TF1* func_mc = funcs_mc[phiBin];
  double sigma2 = func_data->Eval(inversept)*func_data->Eval(inversept)-
    func_mc->Eval(inversept)*func_mc->Eval(inversept);
  return sigma2>0 ? m_scale*sqrt(sigma2) : 0.0;


}


void TrackSmeared::smearStates(  const std::vector< LHCb::State * > & states )
{
  const double tx = states[0]->tx();
  const double alpha = atan(tx);
  const double ty = states[0]->ty();
  const double gamma = atan(ty);
  double xrand = 0.0;
  double yrand = 0.0;
  debug()<<"smear: "<<m_smear<<endmsg;
  double sigmax = getSigma(states[0],m_funcsx_data,m_funcsx_mc);
  double sigmay = getSigma(states[0],m_funcsy_data,m_funcsy_mc);

  if (m_smear)
  {
    xrand = m_rnd->Gaus(0.0,sigmax)/(cos(alpha)*cos(alpha));
    yrand = m_rnd->Gaus(0.0,sigmay)/(cos(gamma)*cos(gamma));
    if (m_plots){
      plot(xrand,"xrand",-0.6,0.6,100);
      plot(yrand,"yrand",-0.6,0.6,100);
      plot(cos(alpha)*cos(alpha),"cos2alpha",0.0,1.0,100);
      plot(cos(gamma)*cos(gamma),"cos2gamma",0.0,1.0,100);

    }
    debug()<<"rnd: "<<xrand<<" "<<yrand<<endmsg;
  }
  BOOST_FOREACH(LHCb::State* st ,states){
    const double x = st->x();
    const double y = st->y();
    st->setX(x+xrand);
    st->setY(y+yrand);
  }


}



ROOT::Math::Plane3D::Vector TrackSmeared::Ip_vector(const LHCb::State *state, const LHCb::VertexBase* vtx )
{
  ROOT::Math::Plane3D::Point pv = vtx->position();
  ROOT::Math::Plane3D::Point sv = state->position();      // secondary vertex = production vertex of this_particle
  ROOT::Math::Plane3D::Vector vector(state->momentum());
  ROOT::Math::Plane3D plane1(vector, pv); //-----Ebene konstruiert aus normale zu vector und pv
  ROOT::Math::Plane3D::Point point_Ip = plane1.ProjectOntoPlane(sv);
  ROOT::Math::Plane3D::Vector Ip = pv - point_Ip;

  return Ip;
}


//=============================================================================
// Main execution
//=============================================================================
StatusCode TrackSmeared::execute() {

  debug()<<"==> Execute! "<<endmsg;


  if  (!exist<LHCb::Track::Range>(m_trackLocation))  return StatusCode::FAILURE;
  if  (!exist<LHCb::RichPIDs>(LHCb::RichPIDLocation::Offline))  return StatusCode::FAILURE;
  if  (!exist<LHCb::ProtoParticle::Container>(LHCb::ProtoParticleLocation::Charged)) return StatusCode::FAILURE;

  LHCb::Tracks* newTracks = new LHCb::Tracks();
  LHCb::RichPIDs* newRichIDs = new LHCb::RichPIDs();
  
  put(newTracks, "Rec/Track/"+m_outputLocation);
  put(newRichIDs,"Rec/Rich/"+m_outputLocation);

  
  LHCb::Track::Range tracks = get<LHCb::Track::Range>(m_trackLocation) ;
  LHCb::RichPIDs* richids  = get<LHCb::RichPIDs>(LHCb::RichPIDLocation::Offline);
  LHCb::ProtoParticles* pp = get< LHCb::ProtoParticles >(LHCb::ProtoParticleLocation::Charged) ;

  const LHCb::RecVertex::Range vtcs = primaryVertices();
  const double nPV = vtcs.size();
  if (m_plots){
    plot(vtcs.size(),"numberPVs",0.0,5.0,6);
    BOOST_FOREACH(const LHCb::VertexBase* vtx, vtcs) {
      plot(vtx->position().x(),"pv_x",10.0,10.0,100);
      plot(vtx->position().y(),"pv_y",10.0,10.0,100);
      plot(vtx->position().z(),"pv_z",150.0,150.0,100);
    }
  }
  debug()<<"Number of pvs: "<<vtcs.size()<<endmsg;

  if (m_smearProto){
    BOOST_FOREACH(const LHCb::ProtoParticle* proto, *pp){
      const std::vector< LHCb::State * > & states = proto->track()->states();
      always()<<"proto before p:"<<proto->track()->states()[0]->p()<<".x: "<< proto->track()->states()[0]->x()<<endmsg;
      smearStates(states);
      always()<<"proto after p:"<<proto->track()->states()[0]->p()<<".x: "<< proto->track()->states()[0]->x()<<endmsg;
    }
  }
  

  BOOST_FOREACH(const LHCb::Track* tr ,tracks)
  {
    LHCb::Track* smearedtrack = new LHCb::Track();
    //info()<<"test"<<endmsg;
    smearedtrack->copy(*tr);
    //smearedtrack = tr->cloneWithKey();

    LHCb::RichPID* myid = 0 ;

    BOOST_FOREACH(LHCb::RichPID* pid , *richids) {
      if (pid->track() == tr) {
        myid = pid;
        break;
      }
    }
    LHCb::RichPID* neueid = myid;
    if (neueid!=0){
      neueid->setTrack(smearedtrack);
      newRichIDs->add(neueid);
    }

    if (!tr->type() || LHCb::Track::Long != tr->type() ){
      debug()<< "No Long track! Not smeared!" <<endmsg;
      newTracks->add(smearedtrack);
    } else {
      // smear state
      const std::vector< LHCb::State * > & states = smearedtrack->states();
      const std::vector< LHCb::State * > & states2 = tr->states();
      if (states.size()>0){
        ROOT::Math::Plane3D::Vector relIP(std::numeric_limits<double>::infinity(),
                                          std::numeric_limits<double>::infinity(),
                                          std::numeric_limits<double>::infinity());

        const LHCb::VertexBase* relPV = 0;
        if (m_plots){
          BOOST_FOREACH(const LHCb::VertexBase* vtx, vtcs) {
            ROOT::Math::Plane3D::Vector IP_vec = Ip_vector(states[0],vtx);
            if (IP_vec.R()<relIP.R()) {
              relIP = IP_vec;
              relPV = vtx;
            }
          }
          if (relPV){
            plot(relIP.R(),"ip",0.0,1.0,100);
            plot(relIP.y(),"ipy",-1.0,1.0,100);
            plot(relIP.z(),"ipz",-1.0,1.0,100);
            plot(relIP.x(),"ipx",-1.0,1.0,100);
            if (states[0]->pt()>0.0){
              plot(1.0/states[0]->pt(),"inversePt",0.0,0.003);
              profile1D(1.0/states[0]->pt(),relIP.R(),"ip_profile",0.0,0.003,100);
              profile1D(1.0/states[0]->pt(),std::abs(relIP.x()),"ipx_profile",0.0,0.003,100);
              profile1D(1.0/states[0]->pt(),std::abs(relIP.y()),"ipy_profile",0.0,0.003,100);
            }
          }
        }


        if (!m_usePhi){
          double sigmax = getSigmax(states[0]);
          double sigmay = getSigmay(states[0]);
          if (m_plots){
            plot(sigmax,"sigmax",0.0,1.0,100);
            plot(sigmay,"sigmay",0.0,1.0,100);
          }

          debug()<<"pt: "<<states[0]->pt()<<" =>sigmax "<<sigmax<<endmsg;
          debug()<<"    "<<states[0]->pt()<<" =>sigmay "<<sigmay<<endmsg;
          debug()<<"x position "<<states[0]->x()<<endmsg;
          debug()<<"y position "<<states[0]->y()<<endmsg;
          debug()<<"z position "<<states[0]->z()<<endmsg;
          
          const double tx = states[0]->tx();
          const double alpha = atan(tx);
          const double ty = states[0]->ty();
          const double gamma = atan(ty);
          double xrand = 0.0;
          double yrand = 0.0;
          debug()<<"smear: "<<m_smear<<endmsg;
          if (m_smear)
          {
            xrand = m_rnd->Gaus(0.0,sigmax)/(cos(alpha)*cos(alpha));
            yrand = m_rnd->Gaus(0.0,sigmay)/(cos(gamma)*cos(gamma));
            if (m_plots){
              plot(xrand,"xrand",-0.6,0.6,100);
              plot(yrand,"yrand",-0.6,0.6,100);
              plot(cos(alpha)*cos(alpha),"cos2alpha",0.0,1.0,100);
              plot(cos(gamma)*cos(gamma),"cos2gamma",0.0,1.0,100);
              
            }
          debug()<<"rnd: "<<xrand<<" "<<yrand<<endmsg;
          }
          BOOST_FOREACH(LHCb::State* st ,states){
            const double x = st->x();
            const double y = st->y();
            st->setX(x+xrand);
            st->setY(y+yrand);
          }
        }else{
          smearStates(states);
          if (m_smearBest) {
            always()<<"Before p:"<<tr->states()[0]->p()<<".x: "<< tr->states()[0]->x()<<endmsg;
            smearStates(states2);
            always()<<"After p:"<<tr->states()[0]->p()<<".x: "<< tr->states()[0]->x()<<endmsg;
                  
          }
        }
        
        
        if (m_plots && relPV){
          relIP = Ip_vector(states[0],relPV);
          plot(relIP.R(),"smeared_ip",0.0,1.0,100);
          plot(relIP.y(),"smeared_ipy",-1.0,1.0,100);
          plot(relIP.z(),"smeared_ipz",-1.0,1.0,100);
          plot(relIP.x(),"smeared_ipx",-1.0,1.0,100);
          if (states[0]->pt() > 0.0 && nPV == 1){
            profile1D(1.0/states[0]->pt(),relIP.R(),"smeared_ip_profile",0.0,0.003,100);
            profile1D(1.0/states[0]->pt(),std::abs(relIP.x()),"smeared_ipx_profile",0.0,0.003,100);
            profile1D(1.0/states[0]->pt(),std::abs(relIP.y()),"smeared_ipy_profile",0.0,0.003,100);
          }
        }
        debug()<<"new x position "<<states[0]->x()<<endmsg;
        debug()<<"new y position "<<states[0]->y()<<endmsg;
        debug()<<"new z position "<<states[0]->z()<<endmsg;
      }
      newTracks->add(smearedtrack);
    }
  }

  debug() << "adding Container to TES " <<m_outputLocation<< endmsg;
  //put(newRichIDs,"Rec/Rich/Refit");

  setFilterPassed(true);  // Mandatory. Set to true if event is accepted.
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TrackSmeared::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  delete m_funcx_data;
  delete m_funcx_mc;
  delete m_funcy_data;
  delete m_funcy_mc;
  delete m_rnd;
  

  for(unsigned int i = 0; i< m_paramsx_data.size(); i++){

    delete m_funcsx_mc[i];
    delete m_funcsy_mc[i];
    delete m_funcsx_data[i];
    delete m_funcsy_data[i];
  }

  return DVAlgorithm::finalize();
}

//=============================================================================
