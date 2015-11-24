// Include files
// from bost
#include <boost/assign/list_of.hpp>
#include <boost/array.hpp>
#include <boost/foreach.hpp>
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "Event/Track.h"
#include "Event/StateParameters.h"
#include "Math/CholeskyDecomp.h"
// local
#include "PrLineFitterY.h"
#include "PrHybridSeeding.h"
#include "PrPlaneHybridCounter.h"
#include "Event/FTCluster.h"
#include "Event/FTLiteCluster.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrHybridSeeding
//
// 2015-03-11 : renato quagliani
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PrHybridSeeding )

//=============================================================================
// Standard constructor, initializes variable//=============================================================================

PrHybridSeeding::PrHybridSeeding( const std::string& name,
                                  ISvcLocator* pSvcLocator)
:
GaudiAlgorithm(name,pSvcLocator),
  m_hitManager(nullptr)
  ,m_geoTool(nullptr)
  ,m_debugTool(nullptr)
  ,m_zones(24)
  ,m_timerTool(nullptr)
{
  //----- Newer Faster stereo method ( on by Default ), faster than normal one but need some retuning of the line Chi2 and tolerances
  declareProperty("stereo2",m_stereo2=false);
  {
    std::vector<int> maxBest = boost::assign::list_of(2)(5)(5);
    declareProperty("maxNbestCluster", m_maxNClusters = maxBest);
  }
  declareProperty( "SlopeCorr", m_SlopeCorr = false); //Changhe weight = 1./err -> 1./err*cos(tx)*cos(tx)
  //Used Params
  declareProperty( "InputName"          ,     m_inputName= LHCb::TrackLocation::Forward);
  declareProperty( "OutputName"         ,     m_outputName=LHCb::TrackLocation::Seed);
  declareProperty( "HitManagerName"     ,     m_hitManagerName= "PrFTHitManager");
  declareProperty( "DecodeData"         ,     m_decodeData= false);
  declareProperty( "XOnly"              ,     m_xOnly= false);
  declareProperty( "MinXPlanes"         ,     m_minXPlanes= 4);         //XZ projection finding ( start with 6->5->4 ( outlier removal). 4 hits on track are killed by default at the first fit
  //as well as the outlier removed form 5->4.
  declareProperty( "MaxNHits"           ,     m_maxNHits =12);          //Force algorithm to find 12 hits track
  declareProperty( "NCases"             ,     m_nCases = 3);            //N Cases to run ( set to 2 to have best timing ( penalising lower P tracks )
  declareProperty( "TimingMeasurement"  ,     m_doTiming= false);       //Measure timing of the algorithm splitting in cases up/down
  declareProperty( "PrintSettings"      ,     m_printSettings= false);  //Print the settings of the algorithm?
  declareProperty( "RemoveClones"       ,     m_removeClones = true);   //To be optimised : track.sorthits LHCbId
  declareProperty( "minNCommonUV"       ,     m_nCommonUV=8);           //Global Clone removal Nb shared hits
  declareProperty( "RemoveClonesX"      ,     m_removeClonesX = true);   //To be optimised ( BloomFilter for instance & use of fraction )
  declareProperty( "FlagHits"           ,     m_FlagHits = true);        //To be impoved
  declareProperty( "RemoveFlagged"      ,     m_removeFlagged = true);   //To be improved
  //Parameter Track Model
  
  declareProperty( "UseCubicCorrection" ,     m_useCubic = true); // use "cubic" track model
  declareProperty( "dRatio"             , m_dRatio = -0.000262); // Const dRatio value ( By = B0+B1*z ; B1/B0 = dRatio
  //dRatio
  declareProperty( "CConst"             , m_ConstC =  2.458e8);  // Const value to compute the backward projection
  //Backward Projection
  declareProperty( "UseCorrPosition"    ,     m_useCorrPos = true);  // Compute the y(z) using the shift from the COS reference system of LHCb ( use the dzDy information of the hits )
  //Hit Flagging Step
  {
    //if SizeToFlag = 10 or 9 errors!
    std::vector<unsigned int> size = boost::assign::list_of( 12)(11)(10);     // Number of hits to flag at each Case >= Value
    declareProperty( "SizeToFlag"            ,m_SizeFlag = size);
    std::vector<double> Chi2 = boost::assign::list_of(0.5)(1.0)(1.0);         // If NHits<12 Flag only tracks having Chi2DoF<Flag_MaxChi2DoF_11Hits[Case]
    declareProperty( "Flag_MaxChi2DoF_11Hits",m_MaxChi2Flag = Chi2); 
    std::vector<double> X0Flag = boost::assign::list_of( 100.)(8000.)(200.);  // If NHits<12 Flag only tracks having |X0| ( Back. Projection ) < Flag_MaxX0_11Hits[Case] 
    declareProperty( "Flag_MaxX0_11Hits"     ,m_MaxX0Flag = X0Flag ); 
  }
  //--------------------X-Search Parametrisation
  //1st / Last Layer search windows
  {
    //find x-Projection 2 - hit Combo
    //Case0 : 1st = T1-1X Last = T3-2X
    //Case1 : 1st = T1-2X Last = T3-1X
    //Case2 : 1st = T1-1X Last = T3-1X
    //Hits in T3 are collected accrding to txInf = x1st/Z1st;
    //[ L0_AlphaCorr*txInf + txInf(ZLast-Z1st) ] - L0_tolHp < (xT3) < [ L0_AlphaCorr*txInf + txInf(ZLast-Z1st) ] +L0_tolHp
    std::vector<double> L0alpha=boost::assign::list_of(120.64)(510.64)(730.64);
    declareProperty( "L0_AlphaCorr"     , m_alphaCorrection = L0alpha);
    std::vector<double> tmp=boost::assign::list_of(280.0)(540.0)(1080.0);
    declareProperty( "L0_tolHp"         , m_TolFirstLast = tmp);
  }
  {
    //find x-Projection 3 - hit Combo
    //2 Hit combination defines the value of the backward value at 0 ;
    //txPicked = (XT3 -XT1)/(ZT3-ZT1)
    //x0 = XT1-txPicked*ZT1
    //Hits are collected in T2 based on the following parameters.
    // See around Line 2165
    declareProperty( "maxParabolaSeedHits", m_maxParabolaSeedHits = 8); // Max Nb Of hits to process in T2-1X and T2-2X given a 2hit combination
    std::vector<double> X0Rotation = boost::assign::list_of(0.002152)(0.001534)(0.001834);
    declareProperty( "x0Corr"             , m_x0Corr = X0Rotation);  // Rotation angle

    std::vector<double> X0SlopeChange = boost::assign::list_of(400.)(500.)(500.);
    declareProperty( "X0SlopeChange"      , m_x0SlopeChange = X0SlopeChange);

    std::vector<double> TolX0SignUp = boost::assign::list_of(0.75)(0.75)(0.75);
    declareProperty( "ToleranceX0Up"      , m_TolX0SameSign = TolX0SignUp);

    std::vector<double> x0Cut = boost::assign::list_of( 1500.)( 4000.)(6000.);
    declareProperty( "x0Cut"              , m_x0Cut = x0Cut);

    std::vector<double> TolAtX0Cut = boost::assign::list_of(4.5)(8.0)(14.0);
    declareProperty( "TolAtX0Cut"         , m_tolAtX0Cut = TolAtX0Cut);

    std::vector<double> x0SlopeChange2 = boost::assign::list_of(1500.)(2000.)(2000.);
    declareProperty( "X0SlopeChangeDown"  , m_x0SlopeChange2 = x0SlopeChange2);

    //Tolerance inferior for |x0| > m_x0SlopeChange2 when x0 = m_x0Cut
    std::vector<double> tolAtx0CutOpp = boost::assign::list_of(1.5)(2.0)(7.0);
    declareProperty( "TolAtX0CutOpp"      , m_tolAtx0CutOppSig = tolAtx0CutOpp);

    std::vector<double> tolOpp = boost::assign::list_of(0.75)(0.75)(0.75);
    declareProperty( "ToleranceX0Down"    , m_tolX0Oppsig = tolOpp);
  }
  
  //find x-Projection Remaining layers & fitting
  {
    std::vector<double> tolRem = boost::assign::list_of(1.0)(1.0)(1.0); // Tolerance for remaining layers ( after the 3 hit combination ) in mm
    declareProperty( "TolXRemaining"      , m_tolRemaining = tolRem);   
    std::vector<double> maxChi2X = boost::assign::list_of( 5.5)(5.5)(5.5);  // Outlier removal parameter MaxChi2 from single hit < 5.5 , if not track rejected, worst removed and refit
    declareProperty( "maxChi2HitsX"       , m_maxChi2HitsX = maxChi2X );
    std::vector<double> maxChi2DoFX = boost::assign::list_of(4.0)(5.0)(6.0);  // Max Chi2PerDOF xz projection
    declareProperty( "maxChi2DoFX"        , m_maxChi2DoFX = maxChi2DoFX);
  }

  
  //--------------------UV search  Parametrisation (inherit from PrSeedingXLayers)
  // --------- COLLECT UV
  declareProperty("RemoveHole"            , m_removeHole     = true);  // Do not collect hits corresponding to tracks passing trough a hole of radius RadiusHole
  declareProperty("RadiusHole"            , m_radiusHole     = 87.0);  
  declareProperty("yMin"                  , m_yMin           = -1.0  *Gaudi::Units::mm); // Y min for hough clusters ( -1, 2700 ) by symmetru ( -2700, +1)
  declareProperty("yMin_TrFix"            , m_yMin_TrFix     = -2.0  *Gaudi::Units::mm); // Y min for hough clusters when triangle fixing Look up tracks and load low modules (y<0) pick only hits at y>-2 mm.
  declareProperty("yMax"                  , m_yMax           = + 2700. *Gaudi::Units::mm);
  declareProperty("yMax_TrFix"            , m_yMax_TrFix     = +30.0 *Gaudi::Units::mm); // Y max for hough clusters when triangle fixing Look up tracks and load low modules (y<0) pick only hits at y< 30 mm.
  declareProperty("DoAsymm"               , m_doAsymmUV      = true); // Do asymmetric => take into account the alpha of the layers
  declareProperty( "TriangleFix"          , m_useFix         = true); // Use triangle fixing
  declareProperty( "TriangleFix2ndOrder"  , m_useFix2ndOrder = true); // Use triangle fixing accessing the PrHit::yMin and PrHit::yMax information

  //--------- SelectHoughCluster
  {
    // if stereo2 set to false
    declareProperty("UseLineY", m_useLineY = true);    // Pre-fit a line?
    std::vector<unsigned int> minUV6 = boost::assign::list_of(4)(4)(4); // minUVLayers when XZ candidate has 6 hits  (planes)
    std::vector<unsigned int> minUV5 = boost::assign::list_of(4)(4)(4); // minUVLayers when XZ candidate has 5 hits  (planes)
    std::vector<unsigned int> minUV4 = boost::assign::list_of(5)(5)(5); // minUVLayers when XZ candidate has 4 hits  (planes)
    declareProperty("minUV6"  ,   m_minUV6   =   minUV6);
    declareProperty("minUV5"  ,   m_minUV5   =   minUV5);
    declareProperty("minUV4"  ,   m_minUV4   =   minUV4);
    //10 or 9 hits on track
    std::vector<double> Chi2LowLine =  boost::assign::list_of( 4.0 )( 6.0 )( 8.0 );
    declareProperty("Chi2LowLine"     ,    m_Chi2LowLine = Chi2LowLine);  // if NXZ + NUV (Planes) <=10  XZChi2DoF + YLineChi2DoF < Chi2LowLine
    //11 or 12 hits on track
    std::vector<double> Chi2HighLine = boost::assign::list_of( 30.0 )( 50.0 )( 80.0 );
    declareProperty("Chi2HighLine"    ,    m_Chi2HighLine = Chi2HighLine);  // if NXZ + NUV (Planes) >10 XZChi2DoF + YLineChi2DoF < Chi2HighLine
    std::vector<unsigned int> mintot = boost::assign::list_of(9)(9)(9);
    declareProperty("minTot"          ,    m_minTot = mintot);
    std::vector<double> ty           = boost::assign::list_of( 0.0017 )( 0.0025 )( 0.007 );  // Hough Cluster window : maxCoord- minCoord < (TolTyOffset + TolTySlope*minCoord) : Hit Coord = |y/z|
    declareProperty( "TolTyOffset"    ,    m_tolTyOffset = ty);
    std::vector<double> tmp          = boost::assign::list_of(  0.0  )(  0.0  )(  0.0 );
    declareProperty( "TolTySlope"     ,    m_tolTySlope = tmp);
  }
  //-------- Simultaneous Fit Y
  {
    //MaxChi2 Hit >10 Hits
    std::vector<double> maxChi2FullFit_11and12Hit = boost::assign::list_of(5.5)(5.5)(5.5);  //N Layers > 10 ( 11 and 12 hits ) outliers removed if MaxChi2Hit > maxChi2Hits_11and12Hit
    declareProperty( "maxChi2Hits_11and12Hit" , m_maxChi2HitFullFitHigh = maxChi2FullFit_11and12Hit);
    //MaxChi2 Hit <11 Hits + Y(0) +  Y(zRef) cut
    std::vector<double> maxChi2FullFit_less11Hit = boost::assign::list_of(2.5)(2.5)(2.5); // N Layers < 11 (9,10) outliers removed if MaxChi2Hit< maxChi2Hits_less11Hit
    declareProperty( "maxChi2Hits_less11Hit" , m_maxChi2HitFullFitLow = maxChi2FullFit_less11Hit);
    std::vector<double> maxY0 = boost::assign::list_of( 50.)(50.)(50.);                  // If N Layers < 11: kill tracks having y(z=0) >50. mm
    declareProperty( "maxYatZeroLow"         , m_maxY0Low             = maxY0);
    std::vector<double> maxyRef = boost::assign::list_of( 500.)(500.)(500.);             // If N Layers < 11 : kill tracks having y(zRef)>500. mm
    declareProperty( "maxYatzRefLow"         , m_maxYZrefLow          = maxyRef);
    //Global Chi2PerDoF
    std::vector<double> chi = boost::assign::list_of(4.0)(6.0)(7.0);                     // Global Chi2PerDoF
    declareProperty( "maxChi2PerDoF"         , m_maxChi2PerDoF        = chi );
  }
  //Global Clone Removal
  // Parameters for debugging
}
//=============================================================================
// Destructor
//=============================================================================
PrHybridSeeding::~PrHybridSeeding() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PrHybridSeeding::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  m_hitManager= tool<PrHitManager>( m_hitManagerName );
  m_geoTool = tool<PrGeometryTool>("PrGeometryTool");
  m_zReference = m_geoTool->zReference();

  m_hitManager->buildGeometry();
  m_debugTool = 0;
  if ( "" != m_debugToolName ) {
    m_debugTool = tool<IPrDebugTool>( m_debugToolName );
    info()<<"Debug tool "<<m_debugToolName<<" loaded."<<endmsg;
  } else {
    m_wantedKey = -100;  //no debug
  }
  if ( m_doTiming) {
    m_timerTool = tool<ISequencerTimerTool>( "SequencerTimerTool/Timer", this );
    m_timeTotal   = m_timerTool->addTimer( "PrSeeding total" );
    m_timerTool->increaseIndent(); //+1
    m_timeFromForward = m_timerTool->addTimer( "Time from Forward" );
    m_timerTool->increaseIndent(); //+2

    //Case 0
    m_timeXProjeUp[0] = m_timerTool->addTimer( "Case 0 : X Projections Up");
    m_timerTool->increaseIndent(); //+3
    m_timeCloneXUp[0] = m_timerTool->addTimer( "Case 0 : Clones X Up");
    m_timerTool->increaseIndent(); //+4
    m_timeStereoUp[0] = m_timerTool->addTimer( "Case 0 : AddStereo Up");
    m_timerTool->increaseIndent();//+5
    m_timeFlagUp[0] = m_timerTool->addTimer( " Case 0 : Flag Up");

    //Case 1
    m_timerTool->decreaseIndent(); // +4
    m_timerTool->decreaseIndent(); // + 3
    m_timerTool->decreaseIndent(); //+ 2
    m_timeXProjeUp[1] = m_timerTool->addTimer( "Case 1 : X Projections Up");
    m_timerTool->increaseIndent(); //+ 3
    m_timeCloneXUp[1] = m_timerTool->addTimer( "Case 1 : Clones X Up");
    m_timerTool->increaseIndent();      //+4
    m_timeStereoUp[1] = m_timerTool->addTimer( "Case 1 : AddStereo Up");
    m_timerTool->increaseIndent();          //+5
    m_timeFlagUp[1] = m_timerTool->addTimer( "Case 1 : Flag Up");
    m_timerTool->decreaseIndent();//+4
    m_timerTool->decreaseIndent();//+3
    m_timerTool->decreaseIndent();//+2

    //Case 2
    m_timeXProjeUp[2] = m_timerTool->addTimer( "Case 2 : X Projections Up");
    m_timerTool->increaseIndent();//+3
    m_timeCloneXUp[2] = m_timerTool->addTimer( "Case 2 : Clones X Up");
    m_timerTool->increaseIndent();//+4
    m_timeStereoUp[2] = m_timerTool->addTimer( "Case 2 : AddStereo Up");
    m_timerTool->increaseIndent();//+5
    m_timeFlagUp[2] = m_timerTool->addTimer( "Case 2 : Flag Up");
    m_timerTool->decreaseIndent();//+4
    m_timerTool->decreaseIndent();//+3
    m_timerTool->decreaseIndent();//+2

    //Case 0 Down
    m_timeXProjeDo[0] = m_timerTool->addTimer( "Case 0 : X Projections Down");
    m_timerTool->increaseIndent();//+3
    m_timeCloneXDo[0] = m_timerTool->addTimer( "Case 0 : Clones X Down");
    m_timerTool->increaseIndent();//+4
    m_timeStereoDo[0] = m_timerTool->addTimer( "Case 0 : AddStereo Dowm");
    m_timerTool->increaseIndent();//+5
    m_timeFlagDo[0] = m_timerTool->addTimer( " Case 0 : Flag Down");
    m_timerTool->decreaseIndent();//+4
    m_timerTool->decreaseIndent();//+3
    m_timerTool->decreaseIndent();//+2


    //Case 1 Down
    m_timeXProjeDo[1] = m_timerTool->addTimer( "Case 1 : X Projections Down");
    m_timerTool->increaseIndent();//+3
    m_timeCloneXDo[1] = m_timerTool->addTimer( "Case 1 : Clones X Down");
    m_timerTool->increaseIndent();//+4
    m_timeStereoDo[1] = m_timerTool->addTimer( "Case 1 : AddStereo Down");
    m_timerTool->increaseIndent();//+5
    m_timeFlagDo[1] = m_timerTool->addTimer( "Case 1 : Flag Down");
    m_timerTool->decreaseIndent();//+4
    m_timerTool->decreaseIndent();//+3
    m_timerTool->decreaseIndent();//+2

    //Case 2 Down
    m_timeXProjeDo[2] = m_timerTool->addTimer( "Case 2 : X Projections Down");
    m_timerTool->increaseIndent(); //+3
    m_timeCloneXDo[2] = m_timerTool->addTimer( "Case 2 : Clones X Down");
    m_timerTool->increaseIndent(); //+4
    m_timeStereoDo[2] = m_timerTool->addTimer( "Case 2 : AddStereo Down");
    m_timerTool->increaseIndent(); //+5
    m_timeFlagDo[2] = m_timerTool->addTimer( "Case 2 : Flag Down");
    m_timerTool->decreaseIndent(); //+4
    m_timerTool->decreaseIndent(); //+3
    m_timerTool->decreaseIndent(); //+2
    m_timerTool->decreaseIndent();//+1
    //if(m_y50Cut)  m_timeClone = m_timerTool->addTimer( "Remove Clones And Recover tracks");

    m_timeClone[0] = m_timerTool->addTimer( "RemoveClones Up");
    m_timeClone[1] = m_timerTool->addTimer( "RemoveClones Down");

    m_timeConvert[0] = m_timerTool->addTimer("Convert Tracks Up");
    m_timeConvert[1] = m_timerTool->addTimer("Convert Tracks Down");
    // m_timeStereo      = m_timerTool->addTimer( "Add stereo" );
    // m_timeFinal       = m_timerTool->addTimer( "Convert tracks" );
    m_timerTool->decreaseIndent();//0
  }
  if( m_decodeData )         info() << "Will decode the FT clusters!" << endmsg;
  if( m_FlagHits)                 info()<<"Will Flag the Hits" << endmsg;
  if( m_removeFlagged)      info()<<"Will Not re-use Flagged"<<endmsg;
  if( m_inputName == "")    info()<<"Standalone Seeding"<<endmsg;
  if( !(m_inputName == "")) info()<<"Forward tracks as input"<<endmsg;
  if( m_removeClones)      info()<<"Will Remove Clones"<<endmsg;
  if( m_xOnly) info()<<"Will use Only X Layers" <<endmsg;
  if( m_useFix && !m_xOnly) info()<<"WIll Use the triangle Fix"<<endmsg;
  if( m_recoverTrack) info()<<"Will Recover the track at the end (to be implemented)"<<endmsg;

  if( m_printSettings){
    info() <<"==================================================="<<endmsg
           << "===============GLOBAL SETTINGS===================="<<endmsg
           << " InputName                                       = "<<  m_inputName              << endmsg
           << " OutputName                                     = "<<  m_outputName             << endmsg
           << " HitManagerName                                 = "<<  m_hitManagerName         << endmsg
           << " DecodeData                                     = "<<  m_decodeData             << endmsg
           << " XOnly                                          = "<<  m_xOnly                  << endmsg
           << " MinXPlanes                                     = "<<  m_minXPlanes             << endmsg
           << " MaxNHits                                       = "<<  m_maxNHits               << endmsg
           << " NCases                                         = "<<  m_nCases                 << endmsg
           << " TimingMeasurement                              = "<<  m_doTiming               << endmsg
           << " dRatiio                                        = "<< m_dRatio                 << endmsg
           << " CConst BackProj                                = "<< m_ConstC<<endmsg
           << "=================ADD UV Layer Settings============"<< endmsg
           << "doAsymmUV                                       = "<< m_doAsymmUV  <<endmsg
           << "Use Triangle Fix                                = " << m_useFix                  << endmsg
           << "Use SecondOrder `Triangle Fixing                = " << m_useFix2ndOrder <<endmsg
           << "*******Fit Settings"<<endmsg
           << "==================Clone Removal and Flag Hits Settings=========="<<endmsg
           << "Remove Clones after X searh                    = " <<m_removeClonesX<<endmsg
           << "RemoveClones after add stereo UV               = " << m_removeClones<<endmsg
           << "Min Hits Common Total                         = " << m_nCommonUV <<endmsg

           << "Flag the hits                                  = " << m_FlagHits <<endmsg
           << "Remove All Flagged                             = " << m_removeFlagged<<endmsg;
    // << "Flag_MaxChi2 Hit                               = " << m_MaxChi2Flag<<endmsg
    // << "Flag_MaxX0                                     = " <<m_MaxX0Flag <<endmsg;
    info()<<"  Will Run N Cases = " << m_nCases<< endmsg;

    info()<<" ============================ Find X Projection Settings ======================="<<endmsg;
    info()<<" 1 - First-Last Layer Settings  (2 Hit Combo)  "<<endmsg;
    for(unsigned int kk =0;m_alphaCorrection.size()>kk;kk++){
      info()<<"\t Case "<<kk<<"   Rotation                  ="<<m_alphaCorrection[kk]<<endmsg;
      info()<<"\t Case "<<kk<<"   Tolerance after Rotation  ="<<m_TolFirstLast[kk]<<endmsg;
    }

    info()<<" 2 - Add Hit in T2 - XLayers Stations  (3 Hit Combo) "<<endmsg;
    info()<<" Allow N Parabola Seed Hits = " << m_maxParabolaSeedHits;
    for(unsigned int kk =0;m_x0Corr.size() > kk; kk++)
    {
      info()<<"\t Case "<<kk<<"       x0Rotation = "<<m_x0Corr[kk]<<endmsg;
      info()<<"\t Case "<<kk<<"    x0SlopeChange = "<<m_x0SlopeChange[kk]<<"    Tolerance at x0SlopeChange "<<  m_TolX0SameSign[kk]<<endmsg;
      info()<<"\t Case "<<kk<<"   x0SlopeChange2 = "<<m_x0SlopeChange2[kk]<<"    Tolerance at x0SlopeChange2"<< m_tolX0Oppsig[kk]<<endmsg;
      info()<<"\t Case "<<kk<<"             x0Max = "<<m_x0Cut[kk]<<"    Tolerance Up at x0Max"<< m_tolAtX0Cut[kk]<< " Tolerance Down at x0Max "<<m_tolAtx0CutOppSig[kk]<<endmsg;
    }
    info()<<" 3 - Add Hit in remaining Layers (4/5/6 Hit Combo) ---- "<<endmsg;
    for( unsigned int kk =0; m_tolRemaining.size()>kk;kk++){
      info()<<"\t Case"<<kk<<"    Tolerance = "<<m_tolRemaining[kk]<<endmsg;
    }
    info()<<" 4 - Fit XZ Projection  "<<endmsg;
    info()<<"\t minXPlanes "<< m_minXPlanes;
    info()<<"\t dRatio " << m_dRatio<<endmsg;
    for( unsigned int kk =0; m_maxChi2HitsX.size()>kk;kk++){
      info()<<"\t Case "<<kk<<"      MaxChi2Hit X Fit    = "<<m_maxChi2HitsX[kk]<<endmsg;
      info()<<"\t Case "<<kk<<"      MaxChi2Track X Fit  = "<<m_maxChi2DoFX[kk]<<endmsg;
    }
    info()<<" Remove Clones X " << m_removeClonesX<< endmsg;
    info()<<" ========================= Add The UV part =================================== "<<endmsg;
    info()<<" 1 - Hit Selection pre-Hough Clustering "<<endmsg;
    info()<<"\t Y Min                 "<< m_yMin      <<endmsg
          <<"\t Y Max                 "<< m_yMax      <<endmsg
          <<"\t UseTrFix              "<<m_useFix     <<endmsg
          <<"\t Y Min TrFix           "<< m_yMin_TrFix<<endmsg
          <<"\t Y Max TrFix           "<< m_yMax_TrFix<<endmsg
          <<"\t Asymmetric UV search  "<< m_doAsymmUV  <<endmsg
          <<"\t RemoveHole            "<<m_removeHole <<endmsg
          <<"\t TrFix 2nd Order       "<<m_useFix2ndOrder<<endmsg
          <<endmsg;

    info()<<" 2 - Hough Cluster in UV Layers (y/z) hits "<<endmsg;
    for( unsigned int kk = 0; m_tolTyOffset.size()>kk; kk++){
      info()<<"\t Case "<<kk<<" maxCoord-minCoord < " <<m_tolTyOffset[kk]<<" + "<< m_tolTySlope[kk] <<" * minCoord"<<endmsg;
    }
    info()<<" 3 - Do preliminary Y line fit "<< m_useLineY;
    if( m_useLineY){
      for( unsigned int kk = 0;  m_Chi2LowLine.size()>kk; kk++){
        info()<<"\t Case"<<kk<<"max   Chi2DoFX + Chi2Line (<11) hit  "<<m_Chi2LowLine[kk]<<endmsg;
        info()<<"\t Case"<<kk<<"max   Chi2DoFX + Chi2Line (>10) hit  "<<m_Chi2HighLine[kk]<<endmsg;
      }
    }
    info()<<" 4 - Fit Full Track "<<endmsg;
    info()<<" Force track to have maxNHits ( if UseLineY, auto 12)  = " << m_maxNHits<<endmsg;
    info()<<" Use dRatioCorrection with XY position " << m_useCorrPos<<endmsg;
    info()<<"\t \t Hits <11 "<<endmsg;
    for( unsigned int kk = 0;  m_maxY0Low.size() >kk; kk++){
      info()<<"\t Case"<<kk<<" maxY at 0 after Fit    "<< m_maxY0Low[kk]<<endmsg;
      info()<<"\t Case"<<kk<<" maxY at zRef after Fit "<<m_maxYZrefLow[kk]<<endmsg;
      info()<<"\t Case"<<kk<<" MaxChi2Hit             "<<m_maxChi2HitFullFitLow[kk]<<endmsg;
    }

    info()<<"\t \t Hits >=11 "<<endmsg;
    for( unsigned int kk =0; m_maxChi2HitFullFitHigh.size()>kk;kk++){
      info()<<"\t Case"<<kk<<" MaxChi2Hit             "<<m_maxChi2HitFullFitHigh[kk]<<endmsg;
    }
    info()<<" 5 - FinalChi2DoF      "<<endmsg;
    for( unsigned int kk = 0; m_maxChi2FullFit.size()>kk;kk++){
      info()<<"\t Case"<<kk<<" MaxChi2PerDoF Total   "<< m_maxChi2FullFit[kk]<<endmsg;
    }
    info()<<" ====================== Flag Hits ============================== " << endmsg;
    for( unsigned int kk = 0; m_SizeFlag.size()>kk;kk++){
      info()<<"\t Case"<<kk<<" Size Flag"<< m_SizeFlag[kk]<<endmsg;
    }
  }
  return StatusCode::SUCCESS;
}


