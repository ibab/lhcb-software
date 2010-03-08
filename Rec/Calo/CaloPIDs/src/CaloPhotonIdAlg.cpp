// $Id: CaloPhotonIdAlg.cpp,v 1.2 2010-03-08 13:11:45 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// local
#include "CaloPhotonIdAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloPhotonIdAlg
//
// 2010-02-27 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CaloPhotonIdAlg );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloPhotonIdAlg::CaloPhotonIdAlg( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  ,m_tracking(true)
  ,m_extrapol(true)
  ,m_seed(false)
  ,m_neig(false)
  ,m_type("PhotonID")
  ,m_inputs()
  ,m_output()
{


  // TODO : split PID estimator (Prs-Spd-Ecal(Chi2, seed/cluster) + add estimator ( E4/E9, Hcal/Ecal, shower-shape,  ...)
  // and let NeutralProtoParticle to combine ...
  
  declareProperty( "Type"                 , m_type = "PhotonID"         );
  declareProperty( "Inputs"               , m_inputs        ); // CaloHypo location (default set in initialize)
  declareProperty( "Output"               , m_output        ); // relation table output location (default set in initialize)
  declareProperty( "Tracking"             , m_tracking=true ); // use track-cluster matching 
  declareProperty( "ClusterMatchLocation" , m_tableLocation ); // track-cluster table
  declareProperty( "Extrapolation"        ,m_extrapol=true  ); // Spd/Prs matching
  declareProperty( "AddSeed"              ,m_seed=false     );
  declareProperty( "AddNeighbors"         ,m_neig=false     );
  
  using namespace LHCb::CaloAlgUtils;
  m_tableLocation =  CaloIdLocation( "ClusterMatch" , context() );

  // Data (temp : should go to condDB)
  declareProperty("EPrs"          ,m_binEPrs);
  declareProperty("Chi2"          ,m_binChi2);
  declareProperty("Seed"          ,m_binSeed);  
  
  declareProperty("SigEPrs_noSpd_0_Data"  ,m_signalEPrsData_0);
  declareProperty("SigEPrs_Spd_0_Data"     ,m_signalEPrsSpdData_0);
  declareProperty("BkgEPrs_noSpd_0_Data"  ,m_backgrEPrsData_0);
  declareProperty("BkgEPrs_Spd_0_Data"      ,m_backgrEPrsSpdData_0);
  declareProperty("SigChi2_noSpd_0_Data"  ,m_signalChi2Data_0);
  declareProperty("SigChi2_Spd_0_Data"     ,m_signalChi2SpdData_0);
  declareProperty("BkgChi2_noSpd_0_Data"  ,m_backgrChi2Data_0);
  declareProperty("BkgChi2_Spd_0_Data"      ,m_backgrChi2SpdData_0);
  declareProperty("SigSeed_noSpd_0_Data"  ,m_signalSeedData_0);
  declareProperty("SigSeed_Spd_0_Data"      ,m_signalSeedSpdData_0);
  declareProperty("BkgSeed_noSpd_0_Data"  ,m_backgrSeedData_0);
  declareProperty("BkgSeed_Spd_0_Data"      ,m_backgrSeedSpdData_0);

  declareProperty("SigEPrs_noSpd_1_Data"  ,m_signalEPrsData_1);
  declareProperty("SigEPrs_Spd_1_Data"     ,m_signalEPrsSpdData_1);
  declareProperty("BkgEPrs_noSpd_1_Data"  ,m_backgrEPrsData_1);
  declareProperty("BkgEPrs_Spd_1_Data"      ,m_backgrEPrsSpdData_1);
  declareProperty("SigChi2_noSpd_1_Data"  ,m_signalChi2Data_1);
  declareProperty("SigChi2_Spd_1_Data"     ,m_signalChi2SpdData_1);
  declareProperty("BkgChi2_noSpd_1_Data"  ,m_backgrChi2Data_1);
  declareProperty("BkgChi2_Spd_1_Data"      ,m_backgrChi2SpdData_1);
  declareProperty("SigSeed_noSpd_1_Data"  ,m_signalSeedData_1);
  declareProperty("SigSeed_Spd_1_Data"      ,m_signalSeedSpdData_1);
  declareProperty("BkgSeed_noSpd_1_Data"  ,m_backgrSeedData_1);
  declareProperty("BkgSeed_Spd_1_Data"      ,m_backgrSeedSpdData_1);

  declareProperty("SigEPrs_noSpd_2_Data"  ,m_signalEPrsData_2);
  declareProperty("SigEPrs_Spd_2_Data"     ,m_signalEPrsSpdData_2);
  declareProperty("BkgEPrs_noSpd_2_Data"  ,m_backgrEPrsData_2);
  declareProperty("BkgEPrs_Spd_2_Data"      ,m_backgrEPrsSpdData_2);
  declareProperty("SigChi2_noSpd_2_Data"  ,m_signalChi2Data_2);
  declareProperty("SigChi2_Spd_2_Data"     ,m_signalChi2SpdData_2);
  declareProperty("BkgChi2_noSpd_2_Data"  ,m_backgrChi2Data_2);
  declareProperty("BkgChi2_Spd_2_Data"      ,m_backgrChi2SpdData_2);
  declareProperty("SigSeed_noSpd_2_Data"  ,m_signalSeedData_2);
  declareProperty("SigSeed_Spd_2_Data"      ,m_signalSeedSpdData_2);
  declareProperty("BkgSeed_noSpd_2_Data"  ,m_backgrSeedData_2);
  declareProperty("BkgSeed_Spd_2_Data"      ,m_backgrSeedSpdData_2);
  
}

  
//=============================================================================
// Destructor
//=============================================================================
CaloPhotonIdAlg::~CaloPhotonIdAlg() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CaloPhotonIdAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;





  // context-dependent inputs/output (if not defined via options)
  using namespace LHCb::CaloAlgUtils;
  if( m_type == "MergedID"){
    if(m_inputs.empty())m_inputs.push_back(CaloHypoLocation( "MergedPi0s"    , context() ) );
    if(""==m_output)m_output =  CaloIdLocation( "MergedID" , context() )  ;
  } else if(m_type == "PhotonID") {
    if(m_inputs.empty())m_inputs.push_back(CaloHypoLocation( "Photons"    , context() ) );
    if(""==m_output)m_output =  CaloIdLocation( "PhotonID" , context() )  ;
  } else if(m_type == "PhotonFromMergedID") {
    if(m_inputs.empty())m_inputs.push_back(CaloHypoLocation( "SplitPhotons"    , context() ) );
    if(""==m_output)m_output =  CaloIdLocation( "PhotonFromMergedID" , context() )  ;
  }

  // Spd/Prs matching based on CaloHypo2Calo :
  m_toCalo = tool<ICaloHypo2Calo>("CaloHypo2Calo", "CaloHypo2Calo" , this );
  // Warning : the algorithm settings overwrite the tool settings
  std::string seed = m_seed     ? "true"  : "false";
  std::string line = m_extrapol ? "true"  : "false";
  std::string neig = m_neig     ? "true" : "false";
  m_toCalo->_setProperty("Seed", seed ).ignore();
  m_toCalo->_setProperty("PhotonLine", line).ignore();
  m_toCalo->_setProperty("AddNeighbors",neig).ignore();
  

  // Check data consistency
  if (m_binEPrs.size()!=6)
  { return Error("Wrong Binning Parameter (EPrs)"); }
  if (m_tracking) {
    if (m_binChi2.size()!=6)
    { return Error("Wrong Binning Parameter (Chi2Tk)"); }
  }
  if (m_binSeed.size()!=6)
  { return Error("Wrong Binning Parameter (Seed)"); }


