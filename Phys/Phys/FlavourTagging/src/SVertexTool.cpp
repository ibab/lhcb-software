// $Id: SVertexTool.cpp,v 1.15 2010-02-11 18:35:45 musy Exp $
#include "SVertexTool.h"
#include "Event/RecVertex.h"
//-----------------------------------------------------------------------------
// Implementation file for class : SVertexTool v1.3
//
// Secondary inclusive vertexing based on a likelihood function
//-----------------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( SVertexTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  SVertexTool::SVertexTool( const std::string& type,
                            const std::string& name,
                            const IInterface* parent ) :
    GaudiTool ( type, name, parent )
{
  declareInterface<ISecondaryVertexTool>(this);

  declareProperty( "lcs_Long_cut",     m_lcs_Long_cut     = 2.5 );
  declareProperty( "lcs_Upstream_cut", m_lcs_Upstream_cut = 5.0 );

  declareProperty( "IPFitPol0", m_ipfitpol0= -0.535);
  declareProperty( "IPFitPol1", m_ipfitpol1= 0.3351 );
  declareProperty( "IPFitPol2", m_ipfitpol2= -0.03102 );
  declareProperty( "IPFitPol3", m_ipfitpol3= 0.001316 );
  declareProperty( "IPFitPol4", m_ipfitpol4= -0.00002598 );
  declareProperty( "IPFitPol5", m_ipfitpol5= 0.0000001919);

  declareProperty( "PtFitPol0", m_ptfitpol0= 0.04332 );
  declareProperty( "PtFitPol1", m_ptfitpol1= 0.9493 );
  declareProperty( "PtFitPol2", m_ptfitpol2= -0.5283 );
  declareProperty( "PtFitPol3", m_ptfitpol3= 0.1296);
  declareProperty( "PtFitPol4", m_ptfitpol4= -0.01094 );

  declareProperty( "AnglePol0", m_anglepol0= 0.4516 );
  declareProperty( "AnglePol1", m_anglepol1= 1.033 );

}

StatusCode SVertexTool::initialize()
{
  const StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;
  m_util = tool<ITaggingUtils> ( "TaggingUtils", this );

  fitter = m_util->getVertexFitter();

  return sc;
}

//=============================================================================
SVertexTool::~SVertexTool(){}

