// $Id: CaloMergedPi0Alg.cpp,v 1.6 2003-12-11 16:33:40 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// Include files
// STD & STL 
#include <numeric>
#include <algorithm>
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h" 
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/INTuple.h"
#include "GaudiKernel/SmartRef.h" 
#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/Stat.h"
// Kernel
#include "Kernel/CaloHypotheses.h"
// CaloInterfaces 
#include "CaloInterfaces/ICaloClusterTool.h"
#include "CaloInterfaces/ICaloHypoTool.h"
// CaloDet
#include "CaloDet/DeCalorimeter.h" 
// CaloEvent 
#include "Event/CaloCluster.h"
#include "Event/CaloHypo.h"
#include "Event/CaloDataFunctor.h"
#include "Event/CaloPosition.h"
// Kernel
#include "CaloKernel/CaloCellID.h"
// local
#include "CaloMergedPi0Alg.h"

// ============================================================================
/** @file CaloMergedPi0Alg.cpp
 * 
 *  Implementation file for class : CaloMergedPi0Alg
 * 
 *  @author Olivier Deschamps 
 *  @date 10/05/2002
 */
// ============================================================================

// ============================================================================
/** Declaration of the Algorithm Factory
 */
// ============================================================================
static const  AlgFactory<CaloMergedPi0Alg>         s_Factory ;
const        IAlgFactory&CaloMergedPi0AlgFactory = s_Factory ;

// ============================================================================
/** Standard constructor
 *  @param   name   algorithm name 
 *  @param   svcloc pointer to service locator 
 */
// ============================================================================
CaloMergedPi0Alg::CaloMergedPi0Alg( const std::string& name    ,
                                    ISvcLocator*       svcloc  )
  : CaloAlgorithm ( name , svcloc ) 
  , m_ntuple              ( 0                                   ) 
  , m_ntupleLUN           ( "FILE1"                             )
  , m_nameOfSplitPhotons  ( CaloHypoLocation::    SplitPhotons  )
  , m_nameOfSplitClusters ( CaloClusterLocation:: EcalSplit     )
  , m_toolTypeNames       ()
  , m_tools               ()
{
  m_toolTypeNames.push_back( "CaloExtraDigits/SpdExtraG" ) ;
  m_toolTypeNames.push_back( "CaloExtraDigits/PrsExtraG" ) ;
  /// NTuple directory
  declareProperty ( "NTupleDirectory"        , m_ntupleLUN           ) ;
  declareProperty ( "SplitPhotons"           , m_nameOfSplitPhotons  ) ;
  declareProperty ( "SplitClusters"          , m_nameOfSplitClusters ) ;
  // tool to be apllyed to all hypos 
  declareProperty ( "Tools"                 , m_toolTypeNames       ) ;
  // set the appropriate defaults for input    data 
  setInputData    ( CaloClusterLocation::   Ecal       ) ;
  // set the appropriate defaults for output   data 
  setOutputData   ( CaloHypoLocation::      MergedPi0s ) ;
  // set the appropriate defaults for detector data 
  setDetData      ( DeCalorimeterLocation:: Ecal       ) ;
};

// ============================================================================
/** destructor
 */
// ============================================================================
CaloMergedPi0Alg::~CaloMergedPi0Alg() {};