// Build useful vector
  int index;
  double width;
  width=(m_binEPrs[ 2 ]-m_binEPrs[ 1 ])/double(m_binEPrs[ 0 ]);
  for ( index=0; index<=int (m_binEPrs[ 0 ]); ++index ) {
    m_ePrsXBin.push_back(double(index)*width+m_binEPrs[ 1 ]);
  }
  if (m_tracking){
    width=(m_binChi2[ 2 ]-m_binChi2[ 1 ])/double(m_binChi2[ 0 ]);
    for ( index=0; index<=int (m_binChi2[ 0 ]); ++index ) {
      m_chi2XBin.push_back(double(index)*width+m_binChi2[ 1 ]);
    }
  }

  width=(m_binSeed[ 2 ]-m_binSeed[ 1 ])/double(m_binSeed[ 0 ]);
  for ( index=0; index<=int (m_binSeed[ 0 ]); ++index ) {
    m_seedXBin.push_back(double(index)*width+m_binSeed[ 1 ]);
  }
  width=(m_binEPrs[ 5 ]-m_binEPrs[ 4 ])/double(m_binEPrs[ 3 ]);
  for ( index=0; index<=int (m_binEPrs[ 3 ]); ++index ) {
    m_ePrsYBin.push_back(double(index)*width+m_binEPrs[ 4 ]);
  }

  if (m_tracking) {
    width=(m_binChi2[ 5 ]-m_binChi2[ 4 ])/double(m_binChi2[ 3 ]);
    for ( index=0; index<=int (m_binChi2[ 3 ]); ++index ) {
      m_chi2YBin.push_back(double(index)*width+m_binChi2[ 4 ]);
    }
  }

  width=(m_binSeed[ 5 ]-m_binSeed[ 4 ])/double(m_binSeed[ 3 ]);
  for ( index=0; index<=int (m_binSeed[ 3 ]); ++index ) {
    m_seedYBin.push_back(double(index)*width+m_binSeed[ 4 ]);
  }



  return StatusCode::SUCCESS;
}