//=============================================================================
std::vector<Vertex> SVertexTool::buildVertex(const RecVertex& RecVert,
                                             const Particle::ConstVector& vtags)
{

  if ( msgLevel(MSG::VERBOSE) )
    verbose()<<"=======SVertex Tool========"<<endmsg;

  const double RVz = RecVert.position().z()/mm;

  //Build Up 2 Seed Particles For Vertexing ------------------------
  double ipl(0), iperrl(0), ips(0), iperrs(0), probf(-1.0) ;
  StatusCode sc = StatusCode::SUCCESS;
  Vertex Vfit(0);
  Vertex vtx;
  std::vector<Vertex> vtxvect(0);
  const Particle *p1=0, *p2=0;
  Particle::ConstVector::const_iterator ip, jp, kp;
  Particle::ConstVector vtags_unique, vtags_toexclude;

  //filter duplicates and apply some pre-cuts to speed up what's next
  if ( msgLevel(MSG::VERBOSE) )
    verbose()<<"Look for seed particles"<<endmsg;
  for ( ip = vtags.begin(); ip != vtags.end(); ip++ ) 
  {
    if( !(*ip)->proto() ) continue;

    bool duplicate=false;
    for ( jp = ip+1; jp != vtags.end(); jp++ ) {
      if( (*ip)->proto() == (*jp)->proto() ) { duplicate=true; break; }
    }
    if( duplicate ) continue;

    if( (*ip)->p()/GeV < 2.0 ) continue;
    if( (*ip)->proto()->track()->type()!= Track::Long
        && (*ip)->proto()->track()->type()!= Track::Upstream) continue;//preselection

    const double lcs = (*ip)->proto()->track()->chi2PerDoF();
    if((*ip)->proto()->track()->type()== Track::Long ){
      if(lcs > m_lcs_Long_cut) continue;
    }
    if((*ip)->proto()->track()->type()== Track::Upstream ){
      if(lcs > m_lcs_Upstream_cut) continue;
    }

    vtags_unique.push_back(*ip);
  }

  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "size of tracks for sec vtx="<<vtags_unique.size()<<endmsg;

  //loop to find seed -----------------------------------
  for ( jp = vtags_unique.begin(); jp != vtags_unique.end(); jp++ ) 
  {

    //FIRST seed particle ----
    m_util->calcIP(*jp, &RecVert, ipl, iperrl);
    if( iperrl==0 ) continue;
    if( iperrl > 1.0 ) continue;                                //preselection
    if( ipl/iperrl < 2.0 ) continue;                            //preselection
    if( ipl/iperrl > 100.0 ) continue;                          //preselection

    if ( msgLevel(MSG::VERBOSE) )
      verbose() << "seed1: pt="<< (*jp)->pt()/GeV <<endmsg;

    //SECOND seed particle ----
    for ( kp = (jp+1) ; kp != vtags_unique.end(); kp++ ) {
      m_util->calcIP(*kp, &RecVert, ips, iperrs);
      if( iperrs ==0 ) continue;                                //preselection
      if( iperrs > 1.0 ) continue;                                //preselection
      if( ips/iperrs < 2.0 ) continue;                            //preselection
      if( ips/iperrs > 100.0 ) continue;                          //preselection
      //cut on the z position of the seed
      // replaced by V.B., 20 aug 2k+9: sc = fitter->fit( **jp, **kp, vtx );
      sc = fitter->fit( vtx , **jp, **kp );
      if( sc.isFailure() ) continue;
      if( vtx.chi2() / vtx.nDoF() > 10.0 ) continue;           //preselection
      if((vtx.position().z()/mm - RVz) < 1.0 ) continue;       //preselection
      double mass = 0.;
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << "    try with: pt="<< (*kp)->pt()/GeV <<endmsg;
      if((*jp)->particleID().pid()!=310 && (*kp)->particleID().pid()!=310)
      {
        //if the couple is compatible with a Ks, drop it         //preselection
        mass = ((*jp)->momentum() + (*kp)->momentum()).M()/GeV;
        if( mass > 0.490 && mass < 0.505
            &&  (*jp)->particleID().abspid() == 211
            &&  (*kp)->particleID().abspid() == 211
            && ((*jp)->charge()) * ((*kp)->charge())< 0)
        {
          if ( msgLevel(MSG::VERBOSE) )
            verbose() << "This is a Ks candidate! skip."<<endmsg;
          vtags_toexclude.push_back(*jp);
          vtags_toexclude.push_back(*kp);
          continue;
        }
      }
      if((*jp)->particleID().pid()!=3122 && (*kp)->particleID().pid()!=3122){
        if( mass > 1.11 && mass < 1.12 )
          if( ((*jp)->charge()) * ((*kp)->charge()) < 0 )
            if( ((*jp)->particleID().abspid()    ==  211
                 && (*kp)->particleID().abspid() == 2212) ||
                ((*jp)->particleID().abspid()    == 2212
                 && (*kp)->particleID().abspid() ==  211) ) 
            {
              if ( msgLevel(MSG::VERBOSE) )
                verbose() << "This is a Lambda0 candidate! skip."<<endmsg;
              vtags_toexclude.push_back(*jp);
              vtags_toexclude.push_back(*kp);
              continue;
            }
      }

      if ( msgLevel(MSG::VERBOSE) )
        verbose() << "    seed2 found: pt="<< (*kp)->pt()/GeV <<endmsg;

      //build a likelihood that the combination comes from B ---------
      //double probi1(0), probi2(0), probp1(0), probp2(0), proba(0), probs(0), probb(0);
      // impact parameter
      const double probi1=ipprob(ipl/iperrl);
      const double probi2=ipprob(ips/iperrs);
      // pt
      const double probp1=ptprob((*jp)->pt()/GeV);
      const double probp2=ptprob((*kp)->pt()/GeV);
      // angle
      const double proba= aprob(angle( (*jp)->momentum(), (*kp)->momentum() ));
      // total
      const double probs=probi1*probi2*probp1*probp2*proba;
      const double probb=(1-probi1)*(1-probi2)*(1-probp1)*(1-probp2)*(1-proba);
      if( probs/(probs+probb) < 0.2 ) continue;                  //preselection
      if( probs/(probs+probb) > probf ) 
      {
        probf = probs/(probs+probb);
        Vfit = vtx;
        p1 = (*jp);
        p2 = (*kp);
        if ( msgLevel(MSG::VERBOSE) )
          verbose() << "complete seed formed with probf="<<probf<<endmsg;
      }
    }
  }

  Vertex VfitTMP;
  Particle::ConstVector Pfit(0);

  if( probf>0.32 && p1 != NULL && p2 != NULL ) 
  {

    //add iteratively other tracks to the seed ----------------------
    Pfit.push_back(p1);
    Pfit.push_back(p2);
    Particle::ConstVector::const_iterator jpp;
    for(jpp = vtags_unique.begin(); jpp != vtags_unique.end(); ++jpp)
    {

      if( (*jpp) == p1 ) continue;
      if( (*jpp) == p2 ) continue;

      double ip(0), ipe(0);
      m_util->calcIP(*jpp, &RecVert, ip, ipe);
      if( ipe==0 ) continue;
      if( ip/ipe < 1.8 ) continue;                                        //cut
      if( ip/ipe > 100 ) continue;                                        //cut
      if( ipe > 1.5    ) continue;                                        //cut
      if( isin(vtags_toexclude, *jpp) ) continue;

      //likelihood for the particle to come from B ------
      const double probi1=ipprob(ip/ipe);
      const double probp1=ptprob((*jpp)->pt()/GeV); // pt
      // total
      const double probs=probi1*probp1;
      const double probb=(1-probi1)*(1-probp1);
      const double likeb=probs/(probs+probb);
      if( likeb < 0.2 ) continue;                                         //cut

      Pfit.push_back(*jpp);

      // replaced by V.B., 20 aug 2k+9: sc = fitter->fit( Pfit, VfitTMP ); /////////FIT///////////
      sc = fitter->fit( VfitTMP , Pfit ); /////////FIT///////////
      if( !sc ) { Pfit.pop_back(); continue; }

      if( VfitTMP.chi2() / VfitTMP.nDoF()  > 5.0 )
      { Pfit.pop_back(); continue; }                                   //cut
      if((VfitTMP.position().z()/mm - RVz) < 1.0 )
      { Pfit.pop_back(); continue; }                                   //cut

      //look what is the part which behaves the worst
      int ikpp = 0;
      double ipmax = -1.0;
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
      if( worse_exist ) 
      {
        if ( msgLevel(MSG::VERBOSE) )
          verbose()<< "Worse=" << (*kpp_worse)->particleID().pid()
                   << " P=" << (*kpp_worse)->p()/GeV << " ipmax=" << ipmax ;
        if ( ipmax > 3.0 && Pfit.size() > 2 )
        {
          Pfit.erase( kpp_worse );
          if ( msgLevel(MSG::VERBOSE) ) verbose() << " killed." << endmsg;
        } else {
          if ( msgLevel(MSG::VERBOSE) ) verbose() << " included." << endmsg;
        }
      }
    }
    sc = fitter->fit( Vfit , Pfit ); //RE-FIT////////////////////
    if( !sc ) Pfit.clear();
  }

  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "================ Fit Results: " << Pfit.size() <<endmsg;

  Vfit.clearOutgoingParticles();
  for( jp=Pfit.begin(); jp!=Pfit.end(); ++jp ) 
  {
    Vfit.addToOutgoingParticles(*jp);
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << "================  pt=" << (*jp)->pt() <<endmsg;
  }

  vtxvect.push_back(Vfit);
  return vtxvect;
}
//=============================================================================
bool SVertexTool::isin(Particle::ConstVector& vtags_toexclude,
                       const Particle* axp)
{
  for( Particle::ConstVector::const_iterator i = vtags_toexclude.begin();
       i != vtags_toexclude.end(); ++i )
  {
    if((*i)->proto() == axp->proto()) return true;
  }
  return false;
}
//=============================================================================
double SVertexTool::angle( const Gaudi::LorentzVector& a, 
                           const Gaudi::LorentzVector& b )
{
  double ang=0;
  if(a.Vect().Mag2() && b.Vect().Mag2()) 
  {
    ang = acos( (a.Vect().Unit()).Dot(b.Vect().Unit()) );
    if(ang>M_PI) ang=2.*M_PI-ang;
    if(ang<0) ang=-ang;
  } 
  else 
  {
    err()<<"Zero vector(s)! Arguments: "<<a.Vect().Mag2()
         <<" "<<b.Vect().Mag2()<<endmsg;
  }
  return ang;
}
double SVertexTool::ipprob(double x) 
{
  if( x > 40. ) return 0.6;
  const double r = m_ipfitpol0 + x * 
    (m_ipfitpol1 + x * 
     (m_ipfitpol2 +
      x * (m_ipfitpol3 + x * (m_ipfitpol4 + x * m_ipfitpol5))));
  return (r < 0.) ? 0. : r;
}
double SVertexTool::ptprob(double x)
{
  if( x > 5.0 ) return 0.65;
  const double r = m_ptfitpol0 + x * 
    (m_ptfitpol1 + x * (m_ptfitpol2 +
                        x * (m_ptfitpol3 + x * m_ptfitpol4)));
  return (r < 0.) ? 0. : r;
}
double SVertexTool::aprob(double x)
{
  if( x < 0.02 ) return 0.32;
  const double r = m_anglepol0 + m_anglepol1*x;
  return (r < 0.) ? 0. : r;
}
//=============================================================================
