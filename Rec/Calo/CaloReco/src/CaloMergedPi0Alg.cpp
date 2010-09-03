// $Id: CaloMergedPi0Alg.cpp,v 1.28 2010-03-08 01:19:40 odescham Exp $
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <numeric>
#include <algorithm>
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h" 
// ============================================================================
#include "CaloInterfaces/ICaloClusterTool.h"
#include "CaloInterfaces/ICaloHypoTool.h"
// ============================================================================
#include "CaloDet/DeCalorimeter.h" 
// ============================================================================
#include "Event/CaloCluster.h"
#include "Event/CaloHypo.h"
#include "Event/CaloPosition.h"
#include "CaloUtils/CaloDataFunctor.h"
#include "CaloUtils/CaloAlgUtils.h"
#include "Event/CellID.h"
// ============================================================================
#include "Kernel/CaloCellID.h"
#include "GaudiKernel/SystemOfUnits.h"
// ============================================================================
// local
// ============================================================================
#include "CaloMergedPi0Alg.h"
// ============================================================================

// ============================================================================
/** @file CaloMergedPi0Alg.cpp
 * 
 *  Implementation file for class : CaloMergedPi0Alg
 * 
 *  @author Olivier Deschamps 
 *  @date 10/05/2002
 *
 *  Revision 1.2 2004/09/02 20:46:40  odescham 
 * - Transv. Shape parameters in option file 
 *
 */
// ============================================================================

DECLARE_ALGORITHM_FACTORY( CaloMergedPi0Alg );

// ============================================================================
/** Standard constructor
 *  @param   name   algorithm name 
 *  @param   svcloc pointer to service locator 
 */
// ============================================================================
CaloMergedPi0Alg::CaloMergedPi0Alg( const std::string& name    ,
                                    ISvcLocator*       svcloc  )
  : GaudiAlgorithm ( name , svcloc ) 
  //
  , m_inputData  (LHCb::CaloClusterLocation::Ecal   )
  , m_outputData (LHCb::CaloHypoLocation::MergedPi0s)
  , m_detData    (DeCalorimeterLocation::Ecal       )
  , m_ntuple              ( 0               ) 
  , m_ntupleLUN           ( "FILE1"                             )
  , m_nameOfSplitPhotons  ( LHCb::CaloHypoLocation::    SplitPhotons  )
  , m_nameOfSplitClusters ( LHCb::CaloClusterLocation:: EcalSplit     )
  , m_toolTypeNames       ()
  , m_tools               ()
  , m_pi0tools               ()
  , TrShOut_nospd ()
  , TrShMid_nospd ()
  , TrShInn_nospd ()
  , TrShOut_spd ()
  , TrShMid_spd ()
  , TrShInn_spd ()
  , SPar ()
  //
  , m_eT_Cut  ( -100 * Gaudi::Units::GeV )
  , m_mX_Iter ( 16         )
{
  m_toolTypeNames.push_back( "CaloExtraDigits/SpdExtraG" ) ;
  m_toolTypeNames.push_back( "CaloExtraDigits/PrsExtraG" ) ;
  /// NTuple directory
  declareProperty ( "NTupleDirectory"        , m_ntupleLUN           ) ;
  declareProperty ( "SplitPhotons"           , m_nameOfSplitPhotons  ) ;
  declareProperty ( "SplitClusters"          , m_nameOfSplitClusters ) ;
  // tool to be apllyed to all hypos 
  declareProperty ( "Tools"                 , m_toolTypeNames       ) ;
  declareProperty ( "Pi0Tools"              , m_pi0toolTypeNames       ) ;
  // Transv. shape parametrization
  declareProperty ( "TrShOut_nospd"         , TrShOut_nospd ) ;
  declareProperty ( "TrShMid_nospd"         , TrShMid_nospd ) ;
  declareProperty ( "TrShInn_nospd"         , TrShInn_nospd ) ;
  declareProperty ( "TrShOut_spd"           , TrShOut_spd ) ;
  declareProperty ( "TrShMid_spd"           , TrShMid_spd ) ;
  declareProperty ( "TrShInn_spd"           , TrShInn_spd ) ;
  declareProperty ( "SPar"                  , SPar ) ;
  declareProperty ( "LPar_Al1"              , LPar_Al1 ) ;
  declareProperty ( "LPar_Al2"              , LPar_Al2 ) ;
  declareProperty ( "LPar_Al3"              , LPar_Al3 ) ;
  declareProperty ( "LPar_Be0"              , LPar_Be0 ) ;
  declareProperty ( "LPar_Be1"              , LPar_Be1 ) ;
  declareProperty ( "LPar_Be2"              , LPar_Be2 ) ;
  declareProperty ( "LPar_Be3"              , LPar_Be3 ) ;
  declareProperty ( "LPar_z0"               , LPar_z0  ) ;

  // added by V.B. 2004-10-27
  declareProperty ( "EtCut"                 , m_eT_Cut  ) ;
  declareProperty ( "MaxIterations"         , m_mX_Iter ) ;

  declareProperty ( "InputData"         , m_inputData    ) ;  
  declareProperty ( "OutputData"        , m_outputData   ) ;  
  declareProperty ( "Detector"          , m_detData      ) ;  
  declareProperty ( "CreateSplitClustersOnly"    , m_createClusterOnly = false) ;


  // default context-dependent locations
  m_inputData  = LHCb::CaloAlgUtils::CaloClusterLocation( "Ecal" , context()  );
  m_outputData = LHCb::CaloAlgUtils::CaloHypoLocation("MergedPi0s", context() );
  m_nameOfSplitPhotons  = LHCb::CaloAlgUtils::CaloHypoLocation("SplitPhotons", context() );
  m_nameOfSplitClusters = LHCb::CaloAlgUtils::CaloSplitClusterLocation(context() );

  
  setProperty ( "PropertiesPrint" , true ) ;
  
}
// ============================================================================
// destructor
// ============================================================================
CaloMergedPi0Alg::~CaloMergedPi0Alg() {};