//=============================================================================
#define INHERITS(T1,T2) \
     (Relations::IsConvertible<const T1*,const T2*>::value && \
     !Relations::IsConvertible<const T1*,const void*>::same)
//=============================================================================
StatusCode CaloPhotonIdAlg::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // create the table
  typedef LHCb::Relation1D<LHCb::CaloHypo,float>   Table  ; 
  BOOST_STATIC_ASSERT(INHERITS(Table,LHCb::Calo2Track::IHypoEvalTable));
  Table* table = new Table;
  put( table , m_output );

  // fill the table
  for( std::vector<std::string>::iterator iloc = m_inputs.begin() ; m_inputs.end() != iloc ; ++iloc){
    std::string loc = *iloc;
    if( exist<LHCb::CaloHypos>( loc ) ){
      const LHCb::CaloHypos* hypos = get<LHCb::CaloHypos>( loc );
      counter ("#Hypos in " + loc ) += hypos->size();
      for( LHCb::CaloHypos::const_iterator h = hypos->begin();hypos->end() != h ; ++ h){
        const LHCb::CaloHypo* hypo = *h;
        if( NULL == hypo ){
          counter("hypo points to NULL in " + loc ) += 1;
          continue;
        }
        double l = likelihood( hypo );        
        counter("likelihood")+= l;
        table->i_push( hypo , (float) l ) ;
      }
    }else{
      if ( msgLevel( MSG::DEBUG) )debug() << "CaloHypo not found at location : " << loc << endmsg;
      counter("Empty location " + loc )+=1;
    }
  }
  table->i_sort() ;
  // statistics 
  counter (Gaudi::Utils::toString( m_inputs )+" => "+ m_output ) += table->i_relations().size();
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CaloPhotonIdAlg::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================



