// $Id: SVertexNNTool.cpp,v 1.7 2010-01-18 22:17:04 musy Exp $
#include "SVertexNNTool.h"
#include "Event/RecVertex.h"
//-----------------------------------------------------------------------------
// Implementation file for class : SVertexNNTool v1.3
//
// 2004-03-18 :
//-----------------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( SVertexNNTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SVertexNNTool::SVertexNNTool( const std::string& type,
                              const std::string& name,
                              const IInterface* parent ) :
  GaudiTool ( type, name, parent ) 
{
  declareInterface<ISecondaryVertexTool>(this);
}

StatusCode SVertexNNTool::initialize() 
{
  const StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;

  m_util = tool<ITaggingUtils> ( "TaggingUtils", this );

  fitter = m_util->getVertexFitter(); //tool<IVertexFit>("UnconstVertexFitter");

  return sc;
}

//=============================================================================
SVertexNNTool::~SVertexNNTool(){}

//=============================================================================
std::vector<Vertex> SVertexNNTool::buildVertex( const RecVertex& RecVert,
                                                const Particle::ConstVector& vtags )
{

  const double RVz = RecVert.position().z()/mm;

  //Build Up 2 Seed Particles For Vertexing ------------------------
  StatusCode sc;
  Vertex Vfit, vtx;
  std::vector<Vertex> vtxvect;
  const Particle* p1=0, *p2=0;
  Particle::ConstVector::const_iterator jp, kp, jpp;
  double ipl(0), iperrl(0), ips(0), iperrs(0), probf(-1.0) ;

  long iijp = 0;
  for ( jp = vtags.begin(); jp != vtags.end()-1; jp++, iijp++ )
  {

    const int jpid = (*jp)->particleID().abspid();
    if( jpid == 2212 ) continue;                                //preselection

    m_util->calcIP(*jp, &RecVert, ipl, iperrl);

    if( iperrl==0 ) continue;
    if( fabs(ipl/iperrl) < 2.0 ) continue;                      //preselection
    if( iperrl > 0.15 ) continue;                               //preselection
    if( fabs(ipl) > 4.0 ) continue;                             //preselection
    if( jpid==11 && fabs(ipl) > 2.5 ) continue;                 //preselection

    double lcs=1000.;
    const ProtoParticle* proto = (*jp)->proto();
    if (!proto) continue;
    const Track* track = proto->track();
    if((track->measurements()).size() > 5)
      lcs = track->chi2()/((track->measurements()).size()-5);
    if( lcs > 2.0 ) continue;                                   //preselection
    if( track->type() == Track::Long ) continue;                //preselection

    //SECOND particle ---------------------------------------
    long long iikp = iijp+1;
    for ( kp = (jp+1) ; kp != vtags.end(); kp++, iikp++ )
    {

      const int kpid = (*kp)->particleID().abspid();
      if( kpid == 2212 ) continue;                             //preselection

      m_util->calcIP(*kp, &RecVert, ips, iperrs);
      if( iperrs==0 ) continue;
      if( fabs(ips/iperrs) < 2.0 ) continue;                   //preselection
      if( iperrs > 0.15 ) continue;                            //preselection
      if( fabs(ips) > 4.0 ) continue;                          //preselection
      if( kpid==11 && fabs(ips) > 2.5 ) continue;              //preselection

      double lcs=1000.;
      const ProtoParticle* proto = (*kp)->proto();
      const Track* track = proto->track();
      if((track->measurements()).size() > 5)
        lcs = track->chi2()/((track->measurements()).size()-5);
      if( lcs > 2.0 ) continue;                                //preselection
      if( track->type() == Track::Long ) continue;             //preselection

      sc = fitter->fit( vtx , **jp, **kp );
      if( sc.isFailure() ) continue;
      if((vtx.position().z()/mm - RVz) < -10.0 ) continue;    //preselection

      //if the couple is compatible with a Ks, drop it          //preselection
      const Gaudi::LorentzVector sum = (*jp)->momentum() + (*kp)->momentum();
      if( sum.M()/GeV > 0.490 && sum.M()/GeV < 0.505
          &&  jpid == 211 &&  kpid == 211
          && ((*jp)->particleID().threeCharge())
          * ((*kp)->particleID().threeCharge()) < 0 ) 
      {
        if ( msgLevel(MSG::DEBUG) )
          debug() << "This is a Ks candidate! skip."<<endreq;
        //set their energy to 0
        //Gaudi::LorentzVector zero(0.0001,0.0001,0.0001,0.0001);
        //(*jp)->setMomentum(zero);//xxx
        //(*kp)->setMomentum(zero);
        continue;
      }

      //build a probability that the combination comes from B ---------
      //P1,P2,PT1,PT2,B0PT,IP1,IP2,JVZ,JCHI
      const double NNoutput = NNseeding( (*jp)->p()/GeV,
                                         (*kp)->p()/GeV,
                                         (*jp)->pt()/GeV,
                                         (*kp)->pt()/GeV,
                                         7.0,
                                         ipl/iperrl,
                                         ips/iperrs,
                                         vtx.position().z()/mm - RVz,
                                         vtx.chi2()/vtx.nDoF() )/2.0+0.5;
      if( NNoutput < 0.2 ) continue;                           //preselection
      if( NNoutput > probf )
      {
        probf = NNoutput;
        Vfit = vtx;
        p1 = (*jp);
        p2 = (*kp);
      }
    }
  }

  Vertex VfitTMP;
  Particle::ConstVector Pfit(0);

  if( probf > 0.8 ) 
  {

    //add iteratively other tracks to the seed ----------------------
    Pfit.push_back(p1);
    Pfit.push_back(p2);
    for(jpp = vtags.begin(); jpp != vtags.end(); jpp++){

      if( (*jpp) == p1 ) continue;
      if( (*jpp) == p2 ) continue;

      double ip(0), ipe(0);
      m_util->calcIP(*jpp, &RecVert, ip, ipe);
      if( ipe==0 ) continue;
      if( ip/ipe < 2.0 ) continue;                                       //cut
      if( ip/ipe > 100 ) continue;                                       //cut
      if( ipe > 1.5    ) continue;                                       //cut

      //likelihood for the particle to come from B ------
      const double probi1=ipprob(ip/ipe); //IP
      const double probp1=ptprob((*jpp)->pt()/GeV); // pt
      const double probs=probi1*probp1; // total
      const double probb=(1-probi1)*(1-probp1);
      if( probs/(probs+probb) < 0.2 ) continue;                          //cut

      sc = fitter->fit( VfitTMP , Pfit ); /////////FIT before
      if( !sc ) continue;

      m_util->calcIP(*jpp, &VfitTMP, ip, ipe);
      if( ipe==0 ) continue;
      if( ip/ipe > 5.0 ) continue;                                       //cut

      //Add track to the fit
      Pfit.push_back(*jpp);

      sc = fitter->fit( VfitTMP , Pfit ); /////////FIT after
      if( !sc ) { Pfit.pop_back(); continue; }

      if( VfitTMP.chi2() / VfitTMP.nDoF() > 5.0 )
      { Pfit.pop_back(); continue; }                                   //cut
      if( VfitTMP.position().z()/mm - RVz < 1.0 )
      { Pfit.pop_back(); continue; }                                   //cut

      if ( msgLevel(MSG::DEBUG) )
        debug()<<"---> chi="<< VfitTMP.chi2() / VfitTMP.nDoF()
               <<" Vz-RVz="<< VfitTMP.position().z()/mm - RVz
               <<" prob="<< probs/(probs+probb)
               <<" IPbef="<< ip/ipe
               <<endreq;

      //look what is the part which behaves worse
      double ipmax = -1.0;
      int ikpp = 0;
      bool worse_exist = false;
      Particle::ConstVector::iterator kpp, kpp_worse;

      for( kpp=Pfit.begin(); kpp!=Pfit.end(); kpp++, ikpp++ )
      {
        if( Pfit.size() < 3 ) break;

        Particle::ConstVector tmplist = Pfit;
        tmplist.erase( tmplist.begin() + ikpp );

        sc = fitter->fit( vtx , tmplist );
        if( !sc ) continue;

        m_util->calcIP(*kpp, &vtx, ip, ipe);
        if( ipe==0 ) continue;
        if( ip/ipe > ipmax )
        {
          ipmax = ip/ipe;
          kpp_worse = kpp;
          worse_exist = true;
        }
      }
      //decide if keep it or kill it
      if( worse_exist && ipmax > 3.0) Pfit.erase( kpp_worse );
    }

    sc = fitter->fit( Vfit , Pfit ); //RE-FIT////////////////////
    if( !sc ) Pfit.clear();
  }
  if ( msgLevel(MSG::DEBUG) )
    debug() << "================ Fit Results: " << Pfit.size() <<endreq;
  Vfit.clearOutgoingParticles();
  for( jp=Pfit.begin(); jp!=Pfit.end(); jp++ ) Vfit.addToOutgoingParticles(*jp);

  vtxvect.push_back(Vfit);
  return vtxvect;
}