// ============================================================================
/** standard algorithm initialization 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloMergedPi0Alg::initialize() 
{
  
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;
  
  StatusCode sc = CaloAlgorithm::initialize();
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class!",sc);}

  // locate tools
  for ( Names::const_iterator it = m_toolTypeNames.begin() ;
        m_toolTypeNames.end() != it ; ++it )
    {
      ICaloHypoTool* t = tool( *it , t );
      if( 0 == t ) { return StatusCode::FAILURE ; }
      m_tools.push_back( t ) ;
    }
  
//    /// NTuple identifier 
//    if( 0 == ntupleSvc() )
//      { return Error("INTupleSvc* points to NULL!");}
//    const std::string ntupleID( m_ntupleLUN+"/"+name()+"/200");
//    NTuplePtr nt( ntupleSvc() , ntupleID );
//    if( !nt ) 
//      {
//        nt = ntupleSvc()->
//          book( ntupleID                           ,
//                CLID_ColumnWiseTuple               , 
//                "Pi0 reconstruction " ) ; 
//      }
//    if( !nt )
//      { return Error("could not book NTUPLE='"+ntupleID+"'");}
  
//    if( sc.isSuccess() ) 
//      { sc = nt -> addItem ( "mas"   , m_mas ) ; }
//    if( sc.isSuccess() ) 
//      { sc = nt -> addItem ( "ncls"    , m_cls ) ; }
//    iff( sc.isSuccess() ) 
//      { sc = nt -> addItem ( "itr"   , m_iter ) ; }
  
//    if( sc.isFailure() )
//      { return Error("Could not add item to ntuple ="+ntupleID+"'",sc);}
//    m_ntuple = nt ;
//    if( 0 == m_ntuple ) 
//      { return Error("INTuple* points to NULL for ntuple='"+ntupleID+"'");}
  

  return StatusCode::SUCCESS;
};

// ============================================================================
/** standard algorithm finalization 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloMergedPi0Alg::finalize() 
{
  
  MsgStream log( msgSvc() , name() );
  log << MSG::DEBUG << "==> Finalize" << endreq;
  
  std::for_each( m_tools.begin () , 
                 m_tools.end   () , std::mem_fun(&IInterface::release));
  m_tools.clear() ;

  /// finalize the base class 
  return CaloAlgorithm::finalize();

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
( const CaloCluster*             cluster ,
  const CaloDigitStatus::Status& status  ) const 
{
  /// check arguments 
  if( 0 == cluster ) 
    { Error(" numberOfDigits! CaloCluster* points to NULL!"); return -1;}
  ///
  long num = 0 ;
  for( CaloCluster::Entries::const_iterator entry = 
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
double CaloMergedPi0Alg::TrShape( const int Neig,
                            const unsigned int area,
                            const double D3D)
{
  double x = D3D;
  double Frac = 0;
  if( 2 == area && 0 == Neig ){
    Frac=2.-0.98408*exp(-0.27643*x)-0.16372*exp(2.2917*x);
  }
  if( 1 == area && 0 == Neig ){
    Frac=2.-0.82251*exp(-0.66440*x)-0.29564*exp(1.8377*x);
  }
  if( 0 == area && 0 == Neig ){
    Frac=2.-0.91188*exp(-0.39684*x)-0.14233*exp(2.4897*x);
  }
  if( 2 == area && 1 == Neig ){
    Frac=6.6407*exp(-6.1296*x)+0.95950E-01*exp(-1.6793*x);
  }
  if( 1 == area && 1 == Neig ){
    Frac=6.4768*exp(-6.3658*x)+0.88887E-01*exp(-1.9469*x);
  }
  if( 0 == area && 1 == Neig ){
    Frac=7.0756*exp(-6.9016*x)+0.26618E-01*exp(-1.9711*x);
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
double CaloMergedPi0Alg::BarZ( const double e,
                         const double x,
                         const double y,
                         const double z )
{
  double TanTheta,CosTheta;         // Tan/Cos of Polar angle
  const double Ec=7.32;             // Critical Energy in MeV
  const double Xfac=17.1;           // X0/mm conversion factor (17.1 mm/X0 )
  double Tmax;                      // Shower maximum (X0 unit)
  double Tbar=1.7;                  // Shower barycenter wrt maximum
  double TPrS=2.5;                  // PreShower depth (X0 unit)
  double DeltaZ=0;
  if( 0 < e) {
    TanTheta=sqrt(x*x+y*y)/z;
    CosTheta=cos(atan(TanTheta));
    Tmax=log(e/Ec)-0.5;
    TPrS=TPrS/CosTheta;
    Tbar=(Tmax+Tbar-TPrS)*Xfac;
    TanTheta=TanTheta/(1.0+Tbar*CosTheta/z);
    CosTheta=cos(atan(TanTheta));
    DeltaZ=CosTheta*Tbar;
  }
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

    double Bparx=0;
    double Bpary=0;
    if(2 == area ){Bparx=0.1479;}
    if(1 == area ){Bparx=0.1250;}
    if(0 == area ){Bparx=0.1159;}
    if(2 == area ){Bpary=0.1489;}
    if(1 == area ){Bpary=0.13660;}
    if(0 == area ){Bpary=0.1216;}
    double ArgX,ArgY;
    double DeltaX=0;
    double DeltaY=0;
    double DeltaXY=0;
    ArgX=2.*AsymX*sinh(0.5/Bparx);
    DeltaX=Bparx*log(ArgX+sqrt(ArgX*ArgX+1.));
    ArgY=2.*AsymY*sinh(0.5/Bpary);
    DeltaY=Bpary*log(ArgY+sqrt(ArgY*ArgY+1.));

  /*
    double Bpar=0;
    
    if(2 == area ){Bpar=0.1462;}
    if(1 == area ){Bpar=0.1326;}
    if(0 == area ){Bpar=0.1093;}
    
    double ArgX,ArgY;
    double DeltaX=0;
    double DeltaY=0;
    double DeltaXY=0;
    
    if ( 0 != Bpar){
    ArgX=2.*AsymX*sinh(0.5/Bpar);
     DeltaX=Bpar*log(ArgX+sqrt(ArgX*ArgX+1.));
     ArgY=2.*AsymY*sinh(0.5/Bpar);
     DeltaY=Bpar*log(ArgY+sqrt(ArgY*ArgY+1.));
     }
  */

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
  
  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;

  /// avoid long names
  using namespace  CaloDataFunctor;
  typedef CaloClusters              Clusters;
  typedef Clusters::iterator        Iterator;
  
  /// load data 
  SmartDataPtr<DeCalorimeter> detector( detSvc() , detData() );
  if( !detector )
    { return Error("could not locate calorimeter '"+detData()+"'");}

  SmartDataPtr<Clusters>  clusters( eventSvc() , inputData() );
  if( 0 == clusters )
    { return Error("Could not load input data ='"+inputData()+"'");}

  //StatusCode sc = StatusCode::SUCCESS ;

  // create the container of pi0s 
  // (used "Proprerty" 'Output' of the algorithm)
  CaloHypos* pi0s = new CaloHypos();
  {  
    StatusCode status = put( pi0s , outputData() );
    if( status.isFailure() ) { return status ; }
  }
  
  // create the container of split photons 
  CaloHypos* phots = new CaloHypos();
  {  
    StatusCode status = put( phots , m_nameOfSplitPhotons);
    if( status.isFailure() ) { return status ; }
  }

  // create the container of split clusters 
  CaloClusters* clusts = new CaloClusters();
  {  
    StatusCode status = put( clusts , m_nameOfSplitClusters);
    if( status.isFailure() ) { return status ; }
  }
  

  /// loop over all clusters  for counting
  long cluscount=0;
  for( Iterator iclus = clusters->begin() ;
       clusters->end() != iclus ; ++iclus )
    {
      cluscount=cluscount+1;
    }

  log << MSG::DEBUG << " -----> #cluster " << cluscount << endreq;
  
  /// loop over all clusters 
  for( Iterator icluster = clusters->begin() ;
       clusters->end() != icluster ; ++icluster )
    {
      CaloCluster* cluster = *icluster ;
      if( 0 == cluster )                { continue ; }   ///< CONTINUE!

      //----------------------//
      //        My code       //
      //----------------------//

      /// Check # of digits/cells in this cluster 
      int m_cluDigs;
      m_cluDigs = cluster->entries().size() ;
      if( 1 >= m_cluDigs )                { return 0; }   ///< CONTINUE!




      /// Find Cluster Seed 
      const CaloCluster::Digits::const_iterator iSeed = 
        clusterLocateDigit( cluster->entries().begin() ,
                            cluster->entries().end  () ,
                            CaloDigitStatus::SeedCell     );
      const CaloDigit* seed = iSeed->digit() ;
      if( 0 == seed) { continue ; }   ///< CONTINUE!
      const CaloCellID  idseed = seed->cellID() ;
      double seede  = seed->e();
      int seedrow  = idseed.row();
      int seedcol  = idseed.col();


      
      log << MSG::DEBUG << " -----> Find SubSeed " << endreq;
      /// Find SubSeed  - Loop on Cluster CaloCluster:Digits
      // (New DigitStatus to be defined)

      double sube=-99;
      int subrow=0;
      int subcol=0;
      for( CaloCluster::Digits::const_iterator it1 =
             cluster->entries().begin() ;
           cluster->entries().end() != it1 ; ++it1 ) 
        {
          const CaloDigit* dig = it1->digit() ;    // CaloDigit
          if( 0 == dig) { continue ; }   ///< CONTINUE!
          const CaloCellID  id = dig->cellID() ;
          int cellrow  = id.row();
          int cellcol  = id.col();
          double ecel = dig->e()*it1->fraction();
          if (ecel>sube 
              && ecel < seede 
              && abs(cellrow-seedrow)<=1 
              && abs(cellcol-seedcol)<=1)
            {
              sube=ecel;
              subrow=cellrow;
              subcol=cellcol;
            }
        }
      

      log << MSG::DEBUG << " -----> Define large Cluster " << endreq;

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
      const CaloDigit* SubClus[2][3][3];
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
      for( CaloCluster::Digits::const_iterator it2 =
              cluster->entries().begin() ;
           cluster->entries().end() != it2 ; ++it2 ) 
        {
          const CaloDigit* dig = it2->digit() ;    
          if( 0 == dig) { continue ; }   ///< CONTINUE!
          const CaloCellID  id = dig->cellID() ;
          int cellrow  = id.row();
          int cellcol  = id.col();
          double ecel = dig->e()*it2->fraction();
          if (abs(seedrow-cellrow)<=1 && abs(seedcol-cellcol)<=1) 
            {
              ir=cellrow-seedrow+1;
              ic=cellcol-seedcol+1;
              SubClusEne[0][ir][ic]=std::max(ecel,0.) ;
              SubClus[0][ir][ic]=dig;
            }
          if (abs(subrow-cellrow)<=1 && abs(subcol-cellcol)<=1) 
            {
              ir=cellrow-subrow+1;
              ic=cellcol-subcol+1;
              SubClusEne[1][ir][ic]=std::max(ecel,0.) ;
              SubClus[1][ir][ic]=dig;
            }
        }
      
      
      log << MSG::DEBUG << " -----> Split cluster " << endreq;
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
          if( 0 == is 
             && SubClusEne[is][idr[is]+1][1] <= SubClusEne[is][1][idc[is]+1]){
            erasel[is]=idr[is]+1;
          }
          if( 0 == is 
             && SubClusEne[is][idr[is]+1][1] >= SubClusEne[is][1][idc[is]+1]){
            erasec[is]=idc[is]+1;
          }
          if( 1 == is 
             && SubClusEne[is][idr[is]+1][1] >= SubClusEne[is][1][idc[is]+1]){
            erasel[is]=idr[is]+1;
          }
          if( 1 == is 
             && SubClusEne[is][idr[is]+1][1] <= SubClusEne[is][1][idc[is]+1]){
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
          }
        }
      }

      
      /// Iterative reconstruction of SubClusters
      log << MSG::DEBUG << " -----> Iterative reconstruction " << endreq;
      double SubSize[2],SubX[2],SubY[2],SubZ[2] ;
      long mxiter=16;
      long iter=0;
      for( long iterat = 0 ; iterat < mxiter ; ++iterat){
        // SubClusters X/Y/Z barycenter
        iter++;
        double Etemp[3][3];
        double Ene3x3[2];
        {
          for(int is=0;is<2;++is){
            const CaloDigit* digit   = SubClus[is][1][1];;
            if( 0 == SubClus[is][1][1]) { continue ; }   ///< CONTINUE!
            const CaloCellID  idigit = digit->cellID() ;
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
            SubZ[is]=SubZ[is]+BarZ(Ene3x3[is],SubX[is],SubY[is],SubZ[is]);
          }
        }
        
        // Reconstructing Overlap
        
        {
          for(int is=0;is<2;++is){
            int js = 0 ;
            if ( 0 == is ){ js = 1; }
            if ( 1 == is ){ js = 0; }
            for(int ir=0;ir<3;++ir){
              for(int ic=0;ic<3;++ic){
                if(1 == SubClusMask[js][ir][ic] ) {
                  const CaloDigit* seed    = SubClus[is][1][1];;
                  if( 0 == seed) { continue ; }   ///< CONTINUE!
                  const CaloCellID  idseed = seed->cellID() ;
                  const CaloDigit* cel      = SubClus[js][ir][ic];;
                  if( 0 == cel) { continue ; }   ///< CONTINUE!
                  const CaloCellID  idcel   = cel->cellID() ;
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
                  double Frac=TrShape(Neig,area,D3D);
                  double EFrac=Ene3x3[is]*Frac;
                  int jr=rowc-rows+1;
                  int jc=colc-cols+1;
                  Weight[js][ir][ic]=1.;
                  Weight[is][jr][jc]=0.;
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
      }
      

      /// End of Reconstruction aLgorithm 
      /// Define photon and pi0 from Subcluster


      log << MSG::DEBUG << " -----> Compute Pi0 mass " << endreq;
      // energy
      double Ene3x3[2];
      {
        for(int is=0;is<2;++is){
          const CaloDigit* digit   = SubClus[is][1][1];;
          if( 0 == SubClus[is][1][1]) { continue ; }   ///< CONTINUE!
          const CaloCellID  idigit = digit->cellID() ;
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
          SubZ[is]=SubZ[is]+BarZ(Ene3x3[is],SubX[is],SubY[is],SubZ[is]);
        }
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
  

      const CaloDigit*  seedig   = SubClus[0][1][1];;
      const CaloCellID  idig     = seedig->cellID() ;
      double zpos                = detector->cellZ   ( idig ) ;
      double csiz                = detector->cellSize( idig ) ;
      double mpi0=0.1349;
      double epi0=(ep1+ep2)/GeV;
      double dmin=zpos*2*mpi0/epi0/csiz;

      
//        log << MSG::WARNING << " ------------------------------ RecPi0Mas " 
//            << RecPi0Mas << " " << cluscount <<  " " << dmin<<endreq;

      /*
      /// write record to  NTuple  and Checks
      m_mas=RecPi0Mas; 
      m_cls=cluscount;  
      m_iter=mxiter; 
      m_ntuple->write() ; 
      */

//        log << MSG::WARNING << " Pi0Mas " <<RecPi0Mas << " " 
//            << cluscount << " " 
//            << iter  << " " 
//            << m_cluDigs
//            << endreq;
//        log << MSG::WARNING << " 4-vector g1 " 
//            << ep1 << "/"
//            << xp1 << "/"
//            << yp1 << "/"
//            << zp1 << "/"
//            << endreq;
//        log << MSG::WARNING << " 4-vector g2 " 
//            << ep2 << "/"
//            << xp2 << "/"
//            << yp2 << "/"
//            << zp2 << "/"
//            << endreq;
      

      /*
        Mid June 2002 
       */

      unsigned int KeepPi0=1;
      const double TruePi0Mas=134.9;
      const double SigmaRec=16.;
      const double LowMasSigmaCut=7.0;
      const double HighMasSigmaCut=10.0;
      /*
        Pi0 MassRec window = [ 12.9 ; 294.9 ]
       */

        
      if(RecPi0Mas < TruePi0Mas - LowMasSigmaCut*SigmaRec){KeepPi0=0;}
      if(RecPi0Mas > TruePi0Mas + HighMasSigmaCut*SigmaRec){KeepPi0=0;}
      if(dmin      >  3.){KeepPi0=0;}
      if(dmin      <= 0.){KeepPi0=0;}
      // Cuts for checks *** TO BE APPLIED ONLY ON SINGLE pi0 SAMPLE*
      //if(cluscount != 1){KeepPi0=0;}
      //if(m_cluDigs <= 1){KeepPi0=0;}
          

      
      /*
         Fill CaloHypos and SubClusters if "good" MergePi0
      */

      if( 1 == KeepPi0){
        
        //Defined new Calo SubCluster pointed by PhotonFromMergedPi0 CaloHypos
        
        CaloCluster* cl1 = new CaloCluster();
        CaloCluster* cl2 = new CaloCluster();
        
        // Init the 2 new CaloClusters 
        // with the original cluster digits, owned-status'ed and  0-weighted
        for( CaloCluster::Digits::const_iterator it3 =
               cluster->entries().begin() ;
             cluster->entries().end() != it3 ; ++it3 ){
          const CaloDigit* d = it3->digit() ;   
          const CaloDigitStatus::Status s1   = CaloDigitStatus::OwnedCell    ;
          const CaloDigitStatus::Status s2   = CaloDigitStatus::OwnedCell    ;
          double     w1=0;
          double     w2=0;
          // Then fill correctly the 3x3 matrix cells around seed and subseed
          {
            for(int ir=0;ir<3;++ir){
              for(int ic=0;ic<3;++ic){
                const CaloDigit* d1 = SubClus[0][ir][ic];
                if (d == d1 ) {
                  w1 = Weight[0][ir][ic] ;      
                  CaloDigitStatus::Status s1   =  
                    CaloDigitStatus::OwnedCell      | 
                    CaloDigitStatus::UseForEnergy   | 
                    CaloDigitStatus::UseForPosition | 
                    CaloDigitStatus::UseForCovariance  ;
                  if(1 == ir && 1==ic){
                    s1   =  
                      CaloDigitStatus::SeedCell       | 
                      CaloDigitStatus::LocalMaximum   | 
                      CaloDigitStatus::UseForEnergy   | 
                      CaloDigitStatus::UseForPosition | 
                      CaloDigitStatus::UseForCovariance  ;
                  }
                }
              }
            }
          }
          {
            for(int ir=0;ir<3;++ir){
              for(int ic=0;ic<3;++ic){
                const CaloDigit* d2 = SubClus[1][ir][ic];
                if (d == d2 ) {
                  w2 = Weight[1][ir][ic] ;      
                  CaloDigitStatus::Status s2   =  
                    CaloDigitStatus::OwnedCell      | 
                    CaloDigitStatus::LocalMaximum   | 
                    CaloDigitStatus::UseForEnergy   | 
                    CaloDigitStatus::UseForPosition | 
                    CaloDigitStatus::UseForCovariance  ;
                  if(1 == ir && 1==ic){
                    s2   =  
                      CaloDigitStatus::SeedCell       | 
                      CaloDigitStatus::UseForEnergy   | 
                      CaloDigitStatus::UseForPosition | 
                      CaloDigitStatus::UseForCovariance  ;
                  }
                }
              }
            }
          }
          CaloClusterEntry entry1( d , s1 , w1 );
          cl1->entries().push_back( entry1 );
          CaloClusterEntry entry2( d , s2 , w2 );
          cl2->entries().push_back( entry2 );
        }


        clusts->insert( cl1 ) ;
        clusts->insert( cl2 ) ;
        
        // now CaloHypo for pi0      
        CaloHypo* pi0 = new CaloHypo();
        
        pi0 -> setHypothesis( CaloHypotheses::Pi0Merged ) ;
        pi0 -> addToClusters( *icluster )                 ;
        CaloMomentum* mom = new CaloMomentum();
        pi0 -> setMomentum ( mom )                        ;
        mom -> setMomentum ( HepLorentzVector( xp1 + xp2 , 
                                               yp1 + yp2 , 
                                               zp1 + zp2 ,
                                               ep1 + ep2 ) );
        // now CaloHypo for gamma's
        CaloHypo* g1   = new CaloHypo() ;
        g1 -> setHypothesis( CaloHypotheses::PhotonFromMergedPi0 ) ;
        g1 -> addToClusters( *icluster )                ;
        g1 -> addToClusters( cl1       )                ;
        CaloMomentum* m1 = new CaloMomentum();
        g1 -> setMomentum ( m1 )                        ;
        m1 -> setMomentum ( HepLorentzVector( xp1 , 
                                              yp1 , 
                                              zp1 ,
                                              ep1 ) );
        CaloPosition* p1 = new CaloPosition();
        p1 ->parameters()( CaloPosition::X ) = SubX[0];
        p1 ->parameters()( CaloPosition::Y ) = SubY[0];
        p1 ->setZ( SubZ[0] ) ;
        p1 ->parameters()( CaloPosition::E ) = ep1    ;

        p1 ->center()( CaloPosition::X ) = SubX[0];
        p1 ->center()( CaloPosition::Y ) = SubY[0];
        g1 -> setPosition( p1);
        pi0 -> addToHypos ( g1 );
        
        cl1->setPosition( *p1 );
        
        
        CaloHypo* g2   = new CaloHypo() ;
        g2 -> setHypothesis( CaloHypotheses::PhotonFromMergedPi0 ) ;
        g2 -> addToClusters( *icluster )                ;
        g2 -> addToClusters( cl2       )                ;
        CaloMomentum* m2 = new CaloMomentum();
        g2 -> setMomentum ( m2 )                        ;
        m2 -> setMomentum ( HepLorentzVector( xp2 , 
                                              yp2 , 
                                              zp2 ,
                                              ep2 ) );
        CaloPosition* p2 = new CaloPosition();
        p2 ->parameters()( CaloPosition::X ) = SubX[1];
        p2 ->parameters()( CaloPosition::Y ) = SubY[1];
        p2 ->setZ( SubZ[1] ) ;
        p2 ->parameters()( CaloPosition::E ) = ep2    ;

        p2 ->center()( CaloPosition::X ) = SubX[1];
        p2 ->center()( CaloPosition::Y ) = SubY[1];
        g2 -> setPosition( p2);
        pi0 -> addToHypos ( g2 );
        
        cl2 -> setPosition( *p2 );
        
        
        pi0s -> insert( pi0 ) ;
        phots ->insert( g1  ) ;
        phots ->insert( g2  ) ;
        
        { // Apply the tool 
          for( Tools::iterator it = m_tools.begin() ; 
               m_tools.end() != it ; ++it ) 
            {
              ICaloHypoTool* t = *it ;
              if( 0 == t ) { continue; } 
              StatusCode sc         = StatusCode::SUCCESS ;
              sc                    = (*t) ( g1 ) ;
              if( sc.isFailure() )
                { Error("Error from 'Tool' for g1 " , sc ) ; }
              sc                    = (*t) ( g2 ) ;          
              if( sc.isFailure() ) 
                { Error("Error from 'Tool' for g2 " , sc ) ; }
            }
        }
        
      } //End of "good" MergedPi0 condition
      
      
    } // End of cluster loop

  /* OD COMMENTS for next version
    - Energy from 3x3 matrix w/o energy calibration 
    - No correlations matrices
    - No Mass versus energy correction
   */ 

  
  log << MSG::DEBUG
      << " # of created MergedPi0 Hypos is  " << pi0s   -> size() << endreq ;
  log << MSG::DEBUG
      << " # of created Split Photons   is  " << phots  -> size() << endreq ;
  log << MSG::DEBUG
      << " # of created Split Clusters  is  " << clusts -> size() << endreq ;

  
  /*
    //SOME CHECKS BEFORE OFFICIALISATION !
    
    // Loop over SubCluster
    log << MSG::DEBUG << " --------------------"<< endreq;
    long newcount=0;
    for( Iterator icluster = clusts ->begin() ; 
    clusts ->end() != icluster ; ++icluster ) 
    { 
    newcount=newcount+1;
    log << MSG::DEBUG << " -----> # new cluster number " << newcount << endreq;
    CaloCluster* newclust = *icluster ;
    for( CaloCluster::Digits::const_iterator it =
    newclust ->entries().begin() ;
    newclust ->entries().end() != it ; ++it ){
    
    const CaloDigit* dig = it->digit() ;   
    double weig = it-> fraction();
    //        const CaloDigitStatus::Status stat = it-> status() ;
    double ene = dig -> e() ;
    const CaloCellID  id = dig->cellID() ;
    log << MSG::DEBUG << " -----> digit " 
    << id   <<  " "
    << ene  <<  " "
    << weig <<  " "
    << endreq; 
    }
    // Loop over SplitPhotons ... 
    
    typedef CaloHypos Hypos ;
    const Hypos* hypos = get( eventSvc (),"Rec/Calo/SplitPhotons", hypos ) ;
    typedef Hypos::const_iterator   Ihypos ;
    for(Ihypos ipos = hypos->begin() ;
    hypos->end() != ipos ; ++ipos ){
    const CaloHypo* thehypo = *ipos;
    log << MSG::DEBUG << " -----> Hypo Split photons " 
    << ipos      << " "
    << thehypo->hypothesis() << " "
    << CaloHypotheses::Photon << " "
    << CaloHypotheses::PhotonFromMergedPi0 << " "
    << thehypo->position()->e() << " "
    << thehypo->position()->x() << " "
    << thehypo->position()->y() << " "
    << thehypo->position()->z() << " "
    << thehypo->momentum()->momentum().e() << " "
    << thehypo->momentum()->momentum().px() << " "
    << thehypo->momentum()->momentum().py() << " "
    << thehypo->momentum()->momentum().pz() << " "
    << endreq; 
    }
    }
    log << MSG::DEBUG << " ------------------------------------ "<< endreq;
    
  */
  log << MSG::DEBUG << " ------------"<< endreq;
  log << MSG::DEBUG << " ------------"<< endreq;
  log << MSG::DEBUG << " -- COUCOU --"<< endreq;
  log << MSG::DEBUG << " ------------"<< endreq;
  log << MSG::DEBUG << " ------------"<< endreq;

   
  return StatusCode::SUCCESS;
};

// ============================================================================
// The End 
// ============================================================================