// ============================================================================
double CaloPhotonIdAlg::likelihood(const LHCb::CaloHypo* hypo )  const
{
  // Get the relevant information - basic checks
  if( 0 == hypo ){
    Warning("CaloHypo points to NULL",StatusCode::SUCCESS ).ignore();
    counter("Null hypo")+=1;
    return -1.;
  }

  double energy, et, eSeed, eSpd, ePrs;
  unsigned int area;
  evalParam(hypo, energy, et, eSeed, eSpd, ePrs, area);
  
  double chi2 = -999.;
  if (m_tracking) {
    if( 1 != hypo->clusters().size() ){
      if ( msgLevel( MSG::DEBUG) )debug() <<"Number of clusters != 1"<<endmsg;
      return -1.;
    }
    const SmartRef<LHCb::CaloCluster> cluster=hypo->clusters().front();
    chi2 = evalChi2(cluster);
  }    
  return evalLikelihood(energy, et, eSeed, eSpd, ePrs, chi2, area);
};  




//***********************************************************
double CaloPhotonIdAlg::evalChi2(const LHCb::CaloCluster* cluster) const {
  double chi2=-999.;
  if( !exist<LHCb::Calo2Track::IClusTrTable>(m_tableLocation)){
    Warning("ClusterMatch table not found at " + m_tableLocation,StatusCode::SUCCESS).ignore();
    counter("ClusterMatch table not found " + m_tableLocation ) +=1;
    return chi2;
  }
  LHCb::Calo2Track::IClusTrTable* table = get<LHCb::Calo2Track::IClusTrTable>( m_tableLocation ) ;
  const LHCb::Calo2Track::IClusTrTable::Range range = table -> relations ( cluster ) ;
  if( range.empty() )  { 
    chi2=1.e+6; 
  }            // bad match -> large value !
  else {
    chi2 = range.front().weight();
  }
  return chi2;
}
  
//***********************************************************
void CaloPhotonIdAlg::evalParam(const LHCb::CaloHypo* hypo,
					  double       &energy,
					  double       &et, 
					  double       &eSeed,
					  double       &eSpd,
					  double       &ePrs,
					  unsigned int &area) const {
  if( 1 != hypo->clusters().size() ){
    if ( msgLevel( MSG::DEBUG) )debug() <<"Number of clusters != 1"<<endmsg;
    return ;
  }

  const SmartRef<LHCb::CaloCluster> cluster=hypo->clusters().front();


  if( cluster->entries().empty() ){
    Warning("Empty entries",StatusCode::SUCCESS ).ignore();
    counter("Empty entries")+=1;
    return;
  }

  LHCb::CaloCluster::Entries::const_iterator iseed =
    LHCb::ClusterFunctors::locateDigit( cluster->entries().begin() ,
                                        cluster->entries().end  () ,
                                        LHCb::CaloDigitStatus::SeedCell  ) ;


  if( iseed == cluster->entries().end() ){
    Warning("Empty Entries",StatusCode::SUCCESS ).ignore();
    counter("Empty entries")+=1;
    return;
  }

  const LHCb::CaloDigit* seed = iseed->digit();
  if( 0 == seed ){
    Warning("Seed point to NULL",StatusCode::SUCCESS ).ignore();
    counter("NULL seed")+=1;
    return;
  }
  
  //***
  // Evaluate Estimator Parameters : Energy, EPrs, Chi2 and Eseed ...


  // Et
  LHCb::CaloMomentum mom( hypo );
  energy=mom.e();
  et=mom.pt();
  
  // Seed energy
  eSeed = energy>0.?(seed->e())/energy:-1.;
  
  // area
  area = seed->cellID().area();

  //Spd hit and Prs deposit
  eSpd = m_toCalo->energy( *hypo ,"Spd");
  ePrs = m_toCalo->energy( *hypo ,"Prs");

  return; 
}