// ============================================================================
/** standard algorithm initialization 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloMergedPi0Alg::initialize() 
{
  
  debug()<< "==> Initialise" << endmsg;
  
  StatusCode sc = GaudiAlgorithm::initialize();
  if( sc.isFailure() ) 
  { return Error("Could not initialize the base class!",sc);}

  //
  if(m_createClusterOnly)info() << "only SplitClusters to be produced" << endmsg;
  
  // locate tools
  for ( Names::const_iterator it = m_toolTypeNames.begin() ;m_toolTypeNames.end() != it ; ++it ){
    ICaloHypoTool* t = tool<ICaloHypoTool>( *it , this );
    if( 0 == t ) { return StatusCode::FAILURE ; }
    m_tools.push_back( t ) ;
  }
  for ( Names::const_iterator it = m_pi0toolTypeNames.begin() ;m_pi0toolTypeNames.end() != it ; ++it ){
    ICaloHypoTool* t = tool<ICaloHypoTool>( *it , this );
    if( 0 == t ) { return StatusCode::FAILURE ; }
    m_pi0tools.push_back( t ) ;
  }
  
  // check vectors of parameters 
  if( 10 != TrShOut_nospd.size() ) 
  { return Error ( "Invalid number of parameters" ) ; }
  if( 10 != TrShMid_nospd.size() ) 
  { return Error ( "Invalid number of parameters" ) ; }
  if( 10 != TrShInn_nospd.size() ) 
  { return Error ( "Invalid number of parameters" ) ; }
  if( 10 != TrShOut_spd.size()   ) 
  { return Error ( "Invalid number of parameters" ) ; }
  if( 10 != TrShMid_spd.size()   ) 
  { return Error ( "Invalid number of parameters" ) ; }
  if( 10 != TrShInn_spd.size()   ) 
  { return Error ( "Invalid number of parameters" ) ; }
  if( 3  != SPar.size()          )
  { return Error ( "Invalid number of parameters" ) ; }
  
  return StatusCode::SUCCESS;
};

// ============================================================================
/** standard algorithm finalization 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloMergedPi0Alg::finalize() 
{  
  m_tools.clear() ;
  m_pi0tools.clear() ;
  /// finalize the base class 
  return GaudiAlgorithm::finalize();
};

// ============================================================================
/** helper function to calculate number of digits 
 *  in the cluster with given status 
 *  @param cluster pointet to the cluster object
 *  @param status  digit statsu to be checked
 *  @return number of digits with given status.
 *       In the case of errors it returns -1
 */
// ============================================================================
long CaloMergedPi0Alg::numberOfDigits 
( const LHCb::CaloCluster*             cluster ,
  const LHCb::CaloDigitStatus::Status& status  ) const 
{
  /// check arguments 
  if( 0 == cluster ) 
  { Error(" numberOfDigits! CaloCluster* points to NULL!").ignore(); return -1;}
  ///
  long num = 0 ;
  for( LHCb::CaloCluster::Entries::const_iterator entry = 
       cluster->entries().begin() ;
       cluster->entries().end() != entry ; ++entry )
  {
    if( entry->status() & status ) { ++num ; }
  }
  ///
  return num ;
};

// ============================================================================
/*  O. Deschamps 10 Mai 2002
 *
 * --------------------------------------------------------
 *  Fit of the transversal energy deposit for PhotonShower 
 * --------------------------------------------------------
 *
 */
// ============================================================================
double CaloMergedPi0Alg::TrShape
( const int          /* Neig */ ,
  const unsigned int    area    ,
  const double          SpdHit  ,
  const double          D3D     )
{
  
  Parameters TrShPar;
  
  if ( 0 == area && 0 == SpdHit ) { TrShPar = TrShOut_nospd; }
  if ( 1 == area && 0 == SpdHit ) { TrShPar = TrShMid_nospd; }
  if ( 2 == area && 0 == SpdHit ) { TrShPar = TrShInn_nospd; }
  if ( 0 == area && 0 != SpdHit ) { TrShPar = TrShOut_spd; }
  if ( 1 == area && 0 != SpdHit ) { TrShPar = TrShMid_spd; }
  if ( 2 == area && 0 != SpdHit ) { TrShPar = TrShInn_spd; }
  
  

  double x = D3D;
  double Frac = 0;
  if( 0.5 < D3D ){
    Frac =  TrShPar[0]*exp(-TrShPar[1]*x) ;
    Frac += TrShPar[2]*exp(-TrShPar[3]*x) ;
    Frac += TrShPar[4]*exp(-TrShPar[5]*x) ;
  }
  if( 0.5 >= D3D ){
    Frac  = 2. ;
    Frac -=  TrShPar[6]*exp(-TrShPar[7]*x) ;
    Frac -=  TrShPar[8]*exp(-TrShPar[9]*x) ;
  }



  if( 0. > Frac ) { Frac=0.; }
  if( 1. < Frac ) { Frac=1.; }
  return Frac ;
};
// ============================================================================
/*  O. Deschamps 10 Mai 2002
 *
 * ---------------------------------------
 *  Longitudinal Photon Shower barycenter 
 * ---------------------------------------
 *
 *  L-Correction for Photon Shower *
 */
// ============================================================================
double CaloMergedPi0Alg::BarZ
( const double       e    ,
  const double       eprs ,
  const unsigned int area ,
  const double       x    ,
  const double       y    ,
  const double    /* z */ )
{
  
  double z0 = LPar_z0[0] ;// Parameter tuned wrt to z0=12566 mm !!
  // Uncorrected angle
  double tth   = sqrt ( pow(x,2) + pow(y,2) ) / z0 ;
  double cth   = cos  ( atan( tth ) ) ;
  // Corrected angle
  double alpha = 
    LPar_Al1[area] -exp(LPar_Al2[area] -  LPar_Al3[area] * eprs/Gaudi::Units::MeV);
  double beta  = 
    LPar_Be1[area] +exp(LPar_Be2[area] -  LPar_Be3[area] * eprs/Gaudi::Units::MeV);

  //  if(0 >= eprs ){beta = LPar_Be0[area]; }
  double tgfps = alpha * log(e/Gaudi::Units::GeV) + beta ;
  tth = tth / ( 1. + tgfps * cth / z0 ) ;
  cth= cos( atan( tth ) ) ;
  double dzfps = cth * tgfps ;
// Recompute Z position 
  double DeltaZ = z0+dzfps;
  return DeltaZ ;
};
// ============================================================================
/*  O. Deschamps 10 Mai 2002
 *
 * ---------------------------------------
 *  Transversal Photon Shower barycenter 
 * ---------------------------------------
 *
 *  barycenter from 3x3 Cluster Matrix
 *  + S-Correction for Photon Shower *
 */