//=============================================================================
// Main execution
//=============================================================================
StatusCode PrHybridSeeding::execute() {
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  if ( m_doTiming )
  {
    m_timerTool->start( m_timeTotal );
    m_timerTool->start( m_timeFromForward );
  }
  LHCb::Tracks* result = new LHCb::Tracks();
  put( result, m_outputName );
  if( m_decodeData ){
    debug()<<"Decoding Data"<<endmsg;
    m_hitManager->decodeData();
    debug()<<"Decoding Done"<<endmsg;
  }
  //  int multiplicity[24];
  int multiplicityTot = 0;
  //char zoneName[100];
  // UNFLAG USED TO ALL HITS
  debug()<<"UNFLAGGING ALL HITS"<<endmsg;
  for ( unsigned int zone = 0; m_hitManager->nbZones() > zone; ++zone ) {
    //multiplicity[zone]=0;
    //sprintf (zoneName, "Multiplicity_InZone_%i",zone);
    for ( PrHits::const_iterator itH = m_hitManager->hits( zone ).begin();
          m_hitManager->hits( zone ).end() != itH; ++itH ) {
      (*itH)->setUsed( false );
      //multiplicity[zone]++;
      multiplicityTot++;
    }
  }
  //========================================================
  // Remove Seed segment if we pass the  Forward as Input
  //========================================================
  if ( "" != m_inputName){
    debug()<<"Removing Seed Segment from Forward Tracking"<<endmsg;
    for(int i = 0; i < 24; i++){
      PrHitZone* zone = m_hitManager->zone(i);
      std::stable_sort( zone->hits().begin(),  zone->hits().end(), compLHCbID());
    }
    LHCb::Tracks* forward = get<LHCb::Tracks>( m_inputName );
    //Loop over forward tracks container
    for ( LHCb::Tracks::iterator itT = forward->begin(); forward->end() != itT; ++itT ) {
      //Vector of LHCbID
      std::vector<LHCb::LHCbID> ids;
      ids.reserve(20);
      // Loop over LHCbIDs of the forward track
      for ( std::vector<LHCb::LHCbID>::const_iterator itId = (*itT)->lhcbIDs().begin();
            (*itT)->lhcbIDs().end() != itId; ++itId ) {
        if ( (*itId).isFT() && (*itId).ftID().layer() < 12 ) {
          LHCb::FTChannelID ftId =(*itId).ftID();
          int zoneNb = 2 * ftId.layer() + ftId.mat(); //zones top are even (0, 2, 4, ....,22)  and zones bottom are odd
          //Load the PrHitZone
          PrHitZone* zone = m_hitManager->zone(zoneNb);
          // -- The hits are sorted according to LHCbID, we can therefore use a lower bound to speed up the search
          PrHits::iterator itH = std::lower_bound(  zone->hits().begin(),  zone->hits().begin(), *itId, lowerBoundLHCbID() );

          for ( ; zone->hits().end() != itH; ++itH ) {
            if( *itId < (*itH)->id() ) break;
            if ( (*itH)->id() == *itId ) (*itH)->setUsed( true );
          }
          ids.push_back( *itId );
        }
      }
      // Forward output loaded to TTrack container with History flag
      LHCb::Track* seed = new LHCb::Track;
      seed->setLhcbIDs( ids );
      seed->setType( LHCb::Track::Ttrack );
      seed->setHistory( LHCb::Track::PrSeeding );
      seed->setPatRecStatus( LHCb::Track::PatRecIDs );
      seed->addToStates( (*itT)->closestState( 9000. ) );
      result->insert( seed );
    }



    //=======================================
    // Sort hits according to x for each zone
    //=======================================
    for(int i = 0; i < 24; i++){
      PrHitZone* zone = m_hitManager->zone(i);
      std::stable_sort( zone->hits().begin(),  zone->hits().end(), compX());
    }
  }
  //=======================================
  // Fill zones
  //=======================================
  m_zones.clear();
  for(int i = 0; i < 24; i++){
    m_zones.push_back( m_hitManager->zone(i) );
  }
  //==========================================================
  //END FLAGGING HITS FROM FORWARD
  //==========================================================
  // Hits are ready to be processed
  //==========================================================
  for(unsigned int i = 0; i< m_trackCandidates.size() ; i++){
    m_xCandidates[(int)i].clear();
    m_trackCandidates[(int)i].clear();
  }
  //if(m_y50Cut) m_trackExcluded.clear();
  if ( m_doTiming ) {
    m_timerTool->stop( m_timeFromForward );
  }
  //========================================================
  //------------------MAIN SEQUENCE IS HERE-----------------
  //========================================================
  // ----- Loop through lower and upper half
  //for( unsigned int part= 0; 2 > part; ++part ){
  //----- Loop For difference Cases
  //Swap them ? externally icase & inner loop part? m_xCandidates
  //for( unsigned int part = 0 ; 2>part; ++part){
  //if(m_OnlyUp && part ==1) continue;
  //if(m_OnlyDown && part ==0) continue;
  for(unsigned int icase = 0; m_nCases>icase ; ++icase){
    for(unsigned int part =0; 2>part;++part){
      if( m_doTiming){
        if( part ==0){m_timerTool->start( m_timeXProjeUp[icase]);}
        if( part ==1){m_timerTool->start( m_timeXProjeDo[icase]);}
      }
      // Find The X Projection
      findXProjections(part,icase);
      if( m_doTiming){
        if( part ==0){
          m_timerTool->stop( m_timeXProjeUp[icase]);
          m_timerTool->start( m_timeCloneXUp[icase]);
        }
        if( part ==1){
          m_timerTool->stop( m_timeXProjeDo[icase]);
          m_timerTool->start(m_timeCloneXDo[icase]);
        }
      }
      if(m_removeClonesX) removeClonesX( part, icase, m_xOnly);
      if( m_doTiming){
        if( part==0 ){
          m_timerTool->stop( m_timeCloneXUp[icase]);
          m_timerTool->start(m_timeStereoUp[icase]);
        }
        if( part==1){
          m_timerTool->stop( m_timeCloneXDo[icase]);
          m_timerTool->start(m_timeStereoDo[icase]);
        }
      }
      //Add The stereo Part
      if(!m_xOnly){
        if(m_stereo2)
          addStereo2( part, icase);
        if(!m_stereo2){ //Prototype for vectorization and faster implementation ( need some retuning of the stereo part )
          addStereo( part, icase );
        }
      }
      //Timing
      if(m_doTiming){
        if(part == 0){
          m_timerTool->stop( m_timeStereoUp[icase]);
          m_timerTool->start(  m_timeFlagUp[icase]);
        }
        if(part ==1){
          m_timerTool->stop( m_timeStereoDo[icase]);
          m_timerTool->start( m_timeFlagDo[icase]);
        }
      }
      //Flag found Hits at the end of each single case ( exclude the latest one )
      if(m_FlagHits && (icase ==0 || icase ==1) && !m_xOnly){
        flagHits(icase,part);
      }
      if( m_doTiming){
        if( part ==0 )
          m_timerTool->stop( m_timeFlagUp[icase]);
        if( part ==1 )
          m_timerTool->stop(m_timeFlagDo[icase]);
      }
    }
  }
  for( unsigned int part = 0 ; part<2; part ++){
    if(m_doTiming){
      m_timerTool->start( m_timeClone[(int)part]);
    }
    removeClones(m_nCommonUV, part);
    if( m_doTiming){
      m_timerTool->stop( m_timeClone[ (int) part]);
    }
    makeLHCbTracks(result, part);
  }
  if( msgLevel(MSG::DEBUG)) debug()<<"Making LHCb Tracks Done"<<endmsg;
  if( m_doTiming){
    m_timerTool->stop( m_timeTotal);
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PrHybridSeeding::finalize() {
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
  return GaudiAlgorithm::finalize();
}

void PrHybridSeeding::CollectUV(  PrHybridSeedTrack& xProje,const std::vector<PrHitZone*>& uvZones, PrHits& myStereo ){
  myStereo.clear();
  //Add Stereo : ask if Associated X part is processed or not
  //partic = MCParticle associated ( updated by AssocTrack method)
  //efficiency = Hit efficiency on track ( nHitsAssociated / nHitsFound )
  //AssocX = bool to tell if the track is associated
  unsigned int part = xProje.zone();


  for( PrHitZone* zoneUV: uvZones ){
    unsigned int kk = zoneUV->number();
    //if(m_zones[kk]->isX()) continue; // Do not load x-layers
    double yMin = -1*std::fabs(m_yMax);
    //yMin = -2500 mm
    double yMax = +1*std::fabs(m_yMax);
    //yMax = +2500 mm
    // double dxDy = m_zones[kk]->dxDy();  //Load the stereo Angle of that given layer
    // double zPlane = m_zones[kk]->z();   //Load the z of the Plane
    double dxDy = zoneUV->dxDy();
    double zPlane = zoneUV->z();
    double xPred = xProje.x(zPlane);
    double recDxDyZ = 1./(dxDy*zPlane);
    double recDxDy = 1./dxDy;

    //Compute the predicted x-position in that given layer
    //unsigned int kk = zoneUV->number();
    //[-2500;+2500]
    if(yMin >yMax){
      //should not happen
      double tmp = yMax;
      yMax = yMin;
      yMin = tmp;
    }
    if(msgLevel(MSG::ERROR) && yMax <yMin) error()<<"Stereo Problem"<<endmsg;
    if(m_doAsymmUV){
      if(part==0){
        //UPPER TRACKS y>0
        //Upper Tracks: expected y > 0 (m_yMin need to be negative , m_yMax need to be positive)
        // yMin updated to be -1 mm , so
        // Upper Tracks y in [ -1,+2500]
        yMin = m_yMin;
        //Upper Track Upper Modyule [-1,2500]
        if(kk%2==1 && m_useFix){
          //kk%2=1 if kk =1,3,5,7 etc, i.e. you are reading Lower modules for a Upper track:
          //UPPER TRACK lower Module
          //y is set in [-30, 2.0]
          // yMax = +30.0;
          // yMin = -2.0;
          //[-2,30]
          yMin = +m_yMin_TrFix;
          //-2.0 mm
          yMax = +m_yMax_TrFix;
          //+30.0 mm
        }
      }
      if(part==1){
        //LOWER TRACKS
        //Lower tracks: expected y < 0 ( m_yMin need to be negative, m_yMax need to be positive)
        //yMax updated from +2500 to +1 mm
        //yMin remains -2500 mm
        yMax = -m_yMin;
        //+1.0 mm
        //[-2500,+1] mm
        if(kk%2==0 && m_useFix){
          //kk%2 = 0 if kk = 0,2,4,6 etx, i.e., you are reading Upper modules for a Lower track:
          //LOWER TRACK upper Module
          // y is set in [-30,+2]
          // yMin = -30.0;
          // yMax = +2.0;
          yMin = -m_yMax_TrFix;
          //-30.0 mm
          yMax = -m_yMin_TrFix;
          //+2.0 mm
          //[-30.,2.0]
        }
      }
    }
    //Compute the xMin value in the rotated reference system
    //Compute the xMax value in the rotated reference system
    //Due to dxDy being positive or negative ( yMin here is always < yMax ) you can have them swapped
    //x(y) ~ xPred = x(0) + dxDy*y => x(0) = xPred - dxDy*y ; xMin xMax from yMin and yMax and swap them if opposite in sign
    double xMin = xPred - yMin*dxDy;
    double xMax = xPred - yMax*dxDy;
    if(xMin > xMax){
      //revert to properly take into account dxDy
      double tmp = xMax;
      xMax = xMin;
      xMin = tmp;
    }
    // PrHits& hits = m_zones[kk]->hits(); //Load all the Hits in that given zone
    PrHits& hits = zoneUV->hits();
    //Place the iterator at xMin predicted
    //unsigned int kk = zoneUV->number();
    PrHits::iterator itH = std::lower_bound(hits.begin(),hits.end(),xMin, lowerBoundX());
    PrHits::iterator  itEnd = std::upper_bound( hits.begin(),hits.end(),xMax, upperBoundX());
    for(  ; itEnd!= itH; ++itH ){
      if( (*itH)->isUsed() && m_removeFlagged ) continue;
      //check if the u/v hit is used: do not use it again
      double y = ( xPred - (*itH)->x() )*recDxDy;
      //compute y hit
      //should not happen actually
      if(y >yMax) continue;
      if(y <yMin) continue;
      if(m_useFix2ndOrder){
        if(part == 0 && kk%2 == 0){
          //Up Up
          if( (*itH)->yMin()<0. ){yMin = m_yMin                          ;yMax = m_yMax;}
          if( (*itH)->yMin()>0. ){yMin = -2.0 + std::fabs((*itH)->yMin());yMax = m_yMax;}
        }
        if(part == 0 && kk%2 == 1){
          //Up Down
          if( (*itH)->yMax()<0. ) continue;
          if( (*itH)->yMax()>0. ){yMin = -1.0 ;yMax = +2.0 + std::fabs((*itH)->yMax());
          }
        }
        if(part ==1 && kk%2 == 1){
          //Down Down
          if( (*itH)->yMax()>0.){yMin = -std::fabs(m_yMax) ;yMax = +1.0;}
          if( (*itH)->yMax()<0.){yMin = -std::fabs(m_yMax) ;yMax = +2.0-std::fabs((*itH)->yMax());}
        }
        if( part ==1 && kk%2 ==0){
          //Down Up
          if( (*itH)->yMin()<0.){yMin = -2.0 - std::fabs((*itH)->yMin());yMax = +1.0;}
          if( (*itH)->yMin()>0.) continue;
        }
      }
      if(msgLevel(MSG::DEBUG)){
        //debug()<<"y Hit computed    "<< y<<endl;
      }
      if(y>yMax || y< yMin) continue;
      if(m_removeHole){
        double radius =  y*y + xPred*xPred;
        if(  radius < m_radiusHole*m_radiusHole) continue;
      }
      //Absolute value!
      (*itH)->setCoord( std::fabs( (xPred - (*itH)->x() )*recDxDyZ ) );
      if( part == 0 && y<0.)
        (*itH)->setCoord( -1.*std::fabs((xPred-(*itH)->x())*recDxDyZ));
      if( part == 1 && y>0.)
        (*itH)->setCoord( -1.*std::fabs((xPred-(*itH)->x())*recDxDyZ));
      // if(std::fabs((*itH)->coord()) > 0.15+0.25/1000*std::fabs((*itT).X0())) continue;
      myStereo.push_back( (*itH) );
    }
  }
}

void PrHybridSeeding::addStereo2( unsigned int part, unsigned int iCase){
  // always()<<"Adding stereo 2"<<endmsg;
  std::vector<PrHitZone*> uvZones;
  if(m_useFix)
    uvZones.reserve(12);
  if(!m_useFix)
    uvZones.reserve(6);
  if(m_useFix){
    //load vector of zones
    for( unsigned int detector_part :{0,1}){
      for( unsigned int uvZoneID :{ s_T1U, s_T1V,s_T2U,s_T2V,s_T3U,s_T3V}){
        if(msgLevel(MSG::DEBUG)) always()<<"Push back zone = "<<uvZoneID+detector_part<<endmsg;
        if( m_zones[uvZoneID|detector_part]->isX()) always()<<"ERROR LOAD"<<endmsg;
        uvZones.push_back( m_zones[uvZoneID | detector_part]);
      }
    }
  }
  if(!m_useFix){
    for( unsigned int uvZoneID :{s_T1U, s_T1V,s_T2U,s_T2V,s_T3U,s_T3V}){
      uvZones.push_back( m_zones[uvZoneID | part]);
    }
  }
  PrHits myStereo;
  myStereo.reserve(400);
  // HitIteratorsPairs Clusters;
  PrPlaneHybridCounter plCount;
  std::vector<PrHits> PlanesMultiple;
  // std::vector<PrHits> UVHits[6];
  PrHits hits1;
  PrHits hits2;
  PrHits hits5;
  PrHits hits6;
  PrHits hits9;
  PrHits hits10;
  PrHits PlanesSingle;
  PlanesMultiple.reserve(6);
  PlanesSingle.reserve(6);
  hits1.reserve(20);
  hits2.reserve(20);
  hits5.reserve(20);
  hits6.reserve(20);
  hits9.reserve(20);
  hits10.reserve(20);
  PrHybridSeedTracks Tracks;
  Tracks.reserve(20);
  int maxUVfound = 0;
  PrLineFitterY BestLine( m_zReference );
  double initCoord = std::numeric_limits<double>::max();
  double ClusterSpread[3][3];
  HitIter initBeg[3][3];
  HitIter initEnd[3][3];
  for( PrHybridSeedTracks::iterator itT =m_xCandidates[(int)part].begin();m_xCandidates[(int)part].end() != itT; ++itT){
    if( (*itT).Case() != iCase) continue;
    if( !(*itT).valid() ) continue;
    CollectUV( (*itT) , uvZones , myStereo);
    std::sort( myStereo.begin(), myStereo.end(), PrHit::LowerByCoord() );
    //Define the min UV hits based on the amount of hits of the track
    unsigned int minUV = 4;
    if((*itT).hits().size()==6) minUV = m_minUV6[iCase];
    if((*itT).hits().size()==5) minUV = m_minUV5[iCase];
    if((*itT).hits().size()==4) minUV = m_minUV4[iCase];
    maxUVfound=-1;
    unsigned int firstSpace = m_trackCandidates[(int)part].size();
    //initialize the clusters ( here 9 )
    for( int i =0; i<3 ; ++i){
      for( int j = 0; j<3; ++j){
        ClusterSpread[i][j] = initCoord;
        initBeg[i][j] = myStereo.end();
        initEnd[i][j] = myStereo.end();
      }
    }
    PrHits::iterator itBeg = myStereo.begin();
    PrHits::iterator itEnd =  itBeg+4<=myStereo.end() ? itBeg+4: myStereo.end();
    // PrHits::iterator itEnds[3];
    // { 6, 5, 4 }
    // itEnds = {itBeg+6, itBeg+5, itBeg+4};
    // for (int i =0 ; i < 3; ++i) itEnds[i] = itBeg+6-i <= myStereo.end() ? itBeg+6-i : myStereo.end();
    // Pre-fill the clusters
    for( ; itEnd-itBeg>=4; IncreaseIters( itBeg, itEnd,4, myStereo)){
      double tolTy = m_tolTyOffset[iCase] + m_tolTySlope[iCase]*(*itBeg)->coord();
      // Check for 6 layers, then 5 layers, and finally 4 layers
      for (int i = 0; i < 3; ++i){
        // i = 0; itLast = itEnd+6
        PrHits::iterator itLast = itEnd+2-i <= myStereo.end() ? itEnd+2-i : myStereo.end();
        // spread6 computed first
        double spread = (*(itLast-1))->coord() - (*itBeg)->coord();
        if(CheckCluster(itBeg, itLast, tolTy)){
          //cluster is fine check
          int j;
          // find the cluster worse than the current cluster
          for (j = 0; j < 3; ++j){
            if(spread < ClusterSpread[i][j]) break;
          }
          if (j == 3) continue;
          // Shift clusters after the found position
          for(int k = 2; k > j; --k){
            initBeg[i][k] = initBeg[i][k-1];
            ClusterSpread[i][k] = ClusterSpread[i][k-1];
            initEnd[i][k] = initEnd[i][k-1];
          }
          // insert current cluster at found position
          initBeg[i][j] = itBeg;
          ClusterSpread[i][j] = spread;
          initEnd[i][j] = itLast;
          break;
        }
      }
    }
    //int maxCl = 9;
    int nXinit = (*itT).hits().size();
    BestLine.setXProj((*itT));
    int nCandidates = 0;
    for( int i = 0; i<3; ++i){
      for( int j = 0; j<3; ++j){
        //you are at the cluster Number (1+j) +(3*i)
        PrHits::iterator itBeg = myStereo.begin();
        PrHits::iterator itEnd = myStereo.end();
        int nLay = 0;
        itBeg = initBeg[i][j];
        itEnd = initEnd[i][j];
        if( itBeg == myStereo.end() &&  itEnd == myStereo.end()){
          break;
        }
        nCandidates++;
        if(nCandidates > m_maxNClusters[(*itT).Case()]) break;
        ExtendCluster( itBeg, itEnd , iCase, myStereo , nLay );
        if( (int)nLay < (int)maxUVfound || (int)nLay < (int)minUV) continue;
        if( (int)nLay < (int)minUV) continue;
        plCount.set( itBeg , itEnd );
        nLay = plCount.nbDifferentUV();
        if( nLay == 4 && !plCount.isOKUV()){
          continue;
        }
        BestLine.setXProj( (*itT) );
        PrHybridSeedTrack temp(*itT);
        bool fit = false;
        if( plCount.nbSingleUV() == plCount.nbDifferentUV()){
          fit = BestLine.fit( itBeg,itEnd );
          if( fit && LineOK( m_Chi2LowLine[iCase], m_Chi2HighLine[iCase], BestLine, temp)){
            for( PrHits::iterator itH = itBeg; itEnd != itH; ++itH){
              temp.addHit( (*itH));
              temp.setYParam( BestLine.ay(), BestLine.by());
            }
          }
        }
        if( plCount.nbSingleUV() != plCount.nbDifferentUV()){
          Tracks.clear();
          hits1.clear();
          hits2.clear();
          hits5.clear();
          hits6.clear();
          hits9.clear();
          hits10.clear();
          PlanesSingle.clear();
          PlanesMultiple.clear();
          for(PrHits::iterator itH = itBeg; itEnd!= itH; ++itH){
            int planeCode = (*itH)->planeCode();
            if(plCount.nbInPlane( planeCode ) ==1 ){
              PlanesSingle.push_back((*itH));
              continue;
            }
            if(planeCode==1 && plCount.nbInPlane(planeCode)>1){
              //UVHits[0].push_back((*itH));
              hits1.push_back((*itH));
            }
            if(planeCode==2 && plCount.nbInPlane(planeCode)>1){
              //UVHits[1].push_back((*itH));
              hits2.push_back((*itH));
            }
            if(planeCode==5 && plCount.nbInPlane(planeCode)>1){
              //UVHits[2].push_back((*itH));
              hits5.push_back((*itH));
            }
            if(planeCode==6 && plCount.nbInPlane(planeCode)>1){
              //UVHits[3].push_back((*itH));
              hits6.push_back((*itH));
            }
            if(planeCode==9 && plCount.nbInPlane(planeCode)>1){
              //UVHits[4].push_back((*itH));
              hits9.push_back((*itH));
            }
            if(planeCode==10 && plCount.nbInPlane(planeCode)>1){
              //UVHits[5].push_back((*itH));
              hits10.push_back((*itH));
            }
          }
          // for( int i = 0; i<6 ; ++i){
          //   if(UVHits[i].size()!=0) PlanesMultiple.push_back(UVHits[i]);
          // }
          if(hits1.size()!=0) PlanesMultiple.push_back(hits1);
          if(hits2.size()!=0) PlanesMultiple.push_back(hits2);
          if(hits5.size()!=0) PlanesMultiple.push_back(hits5);
          if(hits6.size()!=0) PlanesMultiple.push_back(hits6);
          if(hits9.size()!=0) PlanesMultiple.push_back(hits9);
          if(hits10.size()!=0) PlanesMultiple.push_back(hits10);
          //PrHybridSeedTracks Tracks;
          //combinatorical tracks container when >1 hit in at least 1 layer.
          //PrHit* hit1 = nullptr;
          // CombinatoricTracks( Tracks, PlanesMultiple, PlanesSingle , (*itT) );
          for( PrHit* hit1: PlanesMultiple[0]){
            if(PlanesMultiple.size()==1){
              PrHybridSeedTrack tem(*itT);
              if(PlanesSingle.size()!=0){
                tem.addHits(PlanesSingle);
              }
              tem.addHit(hit1);
              Tracks.push_back(tem);
              continue;
            }
            if(PlanesMultiple.size()>1){
              for( PrHit* hit2 : PlanesMultiple[1]){
                if(PlanesMultiple.size() ==2){
                  PrHybridSeedTrack tem(*itT);
                  if(PlanesSingle.size()!=0){
                    //tem.addHits2(PlanesSingle);
                    tem.addHits(PlanesSingle);
                  }
                  tem.addHit(hit1);
                  tem.addHit(hit2);
                  Tracks.push_back(tem);
                  continue;
                }
                if(PlanesMultiple.size()>2){
                  for( PrHit* hit3: PlanesMultiple[2]){
                    if(PlanesMultiple.size() == 3){
                      PrHybridSeedTrack tem(*itT);
                      if(PlanesSingle.size()!=0){
                        tem.addHits(PlanesSingle);
                      }
                      tem.addHit( hit1);
                      tem.addHit( hit2);
                      tem.addHit( hit3);
                      Tracks.push_back(tem);
                      continue;
                    }
                    if(PlanesMultiple.size()>3){
                      for(PrHit* hit4 : PlanesMultiple[3]){
                        if(PlanesMultiple.size() == 4){
                          PrHybridSeedTrack tem(*itT);
                          if(PlanesSingle.size()!=0){
                            tem.addHits(PlanesSingle);
                          }
                          tem.addHit(hit1);
                          tem.addHit(hit2);
                          tem.addHit(hit3);
                          tem.addHit(hit4);
                          Tracks.push_back(tem);
                          continue;
                        }
                        if(PlanesMultiple.size()>4){
                          for(PrHit* hit5 : PlanesMultiple[4]){
                            if(PlanesMultiple.size() == 5){
                              PrHybridSeedTrack tem(*itT);
                              if(PlanesSingle.size()!=0){
                                tem.addHits(PlanesSingle);
                              }
                              tem.addHit(hit1);
                              tem.addHit(hit2);
                              tem.addHit(hit3);
                              tem.addHit(hit4);
                              tem.addHit(hit5);
                              Tracks.push_back(tem);
                              continue;
                            }
                            if(PlanesMultiple.size()>5){
                              for(PrHit* hit6: PlanesMultiple[5]){
                                if(PlanesMultiple.size()==6){
                                  PrHybridSeedTrack tem(*itT);
                                  if(PlanesSingle.size()!=0){
                                    tem.addHits(PlanesSingle);
                                  }
                                  tem.addHit(hit1);
                                  tem.addHit(hit2);
                                  tem.addHit(hit3);
                                  tem.addHit(hit4);
                                  tem.addHit(hit5);
                                  tem.addHit(hit6);
                                  Tracks.push_back(tem);
                                } //end if planes size 6
                              } //end loop hit in multiple 6
                            }//end if plane size >5
                          }//end loop hit in multiple 5
                        }//1
                      }//2
                    }//3
                  }//4
                }//5
              }//6
            }//7
          }//8
          bool fit = false;
          PrHybridSeedTracks::iterator best = Tracks.end();
          double bestchi2 = 1.e4;
          for(PrHybridSeedTracks::iterator tr = Tracks.begin(); Tracks.end()!=tr; ++tr){
            //for each combinatorics (1 hit in 1 layer), choose the best one based on the LineChi2DoF + XZProjectionChi2DoF
            fit = BestLine.fit( (*tr).hits().begin(), (*tr).hits().end() );
            if(fit && LineOK(m_Chi2LowLine[iCase], m_Chi2HighLine[iCase],BestLine,temp) ){
              if( BestLine.Chi2DoF() + temp.chi2PerDoF() < bestchi2){
                bestchi2 = BestLine.Chi2DoF() + temp.chi2PerDoF();
                best = tr;
                (*best).setYParam(BestLine.ay(),BestLine.by());
              }
            }
            //BestLine.reset();
          }
          if( bestchi2 < m_Chi2HighLine[iCase] ){
            for( PrHits::iterator it = (*best).hits().begin(); (*best).hits().end()!= it; ++it){
              if( (*it)->isX()) continue;
              temp.addHit( (*it));
            }
          }
        }
        if(temp.hits().size()< 9 ) continue;
        bool ok = false;
        temp.setnXnY( (*itT).hits().size(), plCount.nbDifferentUV());
        ok = fitSimultaneouslyXY(temp,iCase);
        while(!ok && temp.hits().size() > 9){
          ok = removeWorstAndRefit( temp, iCase);
          if( temp.hits().size()>m_maxNHits) ok = false;
        }
        if( ok  && temp.hits().size()>=9 &&
            temp.chi2PerDoF() < m_maxChi2PerDoF[iCase]){
          setChi2(temp);
          temp.setCase(iCase);
          if(m_removeClones) std::sort(temp.hits().begin(), temp.hits().end(), compLHCbID());
          if((int)temp.hits().size() - nXinit > maxUVfound){
            maxUVfound = (int)temp.hits().size()-nXinit;
          }
          // if(temp.hits().size() >= nXinit+(int)(6-i)){
          //   found[i] = true; //at least one has been found ( no need to go look for clusters with 5 hits
          // }
          m_trackCandidates[(int)part].push_back( temp);
          //nCandidates++;
        }
      }//end loop all pairs i
      //if(found[i]) break;
    }
    if( m_trackCandidates[(int)part].size() > firstSpace+1){
      for( unsigned int kk = firstSpace; m_trackCandidates[(int)part].size() > kk -1 ; ++kk ){
        if(!m_trackCandidates[(int)part][kk].valid()) continue;
        for( unsigned int ll = kk + 1; m_trackCandidates[(int)part].size() > ll; ++ll ){
          if(!m_trackCandidates[(int)part][ll].valid())continue;
          if(m_trackCandidates[(int)part][ll].hits().size() < m_trackCandidates[(int)part][kk].hits().size()){
            m_trackCandidates[(int)part][ll].setValid( false );
          }
          else if ( m_trackCandidates[(int)part][ll].hits().size() > m_trackCandidates[(int)part][kk].hits().size()){
            m_trackCandidates[(int)part][kk].setValid( false );
          }
          else if( m_trackCandidates[(int)part][kk].chi2() < m_trackCandidates[(int)part][ll].chi2()) {
            m_trackCandidates[(int)part][ll].setValid( false );
          }
          else{
            m_trackCandidates[(int)part][kk].setValid( false );
          }
        }
      }//end loop track Candidates for removal bad tracks
    }//loop candidate removal
  }
  m_xCandidates[(int)part].clear();
}





//counting the planes with builtin function ( compiler function )
// int nplanes = 0;
// for ( hit: itBeg ...itEnd){
// plane|=1<<hit->planeCode();
//}
void PrHybridSeeding::ExtendCluster( PrHits::iterator& itBeg, PrHits::iterator& itEnd , unsigned int iCase,PrHits&myStereo , int &nLay){
  int planes = 0;
  for( PrHits::iterator Hit = itBeg; itEnd!=Hit; ++Hit){
    planes|= 1<<(*Hit)->planeCode();
  }
  nLay = __builtin_popcount( planes);
  double tolTy = m_tolTyOffset[iCase] + (*itBeg)->coord()*m_tolTySlope[iCase] ;
  while( itEnd != myStereo.end() && ((*itEnd)->coord() - (*itBeg)->coord() ) < tolTy && nLay<6){//as soon as you find a sixth element exit the loop (in other case extend up to tolerance
    planes|= 1<<(*itEnd)->planeCode();
    ++itEnd;
    nLay = __builtin_popcount( planes);
  }
}

//==================
//Add Stereo void
//==================
void PrHybridSeeding::addStereo(unsigned int part, unsigned int iCase)
{
  std::vector<PrHitZone*> uvZones; //vector of u-v zones
  if(m_useFix)
    uvZones.reserve(12);
  if(!m_useFix)
    uvZones.reserve(6);
  if(m_useFix){ //load vector of zones
    for( unsigned int detector_part : {0,1}){
      for( unsigned int uvZoneID :{ s_T1U, s_T1V,s_T2U,s_T2V,s_T3U,s_T3V}){
        if(msgLevel(MSG::DEBUG)) always()<<"Push back zone = "<<uvZoneID+detector_part<<endmsg;
        if( m_zones[uvZoneID|detector_part]->isX()) always()<<"ERROR LOAD"<<endmsg;
        uvZones.push_back( m_zones[uvZoneID | detector_part]);
      }
    }
  }
  if(!m_useFix){
    for( unsigned int uvZoneID : {s_T1U, s_T1V,s_T2U,s_T2V,s_T3U,s_T3V}){
      uvZones.push_back( m_zones[uvZoneID | part]);
    }
  }
  PrHits myStereo;
  std::vector<PrHits> PlanesMultiple;
  // std::vector<PrHits> UVHits;
  // UVHits.reserve(6);
  PrHits hits1;
  PrHits hits2;
  PrHits hits5;
  PrHits hits6;
  PrHits hits9;
  PrHits hits10;
  PrHits PlanesSingle;
  PlanesMultiple.reserve(6);
  PlanesSingle.reserve(6);
  hits1.reserve(20);
  hits2.reserve(20);
  hits5.reserve(20);
  hits9.reserve(20);
  hits10.reserve(20);
  myStereo.reserve(400);
  PrHybridSeedTracks Tracks;
  PrPlaneHybridCounter plCount;
  PrLineFitterY BestLine(m_zReference);
  for( PrHybridSeedTracks::iterator itT =m_xCandidates[(int)part].begin();m_xCandidates[(int)part].end() != itT; ++itT){
    if( (*itT).Case() != iCase) continue; // Process Only X Candidates found by the Case x
    if( !(*itT).valid() ) continue;       // Process Only x Candidates which are valid
    CollectUV( (*itT) , uvZones , myStereo);
    std::sort( myStereo.begin(), myStereo.end(), PrHit::LowerByCoord() );

    unsigned int minUV = 4;
    if((*itT).hits().size()==6) minUV = m_minUV6[iCase];
    if((*itT).hits().size()==5) minUV = m_minUV5[iCase];
    if((*itT).hits().size()==4) minUV = m_minUV4[iCase];
    unsigned int minTot = m_minTot[iCase];
    unsigned int firstSpace = m_trackCandidates[(int)part].size();
    // PrHits::iterator itBeg = myStereo.begin();  //first hit in U-V layer with small Ty
    // PrHits::iterator itEnd = itBeg + minUV; //go ahead of minUV hits
    //Long tracks >5 GeV tolTyOffset = 0.002 is fine
    //            >2 GeV tolTyOffset = 0.002 looks also good
    //            >1.0 Gevlarger! (0.0035) should be even bigger but timing is exloding
    //additional hough cluster window based on the backward projection of the x-z plane? ( to be studied )
    //Long tracks > 5 GeV:
    // PrLineFitterY BestLine(m_geoTool->zReference(), (*itT));
    //if declared here should need the .reset()
    //unsigned int nXInit = (*itT).hits().size();
    PrHits::iterator itBeg = myStereo.begin();
    //first hit in U-V layer with small Ty
    PrHits::iterator itEnd = (itBeg + minUV<=myStereo.end()? itBeg+minUV: myStereo.end());
    // PrLineFitterY BestLine(m_zReference, (*itT));
    BestLine.setXProj( (*itT)); // Here i set the ax,bx,cx, dRatio
    if(msgLevel(MSG::DEBUG)) debug()<<"Size of MyStereo = " << myStereo.size()<<endmsg;
    for( ; itEnd-itBeg>=minUV; IncreaseIters( itBeg, itEnd, minUV, myStereo)){
      //while( itEnd-itBeg >= minUV){
    	if(msgLevel(MSG::DEBUG)) debug()<< "begining of the loop, itBeg: " << &*itBeg << "  itEnd: " << &*itEnd << "  myStereo.end(): " << &*myStereo.end() <<endmsg;
      double tolTy = m_tolTyOffset[iCase] + m_tolTySlope[iCase]*std::fabs( (*itBeg)->coord() );
      if( !CheckCluster( itBeg, itEnd, tolTy) ){
        if( msgLevel(MSG::DEBUG)) debug()<<"Increase Iters not in tolerance"<<endmsg;
        continue;
      }
      while( itEnd!= myStereo.end() &&
             (*(itEnd))->coord()-(*itBeg)->coord()<tolTy){
        if(msgLevel(MSG::DEBUG)) debug()<<"Extending itEnd"<<endmsg;
        ++itEnd; //extend last hit until you don't reach out of tolerance
      }
      if( msgLevel(MSG::DEBUG)) debug()<<"Increase end iter Done   "<<"ieBeg:  " << &*itBeg << "  itEnd: " << &*itEnd << "  myStereo.end(): "<<&*myStereo.end()<<endmsg;
      plCount.set( itBeg, itEnd);
      if( !plCount.isOKUV() || plCount.nbDifferentUV() < minUV ){
        if( msgLevel(MSG::DEBUG)) debug()<<"Increase Iters PlCounter"<<endmsg;
        continue;
      }
      PrHybridSeedTrack temp( *itT );
      //maybe generate it later on? It's just a copy of the xProjection i am processing: remember, everything here is done for a
      //single x-Projection (outer loop is on xProjection)
      if(m_useLineY){
        unsigned int nLay = plCount.nbDifferentUV();
        bool fit = false;
        if( plCount.nbSingleUV() == nLay){
          //if the case is nUVLayer = nHits in the cluster
          if(msgLevel(MSG::DEBUG)){
            debug()<<"Use LineY : single == nbDifferent"<<endmsg;
            debug()<<"MinUV = "<<minUV<<endmsg;
            debug()<<"Plane Counter nbDifferentUV = "<<plCount.nbDifferentUV();
            debug()<<"Plane Counter nbSingleUV = " <<plCount.nbSingleUV();
            debug()<<"UV Hits"<<endmsg;
            for(PrHits::iterator itH = itBeg; itEnd!=itH;++itH){
              printHit( (*itH));
            }
          }
          fit = BestLine.fit( itBeg,itEnd ); //fit for the line will set the chi2 for BestLine
          if(msgLevel(MSG::DEBUG)) debug()<<"Fit Status Line = "<<fit<<endmsg;
          if(fit &&
             LineOK( m_Chi2LowLine[iCase] ,m_Chi2HighLine[iCase],  BestLine , temp)){
            if(msgLevel(MSG::DEBUG)) debug()<<"adding hits on temp track"<<endmsg;
            for( PrHits::iterator hit = itBeg; itEnd!= hit; ++hit){
              temp.addHit( (*hit) );
            }
          }
        }
        if( nLay != plCount.nbSingleUV()){ //more than 1 hit per layer
          hits1.clear();
          hits2.clear();
          hits5.clear();
          hits6.clear();
          hits9.clear();
          hits10.clear();
          PlanesSingle.clear();
          PlanesMultiple.clear();
          Tracks.clear();
          for(PrHits::iterator itH = itBeg; itEnd!=itH;++itH){
            int planeCode = (*itH)->planeCode();
            if(plCount.nbInPlane( planeCode ) ==1 ){//if 1 hit in plane of processed hit push back the hit in the container of PlaneSingle
              PlanesSingle.push_back((*itH));
              continue;
            }else if(planeCode==1 && plCount.nbInPlane(planeCode)>1){
              hits1.push_back((*itH));
            }else if(planeCode==2 && plCount.nbInPlane(planeCode)>1){
              hits2.push_back((*itH));
            }else if(planeCode==5 && plCount.nbInPlane(planeCode)>1){
              hits5.push_back((*itH));
            }else if(planeCode==6 && plCount.nbInPlane(planeCode)>1){
              hits6.push_back((*itH));
            }else if(planeCode==9 && plCount.nbInPlane(planeCode)>1){
              hits9.push_back((*itH));
            }else if(planeCode==10 && plCount.nbInPlane(planeCode)>1){
              hits10.push_back((*itH));
            }
          }
          if(hits1.size()!=0) PlanesMultiple.push_back(hits1);
          if(hits2.size()!=0) PlanesMultiple.push_back(hits2);
          if(hits5.size()!=0) PlanesMultiple.push_back(hits5);
          if(hits6.size()!=0) PlanesMultiple.push_back(hits6);
          if(hits9.size()!=0) PlanesMultiple.push_back(hits9);
          if(hits10.size()!=0) PlanesMultiple.push_back(hits10);
          //PrHybridSeedTracks Tracks; //combinatorical tracks container when >1 hit in at least 1 layer.
          //PrHit* hit1 = nullptr;
          for( PrHit* hit1: PlanesMultiple[0]){
            if(PlanesMultiple.size()==1){
              PrHybridSeedTrack tem(*itT);
              if(PlanesSingle.size()!=0){
                tem.addHits(PlanesSingle);
              }
              tem.addHit(hit1);
              Tracks.push_back(tem);
              continue;
            }
            if(PlanesMultiple.size()>1){
              for( PrHit* hit2 : PlanesMultiple[1]){
                if(PlanesMultiple.size() ==2){
                  PrHybridSeedTrack tem(*itT);
                  if(PlanesSingle.size()!=0){
                    tem.addHits(PlanesSingle);
                  }
                  tem.addHit(hit1);
                  tem.addHit(hit2);
                  Tracks.push_back(tem);
                  continue;
                }
                if(PlanesMultiple.size()>2){
                  for(PrHit* hit3 : PlanesMultiple[2]){
                    if(PlanesMultiple.size() == 3){
                      PrHybridSeedTrack tem(*itT);
                      if(PlanesSingle.size()!=0){
                        tem.addHits(PlanesSingle);
                      }
                      tem.addHit( hit1);
                      tem.addHit( hit2);
                      tem.addHit( hit3);
                      Tracks.push_back(tem);
                      continue;
                    }
                    if(PlanesMultiple.size()>3){
                      for(PrHit* hit4 : PlanesMultiple[3]){
                        if(PlanesMultiple.size() == 4){
                          PrHybridSeedTrack tem(*itT);
                          if(PlanesSingle.size()!=0){
                            tem.addHits(PlanesSingle);
                          }
                          tem.addHit(hit1);
                          tem.addHit(hit2);
                          tem.addHit(hit3);
                          tem.addHit(hit4);
                          Tracks.push_back(tem);
                          continue;
                        }
                        if(PlanesMultiple.size()>4){
                          for(PrHit* hit5 : PlanesMultiple[4]){
                            if(PlanesMultiple.size() == 5){
                              PrHybridSeedTrack tem(*itT);
                              if(PlanesSingle.size()!=0){
                                tem.addHits(PlanesSingle);
                              }
                              tem.addHit(hit1);
                              tem.addHit(hit2);
                              tem.addHit(hit3);
                              tem.addHit(hit4);
                              tem.addHit(hit5);
                              Tracks.push_back(tem);
                              continue;
                            }
                            if(PlanesMultiple.size()>5){
                              for(PrHit* hit6: PlanesMultiple[5])
                              {
                                if(PlanesMultiple.size()==6)
                                {
                                  PrHybridSeedTrack tem(*itT);
                                  if(PlanesSingle.size()!=0){
                                    tem.addHits(PlanesSingle);
                                  }
                                  tem.addHit(hit1);
                                  tem.addHit(hit2);
                                  tem.addHit(hit3);
                                  tem.addHit(hit4);
                                  tem.addHit(hit5);
                                  tem.addHit(hit6);
                                  Tracks.push_back(tem);
                                }//end if planes size 6
                              }//end loop hit in multiple 6
                            }//end if plane size >5
                          }//end loop hit in multiple 5
                        }//1
                      }//2
                    }//3
                  }//4
                }//5
              }//6
            }//7
          }//8
          if(msgLevel(MSG::DEBUG)){
            always()<<"UVSegments candidates"<<Tracks.size()<<endmsg;
            always()<<"XZ segment start"<<endmsg; printTrack( (*itT));
          }
          // Tracks contains the Tracks with 1 hit per layer
          bool fit = false;
          PrHybridSeedTracks::iterator best = Tracks.end();
          double bestchi2 = 1.e4;
          double chi= 0;
          for(PrHybridSeedTracks::iterator tr = Tracks.begin(); Tracks.end()!=tr; ++tr){ //for each combinatorics (1 hit in 1 layer), choose the best one based on the LineChi2DoF + XZProjectionChi2DoF
            fit = BestLine.fit( (*tr).hits().begin(), (*tr).hits().end() );
            if(fit &&
               LineOK(m_Chi2LowLine[iCase], m_Chi2HighLine[iCase],BestLine,temp) ) {//it will be set only if the line is OK
              chi = BestLine.Chi2DoF() + temp.chi2PerDoF();
              if( chi < bestchi2){
                bestchi2 = chi;
                best = tr;
              }
            }
          }
          if( bestchi2 < m_Chi2HighLine[iCase] ){ //you found one
            for( PrHits::iterator it = (*best).hits().begin(); (*best).hits().end()!= it; ++it){
              if( (*it)->isX()) continue;
              temp.addHit( (*it));
            }
          }
        }
        if(msgLevel(MSG::DEBUG)){
          always()<<"temp track with the best line added"<<endmsg; printTrack(temp);
        }
        if(msgLevel(MSG::DEBUG)){
          always()<<"Will Fit the following track with N Hits = "<<temp.hits().size()<<endmsg;
          printTrack(temp);
        }
      }//end Use Liney
      if(!m_useLineY){
        for(PrHits::iterator itH = itBeg; itEnd!= itH;++itH){
          temp.addHit((*itH));
        }
      }
      if( temp.hits().size() < minTot){
        if( msgLevel(MSG::DEBUG)) debug()<<"Increase Iters Line not OK"<<endmsg;
        //IncreaseIters( itBeg, itEnd, minUV, myStereo);
        continue;
      }
      bool ok = false;
      temp.setnXnY( (*itT).hits().size(), plCount.nbDifferentUV());
      ok = fitSimultaneouslyXY(temp,iCase);
      if(temp.hits().size()>m_maxNHits){
        ok = false; //Max N hits is 13 here
      }
      while( !ok && temp.hits().size() > minTot){
        if( msgLevel(MSG::DEBUG) ) always()<<"RemoveWorst and Refit UV"<<endmsg;
        ok = removeWorstAndRefit( temp   , iCase);
        if(temp.hits().size()>m_maxNHits) ok = false;
      }
      if( ok  && temp.hits().size()>=minTot &&
          temp.chi2PerDoF() < m_maxChi2PerDoF[iCase] ){
        setChi2(temp);
        // double maxChi2 = m_maxChi2PerDoF[iCase];
        // if( temp.hits().size() >= minTot &&
        //     temp.chi2PerDoF() < maxChi2 ){
        temp.setCase(iCase);
        //plCount.set( temp.hits().begin(), temp.hits().end());
        //if(plCount.isOK()){
        if(m_removeClones) std::sort(temp.hits().begin(), temp.hits().end(), compLHCbID());
        if(msgLevel(MSG::DEBUG)) debug()<<"Storing Track"<<endmsg;
        m_trackCandidates[(int)part].push_back( temp );
        itBeg += minUV-1 ;//-1 because later ++ in IncreaseIters
        if( msgLevel(MSG::DEBUG)) debug()<<"Increase Iters Track good"<<endmsg;
      }
    }
    //=== Remove bad candidates: Keep the best one for this input track
    // FirstSpace is the number of track candidates before the finding of the UV Hits
    if( msgLevel(MSG::DEBUG)) always()<<"track Candidates Size"<<m_trackCandidates[(int)part].size()<<"firstSpace"<<firstSpace<<endmsg;
    if( m_trackCandidates[(int)part].size() > firstSpace+1){
      for( unsigned int kk = firstSpace; m_trackCandidates[(int)part].size() > kk -1 ; ++kk ){
        //maybe just use where you find more UV? it's automatic because you start from one X-Z candidate
        if(!m_trackCandidates[(int)part][kk].valid()) continue;
        for( unsigned int ll = kk + 1; m_trackCandidates[(int)part].size() > ll; ++ll ){
          if( !m_trackCandidates[(int)part][ll].valid())continue;
          if( m_trackCandidates[(int)part][ll].hits().size() < m_trackCandidates[(int)part][kk].hits().size()){
            m_trackCandidates[(int)part][ll].setValid( false );
          }else if ( m_trackCandidates[(int)part][ll].hits().size() > m_trackCandidates[(int)part][kk].hits().size()){
            m_trackCandidates[(int)part][kk].setValid( false );
            ////equal size, take the one with the better chi2
          }else if( m_trackCandidates[(int)part][kk].chi2() < m_trackCandidates[(int)part][ll].chi2() ){
            m_trackCandidates[(int)part][ll].setValid( false );
          }else{
            m_trackCandidates[(int)part][kk].setValid( false );
          }
        }
      }//end loop track Candidates for removal bad tracks
    }//loop candidate removal
  }//end loop xProjections
  m_xCandidates[(int)part].clear(); //At the end of each case UV Step
}
bool PrHybridSeeding::CheckCluster( PrHits::iterator& itBeg, PrHits::iterator& itEnd, double tol){
  //double deltaCoord = (*(itEnd))->coord() - (*itBeg)->coord() ;
  return ( (*(itEnd-1))->coord() - (*itBeg)->coord() < tol);
}

void PrHybridSeeding::IncreaseIters( PrHits::iterator& itBeg, PrHits::iterator& itEnd, unsigned int minUV , PrHits& myStereo){
  ++itBeg;
  itEnd = (itBeg + minUV <=myStereo.end()? itBeg+minUV : myStereo.end());
}


bool PrHybridSeeding::LineOK( double minChi2Low, double minChi2High, PrLineFitterY& line, PrHybridSeedTrack& xProje ){
  const int nHits = line.nHitsLine() + xProje.hits().size();
  const double Chi2DoFLineXProj = xProje.chi2PerDoF() + line.Chi2DoF();

  if( nHits > 10 && Chi2DoFLineXProj < minChi2High) return true;
  if( nHits < 11 && Chi2DoFLineXProj < minChi2Low ) return true;
  return false;
}

void PrHybridSeeding::removeClonesX(unsigned int part,unsigned int icase, bool xOnly){
  std::sort(m_xCandidates[(int)part].begin(), m_xCandidates[(int)part].end(),PrHybridSeedTrack::GreaterBySize());
  unsigned int maxCommon = 0;

  for( PrHybridSeedTracks::iterator itT1 = m_xCandidates[(int)part].begin(); m_xCandidates[(int)part].end() !=itT1; ++itT1){
    // if((*itT1).zone() != part)             continue;
    if((*itT1).Case() != icase && !xOnly)  continue; //useless if you clean the m_xCandidates
    if(!(*itT1).valid())                   continue;
    for( PrHybridSeedTracks::iterator itT2 = itT1 + 1; m_xCandidates[(int)part].end() !=itT2; ++itT2 ){
      if(!m_removeClonesX && xOnly)        break;
      if((*itT1).type()%2 != (*itT2).type()%2 )continue;
      if(!(*itT2).valid())                continue;
      if((*itT2).Case() != (*itT1).Case()) continue;
      int Compare = (*itT1).hits().size()+(*itT2).hits().size();
      switch(Compare){
      case 12: //6 vs 6
        maxCommon = 3;
        break;
      case 11: //6 vs 5
        maxCommon = 3;
        break;
      case 10: //6 vs 4 & 5+5
        maxCommon = 2;
        break;
      case 9: //5 vs 4
        maxCommon = 1;
        break;
      case 8: /// 4 vs 4
        maxCommon = 1;
        break;
      }
      unsigned int nCommon = 0;
      //if( (*itT1).Case() == 0) maxCommon =1;
      PrHits::iterator itH1 = (*itT1).hits().begin();
      PrHits::iterator itH2 = (*itT2).hits().begin();
      PrHits::iterator itEnd1 = (*itT1).hits().end();
      PrHits::iterator itEnd2 = (*itT2).hits().end();
      //count number of common hits between track 1 and track 2
      while( itH1 != itEnd1 && itH2 != itEnd2 ){
        if((*itH1)->id() == (*itH2)->id()){
          ++nCommon;
          ++itH1;
          ++itH2;
        }
        else if( (*itH1)->id() < (*itH2)->id() ){
          // while( itH1!=itEnd1 && (*itH1)->id() < (*itH2)->id() )
          ++itH1;
        }
        else{
          // while( itH2!=itEnd2 && (*itH2)->id() < (*itH1)->id() )
          ++itH2;
        }
        if( nCommon >=maxCommon )  break;
      }
      if(nCommon >= maxCommon){
        //you need all the if statement if you change the way you sort the track container
        (*itT1).setValid(false);
        break;
      }
      //if((*itT1).hits().size() > (*itT2).hits().size()){
      //   (*itT2).setValid( false );
      // }else if((*itT1).hits().size() < (*itT2).hits().size()){
      //   (*itT1).setValid( false );
      //   break;
      // }else if((*itT1).chi2PerDoF() < (*itT2).chi2PerDoF()){
      //   (*itT2).setValid( false );
      // }else{
      //   (*itT1).setValid( false );
      //   break;
      // }
    }
    // if( !(*itT1).valid() ) break; //has beed invalidated
    // if(m_xOnly && (*itT1).valid() && (*itT1).zone() == part && icase==(m_nCases-1)){
    //   m_trackCandidates.push_back(*itT1);
    // }
  }
  // if(m_xOnly && icase == m_nCases-1 && part ==1){
  //   for( PrHybridSeedTracks::iterator itT1 = m_xCandidates[(int)part].begin();m_xCandidates[(int)part].end()!= itT1;++itT1){
  //     if( (*itT1).valid()){
  //       m_trackCandidates[part].push_back( (*itT1) );
  //     }
  //   }
  // }
}

void PrHybridSeeding::removeClones(unsigned int maxCommon, unsigned int part){
  //std::sort(m_trackCandidates.begin(),m_trackCandidates.end(),PrHybridSeedTrack::LowerBySize());
  std::sort(m_trackCandidates[(int)part].begin(), m_trackCandidates[(int)part].end(),PrHybridSeedTrack::GreaterBySize());
  for ( PrHybridSeedTracks::iterator itT1 = m_trackCandidates[(int)part].begin(); m_trackCandidates[(int)part].end()!=itT1; ++itT1){
    if( !(*itT1).valid()) continue;
    for ( PrHybridSeedTracks::iterator itT2 = itT1 + 1; m_trackCandidates[(int)part].end() !=itT2; ++itT2 ) {
      if ( !(*itT2).valid()) continue;
      unsigned int nCommon = 0;
      PrHits::iterator itH1 = (*itT1).hits().begin();
      PrHits::iterator itH2 = (*itT2).hits().begin();
      PrHits::iterator itEnd1 = (*itT1).hits().end();
      PrHits::iterator itEnd2 = (*itT2).hits().end();
      while( itH1 != itEnd1 && itH2 != itEnd2 ){
        if ( (*itH1)->id() == (*itH2)->id() ){
          ++nCommon;
          ++itH1;
          ++itH2;
        }
        else if( (*itH1)->id() < (*itH2)->id() ){
          ++itH1;
        }
        else{
          ++itH2;
        }
      }
      unsigned int maxCommonUV = maxCommon;
      if( nCommon>=maxCommonUV ){
        //Due to sorting of tracks from lower quality to higher one
        (*itT1).setValid(false);
        break;
      }

      //   if((*itT1).hits().size() > (*itT2).hits().size()){
      //     (*itT2).setValid( false);
      //   }
      //   if( (*itT1).hits().size() < (*itT2).hits().size())
      //   {
      //     (*itT1).setValid( false);
      //   }
      //   else if((*itT1).chi2PerDoF() < (*itT2).chi2PerDoF()){
      //     (*itT2).setValid(false);
      //   }else
      //   {
      //     (*itT1).setValid(false);
      //   }
      // }
      // if(! (*itT1).valid()) break;
      //    }
    }
  }
}


void PrHybridSeeding::flagHits(unsigned int icase, unsigned int part){
  std::sort( m_trackCandidates[(int)part].begin() , m_trackCandidates[(int)part].end() , PrHybridSeedTrack::LowerBySize()); //bigger size is in front
  for(PrHybridSeedTracks::iterator track = m_trackCandidates[(int)part].begin(); m_trackCandidates[(int)part].end()!=track ; ++track){
    //if( (*track).hits().size() < m_SizeFlag[icase] ) break; // Important the sorting of before for this break
    // if( (*track).Case() != icase || (*track).zone()!=part ) continue;
    if( (*track).Case() != icase ) continue;
    if( !(*track).valid()) continue;
    // PrHybridSeedTrack temp(*track);
    if((*track).hits().size() < m_SizeFlag[icase])break;
    // Important the sorting of before for this break
    if(!(    ((*track).hits().size()==11
              && (*track).chi2PerDoF()< m_MaxChi2Flag[icase]
              && std::fabs((*track).X0())<m_MaxX0Flag[icase])
             || ((*track).hits().size()==12 )
             ) ) continue;
    for(PrHits::iterator it = (*track).hits().begin();(*track).hits().end()!=it; ++it){
      //if( (*it)->zone()%2 != part ) continue; Do you want to flag the hits if they belong to the opposite side of the detector?
      (*it)->setUsed(true);
    }
  }
}
//=========================================================================
//  Convert to LHCb tracks
//=========================================================================
void PrHybridSeeding::makeLHCbTracks( LHCb::Tracks* result , unsigned int part ) {
  for( PrHybridSeedTracks::iterator itT = m_trackCandidates[(int)part].begin();m_trackCandidates[(int)part].end() != itT; ++itT ){
    if ( !(*itT).valid() ) continue;
    if ( msgLevel(MSG::DEBUG) ) debug()<<"Creating LHCb Track"<<endmsg;
    LHCb::Track* tmp = new LHCb::Track;
    tmp->setType( LHCb::Track::Ttrack );
    tmp->setHistory( LHCb::Track::PrSeeding );
    double qOverP = m_geoTool->qOverP( *itT );
    LHCb::State tState;
    double z = StateParameters::ZEndT;
    tState.setLocation( LHCb::State::AtT );
    tState.setState( (*itT).x( z ), (*itT).y( z ), z, (*itT).xSlope( z ), (*itT).ySlope( ), qOverP );
    //== overestimated covariance matrix, as input to the Kalman fit
    tState.setCovariance( m_geoTool->covariance( qOverP ) );
    tmp->addToStates( tState );
    //== LHCb ids
    // Overloading things
    tmp->setPatRecStatus( LHCb::Track::PatRecIDs );
    for ( PrHits::iterator itH = (*itT).hits().begin(); (*itT).hits().end() != itH; ++itH ) {
      tmp->addToLhcbIDs( (*itH)->id() );
    }
    tmp->setChi2PerDoF( (*itT).chi2PerDoF() );
    tmp->setNDoF( (*itT).nDoF() );
    if(msgLevel(MSG::DEBUG)) debug()<<"Inserting a track in container"<<endmsg;
    result->insert( tmp );
  }
}

void PrHybridSeeding::solveParabola(const PrHit* hit1, const PrHit* hit2, const PrHit* hit3, double& a, double& b, double& c){
  const double x3 = hit3->x();

  //New Parabola Method (be adapted to new parametrisation)
  const double z1_PB = hit1->z() - hit3->z();
  const double z2_PB = hit2->z() - hit3->z();
  const double x1_PB = hit1->x() - hit3->x();
  const double x2_PB = hit2->x() - hit3->x();

  const double det_PB = z1_PB*z2_PB*(z1_PB-z2_PB);

  if( std::fabs(det_PB) < 1e-8 ){
    a = 0.0;
    b = 0.0;
    c = 0.0;
    return;

  }

  a=0; b=0;c=0;
  a = (z2_PB*x1_PB-z1_PB*x2_PB)/det_PB;
  b = (-z2_PB*z2_PB*x1_PB+z1_PB*z1_PB*x2_PB)/det_PB;
  const double z3_PB = hit3->z() - m_geoTool->zReference();
  c = x3 + a * z3_PB * z3_PB - b *z3_PB;
  b -= 2 * a * z3_PB;
}

void PrHybridSeeding::solveParabola2(const PrHit* hit1,const PrHit* hit2,const PrHit* hit3,double& a1, double& b1,double& c1){
  const double z1 = hit1->z() - m_zReference;//->zReference();
  const double z2 = hit2->z() - m_zReference;//m_geoTool->zReference();
  const double z3 = hit3->z() - m_zReference;//m_geoTool->zReference();
  const double x1 = hit1->x();
  const double x2 = hit2->x();
  const double x3 = hit3->x();
  //const double e = m_dRatio;
  const double corrZ1 = 1.+m_dRatio*z1;
  const double corrZ2 = 1.+m_dRatio*z2;
  const double corrZ3 = 1.+m_dRatio*z3;
  const double det = (z1*z1)*corrZ1*z2 + z1*(z3*z3)*corrZ3 + (z2*z2)*corrZ2*z3 - z2*(z3*z3)*corrZ3 - z1*(z2*z2)*corrZ2 - z3*(z1*z1)*corrZ1;
  const double recdet = 1./det;
  if( std::fabs(det) < 1e-8 )
  {
    a1 = 0.0;
    b1 = 0.0;
    c1 = 0.0;
    return;
  }
  const double det1 = (x1)*z2 + z1*(x3) + (x2)*z3 - z2*(x3) - z1*(x2) - z3*(x1);
  const double det2 = (z1*z1)*corrZ1*x2 + x1*(z3*z3)*corrZ3 + (z2*z2)*corrZ2*x3 - x2*(z3*z3)*corrZ3 - x1*(z2*z2)*corrZ2 - x3*(z1*z1)*corrZ1;
  const double det3 = (z1*z1)*corrZ1*z2*x3 + z1*(z3*z3)*corrZ3*x2 + (z2*z2)*corrZ2*z3*x1 - z2*(z3*z3)*corrZ3*x1 - z1*(z2*z2)*corrZ2*x3 - z3*(z1*z1)*corrZ1*x2;
  // a1 = det1/det;
  // b1 = det2/det;
  // c1 = det3/det;
  a1 = recdet*det1;
  b1 = recdet*det2;
  c1 = recdet*det3;

}
//=========================================================================
//  Fit the track, return OK if fit sucecssfull
//=========================================================================
bool PrHybridSeeding::fitSimultaneouslyXY( PrHybridSeedTrack& track , unsigned int iCase){
  if( track.nx() <4 || track.ny() <4 ) return false;
  double mat[15];
  double rhs[5];
  // unsigned int nHitsX = 0;
  // unsigned int nHitsStereo = 0;
  const double zRef = m_zReference;// m_geoTool->zReference();
  for ( int loop = 0; 3 > loop ; ++loop ){
    if(loop ==1 && m_useCubic && m_useCorrPos){
      double RadiusPosition = std::sqrt((0.0005*0.0005*track.ax()*track.ax() + 0.001*0.001*track.y(zRef)*track.y(zRef)));
      double dRatioPos = -1.*(2.633e-4 - 3.59957e-6*RadiusPosition + 4.7312e-5*RadiusPosition*RadiusPosition);
      track.setdRatio(dRatioPos);
    }

    std::fill(mat,mat+15,0.);
    std::fill(rhs,rhs+5,0.);
    for( PrHits::iterator itH = track.hits().begin(); track.hits().end() != itH; ++itH ){
      double w = (*itH)->w();
      if( m_SlopeCorr && loop >0){
        const double Corr = std::cos(track.xSlope( (*itH)->z()) );
        track.setSlopeCorr( true);
        w = w*(Corr*Corr);
      }
      const double dxdy = (*itH)->dxDy();
      const double yOnTrack = track.yOnTrack( (*itH) ) ;
      const double   dz = 0.001*((*itH)->z( yOnTrack ) - zRef);
      const double dRatio = track.dRatio();
      const double deta = dz*dz*(1. + dz*dRatio);
      const double wdz = w * dz;
      const double weta = w * deta;
      const double wdxdy = w * dxdy;
      const double wdxdydz = wdxdy * dz;
      const double dist = track.distance(*itH);
      mat[0] += w;
      mat[1] += wdz;  mat[2] += wdz * dz;
      mat[3] += weta; mat[4] += weta * dz; mat[5] += weta * deta;
      mat[6] -= wdxdy;mat[7] -= wdxdydz;   mat[8] -= wdxdy * deta;  mat[9] += wdxdy * dxdy;
      mat[10] -= wdxdydz; mat[11] -= wdxdydz * dz;mat[12] -= wdxdydz * deta;mat[13] += wdxdydz * dxdy; mat[14] += wdxdydz * dz * dxdy;

      // fill right hand side
      rhs[0] += w * dist;
      rhs[1] += wdz * dist;
      rhs[2] += weta * dist;
      rhs[3] -= wdxdy * dist;
      rhs[4] -= wdxdydz * dist;
    }//Loop over Hits to fill the matrix
    // decompose matrix, protect against numerical trouble
    //track.setnXnY( nHitsX, nHitsStereo );
    //if(nHitsX < 4 || nHitsStereo < 4) return false;
    ROOT::Math::CholeskyDecomp<double, 5> decomp(mat);
    if (!decomp) return false;
    decomp.Solve(rhs);
    rhs[1]*=1.e-3;
    rhs[2]*=1.e-6;
    rhs[4]*=1.e-3;
    rhs[3]-=rhs[4]*zRef;
    if( loop >0 &&(std::fabs(rhs[0]) > 1e4 || std::fabs(rhs[1]) > 5. ||
                   std::fabs(rhs[2]) > 1e-3 || std::fabs(rhs[3]) > 1e4 || std::fabs(rhs[4]) > 1.)) return false;
    track.updateParameters(rhs[0],rhs[1],rhs[2],rhs[3],rhs[4]);
  }
  double maxChi2 =0.;
  for( PrHits::iterator itH = track.hits().begin(); track.hits().end() != itH; ++itH ){
    double chi2_onHit = track.chi2( *itH);
    if ( chi2_onHit > maxChi2 ){
      maxChi2 = chi2_onHit;
    }
  }//Set Max Chi2DoF
  double X0 = track.ax() - track.bx()*m_zReference+track.cx()*m_ConstC;
  track.setX0(X0);
  track.setMaxChi2(maxChi2);
  if( (track.hits().size()>10) && maxChi2<m_maxChi2HitFullFitHigh[iCase]) return true;
  if( std::fabs(track.y(0.))< m_maxY0Low[iCase] &&
      maxChi2<m_maxChi2HitFullFitLow[iCase] &&
      track.hits().size()<11 &&
      std::fabs(track.y(zRef)) < m_maxYZrefLow[iCase] ) return true;
  return false;
}
//=======================================
//Fit Only X Projection
//=======================================

bool PrHybridSeeding::fitXProjection(PrHybridSeedTrack& track, unsigned int iCase ){
  if (msgLevel(MSG::DEBUG)) debug()<<"Fitting"<<endmsg;
  if(track.hits().size()<m_minXPlanes) return false;
  double mat[6];
  double rhs[3];
  const double zRef = m_zReference;
  //track.setdRatio(m_dRatio);
  for(int loop = 0;3>loop;++loop)
  {
    std::fill(mat,mat+6,0.);
    std::fill(rhs,rhs+3,0.);
    for( PrHits::iterator itH = track.hits().begin(); track.hits().end() != itH; ++itH ){
      const double dRatio = track.dRatio();
      double w = (*itH)->w();//squared
      if(m_SlopeCorr){
        track.setSlopeCorr( true);
        double Corr = std::cos( track.xSlope( (*itH)->z()));
        if(loop ==0) Corr = 1.;
        w = w*(Corr*Corr);
      }
      const double dz= 0.001*((*itH)->z() - zRef);
      const double deta = dz*dz*(1. + dRatio*dz);
      const double dist = track.distance( *itH );
      if (msgLevel(MSG::DEBUG)) debug()<<"Loop \t"<<loop<<"\n Distance From Hit \t"<<dist<<endmsg;
      mat[0]+= w;
      mat[1]+= w * dz;   mat[2]+= w * dz * dz;
      mat[3]+= w * deta; mat[4]+= w * dz * deta;  mat[5]+= w * deta * deta;
      rhs[0]+= w * dist;
      rhs[1]+= w * dist * dz;
      rhs[2]+= w * dist * deta;
    }
    ROOT::Math::CholeskyDecomp<double,3> decomp(mat);
    if(!decomp){
      return false;
    }
    //Solve linear system
    decomp.Solve(rhs);
    rhs[1]*=1.e-3;
    rhs[2]*=1.e-6;
    if (msgLevel(MSG::DEBUG)) debug()<<"Loop \t"<<loop<<"\n a = \t"<<rhs[0]<<"\n b = \t"<<rhs[1]<<"\n c = \t"<<rhs[2]<<endmsg;
    // protect against unreasonable track parameter corrections
    // (check that out)
    if(std::fabs(rhs[0]) > 1.e4 || std::fabs(rhs[1]) > 5. ||
       std::fabs(rhs[2]) > 1.e-3 ) return false;
    //Small corrections
    track.updateParameters(rhs[0],rhs[1],rhs[2],0.,0.);
    if(loop==0 && m_useCubic){
      track.setdRatio(m_dRatio);
    }
    //Put back later faster maybe
    if(loop >0 && std::abs(rhs[0]) < 5e-5 && std::abs(rhs[1]) < 5e-8 &&
       std::abs(rhs[2]) < 5e-11){
      break;
    }
  }
  //Compute some values on the track
  double maxChi2 = 0.;
  for ( PrHits::iterator itH = track.hits().begin(); track.hits().end() != itH; ++itH )  //Loop over all hits in PrHybridSeedTrack
  {
    double chi2_onHit = track.chi2( *itH );
    if ( chi2_onHit > maxChi2 ){
      maxChi2 = chi2_onHit;
    }
  }
  // double X0 = track.ax() - track.bx()*m_geoTool->zReference()+track.cx()*m_ConstC;
  double X0 = track.ax() - track.bx()*m_zReference+track.cx()*m_ConstC;

  track.setX0(X0);
  track.setMaxChi2(maxChi2);

  return (maxChi2 < m_maxChi2HitsX[iCase]) ;
  return false;
}



bool PrHybridSeeding::removeWorstAndRefit(PrHybridSeedTrack& track, unsigned int iCase){
  //maybe useless?
  // bool removeX = false;
  // bool removeUV = false;
  // if( track.nx() == 4) removeUV = true;
  // if( track.ny() == 4) removeX = true;
  if(msgLevel(MSG::DEBUG)) debug()<<"Removing Worst UV layer"<<endmsg;
  PrHits::iterator worst= track.hits().begin();
  // PrHits::iterator worstUV = track.hits().begin();
  // PrHits::iterator worstDupl = track.hits().begin();
  double maxChi2 = -1.;
  if( track.nx() <4 || track.ny()<4) return false;
  for( PrHits::iterator itH = track.hits().begin(); track.hits().end() != itH; ++itH ){
    // if( removeUV && (*itH)->isX() ) continue;
    // if( removeX && !(*itH)->isX() ) continue;
    if( track.chi2(*itH) > maxChi2){
      maxChi2 = track.chi2( *itH );
      worst = itH;
    }
  }
  track.hits().erase(worst);
  if( (*worst)->isX()){
    track.setnXnY( track.nx()-1, track.ny());
  }else{
    track.setnXnY( track.nx(), track.ny()-1);
  }
  return fitSimultaneouslyXY(track, iCase);
  return false;
}

//=========================================================================
//  Remove the worst hit and refit.
//=========================================================================
bool PrHybridSeeding::removeWorstAndRefitX ( PrHybridSeedTrack& track , unsigned int iCase)
{
  if(track.hits().size()<=m_minXPlanes) return false;
  if (msgLevel(MSG::DEBUG)) debug()<<"Removing Worst and Refitting"<<endmsg;
  //Find maxChi2 contribution of an Hit
  double maxChi2 = 0.;
  PrHits::iterator worst = track.hits().begin();
  for ( PrHits::iterator itH = track.hits().begin(); track.hits().end() != itH; ++itH ){
    double chi2 = track.chi2( *itH );
    if( chi2 > maxChi2 ){
      maxChi2 = chi2;
      worst = itH;
    }
  }
  track.hits().erase(worst);
  bool OK = fitXProjection(track, iCase);
  return OK;
}
void PrHybridSeeding::setChi2 ( PrHybridSeedTrack& track ){
  float chi2 = 0.;
  int   nDoF = -3;
  // Fitted a parabola
  bool hasStereo = false;
  //for ( PrHits::iterator itH = track.hits().begin(); track.hits().end() != itH; ++itH )
  for (PrHit* hit : track.hits()){
    //const double d = track.distance( hit );
    if ( hit->dxDy() != 0 ) hasStereo = true;
    //hasStereo = hasStereo || (hit->dxDy() != 0);
    //const double w = hit->w();
    //chi2 += w * d * d;
    chi2 +=  track.chi2( hit);
    nDoF += 1;
  }
  if (hasStereo) {
    nDoF -= 2;
  }
  track.setChi2( chi2, nDoF );
}



//=========================================================================
//  Set the chi2 of the track
//=========================================================================
void PrHybridSeeding::setChi2X ( PrHybridSeedTrack& track ) {
  double chi2 = 0.;
  int   nDoF = -3;  // Fitted a parabola
  //bool hasStereo = false;
  //for ( PrHits::iterator itH = track.hits().begin(); track.hits().end() != itH; ++itH ) {
  for (PrHit* hit : track.hits()) {
    if(hit->dxDy() !=0 && msgLevel(MSG::DEBUG)) debug()<<"You were picking up Stereo Layers!!!"<<endmsg;
    const double Chi2Hit = track.chi2( hit);
    // const double d = track.distance( hit );
    // const double w = hit->w();
    //chi2 += w * d * d;
    chi2+=  Chi2Hit;
    nDoF += 1;
  }
  if (msgLevel(MSG::DEBUG)) debug()<<"Chi2 Set for track = \t"<<chi2<<endmsg;
  track.setChi2( chi2, nDoF );
}


//===============================================
//  Print the whole track
//=========================================================================
void PrHybridSeeding::printTrack ( PrHybridSeedTrack& track ) {
  for ( PrHits::iterator itH = track.hits().begin(); track.hits().end() != itH; ++itH ) {
    info() << format( "dist %7.3f dy %7.2f chi2 %7.2f ", track.distance( *itH ), track.deltaY( *itH ), track.chi2( *itH ) );
    printHit( *itH );
  }
}

//=========================================================================
//  Print the information of the selected hit
//=========================================================================
void PrHybridSeeding::printHit ( const PrHit* hit, std::string title ) {
  info() << "  " << title << " "
         << format( " Plane%3d zone%2d z0 %8.2f x0 %8.2f  size%2d charge%3d coord %8.3f used%2d ",
                    hit->planeCode(), hit->zone(), hit->z(), hit->x(),
                    hit->size(), hit->charge(), hit->coord(), hit->isUsed() );
  //if ( m_debugTool ) m_debugTool->printKey( info(), hit->id() );
  //if ( matchKey( hit ) ) info() << " ***";
  info() << endmsg;
}


void PrHybridSeeding::findXProjections(unsigned int part, unsigned int iCase)
{
  //m_xCandidates.clear();
  PrHits parabolaSeedHits;
  std::vector<PrHits> xHitsLists; //vector of list of Hits
  PrHits xHits;
  //just do the 1st one here //1st layer and last one
  unsigned int firstZoneId = -1;
  unsigned int lastZoneId = -1;
  if(0 == iCase){
    firstZoneId = s_T1X1 | part;
    lastZoneId  = s_T3X2 | part;
  }else if ( 1 == iCase ){
    firstZoneId = s_T1X2 | part;
    lastZoneId  = s_T3X1 | part;
  }else if ( 2 == iCase ){
    firstZoneId = s_T1X1 | part;
    lastZoneId  = s_T3X1 | part;
  }else if ( 3 == iCase ){
    firstZoneId = s_T1X2 | part;
    lastZoneId  = s_T3X2 | part;
  }
  if (msgLevel(MSG::DEBUG)) debug()<<"\t Loading Case Hit in first and last Zone"<<endmsg;
  PrHitZone* fZone = m_zones[firstZoneId];
  PrHitZone* lZone = m_zones[lastZoneId];
  if (msgLevel(MSG::DEBUG)) debug()<<"Hits in last and first Zone Loaded"<<endmsg;
  //load hits in first zone and last one
  PrHits& fHits = fZone->hits();
  PrHits& lHits = lZone->hits();
  double zFirst     = fZone->z(0.); //First  Zone Z value
  double zLast      = lZone->z(0.); //First  Zone Z value
  double DeltaZ = zLast-zFirst;     //Delta First-Last
  std::vector<PrHitZone*> xZones;
  xZones.reserve(4);
  for (unsigned int xZoneId : {s_T1X1, s_T1X2, s_T2X1, s_T2X2, s_T3X1, s_T3X2}){
    xZoneId |= part;
    if (xZoneId != firstZoneId && xZoneId != lastZoneId){
      //should i have to remove the layers already used?
      xZones.push_back(m_zones[xZoneId]);
    }
  }
  //PrHits::iterator itLBeg = lHits.begin();
  if (msgLevel(MSG::DEBUG)) debug()<<"Hits in the InBetween Zones Loaded"<<endmsg;
  if (msgLevel(MSG::DEBUG)) debug()<<"Will Loop over Hits in first Zone"<<endmsg;
  //for(PrHits::iterator itF = fHits.begin(); fHits.end() != itF; ++itF){
  for (PrHit* fHit : fHits){
    if( 0 != iCase && fHit->isUsed() && m_removeFlagged) continue;
    //define search window as a function of the x in the first layer
    double tx_inf =  fHit->x()/zFirst;
    double xProjeInf = tx_inf*zLast;
    double tolHp = m_TolFirstLast[iCase];
    //From Branch ImproveIt we change the alphaCOrrection : case 1 : 1988.63; case 2 : 2354.0
    //For all cases except case = 0
    double maxXl = xProjeInf + tx_inf*m_alphaCorrection[iCase]  +tolHp;
    double minXl = xProjeInf + tx_inf*m_alphaCorrection[iCase]  -tolHp;;
    if(maxXl < minXl){
      double temp = maxXl;
      minXl = maxXl;
      maxXl = temp;
    }
    if (msgLevel(MSG::DEBUG)) debug()<<"iCase "<<iCase<<"\t X last Min \t "<<minXl<<"\n\t\t\t Max X last \t "<<maxXl<<endmsg;
    if (msgLevel(MSG::DEBUG)) debug()<<"Will Loop over Last Layer"<<endmsg;

    PrHits::iterator itL = std::lower_bound(lHits.begin(), lHits.end(), minXl, lowerBoundX());
    PrHits::iterator itLEnd = std::upper_bound(lHits.begin(), lHits.end(), maxXl, upperBoundX());
    PrHit* lHit;
    for (; itLEnd != itL; ++itL) {
      lHit = *itL;
      if(nullptr == lHit){
        if (msgLevel(MSG::DEBUG)) debug()<<"Not Picking Hits in Last layer in the good search Window"<<endmsg;
        break;
      }
      if(lHit->x() > maxXl) break;
      if (lHit->isUsed() && m_removeFlagged) continue;
      double tx_pickedcombination = (lHit->x()-fHit->x())/DeltaZ;
      // double delta = lHit->x() - xProjeInf;
      parabolaSeedHits.clear();
      parabolaSeedHits.reserve(8);
      double x0 = fHit->x()-tx_pickedcombination*zFirst;
      double CorrX0 = m_x0Corr[iCase]*x0;
      double x0new = x0*(1.+m_x0Corr[iCase]);
      // double hp4alpha = m_x0Corr[iCase];
      if( msgLevel(MSG::DEBUG) ) debug() <<" x0 " << x0 << "CorrX0" << CorrX0 << "x0new" << x0new << "slope"<< m_x0Corr[iCase]<< endmsg;

        if (msgLevel(MSG::DEBUG)) debug()<<"Will loop over Parabola Seed Hits: n Layers"<<m_zones.size()<<endmsg;
        for(PrHitZone* xZone : {m_zones[s_T2X1 | part], m_zones[s_T2X2 | part]}) {
          double xProjected = x0 + xZone->z()*tx_pickedcombination;
          double xProjectedCorrected = xProjected+CorrX0;
          double xMax =0.;
          double xMin =0.;
          double max = 0.;
          double min = 0.;
          double slope = (m_tolAtX0Cut[iCase]-m_TolX0SameSign[iCase])/(m_x0Cut[iCase]-m_x0SlopeChange[iCase]);
          double slopeopp = (m_tolAtx0CutOppSig[iCase] -m_tolX0Oppsig[iCase])/(m_x0Cut[iCase]-m_x0SlopeChange2[iCase]);

          if(x0>0.){
            max = m_TolX0SameSign[iCase];
            //max= 1.2; //tight ?
            //min = x0>400.? -m_hp4_slope*x0 : -1.0; // <0
            min = x0 > m_x0SlopeChange[iCase]?  -slope*( x0 - m_x0SlopeChange[iCase]) - m_TolX0SameSign[iCase] : -m_TolX0SameSign[iCase];
            xMin = xProjectedCorrected + min;
            max = x0>m_x0SlopeChange2[iCase]? slopeopp*( x0 - m_x0SlopeChange2[iCase]) + m_tolX0Oppsig[iCase] : +m_tolX0Oppsig[iCase];
            xMax = xProjectedCorrected + max;
          }
          if(x0 < 0.){
            //max = x0<-400. ? -m_hp4_slope*: 1.0; // >0
            max = x0 <-m_x0SlopeChange[iCase]? -slope*( x0 + m_x0SlopeChange[iCase]) + m_TolX0SameSign[iCase]: m_TolX0SameSign[iCase];
            //min = -1.2;
            min = x0 < - m_x0SlopeChange2[iCase]? slopeopp*( x0 + m_x0SlopeChange2[iCase]) - m_tolX0Oppsig[iCase]: -m_tolX0Oppsig[iCase] ;
            xMin = xProjectedCorrected + min;
            xMax = xProjectedCorrected + max;
          }

          if(xMin > xMax) always()<<"Error xMin xMax"<<endmsg;

          if( xMax<xMin && msgLevel(MSG::DEBUG)) debug()<<"\t\t\t\t\t Wrong xMax/xMin"<<endmsg;
          if( msgLevel(MSG::DEBUG)) debug()<<"Lower bound the zones"<<endmsg;
          PrHits::iterator itH = std::lower_bound(xZone->hits().begin(), xZone->hits().end(), xMin, lowerBoundX());
          if( itH == xZone->hits().end()) continue; //next Zone
          PrHit* mHit = nullptr;;
          if(msgLevel(MSG::DEBUG) )debug()<<"Will Loop over xZones Hits"<<endmsg;
          for(; xZone->hits().end() != itH; ++itH){
            mHit = *itH;
            if( mHit == nullptr) break;
            if( mHit->x() > xMax ) break;
            // we can try to avoid this test
            if( mHit->isUsed() && m_removeFlagged) continue; //Not re use Hits in the middle
            if( msgLevel(MSG::DEBUG)) debug()<<"Filling Parabola Seed Hits"<<endmsg;
            parabolaSeedHits.push_back(mHit);
          }
          // if (parabolaSeedHits.size() > 0 && msgLevel(MSG::DEBUG)) debug()<<"ParabolaSeedHits Size \t ="<<parabolaSeedHits.size()<<endmsg;
          //Look for another Hit in last layer
          //end loop to pick up Hits in the 2 inner Layers (was only)
        }
        if(parabolaSeedHits.size()==0) continue; //go next last layer hit
        //if we don't fine any parabola Seed Hits in the middle 2 Layers then search for another XLast Hit
        // sort the parabola seed hits wrt to distance to the linear projection
        // merged parabolaSeedHits T2-1 & T2-2
        //=======================================================
        // We have 1 Hit in 1st 1 Hit in last and a
        // vector of Hits for in-between
        //=======================================================
        //std::vector<PrHits> xHitsLists; //vector of list of Hits
        xHitsLists.clear();

        //=======================================================
        //Sort the ParabolaSeedHits for in-between layers in increasing distance from the Projected Corrected position only when we have more than 1 ParabolaSeedHit
        //=======================================================
        if(parabolaSeedHits.size()>1){
          //Principle of the Lambda funtion, Hits sorted wrt distance from linear Projection 1st-3rd layer
          std::sort( parabolaSeedHits.begin(),parabolaSeedHits.end(),
                     [x0new,tx_pickedcombination](const PrHit* lhs, const PrHit* rhs)
                     ->bool{
                       // double lhsx0 =0 ;
                       // double rhsx0 =0 ;
                       // lhsx0 = x0new;
                       // rhsx0 = x0new;
                       return std::fabs( lhs->x() - ( x0new + lhs->z()*tx_pickedcombination)) < std::fabs( rhs->x() - (x0new + rhs->z()*tx_pickedcombination));} );
        }
        if(msgLevel(MSG::DEBUG)) debug()<<"The Lambda Function Sorting end"<<endmsg;
        unsigned int maxParabolaSeedHits = m_maxParabolaSeedHits;
        if(parabolaSeedHits.size()<m_maxParabolaSeedHits){
          maxParabolaSeedHits = parabolaSeedHits.size();
        }
         if( parabolaSeedHits.size()>m_maxParabolaSeedHits){
           maxParabolaSeedHits = parabolaSeedHits.size();
         }
         for(unsigned int i = 0; i<maxParabolaSeedHits;++i) //build a parabola for each 3 hit combination
        {
          //if (maxParabolaSeedHits==0) break; //maybe a repetition
          double a = 0;
          double b = 0;
          double c = 0;
          //PrHits xHits;
          xHits.clear();
          if(m_useCubic){
            solveParabola2(fHit,parabolaSeedHits[i],lHit,a,b,c); //Extrapolation with dRatio
          }else{
            solveParabola(fHit,parabolaSeedHits[i],lHit,a,b,c); //Extrapolation without dRatio
          }
          if (msgLevel(MSG::DEBUG)) debug()<<"Parabola Par"
                                           <<"\n a \t"<<a
                                           <<"\n b \t"<<b
                                           <<"\n c \t"<<c<<endmsg;
          //===================================================
          // Look in all the other layers except the
          // 1st/last/zone except the parabolaSeedHit
          //===================================================
          //Loop on all the xZones
          //for ( std::vector<PrHitZone*>::iterator itZ = xZones.begin(); xZones.end() != itZ; ++itZ )
          for (PrHitZone* xZone : xZones){
            if (msgLevel(MSG::DEBUG)) debug()<<"Selecting ParSeedHits"<<endmsg;
            if((int)xZone->planeCode() == (int)parabolaSeedHits[i]->planeCode()) continue;
            // double dz   = xZone->z() - m_geoTool->zReference();
            double dz = xZone->z() - m_zReference;

            double xAtZ = a * dz * dz + b * dz + c; //Parabolic computation
            if(m_useCubic){
              xAtZ= a * dz * dz * (1. + m_dRatio* dz) + b * dz + c; //with Cubic Correction
            }
            // double xMaxAtZ = xAtZ + 2.*fabs(tx_pickedcombination) + m_tolRemaining[iCase];
            // double xMinAtZ = xAtZ - 2.*fabs(tx_pickedcombination) - m_tolRemaining[iCase];
            double xMaxAtZ = xAtZ + m_tolRemaining[iCase];
            // // //std::fabs(tx_pickedcombination)+0.5;
            double xMinAtZ = xAtZ - m_tolRemaining[iCase];
            //std::fabs(tx_pickedcombination)-0.5;
            PrHit* bestProj = nullptr;
            double  bestDist = 10.0; //2.0 mm at the moment (Larger)? tighter? (see offline Seeding) ( 1.0 cm )
            if (xMinAtZ > xMaxAtZ){
              if (msgLevel(MSG::DEBUG)) debug()<<"Bad Settings!!!!!!"<<endmsg;
            }
            PrHits::iterator itH = std::lower_bound(xZone->hits().begin() ,xZone->hits().end(),xMinAtZ,lowerBoundX());
            PrHit* hit;
            for (; xZone->hits().end() != itH; ++itH){
              hit = *itH;
              if (hit->isUsed() && m_removeFlagged ) continue; //try to remove it ? allow for the remaining layers to pick up flagged hits?
              if (hit->x() < xMinAtZ ) continue;
              if (hit->x() > xMaxAtZ ) break;
              //Find Hit with Best distance <2.0mm
              if(std::fabs(hit->x() - xAtZ)  <  bestDist){
                bestDist = std::fabs(hit->x() - xAtZ);
                if (msgLevel(MSG::DEBUG)) debug()<<"I found an Hit from projection"<<endmsg;
                bestProj = hit;
              }
            }
            if(bestProj != nullptr){
              xHits.push_back(bestProj);
            }
          }//end loop xZones
          //in xHits are not present the first layer and last + parabola seed hits
          if (msgLevel(MSG::DEBUG)) debug()<<"End Loop in between zones to pick up Projection of parabola"<<endmsg;
          // xHits.push_back( parabolaSeedHits[i]);
          // Add to the xHits Vector the remaining 3 Hits not considered
          if(xHits.size() < 2 ) continue; //next parabolaSeedHits ; you must find 2 hits
          xHits.push_back(fHit);
          xHits.push_back(parabolaSeedHits[i]);
          xHits.push_back(lHit);
          // xHits.push_back( parabolaSeedHits[i]);
          // xHits.push_back( fHit);
          // xHits.push_back( lHit);
          // if(xHits.size()>6){
          //   always()<<"Smething goes wrong!!!! in the creation of the xHits list"<<endmsg;
          //   always()<<"xHits is bigger than 6 : ERROR"<<endmsg;
          // }
          //end parabola Seed Hits loop in other Layers
          //Still in the L0 loop (selection last layer)
          //at this step we have 1 Hit in 1st Layer
          //at this step we have 1 Hit in last Layer
          //at this step we have 1 Hit in Middle Layer
          //at this step we have Hit in remaining X layers at the
          //best distance to extrapolated parabola All of them are
          //inside xHits i want to have at least min_HitXSize
          //UNDER STUDY CASE 0 Only Keep Tracks found with 6 Hits (reduce ghost rate if add UV too
          //1st Case keep only 6 Hits on found track
          //2nd Case keep tracks with 4/5/6 hits
          //if( xHits.size() <=  m_minXPlanes)  continue; //Require at least m_minXPlanes ! no for first Fit
          //std::stable_sort(xHits.begin(), xHits.end(), compX());
          //is it really needed?
          std::sort(xHits.begin(), xHits.end(), compLHCbID());
          bool isEqual = false;
          // Remove xHits in the xHitsLists which are basically the same
          for(PrHits& hits : xHitsLists){
            if(msgLevel(MSG::DEBUG)) debug()<<"looping on xHitsLists"<<endmsg;
            if(hits == xHits){
              isEqual = true;
              break;
            }
          }
          if(!isEqual){
            if (msgLevel(MSG::DEBUG)) debug()<<"Pushing Back xHits List"<<endmsg;
            xHitsLists.push_back( xHits);
          }
        }//End loop parabolaSeedHits
        if (msgLevel(MSG::DEBUG)) debug()<<"End Loop For pick up Parabola Hits and build the xHitsLists"<<endmsg;
        //End loop Parabola Seed Hits
        //-------- Remove Duplicates from search in parabolaSeedHits
        if(msgLevel(MSG::DEBUG)) debug()<<"xHitsLists size before removing duplicates: "<<xHitsLists.size()<<endmsg;
        if(xHitsLists.size() == 0){
          continue;
        }
        if(xHitsLists.size() > 1){
          //---Remove Duplicates in the HitsList
          std::stable_sort( xHitsLists.begin(), xHitsLists.end());
          xHitsLists.erase( std::unique(xHitsLists.begin(), xHitsLists.end()), xHitsLists.end());
        }
        if(msgLevel(MSG::DEBUG)) debug()<<"xHitsLists size after removing duplicates: "<<xHitsLists.size()<<endmsg;
        //Now let's fit the track
        for(PrHits& xHits : xHitsLists){
          if(msgLevel(MSG::DEBUG)) debug()<<"Fit Track"<<endmsg;
          //Create the track
          // PrHybridSeedTrack temp_track( part , m_geoTool->zReference() , xHits); //Create the track
          PrHybridSeedTrack temp_track( part , m_zReference , xHits);

          //Setters for it: usefull later to parametrise
          //I load in the track these info which are then plotted
          // if(m_useCubic){
          //   temp_track.setdRatio(m_dRatio);
          // }
          //-----------------------------------------------------
          //----------------O-_The Fit_-O------------------
          //-----------------------------------------------------
          int nIter = 0;
          //bool doRefit = true;
          //temp_track.setRefitX(nIter);
          if (msgLevel(MSG::DEBUG) ){ debug()<<"Attempting to Fit the following Track"<<endmsg; printTrack(temp_track);}
          bool OK = false;
          if(temp_track.hits().size()>m_minXPlanes){ //no 4 hits at first fit
            OK = fitXProjection(temp_track,iCase);
          }
          while(!OK && temp_track.hits().size()>m_minXPlanes){
            if(temp_track.hits().size() <=m_minXPlanes){
              OK = false;
              break;
            }
            nIter++;
            if( nIter==1 && temp_track.hits().size() == 5){ OK = false; break;}
            if( temp_track.hits().size() > m_minXPlanes){
              OK = removeWorstAndRefitX(temp_track,iCase);
            }
          }
          if( OK ){
            setChi2X(temp_track);
          }
          if( OK &&
              temp_track.hits().size() >= m_minXPlanes &&
              (( temp_track.chi2PerDoF() < m_maxChi2DoFX[iCase]))){
            temp_track.setCase( iCase );
            int type1 = std::fabs( (temp_track.x( (m_zones[0]->z()))))<500.;
            int type2 = std::fabs( (temp_track.x( (m_zones[22]->z()))))<500.;
            temp_track.setType(type1+type1+type2);
            //temp_track.setnXnY(temp_track.hits().size(), 0);
            //always()<<"temp_track.type();
            m_xCandidates[(int)part].push_back(temp_track); //The X Candidate is created
          }
        }//end Loop xHist:xHitsLists
    }//end loop Last Zone given a firsZone selected
  }//end loop first zone
}





