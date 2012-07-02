// $Id: $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "math.h"

// local
#include "StrippingNBMuMu.h"

//-----------------------------------------------------------------------------
// Implementation file for class : StrippingNBMuMu
//
// 2010-10-15 : Ulrich Kerzel
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( StrippingNBMuMu )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
StrippingNBMuMu::StrippingNBMuMu( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : DaVinciHistoAlgorithm  ( name , pSvcLocator ),
    m_NetworkCut ( -1 ),
    m_PlotHisto  ( false ),
    m_PlotMassMin(  2.9   ),
    m_PlotMassMax(  3.2   ),
    m_PlotNBins  (120     )
{
    declareProperty( "Expertise"     , m_ExpertiseName                );
    declareProperty( "NetworkVersion", m_netVersion  = "TuneSep2010"  );
    declareProperty( "NetworkCut"    , m_NetworkCut  =  -1            );
    declareProperty( "PlotHisto"     , m_PlotHisto   =  false         );
    declareProperty( "PlotMassMin"   , m_PlotMassMin =   2.9          );
    declareProperty( "PlotMassMax"   , m_PlotMassMax =   3.2          );
    declareProperty( "PlotNBins"     , m_PlotNBins   = 120            );
    declareProperty( "PVLocation"    ,
                     m_pvLocation = LHCb::RecVertexLocation::Primary );
} // constructor

//=============================================================================
// Destructor
//=============================================================================
StrippingNBMuMu::~StrippingNBMuMu() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode StrippingNBMuMu::initialize()
{
  StatusCode sc = DaVinciHistoAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;

  //
  // print settings
  //

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() <<  "Expertise      " <<  m_ExpertiseName << endmsg;
    debug() <<  "NetworkVersion " <<  m_netVersion    << endmsg;
    debug() <<  "NetworkCut     " <<  m_NetworkCut    << endmsg; 
    debug() <<  "PlotHisto      " <<  m_PlotHisto     << endmsg;
    debug() <<  "PlotMassMin    " <<  m_PlotMassMin   << endmsg;
    debug() <<  "PlotMassMax    " <<  m_PlotMassMax   << endmsg;
    debug() <<  "PlotNBins      " <<  m_PlotNBins     << endmsg;
  }

  //
  // setup NeuroBayes
  //
#ifdef __GNUC__

  if ( msgLevel(MSG::VERBOSE) ) 
    verbose() << "Setup NeuroBayes for Expertise " << m_ExpertiseName << endmsg;

  if ( m_ExpertiseName.empty() ) 
    return Error( "No NeuroBayes Expertise specified" );

  std::string fullPath;

  // Expertise are in PARAM group of packages
  const std::string paramEnv = "STRIPPINGNEUROBAYESROOT";
  if ( getenv(paramEnv.c_str()) ) {
    debug() << "found environment for Expertise " << paramEnv << endmsg;
    const std::string paramRoot = ( std::string(getenv(paramEnv.c_str())) + 
                                    "/expertise/" + m_netVersion + "/" );
    fullPath = paramRoot+m_ExpertiseName;
  } else {
    return Error("PARAM file not found",StatusCode::FAILURE,1);
  } // if paramEnv

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Take Expertise from " << fullPath << endmsg;
     
  // FPE Guard for NB call
  FPE::Guard guard(true);
  m_NBExpert = new Expert(fullPath.c_str()); 
  
  // prepare arry of input variables, use max. allowed length
  // (easier for switching between different networks)
  m_inArray = new float[NB_MAXNODE];

#endif 

  return sc;
} //initialise

