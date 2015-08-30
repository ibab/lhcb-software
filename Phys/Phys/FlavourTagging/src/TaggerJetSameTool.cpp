// Include files 
#include "TaggerJetSameTool.h"
#include "Event/RecVertex.h"

#include "TaggingHelpers.h"
//--------------------------------------------------------------------
// Implementation file for class : TaggerJetSameTool
//
// Author: Julien Babel
//--------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( TaggerJetSameTool )

//====================================================================
  TaggerJetSameTool::TaggerJetSameTool( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent ) :
    GaudiTool ( type, name, parent )
{
  declareInterface<ITagger>(this);

  declareProperty( "JetSame_Pt_cut", m_Pt_cut_jetS = 0.25 *GeV );
  declareProperty( "JetSame_dR_cut", m_dR_cut_jetS = 0.80 );
  declareProperty( "AverageOmega", m_AverageOmega  = 0.45 );
}

TaggerJetSameTool::~TaggerJetSameTool() {}

//=====================================================================
Tagger TaggerJetSameTool::tag( const Particle* AXB0, 
                               const RecVertex* RecVert,
                               const int nPV,
                               Particle::ConstVector& vtags ) 
{
  Tagger tjetS;

  double JetS   = 0;
  double aux    = 0;
  double norm   = 0;
  double k      = 1.1;
  double Jetcut = 0.2;

  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << " n.PV=" << nPV <<endmsg;
    verbose() << " RecVert=" << RecVert->position().z() <<endmsg;
  }

  for ( Particle::ConstVector::const_iterator ip = vtags.begin();
        ip != vtags.end(); ++ip )
  {
    const Particle* axp = (*ip);
    const double Pt = axp->pt();
    if( Pt < m_Pt_cut_jetS )  continue;

    const Gaudi::LorentzVector& ptotB = AXB0->momentum();
    const double etasig= -std::log(std::tan(ptotB.Theta())/2);
    const double eta   = -std::log(std::tan(axp->momentum().Theta()/2));
    const double dphi  = std::fabs(TaggingHelpers::dphi(axp->momentum().Phi(), ptotB.Phi()));
    //cone condition
    const double dR = std::sqrt((eta-etasig)*(eta-etasig)+dphi*dphi);
    if ( dR < m_dR_cut_jetS )
    {
      // Construction of a Jet charge same side with no kaonS(pionS)
      //if ((*ip) != ikaonS || (*ip) != ipionS){
      aux  += std::pow((*ip)->pt()/GeV,k)*(*ip)->charge();
      norm += std::pow((*ip)->pt()/GeV,k);
      //}
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << " JetS Pt="<< Pt << " dR=" << dR <<endmsg;
    }
  }
  //build jet charge same side
  if (norm) if ( aux/norm<(-Jetcut) || aux/norm>Jetcut ) JetS = aux/norm;
  if(!JetS) return tjetS;

  tjetS.setDecision(JetS>0 ? 1: -1);
  tjetS.setOmega( m_AverageOmega );
  tjetS.setType( Tagger::SS_jetCharge );

  return tjetS;
}
//==========================================================================
