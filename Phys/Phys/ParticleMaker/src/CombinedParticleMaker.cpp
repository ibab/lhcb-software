// $Id: CombinedParticleMaker.cpp,v 1.1.1.1 2004-08-24 06:47:48 pkoppenb Exp $
// Include files 
#include <algorithm>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/Tokenizer.h"

// local
#include "CombinedParticleMaker.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CombinedParticleMaker
//
// 2003-05-19 : Gloria Corti
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<CombinedParticleMaker>          s_factory ;
const        IToolFactory& CombinedParticleMakerFactory = s_factory ; 


namespace
{
  std::string to_upper( const std::string& in )
  {
    std::string  out( in );
    std::transform( in.begin() , in.end() , out.begin () , ::toupper ) ;
    return out ;
  };
};

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CombinedParticleMaker::CombinedParticleMaker( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent )
  : AlgTool ( type, name , parent )
  , m_input( ProtoParticleLocation::Charged )
  , m_exclusive(true)
  , m_addBremPhoton(false)
  , m_longTracks(true)
  , m_downstreamTracks(false)
  , m_vttTracks(false)
{

  // Declaring implemented interfaces
  declareInterface<IParticleMaker>(this);
  declareProperty("InputProtoP", m_input );
  
  m_particleList.push_back("muon");
  m_particleList.push_back("electron");
  m_particleList.push_back("kaon");
  m_particleList.push_back("proton");
  m_particleList.push_back("pion");
  declareProperty("Particles", m_particleList );

  declareProperty("ExclusiveSelection", m_exclusive );
  
  m_muonSelection.push_back("det='MUON' mu-pi='-8.0'");
  declareProperty("MuonSelection", m_muonSelection );

  m_electronSelection.push_back("det='CALO' e-pi='0.0'");
  declareProperty("ElectronSelection", m_electronSelection );
  
  m_kaonSelection.push_back("det='RICH' k-pi='2.0' k-p='-2.0'");
  declareProperty("KaonSelection", m_kaonSelection );

  m_protonSelection.push_back("det='RICH' p-pi='3.0'");
  declareProperty("ProtonSelection", m_protonSelection );

  m_pionSelection.clear();
  declareProperty("PionSelection", m_pionSelection );  
 
  declareProperty("AddBremPhoton", m_addBremPhoton );

  declareProperty("UseLongTracks",     m_longTracks );
  declareProperty("UseDownstreamTracks", m_downstreamTracks );
  declareProperty("UseVTTTracks",      m_vttTracks );

}

CombinedParticleMaker::~CombinedParticleMaker( ) { } ;