//=============================================================================
// Main execution
//=============================================================================
StatusCode StrippingNBMuMu::execute() {

  if ( msgLevel(MSG::DEBUG) )
    debug() << "==> Execute" << endmsg;


  StatusCode sc = StatusCode::SUCCESS;

#ifdef __GNUC__

  // dont' accept by default
  setFilterPassed(false);

  // get particles - DiMu candidate  
  const LHCb::Particle::ConstVector& cands = this->i_particles();
  const int nCand = cands.size();

  if ( msgLevel(MSG::VERBOSE) ) {
    verbose() << "got # particles from local storage " << nCand << endmsg;
  }

  LHCb::Particle::ConstVector::const_iterator iCand = cands.begin();
  LHCb::Particle::ConstVector::const_iterator iCandEnd   = cands.end();

  for ( ; iCand != iCandEnd; iCand++) {

    sc = StrippingNBMuMu::getInputVar(*(*iCand));

    if (sc == StatusCode::SUCCESS) {

      if ( msgLevel(MSG::DEBUG) ) {
        for (int i = 0; i<24; i++) {
          debug() << "input var [" << i << "] = " << m_inArray[  i ] << endmsg;
        } //for i
      } // if msg
    
      const double netOut = m_NBExpert->nb_expert(m_inArray);
      const double prob   = (1.0 + netOut)*0.5;

      if ( msgLevel(MSG::DEBUG) ) {
        debug() << "MuMu cand: Network output " << netOut 
                << " probability " << prob << endmsg;
      }
      
      const double mass = (*iCand)->measuredMass()/Gaudi::Units::GeV;
      
      if (m_PlotHisto) {
        plot1D(netOut, "MuMuNet" , "NeuroBayes MuMu network output ", -1.0          , 1.0          ,         120);
        plot1D(prob  , "MuMuProb", "NeuroBayes MuMu network prob "  ,  0.0          , 1.0          ,         120);
        plot1D(mass  , "mAll"    , "mass, all cand"                 ,  m_PlotMassMin, m_PlotMassMax, m_PlotNBins);
      } //if

      // accept candidate?
      if (prob > m_NetworkCut) {
        if ( msgLevel(MSG::DEBUG) ) 
          debug() << "MuMu cand pass cut "  << m_NetworkCut << endmsg;
        if (m_PlotHisto) {
          plot1D(mass  , "mAcc"        , "mass, acc cand"                         ,   m_PlotMassMin, m_PlotMassMax, m_PlotNBins); 
          plot1D(prob  , "MuMuProbAcc" , "NeuroBayes MuMu network prob acc cand"  ,  0.0           , 1.0          ,         120);
        } //if
        LHCb::Particle* particle = (*iCand)->clone();
        setFilterPassed(true);
        particle->addInfo(LHCb::Particle::LastGlobal +  1, netOut);   
        particle->addInfo(LHCb::Particle::LastGlobal +  2, prob);   
        this->markNewTree(particle);
      } // if prob
    } // if sc
  } // for iCand
  
  return sc;
  
#endif 
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode StrippingNBMuMu::finalize() {

  if ( msgLevel(MSG::DEBUG) ) 
    debug() << "==> Finalize" << endmsg;
#ifdef __GNUC__

  if (m_NBExpert)
    delete m_NBExpert;

  if (m_inArray)
    delete[] m_inArray;
#endif 

  return DaVinciHistoAlgorithm::finalize();
}

//=============================================================================
StatusCode  StrippingNBMuMu::getInputVar(const LHCb::Particle& particle) {
  
  StatusCode sc = StatusCode::SUCCESS;

#ifdef __GNUC__
  // get daughters
  const SmartRefVector< LHCb::Particle > &muons = particle.endVertex()->outgoingParticles();

  if (muons.size() != 2) {
    warning() << "number of daughters at vertex, skip candidate " << muons.size() << endmsg;
    return StatusCode::FAILURE;
  }//if #Kaons

  const LHCb::Particle* muPlus  = NULL;
  const LHCb::Particle* muMinus = NULL;
  SmartRefVector< LHCb::Particle >::const_iterator iMu;
  SmartRefVector< LHCb::Particle >::const_iterator iMuBegin = muons.begin();
  SmartRefVector< LHCb::Particle >::const_iterator iMuEnd   = muons.end();

  if ( msgLevel(MSG::VERBOSE) ) 
    verbose() << " muon 1 charge " << (*muons[0]).charge() 
              << " muon 2 charge " << (*muons[1]).charge()
              << endmsg;

  for (iMu = iMuBegin; iMu != iMuEnd; iMu++){
        if ( (*iMu)->charge() > 0) {
          muPlus  = *iMu;
        } else {
          muMinus = *iMu;
        }// if
      }//for iKaon

  // sanity check
  if (!muPlus || !muMinus) {
        warning() << " Muons obtained from not valid, skip candidate" << endmsg;
        return StatusCode::FAILURE;
  }// if


  
  //
  // get input variables, move "spikes" to -999
  //
  float alpha = 0;
  if (muPlus->p() < muMinus->p())
    alpha = 1 - muPlus->p()/muMinus->p();
  else
    alpha = -(1 - muMinus->p()/muPlus->p());
  
  const Gaudi::XYZVector v1 = muPlus ->momentum().Vect().Unit();
  const Gaudi::XYZVector v2 = muMinus->momentum().Vect().Unit();

  
  
  //
  // apply the same soft pre-cuts as
  // during the evaluation of the sWeights
  //
  
  //
  // move delta functions to -999
  //
  double muplusE = muPlus->proto()->info(LHCb::ProtoParticle::CaloEcalE, -999);
  if ( (muplusE>-0.1 && muplusE<0.1) ||
       muplusE < 0.0)
    muplusE = -999;
  
  double muminusE = muMinus->proto()->info(LHCb::ProtoParticle::CaloEcalE, -999);
  if ( (muminusE>-0.1 && muminusE<0.1) ||
       muminusE < 0.0)
    muminusE = -999;
  
  double muplusPIDK = muPlus->proto()->info(LHCb::ProtoParticle::CombDLLk,-999);
  if (muplusPIDK > -0.01 && muplusPIDK < 0.01)
    muplusPIDK = -999;
  double muminusPIDK = muMinus->proto()->info(LHCb::ProtoParticle::CombDLLk,-999);
  if (muminusPIDK > -0.01 && muminusPIDK < 0.01)
    muminusPIDK = -999;
  
  double muplusPIDe = muPlus->proto()->info(LHCb::ProtoParticle::CombDLLe,-999);
  if (muplusPIDe > -0.5 && muplusPIDe < 0.5)
    muplusPIDe = -999;
  double muminusPIDe = muMinus->proto()->info(LHCb::ProtoParticle::CombDLLe,-999);
  if (muminusPIDe > -0.5 && muminusPIDe < 0.5)
    muminusPIDe = -999;
  
  double muplusPIDp = muPlus->proto()->info(LHCb::ProtoParticle::CombDLLp,-999);
  if (muplusPIDp > -0.01 && muplusPIDp < 0.01)
    muplusPIDp = -999;
  double muminusPIDp = muMinus->proto()->info(LHCb::ProtoParticle::CombDLLp,-999);
  if (muminusPIDp > -0.01 && muminusPIDp < 0.01)
    muminusPIDp = -999;
  
  

  
  double muplusChi2 = muPlus->proto()->track()->probChi2();
  if (muplusChi2 < 0.001) {
    muplusChi2 = -999;
  }
  
  double muminusChi2 = muMinus->proto()->track()->probChi2();
  if (muminusChi2 < 0.001) {
    muminusChi2 = -999;
  }

  double jPsiDira                      = -999;
  const LHCb::RecVertex::Container* PV =    0;
  if (exist<LHCb::RecVertex::Container>(m_pvLocation)) 
  {
    PV = get<LHCb::RecVertex::Container>(m_pvLocation);
    if (PV != NULL && PV->size() >0) {
      const LHCb::VertexBase *relatedPV = this->bestVertex(&particle);
      Gaudi::XYZVector        A         = particle.momentum().Vect();
      Gaudi::XYZVector        B         = particle.endVertex()->position() - relatedPV->position();  
      jPsiDira  = A.Dot( B ) / std::sqrt( A.Mag2()*B.Mag2() );
  } else {
    debug() << "No primary vertices found" << endmsg;
    } // else if NULL
  } // if exist




  //
  // pre-cuts, if any
  // 

  //
  // fill input array
  //

  bool filledInput = false;
  
  std::size_t found;

  
  // "full" network, all variables, including (lifetime-) biasing ones
  found=m_ExpertiseName.find("full.nb");
  if (found != std::string::npos) {
    m_inArray[  0 ] =  muPlus->pt()/Gaudi::Units::GeV;      
    m_inArray[  1 ] =  muMinus->pt()/Gaudi::Units::GeV;     
    m_inArray[  2 ] =  alpha;                   
    m_inArray[  3 ] =  acos(v1.Dot(v2));            
    m_inArray[  4 ] =  StrippingNBMuMu::minIPChi2(*muPlus);
    m_inArray[  5 ] =  StrippingNBMuMu::minIPChi2(*muMinus);
    m_inArray[  6 ] =  muPlus->proto()->info(LHCb::ProtoParticle::CombDLLmu,-999);       
    m_inArray[  7 ] =  muMinus->proto()->info(LHCb::ProtoParticle::CombDLLmu,-999);      
    m_inArray[  8 ] =  particle.endVertex()->chi2(); 
    m_inArray[  9 ] =  particle.pt()/Gaudi::Units::GeV;       
    m_inArray[ 10 ] =  particle.measuredMassErr()/Gaudi::Units::GeV;     
    m_inArray[ 11 ] =  StrippingNBMuMu::minIPChi2(particle);
    m_inArray[ 12 ] =  jPsiDira;
    m_inArray[ 13 ] =  particle.p()/Gaudi::Units::GeV; 
    m_inArray[ 14 ] =  muplusChi2;               
    m_inArray[ 15 ] =  muminusChi2;              
    m_inArray[ 16 ] =  muplusPIDK;               
    m_inArray[ 17 ] =  muminusPIDK;              
    m_inArray[ 18 ] =  muplusPIDe;               
    m_inArray[ 19 ] =  muminusPIDe;              
    m_inArray[ 20 ] =  muplusPIDp;               
    m_inArray[ 21 ] =  muminusPIDp;              
    m_inArray[ 22 ] =  muplusE;                  
    m_inArray[ 23 ] =  muminusE;                 
    
    filledInput = true;
  } // if full net

  
  // "medium" network: kinematic variables, Calorimeter, PID
  found=m_ExpertiseName.find("noip.nb");
  if (found != std::string::npos) {

    m_inArray[  0 ] =  muPlus->pt()/Gaudi::Units::GeV; 
    m_inArray[  1 ] =  muMinus->pt()/Gaudi::Units::GeV;     
    m_inArray[  2 ] =  alpha;                 
    m_inArray[  3 ] =  acos(v1.Dot(v2));          
    m_inArray[  4 ] =  muPlus->proto()->info(LHCb::ProtoParticle::CombDLLmu,-999);  
    m_inArray[  5 ] =  muMinus->proto()->info(LHCb::ProtoParticle::CombDLLmu,-999);     
    m_inArray[  6 ] =  particle.endVertex()->chi2();
    m_inArray[  7 ] =  particle.measuredMassErr()/Gaudi::Units::GeV;  
    m_inArray[  8 ] =  particle.p()/Gaudi::Units::GeV; 
    m_inArray[  9 ] =  muplusChi2;               
    m_inArray[ 10 ] =  muminusChi2;              
    m_inArray[ 11 ] =  muplusPIDK;               
    m_inArray[ 12 ] =  muminusPIDK;              
    m_inArray[ 13 ] =  muplusPIDe;               
    m_inArray[ 14 ] =  muminusPIDe;              
    m_inArray[ 15 ] =  muplusPIDp;               
    m_inArray[ 16 ] =  muminusPIDp;              
    m_inArray[ 17 ] =  muplusE;                  
    m_inArray[ 18 ] =  muminusE;                   

    filledInput = true;
  } // if NoIP net

   if (!filledInput) {
    warning() << "StrippingNBMuMu::getInputVar: Input variables not filled - which network?" << endmsg;
    return StatusCode::FAILURE;
  } //if

#endif  

  return sc;
} //double getInputVar
//=============================================================================
double StrippingNBMuMu::minIPChi2(const LHCb::Particle& particle){
  // as taken from TupleToolGeometry
  

  double returnValue = -999; 

  double minIP       = -1;
  double minChi2     = -1;

  double ip          = -999;
  double ipChi2      = -999;
  
  StatusCode sc;

  const LHCb::RecVertex::Range primVtx = this->primaryVertices();
  if (! primVtx.empty() ) {
    for ( LHCb::RecVertex::Range::const_iterator pv = primVtx.begin() ; pv!= primVtx.end() ; ++pv){
      const LHCb::VertexBase* thePV = *pv;
      sc = distanceCalculator()->distance(&particle, thePV, ip, ipChi2);
      if(msgLevel(MSG::VERBOSE))
        verbose() << "particle " << particle.particleID().pid() << " IP " << ip << " ipChi2 " << ipChi2 << endmsg;
      if (sc = StatusCode::SUCCESS) {
        if ( (ip     < minIP)  ||
             (minIP  < 0.0  ) )
          minIP = ip;

        if ( (ipChi2  < minChi2) ||
             (minChi2 < 0.0) )
          minChi2 = ipChi2;
      } // if sc
    } //for PV
  } // if !empty

  
  returnValue = minChi2;

#ifdef __GNUC__  
  if ( isnan(returnValue)     ||
       abs(isinf(returnValue)))
    warning() << "StrippingNBMuMu::minIPChi2: something went wrong, minIP not valid " << returnValue << endmsg;
  returnValue = -999;
#endif    

 if(msgLevel(MSG::VERBOSE))
   verbose() << " returning " << returnValue  << " for minIPChi2" << endmsg;
 return returnValue;

} // double minIPChi2
//=============================================================================