// ============================================================================
double CaloMergedPi0Alg::BarXY(const int axis,
                         const unsigned int area,
                         const double e[3][3])
{
  // Barycenter from 3x3 Energy Matrix

  const double dpx=-1.;
  const double dpy=+1.;
  const double dzx=0.;
  const double dzy=0.;
  const double dmx=+1.;
  const double dmy=-1.;
  double Esum=0;
  double AsymX=0;
  double AsymY=0;
  Esum = e[0][0] + e[0][1] + e[0][2]
       + e[1][0] + e[1][1] + e[1][2]
       + e[2][0] + e[2][1] + e[2][2];
  AsymX = dpx*e[0][2] + dpx*e[1][2] + dpx*e[2][2]
        + dzx*e[0][1] + dzx*e[1][1] + dzx*e[2][1]
        + dmx*e[0][0] + dmx*e[1][0] + dmx*e[2][0];
  AsymY = dpy*e[0][0] + dpy*e[0][1] + dpy*e[0][2]
        + dzy*e[1][0] + dzy*e[1][1] + dzy*e[1][2]
        + dmy*e[2][0] + dmy*e[2][1] + dmy*e[2][2];
  if(0 != Esum){AsymX = AsymX/Esum;}
  if(0 != Esum){AsymY = AsymY/Esum;}
  
          
  // S-correction of Energy-weighted Barycenter

  double Bparx=SPar[area];
  double Bpary=SPar[area];
  
  double ArgX,ArgY;
  double DeltaX=0;
  double DeltaY=0;
  double DeltaXY=0;
  ArgX=2.*AsymX*sinh(0.5/Bparx);
  DeltaX=Bparx*log(ArgX+sqrt(ArgX*ArgX+1.));
  ArgY=2.*AsymY*sinh(0.5/Bpary);
  DeltaY=Bpary*log(ArgY+sqrt(ArgY*ArgY+1.));
  
  if(1 == axis){DeltaXY = DeltaX;} 
  if(2 == axis){DeltaXY = DeltaY;} 
  
  return DeltaXY ;
};