//=============================================================================
// Standard initialize method
//=============================================================================
StatusCode CombinedParticleMaker::initialize() {
  
  MsgStream msg(msgSvc(), name());

  if( m_particleList.empty() ) {
    msg << MSG::INFO << "A list of particles types must be specified" 
        << endreq;
    return StatusCode::FAILURE;
  }

  IParticlePropertySvc* ppSvc = 0;
  StatusCode sc = service("ParticlePropertySvc", ppSvc, true);
  if( sc.isFailure() ) {
    msg << MSG::FATAL << "Cannot retrieve ParticlePropertySvc"
        << endreq;
    return sc;
  }
  
  if( m_pionSelection.empty() ) {
    std::vector<std::string>::const_iterator iPType = m_particleList.begin();
    bool makePion = false;
    while( m_particleList.end() != iPType ) {
      if( "PION" == to_upper( *iPType ) ) { 
        makePion = true;
        break;
      }
      ++iPType;
    }
    const std::string lastPID = m_particleList.back();
    if( makePion && ("PION" != to_upper(lastPID)) ) {
      msg << MSG::FATAL 
          << "When PionSelections not specified pion must be last in list"
          << endreq;
      return StatusCode::FAILURE;
    }
  }

  std::vector<std::string>::const_iterator aPID = m_particleList.begin();
  std::string temp;
  const ParticleProperty* partProp;
  while( aPID != m_particleList.end() ) {
    SelectionSpecs selSpecs;
    const std::string testPID ( to_upper( *aPID ) ) ;
    if( "MUON" == testPID ) {
      temp = "mu+";
      SelectionDescs selDescs(m_muonSelection.begin(),m_muonSelection.end());
      setSelections(selDescs, selSpecs);
    }

    else if( "ELECTRON" == testPID ) {
      temp = "e+";
      SelectionDescs selDescs(m_electronSelection.begin(),
                              m_electronSelection.end());
      setSelections(selDescs, selSpecs);
    }
    else if( "KAON" == testPID ) {
      temp = "K+";
      SelectionDescs selDescs(m_kaonSelection.begin(),m_kaonSelection.end());
      setSelections(selDescs, selSpecs);
    }
    else if( "PROTON" == testPID ) {
      temp = "p+";
      SelectionDescs selDescs(m_protonSelection.begin(),
                              m_protonSelection.end());
      setSelections(selDescs, selSpecs);
    }
    else if( "PION" == testPID ) {
      temp = "pi+";
      SelectionDescs selDescs(m_pionSelection.begin(),m_pionSelection.end());
      setSelections(selDescs, selSpecs);
    }
    else {
      msg << MSG::FATAL << "Unknown particle type" << endreq;
      return StatusCode::FAILURE;
    }
    
    partProp = ppSvc->find( temp );
    if( 0 == partProp ) {
      msg << MSG::FATAL << "Unknown property for particle type " 
          << temp << endreq;
      return StatusCode::FAILURE;
    }

    //     SelectionType typeSel;
    //     typeSel.first = partProp;
    //     typeSel.second = selections;
    m_typeSelections.push_back( std::make_pair( partProp, selSpecs ) );

    ++aPID;
    
  }

  // Retrieve the data service
  sc = service("EventDataSvc", m_EDS, true);
  if( sc.isFailure() ) {
    msg << MSG::FATAL << "Unable to locate Event Data Service" << endreq;
    return sc;
  }

  // Log selection criteria
  msg << MSG::INFO << "Selection of particle types have been set" << endreq;
  msg << MSG::INFO << "Particle Type" << "     Criteria" << endreq;
  for( TypeSelections::const_iterator itype=m_typeSelections.begin();
       m_typeSelections.end()!=itype; ++itype ) {
    std::string ptype = ((*itype).first)->particle();
    partProp = ppSvc->findByStdHepID( -(((*itype).first)->jetsetID()) );
    if( partProp ) { ptype += "/"+partProp->particle(); }
    msg << MSG ::INFO << ptype << "   ";
    for( SelectionSpecs::const_iterator isel=((*itype).second).begin();
         ((*itype).second).end()!=isel; ++isel) {
      std::string det = "";
      if( (*isel)->HasRich() ) det += "RICH ";
      if( (*isel)->HasCalo() ) det += "CALO ";
      if( (*isel)->HasMuon() ) det += "MUON ";
      msg << MSG::INFO << det << "   ";
      const std::vector<double>& cuts = (*isel)->dllCuts();
      for( unsigned int ipos = 0; ipos < cuts.size(); ++ipos ) {
        if( cuts[ipos] > -999.0 ) {
          PMakerSelection::DLLCuts a = PMakerSelection::DLLCuts(ipos);
          std::string explanation = (*isel)->cutType( a );
          msg << MSG::INFO << explanation << " > " << cuts[ipos] << "  ";
        }
      }
      msg << MSG::INFO << std::endl;
    }
    msg << MSG::INFO << endreq;  
  }

  msg << MSG::INFO << "The type of tracks to be used are :";
  bool atLeastOneType = false;
  if( m_longTracks ) {
    msg << MSG::INFO << " Long";
    atLeastOneType = true;
  }
  if( m_downstreamTracks ) {
    msg << MSG::INFO << " Downstream";
    atLeastOneType = true;
  }
  if( m_vttTracks ) {
    msg << MSG::INFO << " Upstream (VTT)";
    atLeastOneType = true;
  }
  msg << MSG::INFO << endreq;
  if( !atLeastOneType ) {
    msg << MSG::INFO << "At least one track type needs to be selected" 
        << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
  
}

//===========================================================================
// Finalize
//===========================================================================
StatusCode CombinedParticleMaker::finalize() {
  
  MsgStream msg(msgSvc(), name());

  msg << MSG::DEBUG << "Delete selection criteria" << endreq;  
  // loop over m_typeSelections, pop back vector of selectionDesc
  // where new was done
  for( TypeSelections::iterator itype=m_typeSelections.begin();
       m_typeSelections.end()!=itype; ++itype ) {
    while( ((*itype).second).size() > 0 ) {
      PMakerSelection* aSel = ((*itype).second).back();
      ((*itype).second).pop_back();
      delete aSel;
    }
  }

  return StatusCode::SUCCESS;
}

//===========================================================================
// Set selection criteria for muons
//===========================================================================
void CombinedParticleMaker::setSelections(const SelectionDescs& selDescriptions,
                                          SelectionSpecs& selections) {
  
  for( SelectionDescs::const_iterator iDesc = selDescriptions.begin();
       iDesc != selDescriptions.end(); ++iDesc ) {
    PMakerSelection* selection = new PMakerSelection();
    defineSelection( *iDesc, selection );
    selections.push_back( selection );
  }
  
}

//===========================================================================
// Decode criteria for selection for a particle type
//===========================================================================
void CombinedParticleMaker::defineSelection(const std::string description, 
                                            PMakerSelection* selection ) {
  
  Tokenizer tok;
  tok.analyse( description, " ", "", "", "=", "'", "'" );
  for ( Tokenizer::Items::iterator i = tok.items().begin(); 
        i != tok.items().end(); ++i )   {
    const std::string& tag   = to_upper((*i).tag());
    const std::string& value = (*i).value();
    if( "DET" == tag ) {
      setDetectorFlag( value, selection );
    }
    else if( "E-PI" == tag ) {
      (selection->dllCuts())[PMakerSelection::DLLE_PI] = atof(value.c_str());
    }
    else if( "E-MU" == tag ) {
      (selection->dllCuts())[PMakerSelection::DLLE_MU] = atof(value.c_str());
    }
    else if( "E-K" == tag ) { 
      (selection->dllCuts())[PMakerSelection::DLLE_K] = atof(value.c_str());
    }
    else if( "E-P" == tag ) {
      (selection->dllCuts())[PMakerSelection::DLLE_P] = atof(value.c_str());
    }
    else if( "MU-PI" == tag ) {
      (selection->dllCuts())[PMakerSelection::DLLMU_PI] = atof(value.c_str());
    }
    else if( "MU-E" == tag ) {
      (selection->dllCuts())[PMakerSelection::DLLMU_E] = atof(value.c_str());
    }
    else if( "MU-K" == tag ) {
      (selection->dllCuts())[PMakerSelection::DLLMU_K] = atof(value.c_str());
    }
    else if( "MU-P" == tag ) {
      (selection->dllCuts())[PMakerSelection::DLLMU_P] = atof(value.c_str());
    }
    else if( "K-PI" == tag ) {
      (selection->dllCuts())[PMakerSelection::DLLK_PI] = atof(value.c_str());
    }
    else if( "K-MU" == tag ) {
      (selection->dllCuts())[PMakerSelection::DLLK_MU] = atof(value.c_str());
    }
    else if( "K-E" == tag ) {
      (selection->dllCuts())[PMakerSelection::DLLK_E] = atof(value.c_str());
    }
    else if( "K-P" == tag ) {
      (selection->dllCuts())[PMakerSelection::DLLK_P] = atof(value.c_str());
    }
    else if( "P-PI" == tag ) {
      (selection->dllCuts())[PMakerSelection::DLLP_PI] = atof(value.c_str());
    }
    else if( "P-MU" == tag ) {
      (selection->dllCuts())[PMakerSelection::DLLP_MU] = atof(value.c_str());
    }
    else if( "P-K" == tag ) {
      (selection->dllCuts())[PMakerSelection::DLLP_K] = atof(value.c_str());
    }
    else if( "PI-E" == tag ) {
      (selection->dllCuts())[PMakerSelection::DLLPI_E] = atof(value.c_str());
    }
    else if( "PI-MU" == tag ) {
      (selection->dllCuts())[PMakerSelection::DLLPI_MU] = atof(value.c_str());
    }
    else if( "PI-K" == tag ) {
      (selection->dllCuts())[PMakerSelection::DLLPI_K] = atof(value.c_str());
    }
    else if( "PI-P" == tag ) {
      (selection->dllCuts())[PMakerSelection::DLLPI_P] = atof(value.c_str());
    } 
  }
} 

//===========================================================================
// setDetectorFlags for one property
//===========================================================================
void CombinedParticleMaker::setDetectorFlag( const std::string detector,
                                             PMakerSelection* selection ) {  
  switch( ::toupper(detector[0]) ) {
  case 'R':
    selection->setHasRich(true);
    break;
  case 'C':
    selection->setHasCalo(true);
    break;
  case 'M':
    selection->setHasMuon(true);
    break;
  }
  long comma = -1;
  comma = detector.find(',');
  if( comma > 0 ) {
    std::string det = detector.substr(comma+1, detector.length()-comma-1);
    setDetectorFlag( det, selection );
  }
}

 
//=============================================================================
// Main execution
//=============================================================================
StatusCode CombinedParticleMaker::makeParticles( ParticleVector& parts ) {
  
  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "CombinedParticleMaker::makeParticles()" << endreq;

  int nParticles = 0;   // Counter of particles created  
  SmartDataPtr<ProtoParticles> protos ( eventSvc(), m_input );
  if( !protos ) {
    msg << MSG::ERROR << "Charged ProtoParticles do not exist" << endreq;
    return StatusCode::FAILURE;
  }
  if( 0 == protos->size() ) { 
    msg << MSG::INFO << "Charged ProtoParticles container is empty at "
        << m_input << endreq;
    return StatusCode::SUCCESS;
  }
  
  // Debug number of ProtoPartCandidates retrieved
  msg << MSG::DEBUG << "Number of Charged ProtoParticles retrieved from " 
      << m_input << " = " << protos->size() << endreq;
  
  // Now make Particles based on criterias
  // First loop on all ProtoParticles
  StatusCode sc = StatusCode::SUCCESS;
  for( ProtoParticles::const_iterator iProto=protos->begin(); 
       protos->end()!=iProto; ++iProto ) {

    // check if the track type is to be used
    bool trkeep = false;
    const TrStoredTrack* ptrack = (*iProto)->track();

    if( m_longTracks && ( ptrack->isLong() ) ) {
      trkeep = true;
    }
    if( m_downstreamTracks && ptrack->isDownstream() ) {
      trkeep = true;
    }
    if( m_vttTracks && ptrack->isUpstream() ) {
      trkeep = true;
    }
    if( !trkeep ) continue;
    // then loop on all particle types to make and their associated criteria
    for( TypeSelections::const_iterator iSel=m_typeSelections.begin();
         m_typeSelections.end()!=iSel; ++iSel ) {
      // Check if selection criteria for this particle type are satisfied by
      // protoparticle
      bool kept = false;
      for( SelectionSpecs::const_iterator iSpec=((*iSel).second).begin();
           ((*iSel).second).end()!=iSpec; ++iSpec ) {
        if( selectionIsSatisfied(*iProto, *iSpec) ) {
          // if satifisfied fillParticle,
          Particle* aParticle = new Particle();
          sc = fillParticle( *iProto, (*iSel).first, aParticle );
          if( sc.isSuccess() ) {
            parts.push_back(aParticle);
            ++nParticles;
            kept = true;
            break; // if particle kept with a selection condition
                   // do not check the others
          } else {
            delete aParticle;
          }
        }
      } // end loop on selection specifications for this particle type
      // if exclusive selection and particle has been kept as this type
      // go to next particle
      if( m_exclusive && kept ) break;
      // if no selection criteria for pions make it as such
      if( ("pi+" == ((*iSel).first)->particle()) && ((*iSel).second).empty() ) {
        Particle* aParticle = new Particle();
        sc = fillParticle( *iProto, (*iSel).first, aParticle );
        if( sc.isSuccess() ) {
          parts.push_back(aParticle);
          ++nParticles;
        } else {
          delete aParticle;
        }
      }
    } // end loop on particle types
  } // end loop on ProtoParticles

  return StatusCode::SUCCESS;
}
 
//=========================================================================
// Fill particles parameters
//=========================================================================
StatusCode CombinedParticleMaker::fillParticle( const ProtoParticle* proto,
                                                const ParticleProperty* pprop, 
                                                Particle* particle ) {
  // PK  
  MsgStream msg( msgSvc(), name() );
// Check it is a Charged ProtoP hence TrStateP is accessible

  const TrStateP* trackState = proto->trStateP();
  if( !trackState ) return StatusCode::FAILURE;
  if ( proto->track()->isDownstream()) {
//  New (PK): Use first measured point
    trackState = proto->firstMeasured();
  }
  // Start filling particle with orgininating ProtoParticle
  particle->setOrigin(proto);

  // ParticleID and set confidence level
  int pID =  pprop->jetsetID() * (int)(proto->charge());
  particle->setParticleID( ParticleID( pID ) );
  double ve  = exp(dllValue( proto, ProtoParticle::LkhPIDe ));
  double vmu = exp(dllValue( proto, ProtoParticle::LkhPIDmu )); 
  double vk  = exp(dllValue( proto, ProtoParticle::LkhPIDK )); 
  double vp  = exp(dllValue( proto, ProtoParticle::LkhPIDp ));
  double confLevel = 0.0;
  confLevel = 1./(1.+ve+vmu+vk+vp); // conf level for pion
  if( "e+" == pprop->particle() ) {
    confLevel = ve*confLevel;
  }
  else if ( "mu+" == pprop->particle() ) {
    confLevel = vmu*confLevel;
  }
  else if ( "K+" == pprop->particle() ) {
    confLevel = vk*confLevel;
  }
  else if ( "p+" == pprop->particle() ) {
    confLevel = vp*confLevel;
  }
  particle->setConfLevel( confLevel );
  // Mass and resonace status
  double mass = pprop->mass();
  particle->setMass( mass );
  particle->setMassErr(0.0); // in reality we should take value from PDG...
  particle->setIsResonance(false);  // in constructor but reiterate

  // Set position of first measured point on track:
  HepPoint3D position( trackState->x(), trackState->y(), trackState->z() ) ;
  particle->setPointOnTrack( position );

  msg << MSG::VERBOSE << "PK:" ;
  if (proto->track()->isLong())       msg << "long           " ;
  if (proto->track()->isUpstream())   msg << "upstream (VTT) " ;
  if (proto->track()->isDownstream()) msg << "downstream     " ;
  msg << " position z=" << position[2] << endreq ;
    
  // Calculate and set four momentum
  double momentum = trackState->p();
  double slopeX   = trackState->tx();
  double slopeY   = trackState->ty();	  

  // If electron and Brem then add calohypothesis momentum
  if( ("e+" == pprop->particle()) && m_addBremPhoton ) {
    momentum += bremMomentum( proto );
  }
  
  double pZ = momentum/sqrt( 1.0 + slopeX*slopeX + slopeY*slopeY );
  HepLorentzVector quadriMomentum;
  quadriMomentum.setPx( pZ*slopeX );
  quadriMomentum.setPy( pZ*slopeY );
  quadriMomentum.setPz( pZ );
  quadriMomentum.setE( sqrt( mass*mass + momentum*momentum) );
  particle->setMomentum( quadriMomentum );

  // Set pointOnTrackErr: (Error on x and y. No error on z!)
  const HepSymMatrix& trkCov = trackState->pCovMatrix();
  HepSymMatrix pointOnTrackErr(3, 0);
  pointOnTrackErr = trkCov.sub(1,3);
  pointOnTrackErr(3,1) = 0.0;
  pointOnTrackErr(3,2) = 0.0;
  pointOnTrackErr(3,3) = 0.0;
  particle->setPointOnTrackErr(pointOnTrackErr);

  // Set slope+Momentum error:
  HepSymMatrix slpMomErr(3, 0);
  slpMomErr = trkCov.sub(3,5);
  particle->setSlopesMomErr(slpMomErr);
  // Set position-slopes correlation matrix. 
  // Position X Momentum correlation matrix also automatically set.
  // No correlation with Z
  HepMatrix posSlopesCorr(3, 3, 0);
  int i, j;
  for( i = 1; i <= 3; i++ ) {
    for ( j = 1; j <= 2; j++ ) {
      int istart = i+2;
      posSlopesCorr(i,j) = trkCov(istart,j);
    }
  }
  particle->setPosSlopesCorr(posSlopesCorr);

  return StatusCode::SUCCESS;

}
 
//============================================================================
// get momentum of Brem photon
//=============================================================================
double CombinedParticleMaker::bremMomentum( const ProtoParticle* proto ) {

  double bremMom = 0.0;
  const SmartRefVector<CaloHypo>& hypos = proto->calo();
  double XCluster = 1000000;
  double YCluster = 1000000;
  if( hypos.size() == 2 ){ /// the second hypo is the brem phot one
    SmartRefVector<CaloHypo>::const_reverse_iterator bremHypo = hypos.rbegin();
    SmartRefVector<CaloCluster> clusters = (*bremHypo)->clusters();
        
    SmartRefVector<CaloCluster>::iterator icluster;
    for( icluster = clusters.begin() ;
         icluster != clusters.end()  ;
         icluster++ ) {
      XCluster = (*icluster)->position().x();
      YCluster = (*icluster)->position().y();
    }
    const CaloMomentum* caloMomentum = (*bremHypo)->momentum() ;
    if( 0 != caloMomentum ) {
      bremMom = caloMomentum->momentum().e();
    }
  }  

  //double chi2Brem  = chi2BremMatch(*icand);
  
  // inner part of ECAL
//   if( (Xcluster<970.0) && (Ycluster<727.0) ){
//     //    if ( chi2Brem <= m_CutOnChi2BremMatchIP ){
//     bremMomentum = momentum;
//   }
  // middle part of ECAL
//   if( (Xcluster<1939.0) && (Ycluster<1212.0) ) {
//     if ( (Xcluster<970.0) && (Ycluster<727.0) ) { /* do nothing */ }
//     else {
//       //if ( chi2Brem <= m_CutOnChi2BremMatchMP ) {
//       //  m_4momOfBremPhot =  MomOfBremPhBremMatch(*icand);
//         //}
//       bremMomentum = momentum; 
//     }
//   }
//   // outer part  
//   if( (Xcluster<1939.0) && (Ycluster<1212.0) )
//   else {
// //     if ( chi2Brem <= m_CutOnChi2BremMatchOP ) {
// //       m_4momOfBremPhot = MomOfBremPhBremMatch(*icand);
// //     }
//     bremMomentum = momentum; 
//   }
  return bremMom;
}

 
//===========================================================================
// Check criteria are satified for a particular particle type
//===========================================================================
bool CombinedParticleMaker::selectionIsSatisfied(const ProtoParticle* proto,
                                                 const PMakerSelection* spec ) {
  
  // Check that required detectors are present
  if( (spec->HasRich()) && !(proto->richBit()) ) {
      return false;
  }
  if( (spec->HasCalo()) && !(proto->caloeBit()) ) {
    return false;
  }
  if( (spec->HasMuon()) && !(proto->muonBit()) ) {
    return false;
  }
  // For required detectors check that cuts are satisfied
  double de_pi  = dllValue( proto, ProtoParticle::LkhPIDe );
  double dmu_pi = dllValue( proto, ProtoParticle::LkhPIDmu ); 
  double dk_pi  = dllValue( proto, ProtoParticle::LkhPIDK ); 
  double dp_pi  = dllValue( proto, ProtoParticle::LkhPIDp );

  bool keep = true;
  // electron vs other particles hypothesis
  double dllCut = (spec->dllCuts())[PMakerSelection::DLLE_PI];
  if( (dllCut > -999.0) && (de_pi < dllCut) )            { keep = false; }
  dllCut = (spec->dllCuts())[PMakerSelection::DLLE_MU];
  if( (dllCut > -999.0) && ((de_pi - dmu_pi) < dllCut) ) { keep = false; }
  dllCut = (spec->dllCuts())[PMakerSelection::DLLE_K];
  if( (dllCut > -999.0) && ((de_pi - dk_pi) < dllCut) )  { keep = false; }
  dllCut = (spec->dllCuts())[PMakerSelection::DLLE_P];
  if( (dllCut > -999.0) && ((de_pi - dp_pi) < dllCut) )  { keep = false; }
  
  // muon vs other particles hypothesis
  dllCut = (spec->dllCuts())[PMakerSelection::DLLMU_PI];
  if( (dllCut > -999.0) && (dmu_pi < dllCut) )           { keep = false; }
  dllCut = (spec->dllCuts())[PMakerSelection::DLLMU_E];
  if( (dllCut > -999.0) && ((dmu_pi - de_pi) < dllCut) ) { keep = false; }
  dllCut = (spec->dllCuts())[PMakerSelection::DLLMU_K];
  if( (dllCut > -999.0) && ((dmu_pi - dk_pi) < dllCut) ) { keep = false; }
  dllCut = (spec->dllCuts())[PMakerSelection::DLLMU_P];
  if( (dllCut > -999.0) && ((dmu_pi - dp_pi) < dllCut) ) { keep = false; }

  // kaon vs other particle hypothesis
  dllCut = (spec->dllCuts())[PMakerSelection::DLLK_PI];
  if( (dllCut > -999.0) && (dk_pi < dllCut ) )           { keep = false; }
  dllCut = (spec->dllCuts())[PMakerSelection::DLLK_E];
  if( (dllCut > -999.0) && ((dk_pi - de_pi) < dllCut) )  { keep = false; }
  dllCut = (spec->dllCuts())[PMakerSelection::DLLK_MU];
  if( (dllCut > -999.0) && ((dk_pi - dmu_pi) < dllCut) ) { keep = false; }
  dllCut = (spec->dllCuts())[PMakerSelection::DLLK_P];
  if( (dllCut > -999.0) && ((dk_pi - dp_pi) < dllCut) )  { keep = false; }
  
  // proton vs other hypothesis
  dllCut = (spec->dllCuts())[PMakerSelection::DLLP_PI];
  if( (dllCut > -999.0) && (dp_pi < dllCut) )             { keep = false; }
  dllCut = (spec->dllCuts())[PMakerSelection::DLLP_E];
  if( (dllCut > -999.0) && ((dp_pi - de_pi) < dllCut) )  { keep = false; }
  dllCut = (spec->dllCuts())[PMakerSelection::DLLP_MU];
  if( (dllCut > -999.0) && ((dp_pi - dmu_pi) < dllCut) ) { keep = false; }
  dllCut = (spec->dllCuts())[PMakerSelection::DLLP_K];
  if( (dllCut > -999.0) && ((dp_pi - dk_pi) < dllCut) )  { keep = false; }
 
  // pions vs other hypothesis
  dllCut = (spec->dllCuts())[PMakerSelection::DLLPI_E];
  if( (dllCut > -999.0) && ((-de_pi) < dllCut) )         { keep = false; }
  dllCut = (spec->dllCuts())[PMakerSelection::DLLPI_MU];
  if( (dllCut > -999.0) && ((-dmu_pi) < dllCut) )        { keep = false; }
  dllCut = (spec->dllCuts())[PMakerSelection::DLLPI_K];
  if( (dllCut > -999.0) && ((-dk_pi) < dllCut) )         { keep = false; }
  dllCut = (spec->dllCuts())[PMakerSelection::DLLPI_P];
  if( (dllCut > -999.0) && ((-de_pi) < dllCut) )         { keep = false; }

  return keep;
}
 

//============================================================================
// dllValue (similar to method in ProtoParticle but with reverse iterator
//============================================================================
double CombinedParticleMaker::dllValue(const ProtoParticle* proto,
                                       const ProtoParticle::detectorPID& det) {
  
  double value = -999.0;
  for( ProtoParticle::PIDDetVector::const_reverse_iterator 
         id = proto->pIDDetectors().rbegin();
       proto->pIDDetectors().rend() != id; ++id ) {
    if( det == (*id).first ) {
      value = (*id).second;
      break;
    }
  }
  return value;
}

//=============================================================================
// Pointer to the Event Data service
//=============================================================================
IDataProviderSvc* CombinedParticleMaker::eventSvc() const
{
  return m_EDS;
} 

  