//***********************************************************
double CaloPhotonIdAlg::evalLikelihood(double energy,
                                               double et,
                                               double eSeed,
                                               double eSpd,
                                               double ePrs,
                                               double chi2,
                                               unsigned int area) const {
  
  // Corresponding Energy bin
  double transfE=log(1.35914*energy)-6.21461;
  unsigned int ibin;

  // Initialization
  double estimator;
  double signal, backgr;
  signal=1.; 	backgr=1.;

  // SPD Hit
  if (eSpd>1) {
    if (0==area){
      if ( msgLevel( MSG::DEBUG) )debug()<<"Spy Lh(Photon) - Area 0/Spd ( Etransf="<<transfE<<")"<<endmsg;
      ibin=bin(transfE,m_ePrsYBin)+(m_ePrsYBin.size()+1)*bin(ePrs,m_ePrsXBin);
      if ( msgLevel( MSG::DEBUG) )debug()<<"EPrs bin="<<ibin<<endmsg;
      signal *= m_signalEPrsSpdData_0[ ibin ];
      backgr *= m_backgrEPrsSpdData_0[ ibin ];
      if ( msgLevel( MSG::DEBUG) )debug()<<"Partial Sig Pdf="<<m_signalEPrsSpdData_0[ ibin ]<<endmsg;
      if ( msgLevel( MSG::DEBUG) )debug()<<"Partial Bkg Pdf="<<m_backgrEPrsSpdData_0[ ibin ]<<endmsg;
      if (m_tracking) {
        ibin=bin(transfE,m_chi2YBin)+(m_chi2YBin.size()+1)*bin(chi2,m_chi2XBin);
        if ( msgLevel( MSG::DEBUG) )debug()<<"Chi2 bin="<<ibin<<endmsg;      
        signal *= m_signalChi2SpdData_0[ ibin ];
        backgr *= m_backgrChi2SpdData_0[ ibin ];
        if ( msgLevel( MSG::DEBUG) )debug()<<"Partial Sig Pdf="<<m_signalChi2SpdData_0[ ibin ]<<endmsg;
        if ( msgLevel( MSG::DEBUG) )debug()<<"Partial Bkg Pdf="<<m_backgrChi2SpdData_0[ ibin ]<<endmsg;
      }
      ibin=bin(transfE,m_seedYBin)+(m_seedYBin.size()+1)*bin(eSeed,m_seedXBin);
      if ( msgLevel( MSG::DEBUG) )debug()<<"Seed bin="<<ibin<<endmsg;      
      signal *= m_signalSeedSpdData_0[ ibin ];
      backgr *= m_backgrSeedSpdData_0[ ibin ];
      if ( msgLevel( MSG::DEBUG) )debug()<<"Partial Sig Pdf="<<m_signalSeedSpdData_0[ ibin ]<<endmsg;
      if ( msgLevel( MSG::DEBUG) )debug()<<"Partial Bkg Pdf="<<m_backgrSeedSpdData_0[ ibin ]<<endmsg;
    }
    if (1==area){
      ibin=bin(transfE,m_ePrsYBin)+(m_ePrsYBin.size()+1)*bin(ePrs,m_ePrsXBin);
      signal *= m_signalEPrsSpdData_1[ ibin ];
      backgr *= m_backgrEPrsSpdData_1[ ibin ];
      if (m_tracking){
        ibin=bin(transfE,m_chi2YBin)+(m_chi2YBin.size()+1)*bin(chi2,m_chi2XBin);
        signal *= m_signalChi2SpdData_1[ ibin ];
        backgr *= m_backgrChi2SpdData_1[ ibin ];
      }
      ibin=bin(transfE,m_seedYBin)+(m_seedYBin.size()+1)*bin(eSeed,m_seedXBin);
      signal *= m_signalSeedSpdData_1[ ibin ];
      backgr *= m_backgrSeedSpdData_1[ ibin ];
    }
    if (2==area){
      ibin=bin(transfE,m_ePrsYBin)+(m_ePrsYBin.size()+1)*bin(ePrs,m_ePrsXBin);
      signal *= m_signalEPrsSpdData_2[ ibin ];
      backgr *= m_backgrEPrsSpdData_2[ ibin ];
      if (m_tracking){
        ibin=bin(transfE,m_chi2YBin)+(m_chi2YBin.size()+1)*bin(chi2,m_chi2XBin);
        signal *= m_signalChi2SpdData_2[ ibin ];
        backgr *= m_backgrChi2SpdData_2[ ibin ];
      }
      ibin=bin(transfE,m_seedYBin)+(m_seedYBin.size()+1)*bin(eSeed,m_seedXBin);
      signal *= m_signalSeedSpdData_2[ ibin ];
      backgr *= m_backgrSeedSpdData_2[ ibin ];
    }
  }

  // No SPD Hit
  else {
    if (0==area){
      ibin=bin(transfE,m_ePrsYBin)+(m_ePrsYBin.size()+1)*bin(ePrs,m_ePrsXBin);
      signal *= m_signalEPrsData_0[ ibin ];
      backgr *= m_backgrEPrsData_0[ ibin ];
      if (m_tracking){
	ibin=bin(transfE,m_chi2YBin)+(m_chi2YBin.size()+1)*bin(chi2,m_chi2XBin);
	signal *= m_signalChi2Data_0[ ibin ];
	backgr *= m_backgrChi2Data_0[ ibin ];
      }
      ibin=bin(transfE,m_seedYBin)+(m_seedYBin.size()+1)*bin(eSeed,m_seedXBin);
      signal *= m_signalSeedData_0[ ibin ];
      backgr *= m_backgrSeedData_0[ ibin ];
    }
    if (1==area){
      ibin=bin(transfE,m_ePrsYBin)+(m_ePrsYBin.size()+1)*bin(ePrs,m_ePrsXBin);
      signal *= m_signalEPrsData_1[ ibin ];
      backgr *= m_backgrEPrsData_1[ ibin ];
      if (m_tracking){
        ibin=bin(transfE,m_chi2YBin)+(m_chi2YBin.size()+1)*bin(chi2,m_chi2XBin);
        signal *= m_signalChi2Data_1[ ibin ];
        backgr *= m_backgrChi2Data_1[ ibin ];
      }
      ibin=bin(transfE,m_seedYBin)+(m_seedYBin.size()+1)*bin(eSeed,m_seedXBin);
      signal *= m_signalSeedData_1[ ibin ];
      backgr *= m_backgrSeedData_1[ ibin ];
    }
    if (2==area){
      ibin=bin(transfE,m_ePrsYBin)+(m_ePrsYBin.size()+1)*bin(ePrs,m_ePrsXBin);
      signal *= m_signalEPrsData_2[ ibin ];
      backgr *= m_backgrEPrsData_2[ ibin ];
      if (m_tracking){
        ibin=bin(transfE,m_chi2YBin)+(m_chi2YBin.size()+1)*bin(chi2,m_chi2XBin);
        signal *= m_signalChi2Data_2[ ibin ];
        backgr *= m_backgrChi2Data_2[ ibin ];
      }
      ibin=bin(transfE,m_seedYBin)+(m_seedYBin.size()+1)*bin(eSeed,m_seedXBin);
      signal *= m_signalSeedData_2[ ibin ];
      backgr *= m_backgrSeedData_2[ ibin ];
    }
  }

  estimator=(signal+backgr>0.)?signal/(signal+backgr):-1.;

  if ( msgLevel( MSG::DEBUG) ){    
    debug() <<"Photon Candidate :"<<endmsg;
    debug() <<" -E       ="<<energy<<endmsg;
    debug() <<" -Et      ="<<et<<endmsg;
    debug() <<" -Spd hit ="<<eSpd<<endmsg;
    debug() <<" -EPrs    ="<<ePrs<<endmsg;
    if (m_tracking) debug() <<" -Chi2    ="<<chi2<<endmsg;
    debug() <<" -ESeed   ="<<eSeed<<endmsg;
    debug() <<"      => estimator="<<estimator<<endmsg;
  }
  
  return estimator;

};