// ============================================================================
/** standard algorithm execution 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloMergedPi0Alg::execute() 
{
  if ( msgLevel ( MSG::DEBUG ) ){ debug() << "==> Execute" << endmsg; }

  /// avoid long names
  
  using namespace  LHCb::CaloDataFunctor;
  typedef LHCb::CaloClusters              Clusters;
  typedef Clusters::iterator        Iterator;

  /// load data
  SmartDataPtr<DeCalorimeter> detector( detSvc() , m_detData );
  if( !detector ){ return Error("could not locate calorimeter '"+m_detData+"'");}
  SmartDataPtr<Clusters>  clusters( eventSvc() , m_inputData );
  if( 0 == clusters ){ return Error("Could not load input data ='"+m_inputData+"'");}

  // create the container of split clusters (check it does not exist first)
  bool doSplitC = true;
  LHCb::CaloClusters* clusts = new LHCb::CaloClusters();
  try {
    put(clusts , m_nameOfSplitClusters);
  }
  catch (GaudiException &e) {
    Warning("Exception is caught",StatusCode::SUCCESS).ignore();
    doSplitC = false;
    delete clusts;
  }



  //    doSplitC = false;
  //  delete clusts;
  //};

  // create the containers of pi0s & SPlitPhotons
  LHCb::CaloHypos* pi0s = new LHCb::CaloHypos();
  LHCb::CaloHypos* phots = new LHCb::CaloHypos();
  // put on TES if required
  if(!m_createClusterOnly){
    put( pi0s , m_outputData );            
    put( phots , m_nameOfSplitPhotons);
  }
  
  

  // count all clusters 
  // modified by V.B. 2004-10-27
  const size_t cluscount = clusters->size() ;
  if ( msgLevel ( MSG::DEBUG ) ){ 
    debug() << " -----> #cluster " << cluscount << endmsg;
  }
  
  // SpdHit in front of Cluster Seed (new 09/02/2004)
  // (moved from internal loop)
  LHCb::CaloDigits* spds = get<LHCb::CaloDigits>( LHCb::CaloDigitLocation::Spd );
  
  // Prs deposit in front of Cluster Seed (new 09/02/2004)
  // (moved from internal loop)
  LHCb::CaloDigits* prss = get<LHCb::CaloDigits>( LHCb::CaloDigitLocation::Prs );
  
  // added by V.B 2004-10-27: estimator of cluster transverse energy 
  LHCb::CaloDataFunctor::EnergyTransverse<const LHCb::CaloCluster*,const DeCalorimeter*> eT ( detector ) ;
  
  int ik = 0;
  /// loop over all clusters 
  for( Iterator icluster = clusters->begin() ; clusters->end() != icluster ; ++icluster ){
    LHCb::CaloCluster* cluster = *icluster ;
    if( 0 == cluster )                { continue ; }   ///< CONTINUE!
    ik++;
    if ( msgLevel ( MSG::DEBUG ) ){
      debug() << "Cluster " << ik << "/" << cluscount << endmsg;
    }
    // added by V.B. 2004-10-27
    if ( 0 < m_eT_Cut &&  m_eT_Cut > eT( cluster ) ) { continue ; }
    if ( msgLevel ( MSG::DEBUG ) ){ 
      debug() << " selected clusters " << eT( cluster ) << endmsg;
    }
 
    //----------------------//
    //        My code       //
    //----------------------//
    
    /// Check # of digits/cells in this cluster 
    int m_cluDigs;
    m_cluDigs = cluster->entries().size() ;
    if( 1 >= m_cluDigs )                { return 0; }   ///< CONTINUE!


    /// Find Cluster Seed 
    const LHCb::CaloCluster::Digits::const_iterator iSeed = 
      clusterLocateDigit( cluster->entries().begin() ,
                          cluster->entries().end  () ,
                          LHCb::CaloDigitStatus::SeedCell     );
    const LHCb::CaloDigit* seed = iSeed->digit() ;
    if( 0 == seed) { continue ; }   ///< CONTINUE!
    const LHCb::CaloCellID  idseed = seed->cellID() ;
    double seede  = seed->e();
    int seedrow  = idseed.row();
    int seedcol  = idseed.col();
    
    // 
    if ( msgLevel ( MSG::DEBUG ) ){ 
      debug() << " get Spd/Prs digits " << endmsg;
    }
    double SpdHit = 0 ;
    const LHCb::CaloDigit* spddigit = spds->object( seed->key() );
    if( 0 != spddigit ) { SpdHit = spddigit->e() ; }
    double PrsDep = 0 ;
    const LHCb::CaloDigit* prsdigit = prss->object( seed->key() );
    if( 0 != prsdigit ) { PrsDep = prsdigit->e() ; }
      
    if ( msgLevel ( MSG::DEBUG ) ){ 
      debug() << " -----> Find SubSeed " << endmsg;
    }
    /// Find SubSeed  - Loop on Cluster CaloCluster:Digits
    // (New DigitStatus to be defined)
    
    // double sube=-99; commented by I.B 2004-08-20
    // int subrow=0;
    // int subcol=0;
    double sube   = -1 * Gaudi::Units::TeV ;
    int    subrow = -1000    ;
    int    subcol = -1000    ;
      
    if ( msgLevel ( MSG::DEBUG ) ){ 
      debug() << " Loop over digits " << endmsg;
    }
    for( LHCb::CaloCluster::Digits::const_iterator it1 =cluster->entries().begin() ;
         cluster->entries().end() != it1 ; ++it1 ){
      const LHCb::CaloDigit* dig = it1->digit() ;    // CaloDigit
      if( 0 == dig) { continue ; }   ///< CONTINUE!
      const LHCb::CaloCellID  id = dig->cellID() ;
      int cellrow  = id.row();
      int cellcol  = id.col();
      double ecel = dig->e()*it1->fraction();
      if (ecel>sube 
          && ecel < seede 
          && abs(cellrow-seedrow)<=1 
          && abs(cellcol-seedcol)<=1){
        sube=ecel;
        subrow=cellrow;
        subcol=cellcol;
      }
    }
      
    if ( -1000 == subrow && -1000 == subcol ) 
    { Warning("Cluster without 'subcel' is found, skip it").ignore() ; continue ; }
    
    if ( msgLevel ( MSG::DEBUG ) ){ 
      debug() << " -----> Define large Cluster " << endmsg;
    }

      /// Fill  3x3 SubClusters - Loop on all CaloCluster:Digits
      //   +-----+-----+-----+
      //   | 2 0 | 2 1 | 2 2 | 
      //   +-----+-----+-----+
      //r  | 1 0 | 1 1 | 1 2 | 
      //o  +-----+-----+-----+
      //w  | 0 0 | 0 1 | 0 2 | 
      //^  +-----+-----+-----+
      //| -> col    
    
    double SubClusEne[2][3][3];
    int SubClusMask[2][3][3];
    const LHCb::CaloDigit* SubClus[2][3][3];
    for(int iss=0;iss<2;++iss){
      for(int irr=0;irr<3;++irr){  
        for(int icc=0;icc<3;++icc){  
          SubClusMask[iss][irr][icc]=1;
          SubClusEne[iss][irr][icc]=0;
          SubClus[iss][irr][icc]=0;
        }
      }
    }
    int ir,ic;
    for( LHCb::CaloCluster::Digits::const_iterator it2 =cluster->entries().begin() ;
           cluster->entries().end() != it2 ; ++it2 ){
      const LHCb::CaloDigit* dig = it2->digit() ;    
      if( 0 == dig) { continue ; }   ///< CONTINUE!
      const LHCb::CaloCellID  id = dig->cellID() ;
      int cellrow  = id.row();
      int cellcol  = id.col();
      double ecel = dig->e()*it2->fraction();
      
      if (abs(seedrow-cellrow)<=1 && abs(seedcol-cellcol)<=1){
        ir=cellrow-seedrow+1;
        ic=cellcol-seedcol+1;
        SubClusEne[0][ir][ic]=std::max(ecel,0.) ;
              SubClus[0][ir][ic]=dig;
      }
      if (abs(subrow-cellrow)<=1 && abs(subcol-cellcol)<=1){
        ir=cellrow-subrow+1;
        ic=cellcol-subcol+1;
        SubClusEne[1][ir][ic]=std::max(ecel,0.) ;
        SubClus[1][ir][ic]=dig;
      }
    }
    
    if ( msgLevel ( MSG::DEBUG ) ){ 
      debug() << " -----> Split cluster " << endmsg;
    }
    /// Defined proto-SubCluster from SubClusEne (split procedure)
    
    double Emax[2];
    Emax[0]=-99;
    Emax[1]=-99;
    int idr[2];
    int idc[2];
    idr[0]=0;
    idc[0]=0;
    idr[1]=0;
    idc[1]=0;
    for(int is1=0;is1<2;++is1){
      for(int ir1=0;ir1<3;++ir1){  
        for(int ic1=0;ic1<3;++ic1){  
          if(SubClusEne[is1][ir1][ic1]>Emax[is1]  && ir1*ic1 !=1 ){
            Emax[is1]=SubClusEne[is1][ir1][ic1];
            idr[is1]=ir1-1;
            idc[is1]=ic1-1;
          }
        }
      }
    }


    int erasel[2];
    int erasec[2];
    erasel[0]=-1;
    erasec[0]=-1;
    erasel[1]=-1;
    erasec[1]=-1;
    // Define working array
    double Ework[2][3][3];
    double Weight[2][3][3];
    for(int is=0;is<2;++is){
      if( 0 == idr[is]*idc[is]){
        if(idc[is] == 0){
          erasel[is]=idr[is]+1;
        }
        if( 0 == idr[is] ){
          erasec[is]=idc[is]+1;
        }
      }
      
      if( 1 == abs(idr[is]*idc[is]) ){
        if( 0 == is           && SubClusEne[is][idr[is]+1][1] <= SubClusEne[is][1][idc[is]+1]){
          erasel[is]=idr[is]+1;
        }
        if( 0 == is && SubClusEne[is][idr[is]+1][1] >= SubClusEne[is][1][idc[is]+1]){
          erasec[is]=idc[is]+1;
        }
        if( 1 == is && SubClusEne[is][idr[is]+1][1] >= SubClusEne[is][1][idc[is]+1]){
          erasel[is]=idr[is]+1;
        }
        if( 1 == is           && SubClusEne[is][idr[is]+1][1] <= SubClusEne[is][1][idc[is]+1]){
          erasec[is]=idc[is]+1;
        }
      }
    }
    for(int is2=0;is2<2;++is2){
      for(int ik=0;ik<3;++ik){
        if(-1 != erasel[is2] ){
          SubClusEne[is2][erasel[is2]][ik]=0;
          SubClusMask[is2][erasel[is2]][ik]=0;
        }
        if(-1 != erasec[is2] ){
          SubClusEne[is2][ik][erasec[is2]]=0;
          SubClusMask[is2][ik][erasec[is2]]=0;
        }
      }
    }
    
    for(int isi=0;isi<2;++isi){
      for(int iri=0;iri<3;++iri){
        for(int ici=0;ici<3;++ici){
          Ework[isi][iri][ici]=SubClusEne[isi][iri][ici];
          Weight[isi][iri][ici] = 0.;
        }
      }
    }
    
      
    /// Iterative reconstruction of SubClusters
    if ( msgLevel ( MSG::DEBUG ) ){ 
      debug() << " -----> Iterative reconstruction " << endmsg;
    }
    double SubSize[2],SubX[2],SubY[2],SubZ[2] ;
    
    long mxiter = m_mX_Iter;
    // long mxiter=16;
    
    long iter=0;
    for( long iterat = 0 ; iterat < mxiter ; ++iterat){
      // SubClusters X/Y/Z barycenter
      iter++;
      double Etemp[3][3];
      double Ene3x3[2];
      
      for(int is=0;is<2;++is){
        const LHCb::CaloDigit* digit   = SubClus[is][1][1];
        if( 0 == SubClus[is][1][1]) { continue ; }   ///< CONTINUE!
        const LHCb::CaloCellID  idigit = digit->cellID() ;
        SubSize[is] = detector->cellSize( idigit ) ;
        SubX[is]    = detector->cellX   ( idigit ) ;
        SubY[is]    = detector->cellY   ( idigit ) ;
        SubZ[is]    = detector->cellZ   ( idigit ) ;
        unsigned int area = idigit.area();
        Ene3x3[is]=0;
        for(int ir=0;ir<3;++ir){
          for(int ic=0;ic<3;++ic){
            Etemp[ir][ic]=Ework[is][ir][ic];
            Ene3x3[is]=Ene3x3[is]+Ework[is][ir][ic];
            Ework[is][ir][ic]=SubClusEne[is][ir][ic];
          }
        }
        SubX[is]=SubX[is]-BarXY(1,area,Etemp)*SubSize[is];
        SubY[is]=SubY[is]-BarXY(2,area,Etemp)*SubSize[is];
        SubZ[is]=BarZ(Ene3x3[is],PrsDep,area,SubX[is],SubY[is],SubZ[is]);
        if ( msgLevel ( MSG::DEBUG ) ){ 
          debug() << "==> Lcorr " << SubZ[is] << endmsg;
        }
      }
      // Reconstructing Overlap
      
      
      for(int is=0;is<2;++is){
        int js = 0 ;
        if ( 0 == is ){ js = 1; }
        if ( 1 == is ){ js = 0; }
        for(int ir=0;ir<3;++ir){
          for(int ic=0;ic<3;++ic){
            if(1 == SubClusMask[js][ir][ic] ) {
              const LHCb::CaloDigit* seed    = SubClus[is][1][1];;
              if( 0 == seed) { continue ; }   ///< CONTINUE!
              const LHCb::CaloCellID  idseed = seed->cellID() ;
              const LHCb::CaloDigit* cel      = SubClus[js][ir][ic];;
              if( 0 == cel) { continue ; }   ///< CONTINUE!
              const LHCb::CaloCellID  idcel   = cel->cellID() ;
              unsigned int area = idcel.area();
              int rowc = idcel.row();
              int colc = idcel.col();
              int rows = idseed.row();
              int cols = idseed.col();
              double CelSize = detector->cellSize( idcel);
              double CelX    = detector->cellX   ( idcel);
              double CelY    = detector->cellY   ( idcel);
              // 3D  distance from the current cell to 
              // the other SubCluster barycenter
              double CelZ=(  SubX[is]*SubX[is]+
                             SubY[is]*SubY[is]+
                             SubZ[is]*SubZ[is]-
                             SubX[is]*CelX-
                             SubY[is]*CelY)/SubZ[js];
              double D3D=sqrt((SubX[is]-CelX)*(SubX[is]-CelX)+
                              (SubY[is]-CelY)*(SubY[is]-CelY)+
                              (SubZ[is]-CelZ)*(SubZ[is]-CelZ))/CelSize;
              double D2D=sqrt((SubX[is]-CelX)*(SubX[is]-CelX)+
                              (SubY[is]-CelY)*(SubY[is]-CelY))/CelSize;
              int Neig=1;
              if(0.5 > D2D)Neig=0;
              double Frac=TrShape(Neig,area,SpdHit,D3D);
              double EFrac=Ene3x3[is]*Frac;
              int jr=rowc-rows+1;
              int jc=colc-cols+1;
              Weight[js][ir][ic]=1.;
              if(1 >= abs(jr-1) && 1 >= abs(jc-1))Weight[is][jr][jc]=0.;
              if( EFrac < Ework[js][ir][ic]  ){
                Ework[js][ir][ic]=Ework[js][ir][ic]-EFrac;
                Weight[js][ir][ic]=1.-Frac;
                if(1 >= abs(jr-1) && 1 >= abs(jc-1)){
                  Ework[is][jr][jc]=EFrac;
                  Weight[is][jr][jc]=Frac;
                }
              }
            }
          }
        }
      }
    }
    
    
    
    /// End of Reconstruction aLgorithm 
    /// Define photon and pi0 from Subcluster
    
    if ( msgLevel ( MSG::DEBUG ) ){ 
      debug() << " -----> Compute Pi0 mass " << endmsg;
    }
    
    // energy
    double Ene3x3[2];
    double PosX[2];
    double PosY[2];
    
    for(int is=0;is<2;++is){
      const LHCb::CaloDigit* digit   = SubClus[is][1][1];;
      if( 0 == SubClus[is][1][1]) { continue ; }   ///< CONTINUE!
      const LHCb::CaloCellID  idigit = digit->cellID() ;
      unsigned int area = idigit.area();
      SubSize[is] = detector->cellSize( idigit ) ;
      SubX[is]    = detector->cellX   ( idigit ) ;
      SubY[is]    = detector->cellY   ( idigit ) ;
      SubZ[is]    = detector->cellZ   ( idigit ) ;
      double Etemp[3][3];
      Ene3x3[is]=0;
      for(int ir=0;ir<3;++ir){
        for(int ic=0;ic<3;++ic){
          Etemp[ir][ic]=Ework[is][ir][ic];
          Ene3x3[is]=Ene3x3[is]+Ework[is][ir][ic];
        }
        }
      SubX[is]=SubX[is]-BarXY(1,area,Etemp)*SubSize[is];
      SubY[is]=SubY[is]-BarXY(2,area,Etemp)*SubSize[is];
      SubZ[is]=BarZ(Ene3x3[is],PrsDep,area,SubX[is],SubY[is],SubZ[is]);
      PosX[is]=detector->cellX( SubClus[is][1][1]->cellID() )
        -SubSize[is]*(+ Etemp[0][0] + Etemp[1][0] + Etemp[2][0]
                      - Etemp[0][2] - Etemp[1][2] - Etemp[2][2])/Ene3x3[is];
      PosY[is]=detector->cellY( SubClus[is][1][1]->cellID() )
        -SubSize[is]*(+ Etemp[0][0] + Etemp[0][1] + Etemp[0][2]
                      - Etemp[2][0] - Etemp[2][1] - Etemp[2][2])/Ene3x3[is];
    }
    
    double ep1=0;
    double ep2=0;
    ep1=Ene3x3[0];
    ep2=Ene3x3[1];
    
    // momenta
    double xp1,yp1,zp1,xn1;
    double xp2,yp2,zp2,xn2;
    
    xp1=SubX[0];
    yp1=SubY[0];
    zp1=SubZ[0];
    xn1=sqrt(xp1*xp1+yp1*yp1+zp1*zp1);
    
    
    xp1=xp1*ep1/xn1;
    yp1=yp1*ep1/xn1;
    zp1=zp1*ep1/xn1;
    
    
    xp2=SubX[1];
    yp2=SubY[1];
    zp2=SubZ[1];
    xn2=sqrt(xp2*xp2+yp2*yp2+zp2*zp2);
    xp2=xp2*ep2/xn2;
    yp2=yp2*ep2/xn2;
    zp2=zp2*ep2/xn2;
    
    // invariant mass
    
    double RecPi0Mas;
    double RecPi0Masq;
    RecPi0Masq=(ep1+ep2)*(ep1+ep2);
    RecPi0Masq=RecPi0Masq-(xp1+xp2)*(xp1+xp2);
    RecPi0Masq=RecPi0Masq-(yp1+yp2)*(yp1+yp2);
    RecPi0Masq=RecPi0Masq-(zp1+zp2)*(zp1+zp2);
    
    RecPi0Mas=0;

    if ( 0 < RecPi0Masq){
      RecPi0Mas=sqrt(RecPi0Masq);
    }
    
    
    const LHCb::CaloDigit*  seedig   = SubClus[0][1][1];;
    const LHCb::CaloCellID  idig     = seedig->cellID() ;
    // commented by I.B. 2004-08-20
    // double zpos                = detector->cellZ   ( idig ) ;
    // double csiz                = detector->cellSize( idig ) ;
    // double mpi0=0.1349;
    // double epi0=(ep1+ep2)/GeV;
    // double dmin=zpos*2*mpi0/epi0/csiz; 
    
    if ( msgLevel ( MSG::DEBUG ) ){ 
      debug() << " Pi0Mas =  " <<RecPi0Mas << endmsg;
    }
    
    /*
      Mid June 2002 
    */
    
    unsigned int KeepPi0=1;
    const double TruePi0Mas=134.9;
    const double SigmaRec=16.;
    const double LowMasSigmaCut=7.0;
    const double HighMasSigmaCut=999.0;
    // revision 1.7 (February 2004) : 
    // remove dmin cut and relax HighMasSigmaCut ...
    
    if(RecPi0Mas < TruePi0Mas - LowMasSigmaCut*SigmaRec){KeepPi0=0;}
    if(RecPi0Mas > TruePi0Mas + HighMasSigmaCut*SigmaRec){KeepPi0=0;}
    //if(dmin      >  3.){KeepPi0=0;}
    //if(dmin      <= 0.){KeepPi0=0;}
    
    /*
      Fill CaloHypos and SubClusters if "good" MergePi0
    */
    if ( msgLevel ( MSG::DEBUG ) ){ 
      debug() << "Keep MergedPi0 ? " << KeepPi0 << endmsg;
    }
    
    if( 1 == KeepPi0){
      if ( msgLevel ( MSG::DEBUG ) ){ 
        debug() << " Store Merged Pi0 " << endmsg;
      }
      //Defined new Calo SubCluster pointed by PhotonFromMergedPi0 CaloHypos
      LHCb::CaloCluster* cl1 = new LHCb::CaloCluster();
      LHCb::CaloCluster* cl2 = new LHCb::CaloCluster();
      LHCb::CaloCellID seed1;
      LHCb::CaloCellID seed2;
      // Init the 2 new CaloClusters 
      // with the original cluster digits, owned-status'ed and  0-weighted
      if ( msgLevel ( MSG::DEBUG ) ){ 
        debug() << "Loop over cluster entries " << cluster->entries().size() << endmsg;
      }
      
      for( LHCb::CaloCluster::Digits::const_iterator it3 = cluster->entries().begin() ;
           cluster->entries().end() != it3 ; ++it3 ){
        const LHCb::CaloDigit* d = it3->digit() ;   
        if(NULL == d)continue;
        LHCb::CaloDigitStatus::Status s1   = LHCb::CaloDigitStatus::OwnedCell    ;
        LHCb::CaloDigitStatus::Status s2   = LHCb::CaloDigitStatus::OwnedCell    ;
        double     w1=0.;
        double     w2=0.;
        // Then fill correctly the 3x3 matrix cells around seed and subseed
        for(int ir=0;ir<3;++ir){
          for(int ic=0;ic<3;++ic){
            const LHCb::CaloDigit* d1 = SubClus[0][ir][ic];
            if (d == d1 ) {
              w1 = Weight[0][ir][ic] ;      
              s1   = LHCb::CaloDigitStatus::OwnedCell      | 
                LHCb::CaloDigitStatus::UseForEnergy   | 
                LHCb::CaloDigitStatus::UseForPosition | 
                LHCb::CaloDigitStatus::UseForCovariance  ;
              if(1 == ir && 1==ic){
                s1   =  LHCb::CaloDigitStatus::SeedCell       | 
                  LHCb::CaloDigitStatus::LocalMaximum   | 
                  LHCb::CaloDigitStatus::UseForEnergy   | 
                  LHCb::CaloDigitStatus::UseForPosition | 
                  LHCb::CaloDigitStatus::UseForCovariance  ;
                seed1 = d1->cellID();
              }
            }
          }
        }
        for(int ir=0;ir<3;++ir){
          for(int ic=0;ic<3;++ic){
            const LHCb::CaloDigit* d2 = SubClus[1][ir][ic];
            if (d == d2 ) {
              w2 = Weight[1][ir][ic] ;      
              s2   =  LHCb::CaloDigitStatus::OwnedCell      | 
                LHCb::CaloDigitStatus::UseForEnergy   | 
                LHCb::CaloDigitStatus::UseForPosition | 
                LHCb::CaloDigitStatus::UseForCovariance  ;
              if(1 == ir && 1==ic){
                s2   = LHCb::CaloDigitStatus::SeedCell       | 
                  LHCb::CaloDigitStatus::UseForEnergy   | 
                  LHCb::CaloDigitStatus::LocalMaximum   | 
                  LHCb::CaloDigitStatus::UseForPosition | 
                  LHCb::CaloDigitStatus::UseForCovariance  ;
                seed2 = d2->cellID();
              }
            }
          }
        }
        if ( msgLevel ( MSG::DEBUG ) ){ 
          debug() << "Logging cluster entry " << w1 << "/" << w2 << endmsg;
        }
        LHCb::CaloClusterEntry entry1( d , s1 , w1 );
        cl1->entries().push_back( entry1 );
        LHCb::CaloClusterEntry entry2( d , s2 , w2 );
        cl2->entries().push_back( entry2 );
        if ( msgLevel ( MSG::DEBUG ) ){ 
          debug() << " s1 after loop =  " <<s1<< endmsg;
        }
      }  
      
      if ( msgLevel ( MSG::DEBUG ) ){ 
        debug() << "Set seed " << endmsg;
      }
      
      cl1->setSeed( seed1 );
      cl2->setSeed( seed2 );
      
      if ( msgLevel ( MSG::DEBUG ) ){ 
        debug() << "Calculate position " << endmsg;
      }
      
      // OD calculate position for cluster
      LHCb::CaloPosition pp1 ;
      pp1.parameters()( LHCb::CaloPosition::X ) = PosX[0];
      pp1.parameters()( LHCb::CaloPosition::Y ) = PosY[0];
      pp1.setZ( SubZ[0] ) ;
      pp1.parameters()( LHCb::CaloPosition::E ) = ep1    ;
      pp1.center()( LHCb::CaloPosition::X ) = PosX[0];
      pp1.center()( LHCb::CaloPosition::Y ) = PosY[0];
      cl1->setPosition( pp1 );
      
      LHCb::CaloPosition pp2;
      pp2.parameters()( LHCb::CaloPosition::X ) = PosX[1];
      pp2.parameters()( LHCb::CaloPosition::Y ) = PosY[1];
      pp2.setZ( SubZ[1] ) ;
      pp2.parameters()( LHCb::CaloPosition::E ) = ep2    ;
      pp2.center()( LHCb::CaloPosition::X ) = PosX[1];        
      pp2.center()( LHCb::CaloPosition::Y ) = PosY[1];
      cl2 -> setPosition( pp2 );
      
      if ( msgLevel ( MSG::DEBUG ) ){ 
        debug() << "Store clusters " << endmsg;
      }

      if( doSplitC ){
        clusts->insert( cl1 ) ;
        clusts->insert( cl2 ) ;
      }
      
      
      if(!m_createClusterOnly){
        if ( msgLevel ( MSG::DEBUG ) ){ 
          debug() << "Store Pi0 CaloHypo" << endmsg;
        }
        
        // new CaloHypo for pi0      
        LHCb::CaloHypo* pi0 = new LHCb::CaloHypo();          
        pi0 -> setHypothesis( LHCb::CaloHypo::Pi0Merged ) ;
        pi0 -> addToClusters( *icluster );
        
        // new CaloHypo for gamma's
        if ( msgLevel ( MSG::DEBUG ) ){ 
          debug() << "Store gamma1 CaloHypo" << endmsg;
        }
        
        LHCb::CaloHypo* g1   = new LHCb::CaloHypo() ;
        g1 -> setHypothesis( LHCb::CaloHypo::PhotonFromMergedPi0 ) ;
        g1 -> addToClusters( *icluster )                ;
        g1 -> addToClusters( cl1       )                ;
        LHCb::CaloPosition* p1 = new LHCb::CaloPosition(pp1);//@todo use previously define pp1
        /*
          p1 ->parameters()( LHCb::CaloPosition::X ) = PosX[0];
          p1 ->parameters()( LHCb::CaloPosition::Y ) = PosY[0];
          p1 ->setZ( SubZ[0] ) ;
          p1 ->parameters()( LHCb::CaloPosition::E ) = ep1    ;
          
          p1 ->center()( LHCb::CaloPosition::X ) = PosX[0];
          p1 ->center()( LHCb::CaloPosition::Y ) = PosY[0];
        */
        g1 -> setPosition( p1);
        pi0 -> addToHypos ( g1 );
        
        if ( msgLevel ( MSG::DEBUG ) ){ 
          debug() << "Store gamma2 CaloHypo" << endmsg;
        }
        
        LHCb::CaloHypo* g2   = new LHCb::CaloHypo() ;
        g2 -> setHypothesis( LHCb::CaloHypo::PhotonFromMergedPi0 ) ;
        g2 -> addToClusters( *icluster )                ;
        g2 -> addToClusters( cl2       )                ;
        LHCb::CaloPosition* p2 = new LHCb::CaloPosition(pp2);//@todo use previously define pp2
        /*
          p2 ->parameters()( LHCb::CaloPosition::X ) = PosX[1];
          p2 ->parameters()( LHCb::CaloPosition::Y ) = PosY[1];
          p2 ->setZ( SubZ[1] ) ;
          p2 ->parameters()( LHCb::CaloPosition::E ) = ep2    ;
          
          p2 ->center()( LHCb::CaloPosition::X ) = PosX[1];
          p2 ->center()( LHCb::CaloPosition::Y ) = PosY[1];
        */
        g2 -> setPosition( p2);
        pi0 -> addToHypos ( g2 );
        pi0s -> insert( pi0 ) ;
        phots ->insert( g1  ) ;
        phots ->insert( g2  ) ;
        
        { // Apply the tool 
          if ( msgLevel ( MSG::DEBUG ) ){ 
            debug() << "Apply hypo tools on splitPhotons ("<< m_tools.size() << ")" << endmsg;
            debug() << "Apply hypo tools on MergedPi0s ("<< m_pi0tools.size() << ")" << endmsg;
          }
          
          int i = 0;
          for( Tools::iterator it = m_tools.begin() ; m_tools.end() != it ; ++it ){
            i++;
            if ( msgLevel ( MSG::DEBUG ) )debug() << " apply SplitPhoton tool " << i << "/" << m_tools.size() << endmsg;
            ICaloHypoTool* t = *it ;
            if( 0 == t ) { continue; } 
            StatusCode sc         = StatusCode::SUCCESS ;
            sc                    = (*t) ( g1 ) ;
            if( sc.isFailure() ) Error("Error from 'Tool' for g1 " , sc ).ignore() ; 
            sc                    = (*t) ( g2 ) ;          
            if( sc.isFailure() )Error("Error from 'Tool' for g2 " , sc ).ignore() ; 
          }
          i = 0;
          for( Tools::iterator it = m_pi0tools.begin() ; m_pi0tools.end() != it ; ++it ){
            i++;
            if ( msgLevel ( MSG::DEBUG ) )debug() << " apply MergedPi0 tool " << i << "/" << m_pi0tools.size() << endmsg;
            ICaloHypoTool* t = *it ;
            if( 0 == t ) { continue; } 
            StatusCode sc         = StatusCode::SUCCESS ;
            sc                    = (*t) ( pi0 ) ;
            if( sc.isFailure() )Error("Error from 'Tool' for pi0 " , sc ).ignore() ; 
          }



        } 
      }
    } //End of "good" MergedPi0 condition      
  } // End of cluster loop
  
  /* OD COMMENTS for next version
     - Split code and use standard S/L correction
     - Energy from 3x3 matrix w/o energy calibration 
     - No correlations matrices
     - No Mass versus energy correction
  */ 
  
  if ( msgLevel ( MSG::DEBUG ) ){ 
    debug() << " # of created MergedPi0 Hypos is  " << pi0s -> size() << endmsg ;
    debug() << " # of created Split Photons   is  " << phots -> size() << endmsg ;
    if(doSplitC)debug() << " # of created Split Clusters  is  " << clusts -> size() << endmsg ;
    debug() << "post-processing cleaning" << endmsg;
  }

  counter ( m_inputData + "=>" + m_outputData )         += pi0s   -> size() ;
  counter ( m_inputData + "=>" + m_nameOfSplitPhotons)  += phots  -> size() ;
  if(doSplitC)counter ( m_inputData + "=>" + m_nameOfSplitClusters) += clusts -> size() ;

  // delete (empty) container* if not on TES 
  if(m_createClusterOnly){
    if( 0 != pi0s->size() || 0 != phots->size() ){
      return Error( "Container* to be deleted are not empty", StatusCode::FAILURE);
    }
    delete pi0s;
    delete phots;
  }
  
  return StatusCode::SUCCESS;
};