//=============================================================================
double SVertexNNTool::NNseeding(double P1,
                                double P2,
                                double PT1,
                                double PT2,
                                double B0PT,
                                double IP1,
                                double IP2,
                                double JVZ,
                                double JCHI )
{
  //normalise inputs
  double OUT1 = std::min(P1/ 80., 1.);
  double OUT2 = std::min(P2/ 80., 1.);
  double OUT3 = std::min(PT1/ 8., 1.);
  double OUT4 = std::min(PT2/ 8., 1.);
  double OUT5 = std::min(B0PT/25., 1.);
  double OUT6 = std::min(fabs(IP1/40.)-.05, 1.);
  double OUT7 = std::min(fabs(IP2/40.)-.05, 1.);
  double OUT8 = std::min(JVZ/30., 1.);
  double OUT9 = std::log(JCHI)/10.;

  double RIN1 = 9.391623e-01
    +(-5.951788e+00) * OUT1
    +(-3.643498e+00) * OUT2
    +(-1.364269e+00) * OUT3
    +(1.414424e+00) * OUT4
    +(4.470808e-01) * OUT5
    +(-6.285307e+00) * OUT6
    +(-5.931499e+00) * OUT7
    +(-3.205423e+00) * OUT8
    +(-1.104598e+00) * OUT9;
  double RIN2 = -1.747574e-01
    +(3.133887e-01) * OUT1
    +(4.566571e-01) * OUT2
    +(-2.023576e+00) * OUT3
    +(1.525844e+00) * OUT4
    +(-6.491805e-01) * OUT5
    +(-1.473083e+00) * OUT6
    +(-5.245672e+00) * OUT7
    +(-6.187496e+00) * OUT8
    +(9.767284e-02) * OUT9;
  double RIN3 = 8.948249e-01
    +(7.731287e-01) * OUT1
    +(1.597335e+00) * OUT2
    +(5.308616e-01) * OUT3
    +(-2.925036e+00) * OUT4
    +(3.082963e+00) * OUT5
    +(6.905531e+00) * OUT6
    +(4.739557e+00) * OUT7
    +(-5.694037e-01) * OUT8
    +(4.910773e-01) * OUT9;
  double RIN4 = 4.027563e-01
    +(-4.363552e-01) * OUT1
    +(-2.553686e-01) * OUT2
    +(3.182271e-01) * OUT3
    +(1.288926e-02) * OUT4
    +(6.456403e-01) * OUT5
    +(1.528162e+01) * OUT6
    +(1.210068e+01) * OUT7
    +(-8.270981e-01) * OUT8
    +(2.113075e-01) * OUT9;
  double RIN5 = 2.674934e+00
    +(-6.622229e-01) * OUT1
    +(-1.498590e+00) * OUT2
    +(7.153726e+00) * OUT3
    +(1.092053e+01) * OUT4
    +(-1.866176e-01) * OUT5
    +(5.295907e-01) * OUT6
    +(1.764385e-01) * OUT7
    +(-4.817290e-01) * OUT8
    +(-4.241088e+00) * OUT9;

  OUT1 = SIGMOID(RIN1);
  OUT2 = SIGMOID(RIN2);
  OUT3 = SIGMOID(RIN3);
  OUT4 = SIGMOID(RIN4);
  OUT5 = SIGMOID(RIN5);

  return -2.873225e+00
    +(-8.990243e-01) * OUT1
    +(-6.287556e-01) * OUT2
    +(-4.041671e+00) * OUT3
    +(4.500425e+00) * OUT4
    +(3.393500e+00) * OUT5;
}
double SVertexNNTool::SIGMOID(double x)
{
  if(x >  37.0) return 1.0;
  if(x < -37.0) return 0.0;
  return 1.0/(1.0+exp(-x));
}
double SVertexNNTool::ipprob(double x) 
{
  if( x > 40. ) return 0.6;
  const double r = - 0.535 + x * 
    (0.3351 + x *(-0.03102 + x * (0.001316 +
                                  x * (-0.00002598 + x * 0.0000001919))));
  return (r < 0.) ? 0. : r;
}
double SVertexNNTool::ptprob(double x)
{
  if( x > 5.0 ) return 0.65;
  const double r = 0.04332 + x * 
    (0.9493 + x * (-0.5283 + x *(0.1296 +
                                 x * -0.01094)));
  return (r < 0.) ? 0. : r;
}
//=============================================================================
