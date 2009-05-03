// $Id: D02KPiNoPID.cpp,v 1.3 2009-05-03 14:49:33 jonrob Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "D02KPiNoPID.h"

using namespace Gaudi::Units;

//-----------------------------------------------------------------------------
// Implementation file for class : D02KPiNoPID
//
// 2009-02-03 : Andrew Stephen Powell
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( D02KPiNoPID );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
D02KPiNoPID::D02KPiNoPID( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
{
  //Declare Algorithm Properties:
  declareProperty( "D0MassWindow", m_D0MassWin = 1000.0*MeV );
  declareProperty( "D0VertexChi2Cut", m_VertexChiD0 = 100.0 );
  declareProperty( "D0MinPtCut", m_D0PtCut = 0.0*MeV );
  declareProperty( "D0MinIPChi2Cut", m_D0IPChi2 = 20.0 );

}
//=============================================================================
// Destructor
//=============================================================================
D02KPiNoPID::~D02KPiNoPID() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode D02KPiNoPID::initialize() {
  StatusCode sc = DVAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  // Retrieve Geant3 ID Numbers for Each Particle
  const LHCb::ParticleProperty* partProp;
  partProp = ppSvc()->find( "pi+" );
  m_PiPlusID  =  LHCb::ParticleID(partProp->pdgID());
  partProp = ppSvc()->find( "K+" );
  m_KPlusID =  LHCb::ParticleID(partProp->pdgID());
  partProp = ppSvc()->find( "D0" );
  m_D0ID = LHCb::ParticleID(partProp->pdgID());
  partProp = ppSvc()->find( "D~0" );
  m_D0Mass = partProp->mass();
  m_D0barID =  LHCb::ParticleID(partProp->pdgID());

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode D02KPiNoPID::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  StatusCode sc = StatusCode::SUCCESS ;
  setFilterPassed(false);  // Mandatory. Set to true if event is accepted.

  counter("Events")++;

  // The Algorithm Code:
  const LHCb::Particle::ConstVector& daughters = desktop()->particles();
  return MakeD02KPi(daughters);
}

//=========================================================================
//
//=========================================================================
StatusCode D02KPiNoPID::MakeD02KPi(const LHCb::Particle::ConstVector& daughters ) 
{
  // Obtain Primary Vertices:
  const LHCb::RecVertex::Container* vPrimaryVertices =
    desktop()->primaryVertices();

  // Event Number Now Processing
  if ( msgLevel(MSG::DEBUG) )
  {
    debug()<< "Processing event number "
           << counter("Events").nEntries()
           << endmsg;
    debug()<< "Particle vector size = "
           << daughters.size() << endmsg;
    debug()<< "PV vector size = "
           << vPrimaryVertices->size() << endmsg;
  }

  // Veto Event if Insufficient Tracks Present
  if( daughters.size() <3 ) {
    debug() << "This Event Contains Fewer Than 3 Charged Tracks"
            << endmsg;
    return StatusCode::SUCCESS;
  }

  // Isolate Kaons and Pions
  LHCb::Particle::ConstVector vPions, vKaons;
  LHCb::RecVertex::Container::const_iterator ivert;

  StatusCode sc = StatusCode::SUCCESS;
  sc = particleFilter()->filterByPID( daughters, vKaons, "K+" );
  // CC Done by Default
  sc = particleFilter()->filterByPID( daughters, vPions, "pi+");
  // CC Done by Defualt

  debug()<< "Number of pions found = "
         << vPions.size() << endmsg;
  debug()<< "Number of Kaons found = "
         << vKaons.size() << endmsg;

  if (vPrimaryVertices->size() == 0 ) 
  {
    debug() << "No primary vertex: skip event"
            << endmsg;
    return StatusCode::SUCCESS;
  }

  /*plot(vPrimaryVertices->size(),
    99,
    "reconstructed primary vertices",
    0.0,
    10.0);*/

  // Find All Two Particle Combinations to Form D0 Candidates:
  counter("DCandsInEvent").reset();
  LHCb::Particle::ConstVector::const_iterator  i1, i2;
  debug()<< "Finding All Possible D->KPi Combinations" << endmsg;

  // Loop Over All Pions
  for ( i1 = vPions.begin(); i1 != vPions.end(); i1++ ) {
    int  trtypefornt=0;
    LHCb::Track::Types trtype=LHCb::Track::TypeUnknown;
    const LHCb::ProtoParticle* protoPartCurrentPart =
      dynamic_cast<const LHCb::ProtoParticle*> ((*i1)->proto());

    // Determine Track Type of Pion
    if(protoPartCurrentPart){
      const LHCb::Track* CurrentParttrack=protoPartCurrentPart->track();
      if(CurrentParttrack){
        trtype=CurrentParttrack->type();
        if(trtype==LHCb::Track::Long) trtypefornt=4;//Long track
        else if (trtype==LHCb::Track::Velo)  trtypefornt=1;
        else if (trtype==LHCb::Track::Upstream)  trtypefornt=3;
        else if (trtype==LHCb::Track::Downstream)  trtypefornt=5;
        else if (trtype==LHCb::Track::Ttrack)trtypefornt =6;
        else if (trtype==LHCb::Track::TypeUnknown)trtypefornt =7;
        else {
          trtypefornt =8;
        }//else
      }//if(CurrentParttrack)
    }//if(protoPartCurrentPart)

    verbose()<<"Pion Track Type: "<< trtypefornt <<endmsg;
    verbose() << "Pion Pt = " << (*i1)->pt()/GeV << " GeV/c : "
              << "Pion P =" << (*i1)->p()/GeV << " GeV/c : " <<endmsg;

    // Loop Over All Kaons
    for ( i2 = vKaons.begin(); i2 != vKaons.end(); i2++ ) {

      verbose() << "kaon Pt = " << (*i2)->pt()/GeV << " GeV/c : "
                << "kaon P =" << (*i2)->p()/GeV << " GeV/c : " <<endmsg;

      // Check We Have a Neutral Combination
      if ( (*i1)->charge() * (*i2)->charge()< 0 ) {
        debug() << "Neutral Combination Found" << endmsg;

        // Find invariant mass of pi/K combination:
        Gaudi::LorentzVector PionKaonComb =
          (*i1)->momentum() + (*i2)->momentum() ;
        double mass  = PionKaonComb.M()/MeV;
        verbose()<<"Nominal D Mass: "<<m_D0Mass<<endmsg;
        verbose()<<"Invariant Mass of Candidate: "<<mass<<endmsg;

        // Apply Mass Window Cut
        if ( fabs( mass - m_D0Mass ) < m_D0MassWin ) {
          debug() << "Invariant Mass of K/Pi Combination = "
                  << mass << " MeV/c^2 : "
                  << "Passed "
                  << m_D0MassWin
                  <<" MeV/c^2 Mass Window Cut" << endmsg;

          // Assumming Cabibbo Favoured D->KPi,
          // Determine the ID of the D Meson
          if ( (*i1)->charge() <0 && (*i2)->charge()>0) {
            m_D0PID=m_D0barID;
            verbose()<<"D0bar -> K^- Pi^+"<<endmsg;
          }
          else if( (*i1)->charge()>0 && (*i2)->charge() <0  ) {
            m_D0PID=m_D0ID;
            verbose()<<"D0 -> K^+ Pi^-"<<endmsg;
          }
          debug() << "PID of D Meson Before Creating Particle"
                  << m_D0PID<<endmsg;

          // Perform Unconstrained Vertex Fit:
          LHCb::Vertex  PionKaonVertex_NoPid;
          LHCb::Particle candD0(m_D0PID);
          StatusCode SCPionKaonVertex_NoPid=
            vertexFitter()->fit(*(*i1),
                                *(*i2),
                                candD0,
                                PionKaonVertex_NoPid);

          if (!SCPionKaonVertex_NoPid) {
            Warning("Fit error",StatusCode::SUCCESS,1).ignore();
            continue;
          }

          // Apply Vertex Chi2 Cut
          if ( SCPionKaonVertex_NoPid
               && (PionKaonVertex_NoPid.chi2() < m_VertexChiD0)
               )
          {
            debug() << "Chi2 of Unconstrained K/Pi Vertex Fit = "
                    << PionKaonVertex_NoPid.chi2()
                    << " : Passed "
                    << m_VertexChiD0
                    <<" cut" << endmsg;
            
            debug()<<"PID of D After Creating Particle "
                   << (&candD0)->particleID().pid() <<endmsg;

            debug()<<"Mass of D After Creating Particle "
                   << (&candD0)->momentum().M()/GeV <<endmsg;

            // Apply D Pt and Mass Window Cuts
            if( ( (((&candD0)->pt()) > m_D0PtCut) &&
                  (fabs( ((&candD0)->momentum().M())- m_D0Mass ) <
                   m_D0MassWin)) ) {

              // Determine the Imapact Parameter and
              // Seperation Significance Chi2
              double ipCurrentPart   = -1.;
              double min_ipCurrentPartsign=999999;
              double min_ipCurrentPart=999999;
              double ipCurrentPartEr = -1.;
              double ipCurrentPartErmin=-1;
              double f_min=-999;
              double fe_min = -999;
              double f, fe;

              debug() << "Calculate Impact Parameter D0" <<endmsg;
              for ( ivert = vPrimaryVertices->begin();
                    ivert != vPrimaryVertices->end(); ivert++){
                distanceCalculator()->distance(  (&candD0),
                                                 (*ivert),
                                                 ipCurrentPart,
                                                 ipCurrentPartEr );
                debug()<<"IP of Current Particle"
                       <<ipCurrentPart<<endmsg;

                if(ipCurrentPartEr!=0 && (ipCurrentPart/ipCurrentPartEr)
                   < min_ipCurrentPartsign){
                  min_ipCurrentPart=ipCurrentPart;
                  ipCurrentPartErmin=ipCurrentPartEr;
                  min_ipCurrentPartsign =
                    (min_ipCurrentPart/ipCurrentPartErmin);

                  ipCurrentPart=min_ipCurrentPart;
                  ipCurrentPartEr= ipCurrentPartErmin;
                  const LHCb::Vertex *endD0 = (&candD0)->endVertex();
                  StatusCode sc =
                    distanceCalculator()->distance( (*ivert),
                                                    endD0,
                                                    f,
                                                    fe);

                  if (sc.isFailure()) continue;
                }

              }

              ipCurrentPart=min_ipCurrentPart;
              ipCurrentPartEr= ipCurrentPartErmin;
              f_min = f;
              fe_min = fe;
              /*plot(f,
                "Vertex Sep",
                "Vertex Seperation",
                0,
                50);
                plot(fe,
                "Sep. Signif. Chi2",
                "Sep. Signif. Chi2",
                0,
                200);*/

              // Determine if D Candidate Should be Saved
              if(fe < m_D0IPChi2){
                setFilterPassed(true);
                if ( m_D0PID == m_D0barID )
                  counter("D0barCand")++;
                else
                  counter("D0Cand")++;
                counter("DCandsInEvent")++;

                // D Candidate Passed All Cuts! - Save to Desktop
                desktop()->keep(&candD0);
                debug()<<"Saved a D Candidate to the Physics Desktop "
                       <<counter("DCandsInEvent").nEntries()
                       <<endmsg;

                /*plot((&candD0)->momentum().M(),
                  "D Mass",
                  "Mass",
                  m_D0Mass-m_D0MassWin,
                  m_D0Mass+m_D0MassWin);*/
              }
            }

          }
          // Closes if ( pionKaonVertex_NoPid.chi2()...)

        }
        // Closes if ( fabs(mass...) )

      }
      // Closes for ( neutral combination)

    }
    // Closes for ( Kaon...)

  }
  // Closes if ( Pion...)

  return desktop()->saveDesktop();

}

//=============================================================================
