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
  declareProperty("maxNbestCluster", m_maxNClusters = {2,4,4});
  //Used Params
  //declareProperty( "InputName"          ,     m_inputName= LHCb::TrackLocation::Forward);
  declareProperty( "InputName"          ,     m_inputName = ""); // Standalone by Default !
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
  // declareProperty( "FracCommon"         ,     m_fracCommon=0.71);           //Global Clone removal % shared hits ( to be implemented )
  declareProperty( "NCommonTot" , m_nCommonTot = 8 );
  declareProperty( "RemoveClonesX"      ,     m_removeClonesX = true);   //To be optimised ( BloomFilter for instance & use of fraction )
  // declareProperty( "FracCommonX"        ,     m_fracCommonX = {0.49, 0.49 , 0.29});
  declareProperty( "FlagHits"           ,     m_FlagHits = true);        //To be impoved
  declareProperty( "RemoveFlagged"      ,     m_removeFlagged = true);   //To be improved
  //Parameter Track Model
  declareProperty( "dRatio"             , m_dRatio = -0.000262); // Const dRatio value ( By = B0+B1*z ; B1/B0 = dRatio )
  //dRatio
  declareProperty( "CConst"             , m_ConstC =  2.458e8);  // Const value to compute the backward projection
  //Backward Projection
  declareProperty( "UseCorrPosition"    ,     m_useCorrPos = true);  // Compute the y(z) using the shift from the COS reference system of LHCb ( use the dzDy information of the hits )
  //Hit Flagging Step
  //if SizeToFlag = 10 or 9 errors!
  // Number of hits to flag at each Case >= Value
  declareProperty( "SizeToFlag"            ,m_SizeFlag = {12,11,10});
  // IfNHits<12 Flag only tracks having Chi2DoF<Flag_MaxChi2DoF_11Hits[Case]
  declareProperty( "Flag_MaxChi2DoF_11Hits",m_MaxChi2Flag = {0.5,1.0,1.0});
  // If NHits<12 Flag only tracks having |X0| ( Back. Projection ) < Flag_MaxX0_11Hits[Case] 
  declareProperty( "Flag_MaxX0_11Hits"     ,m_MaxX0Flag = {100.,8000.,200.});
  //--------------------X-Search Parametrisation
  //1st / Last Layer search windows
  //find x-Projection 2 - hit Combo
  //Case0 : 1st = T1-1X Last = T3-2X
  //Case1 : 1st = T1-2X Last = T3-1X
  //Case2 : 1st = T1-1X Last = T3-1X
  //Hits in T3 are collected accrding to txInf = x1st/Z1st;
  declareProperty( "L0_AlphaCorr"     , m_alphaCorrection = {120.64,510.64,730.64});
  declareProperty( "L0_tolHp"         , m_TolFirstLast = {280.0,540.0,1080.0});
  //find x-Projection 3 - hit Combo
  //2 Hit combination defines the value of the backward value at 0 ;
  //txPicked = (XT3 -XT1)/(ZT3-ZT1)
  //x0 = XT1-txPicked*ZT1
  //Hits are collected in T2 based on the following parameters.
  // See around Line 2165
  declareProperty( "maxParabolaSeedHits", m_maxParabolaSeedHits = 8); // Max Nb Of hits to process in T2-1X and T2-2X given a 2hit combination
  // std::vector<float> X0Rotation = boost::assign::list_of(0.002152)(0.001534)(0.001834);
  declareProperty( "x0Corr"             , m_x0Corr = {0.002152 , 0.001534,0.001834});  // Rotation angle
  declareProperty( "X0SlopeChange"      , m_x0SlopeChange = {400.,500.,500.});
  declareProperty( "ToleranceX0Up"      , m_TolX0SameSign = {0.75,0.75,0.75});
  //x0Cut needs to be different from X0SlopeChangeDown & X0SlopeChange 
  declareProperty( "x0Cut"              , m_x0Cut = {1500.,4000.,6000.});
  declareProperty( "TolAtX0Cut"         , m_tolAtX0Cut = {4.5,8.0,14.0});
  declareProperty( "X0SlopeChangeDown"  , m_x0SlopeChange2 = {2000.,2000.,2000.});
  //Tolerance inferior for |x0| > m_x0SlopeChange2 when x0 = m_x0Cut
  declareProperty( "TolAtX0CutOpp"      , m_tolAtx0CutOppSig = {0.75,2.0,7.0});
  declareProperty( "ToleranceX0Down"    , m_tolX0Oppsig = {0.75,0.75,0.75});

  //find x-Projection Remaining layers & fitting
  // Tolerance for remaining layers ( after the 3 hit combination ) in mm
  declareProperty( "TolXRemaining"      , m_tolRemaining = {1.0,1.0,1.0});
  declareProperty( "maxChi2HitsX"       , m_maxChi2HitsX = {5.5,5.5,5.5});
  declareProperty( "maxChi2DoFX"        , m_maxChi2DoFX = {4.0,5.0,6.0});
  
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
  // if stereo2 set to false
  declareProperty("minUV6"  ,   m_minUV6   =   {4,4,4});
  declareProperty("minUV5"  ,   m_minUV5   =   {5,5,4});
  declareProperty("minUV4"  ,   m_minUV4   =   {6,6,5});
  //10 or 9 hits on track
  //stereo2 OFF: declareProperty("Chi2LowLine"     ,    m_Chi2LowLine = {4.0,5.0,7.0}); 
  declareProperty("Chi2LowLine"     ,    m_Chi2LowLine ={5.0,6.5,7.5} );
  // if NXZ + NUV (Planes) <=10  XZChi2DoF + YLineChi2DoF < Chi2LowLine
  //11 or 12 hits on track
  declareProperty("Chi2HighLine"    ,    m_Chi2HighLine = {30.0, 50.0, 80.0});
  declareProperty("minTot"          ,    m_minTot = {9,9,9});
  declareProperty( "TolTyOffset"    ,    m_tolTyOffset = {0.0017, 0.0025, 0.0035});
  declareProperty( "TolTySlope"     ,    m_tolTySlope =  {0.0, 0.025 , 0.035});
  //-------- Simultaneous Fit Y
  //MaxChi2 Hit >10 Hits
  //N Layers > 10 ( 11 and 12 hits ) outliers removed if MaxChi2Hit > maxChi2Hits_11and12Hit
  declareProperty( "maxChi2Hits_11and12Hit" , m_maxChi2HitFullFitHigh = {5.5,5.5,5.5});
  //MaxChi2 Hit <11 Hits + Y(0) +  Y(zRef) cut
  
  // N Layers < 11 (9,10) outliers removed if MaxChi2Hit< maxChi2Hits_less11Hit
  declareProperty( "maxChi2Hits_less11Hit" , m_maxChi2HitFullFitLow = {2.5,2.5,2.5});
  // If N Layers < 11: kill tracks having y(z=0) >50. mm
  //stere2 Off: declareProperty( "maxYatZeroLow"         , m_maxY0Low             = {50.,50.,50.});
  declareProperty( "maxYatZeroLow"         , m_maxY0Low             ={50.,60.,70.});

  // If N Layers < 11 : kill tracks having y(zRef)>500. mm
  //stereo2 Off : declareProperty( "maxYatzRefLow"         , m_maxYZrefLow          =  {500.,500.,500.});
  declareProperty( "maxYatzRefLow"         , m_maxYZrefLow          ={400.,550.,700.});
  
  //Global Chi2PerDoF
  declareProperty( "maxChi2PerDoF"         , m_maxChi2PerDoF        = {4.0,6.0,7.0});
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
  m_debug = msgLevel(MSG::DEBUG);
  
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  if ( m_debug ) debug() << "==> Initialize" << endmsg;
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

  if( m_printSettings){
    if( m_decodeData )        info() << "Will decode the FT clusters!" << endmsg;
    if( m_FlagHits)           info()<<"Will Flag the Hits" << endmsg;
    if( m_removeFlagged)      info()<<"Will Not re-use Flagged"<<endmsg;
    if( m_inputName == "")    info()<<"Standalone Seeding"<<endmsg;
    if( !(m_inputName == "")) info()<<"Forward tracks as input"<<endmsg;
    if( m_removeClones)      info()<<"Will Remove Clones"<<endmsg;
    if( m_xOnly) info()<<"Will use Only X Layers" <<endmsg;
    if( m_useFix && !m_xOnly) info()<<"WIll Use the triangle Fix"<<endmsg;
       
    info() <<"==================================================="<<endmsg
           << "===============GLOBAL SETTINGS===================="<<endmsg
           << " InputName                                       = "<<  m_inputName              << endmsg
           << " OutputName                                     = "<<  m_outputName              << endmsg
           << " HitManagerName                                 = "<<  m_hitManagerName          << endmsg
           << " DecodeData                                     = "<<  m_decodeData              << endmsg
           << " XOnly                                          = "<<  m_xOnly                   << endmsg
           << " MinXPlanes                                     = "<<  m_minXPlanes              << endmsg
           << " MaxNHits                                       = "<<  m_maxNHits                << endmsg
           << " NCases                                         = "<<  m_nCases                  << endmsg
           << " TimingMeasurement                              = "<<  m_doTiming                << endmsg
           << " dRatiio                                        = "<< m_dRatio                   << endmsg
           << " CConst BackProj                                = "<< m_ConstC<<endmsg
           << "=================ADD UV Layer Settings============"                              << endmsg
           << "doAsymmUV                                       = "<< m_doAsymmUV                <<endmsg
           << "Use Triangle Fix                                = " << m_useFix                  << endmsg
           << "Use SecondOrder `Triangle Fixing                = " << m_useFix2ndOrder <<endmsg
           << "*******Fit Settings"<<endmsg
           << "==================Clone Removal and Flag Hits Settings=========="<<endmsg
           << "Remove Clones after X searh                    = " <<m_removeClonesX<<endmsg
           << "RemoveClones after add stereo UV               = " << m_removeClones<<endmsg
           << "N Common Total                         = " << m_nCommonTot <<endmsg

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
    // for( unsigned int kk = 0; m_fracCommonX.size()>kk; ++kk){
    //   info()<<"\t Case" <<kk <<"     FracCommonX   = " << m_fracCommonX[kk]<<endmsg;
    // }
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
    for( unsigned int kk = 0;  m_Chi2LowLine.size()>kk; kk++){
      info()<<"\t Case"<<kk<<"max   Chi2DoFX + Chi2Line (<11) hit  "<<m_Chi2LowLine[kk]<<endmsg;
      info()<<"\t Case"<<kk<<"max   Chi2DoFX + Chi2Line (>10) hit  "<<m_Chi2HighLine[kk]<<endmsg;
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
  if( m_nCases >3) return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}


//=============================================================================
// Main execution
//=============================================================================
StatusCode PrHybridSeeding::execute() {
  if ( m_debug ) debug() << "==> Execute" << endmsg;
  if ( m_doTiming ){
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
        if( (*itId).isFT() && (*itId).ftID().layer() < 12 ){
          LHCb::FTChannelID ftId =(*itId).ftID();
          int zoneNb = 2 * ftId.layer() + ftId.mat(); //zones top are even (0, 2, 4, ....,22)  and zones bottom are odd
          //Load the PrHitZone
          PrHitZone* zone = m_hitManager->zone(zoneNb);
          // -- The hits are sorted according to LHCbID, we can therefore use a lower bound to speed up the search
          auto itH = std::lower_bound(  zone->hits().begin(),  zone->hits().begin(), *itId, lowerBoundLHCbID() );
          for( ; zone->hits().end() != itH; ++itH ) {
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
  // up / down ( for m_trackCandidates
  for(unsigned int i = 0; i< m_trackCandidates.size() ; i++){
    m_xCandidates[(int)i].clear();
    m_trackCandidates[(int)i].clear();
  }
  if ( m_doTiming ) {
    m_timerTool->stop( m_timeFromForward );
  }
  //========================================================
  //------------------MAIN SEQUENCE IS HERE-----------------
  //========================================================
  // ----- Loop through lower and upper half
  for( unsigned int part= 0; 2 > part; ++part ){
    //----- Loop For difference Cases
    //Swap them ? externally icase & inner loop part? m_xCandidates
    for(unsigned int icase = 0; m_nCases > icase ; ++icase){
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
        addStereo( part, icase);
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
    removeClones( part );
    if( m_doTiming){
      m_timerTool->stop( m_timeClone[ (int) part]);
    }
    makeLHCbTracks(result, part);
  }
  if( m_debug) debug()<<"Making LHCb Tracks Done"<<endmsg;
  if( m_doTiming){
    m_timerTool->stop( m_timeTotal);
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PrHybridSeeding::finalize() {
  if ( m_debug ) debug() << "==> Finalize" << endmsg;
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
    float yMin = -1*std::fabs(m_yMax);
    //yMin = -2500 mm
    float yMax = +1*std::fabs(m_yMax);
    //yMax = +2500 mm
    // floatdxDy = m_zones[kk]->dxDy();  //Load the stereo Angle of that given layer
    // floatzPlane = m_zones[kk]->z();   //Load the z of the Plane
    float dxDy = zoneUV->dxDy();
    float zPlane = zoneUV->z();
    float xPred = xProje.x(zPlane);
    float recDxDyZ = 1./(dxDy*zPlane);
    float recDxDy = 1./dxDy;
    //Compute the predicted x-position in that given layer
    //unsigned int kk = zoneUV->number();
    //[-2500;+2500]
    if(yMin >yMax){
      //should not happen
      std::swap( yMin, yMax);      
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
      }else{
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
    float xMin = xPred - yMin*dxDy;
    float xMax = xPred - yMax*dxDy;
    if(xMin > xMax){
      std::swap( xMax, xMin);      //revert to properly take into account dxDy
    }
    // PrHits& hits = m_zones[kk]->hits(); //Load all the Hits in that given zone
    PrHits& hits = zoneUV->hits();
    //Place the iterator at xMin predicted
    //unsigned int kk = zoneUV->number();
    auto itH = std::lower_bound(hits.begin(),hits.end(),xMin, lowerBoundX());
    auto  itEnd = std::upper_bound( hits.begin(),hits.end(),xMax, upperBoundX());
    for(  ; itEnd!= itH; ++itH ){
      if( (*itH)->isUsed() && m_removeFlagged ) continue;
      //check if the u/v hit is used: do not use it again
      float y = ( xPred - (*itH)->x() )*recDxDy;
      //compute y hit
      //should not happen actually
      if(y >yMax) continue;
      if(y <yMin) continue;
      if(m_useFix2ndOrder){
        if(part == 0){
          if(kk%2 == 0){
            //Up Up
            if( (*itH)->yMin()<0. ){yMin = m_yMin;
              yMax = m_yMax;
            }else{yMin = -2.0 + std::fabs((*itH)->yMin());yMax = m_yMax;}
          }else{ // kk%2 ==1
            //Up Down
            if( (*itH)->yMax()<0. ){  continue;
            }else{  yMin = -1.0 ;yMax = +2.0 + std::fabs((*itH)->yMax());
            }
          }
        }else{ //part ==1
          if(kk%2 == 1){
            //Down Down
            if( (*itH)->yMax()>0.){yMin = -std::fabs(m_yMax) ;yMax = +1.0;
            }else{yMin = -std::fabs(m_yMax) ;yMax = +2.0-std::fabs((*itH)->yMax());}
          }else{ //kk%2 == 0
            //Down Up
            if( (*itH)->yMin()<0.){yMin = -2.0 - std::fabs((*itH)->yMin());yMax = +1.0;
            }else{ continue; 
            }
          }
        }
      }
      if(m_debug){
        //debug()<<"y Hit computed    "<< y<<endl;
      }
      if(y>yMax || y< yMin) continue;
      if(m_removeHole){
        float radius =  y*y + xPred*xPred;
        if(  radius < m_radiusHole*m_radiusHole) continue;
      }
      //Absolute value!
      (*itH)->setCoord( std::fabs( (xPred - (*itH)->x() )*recDxDyZ ) );
      if( part == 0 && y<0.)
        (*itH)->setCoord( -1.*std::fabs((xPred-(*itH)->x())*recDxDyZ));
      if( part == 1 && y>0.)
        (*itH)->setCoord( -1.*std::fabs((xPred-(*itH)->x())*recDxDyZ));
      myStereo.push_back( (*itH) );
    }
  }
}

void PrHybridSeeding::addStereo( unsigned int part, unsigned int iCase){
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
        if(m_debug) always()<<"Push back zone = "<<uvZoneID+detector_part<<endmsg;
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
  PrPlaneHybridCounter plCount;
  //PrHits line; // it's used when you end up in a very busy cluster. ( 1 line achieved with lot of outliers removal up to having 1 hit per layer)
  int maxUVfound = 0;
  PrLineFitterY BestLine( m_zReference );
  float initCoord = std::numeric_limits<float>::max();
  float ClusterSpread[3][DEPTH_HOUGH];//[3];
  HitIter initBeg[3][DEPTH_HOUGH];//[3];
  HitIter initEnd[3][DEPTH_HOUGH];//[3];
  //Hough utils from Manuel ? To be implemented
  //unsigned int index = part*m_nCases+iCase;
  
  
  if( m_debug) always() << " Will loop on x-candidates case "<<iCase <<" Part = "<<part<<endmsg;
  for( auto itT =m_xCandidates[part].begin();m_xCandidates[part].end() != itT; ++itT){
    //if( (*itT).Case() != iCase) continue;
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
    if(m_debug) always()<<" initializing clusters "<<endmsg;
    for( int i =0; i<3 ; ++i){
      for( int j = 0; j<DEPTH_HOUGH; ++j){
        ClusterSpread[i][j] = initCoord;
        initBeg[i][j] = myStereo.end();
        initEnd[i][j] = myStereo.end();
      }
    }
    PrHits::iterator itBeg = myStereo.begin();
    PrHits::iterator itEnd =  itBeg+4<=myStereo.end() ? itBeg+4: myStereo.end();
    // auto itEnds[3];
    // { 6, 5, 4 }
    // itEnds = {itBeg+6, itBeg+5, itBeg+4};
    // for (int i =0 ; i < 3; ++i) itEnds[i] = itBeg+6-i <= myStereo.end() ? itBeg+6-i : myStereo.end();
    
    // Pre-fill the clusters
    if(m_debug) always()<<"Storing clusters"<<endmsg;
    for( ; itEnd-itBeg>=4; IncreaseIters( itBeg, itEnd,4, myStereo)){
      float tolTy = m_tolTyOffset[iCase] + m_tolTySlope[iCase]*(*itBeg)->coord();
      // Check for 6 layers, then 5 layers, and finally 4 layers
      for (int i = 0; i < 3; ++i){
        // i = 0; itLast = itEnd+6
        auto itLast = itEnd+2-i <= myStereo.end() ? itEnd+2-i : myStereo.end();
        // spread6 computed first
        float spread = (*(itLast-1))->coord() - (*itBeg)->coord();
        if(CheckCluster(itBeg, itLast, tolTy)){
          //cluster is fine check
          int j;
          // find the cluster worse than the current cluster
          for (j = 0; j < DEPTH_HOUGH; ++j){//4=>3
            if(spread < ClusterSpread[i][j]) break;
          }
          if (j == DEPTH_HOUGH) continue;
          // Shift clusters after the found position
          for(int k = DEPTH_HOUGH-1; k > j; --k){//2=>3
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
    if(m_debug) always()<<"Cluster stored"<<endmsg;
    int nXinit = (*itT).hits().size();
    BestLine.setXProj((*itT));
    int nCandidates = 0;
    for( int i = 0; i<3; ++i){
      for( int j = 0; j<DEPTH_HOUGH; ++j){
        if(m_debug) always()<<"Processing cluster i ="<<i<<"  j = "<<j<<endmsg;
        //you are at the cluster Number (1+j) +(3*i)
        PrHits::iterator itBeg = myStereo.begin();
        PrHits::iterator itEnd = myStereo.end();
        int nLay = 0;
        itBeg = initBeg[i][j];
        itEnd = initEnd[i][j];
        if( itBeg == myStereo.end() && itEnd == myStereo.end()){
          break;
        }
        if(m_debug) always()<<"Extending cluster "<<endmsg;
        ExtendCluster( itBeg, itEnd , iCase, myStereo , nLay );
        if( m_debug) always() << " Cluster extended"<<endmsg;
        //maxUVfound are the minimal UV layers to find !
        if( (int)nLay < (int)maxUVfound || (int)nLay < (int)minUV) continue;
        nCandidates++;
        if( m_debug) always() << "Checkiung nCandidates case "<<endmsg;
        if(nCandidates > m_maxNClusters[(*itT).Case()]) break;
        //it is set with the hits in the cluster under process ( no x-z )
        if( m_debug){
          always()<<"Setting pl counter"<<endmsg;
        }
        plCount.set( itBeg , itEnd ); // ( useful for later )
        if( m_debug){
          always()<<" Plane counter diff UV = "<< plCount.nbDifferentUV() <<" n Lay from extending cluter   "<< nLay<<endmsg;
        }
        nLay = plCount.nbDifferentUV(); //useless???
        if( nLay == 4 && !plCount.isOKUV()){
          continue;
        }
        BestLine.setXProj( (*itT) );
        PrHybridSeedTrack temp(*itT);
        bool fit = false;
        if( plCount.nbSingleUV() == plCount.nbDifferentUV()){
          if(m_debug) always()<<"Fitting line when same amount of UV in Planes"<<endmsg;
          fit = BestLine.fit( itBeg,itEnd );
          if( fit && LineOK( m_Chi2LowLine[iCase], m_Chi2HighLine[iCase], BestLine, temp)){
            for( auto itH = itBeg; itEnd != itH; ++itH){
              temp.addHit( (*itH));
              //temp.setYParam( BestLine.ay(), BestLine.by());
            }
            temp.setYParam( BestLine.ay(), BestLine.by());
          }
        }else{ // more than one hit per layer in the cluster....try out the outliers removal from line fit
          if(m_debug) always()<<"More hit than layers"<<endmsg;
          if(m_debug) always()<<"Fitting line (fit2)"<<endmsg;
          fit = BestLine.fit2( itBeg, itEnd, plCount );
          //you have done the line fit even if the amount of hits in the layers is with multiplicity
          //here is the final line you want
          //line.clear();
          for( auto itH = itBeg; itEnd!= itH; ++itH){
            temp.addHit( (*itH) );
          }
          if(m_debug) always()<<"Hit loaded in the line form cluster"<<endmsg;
          while( (temp.hits().size() - (*itT).hits().size() ) > plCount.nbDifferentUV() ){
            PrHits::iterator worstsamelayer = temp.hits().end();
            BestLine.fit2( temp.hits().begin(), temp.hits().end() , plCount);
            if(m_debug) always()<<" Removing outliers"<<endmsg;
            auto worstMultiple = temp.hits().end();
            float worstchi = std::numeric_limits<float>::min();
            float chi_hit = std::numeric_limits<float>::min();
            for( auto line_hit = temp.hits().begin(); line_hit!=temp.hits().end(); ++line_hit){
              if( m_debug) always()<<" looping line hits!"<<endmsg;
              if( (*line_hit) ->isX() ) continue;
              if( plCount.nbInPlane( (*line_hit)->planeCode() ) < 2) continue;
              chi_hit = BestLine.chi2hit( (*line_hit));
              if( chi_hit > worstchi ){
                worstchi = chi_hit ;
                worstMultiple = line_hit;
              }
            }
            plCount.removeHitInPlane( (*worstMultiple)->planeCode() );
            //it will just change the m_planeCode[int] not the whole internal hit counting ! )
            temp.hits().erase( worstMultiple );
            fit = BestLine.fit2( temp.hits().begin(), temp.hits().end(), plCount);
          }
          if( fit ){
            fit = LineOK( m_Chi2LowLine[iCase], m_Chi2HighLine[iCase], BestLine, (*itT) );
            temp.setYParam( BestLine.ay(), BestLine.by());
          }
        }
        if( temp.hits().size()< 9 || !fit ) continue;
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
          m_trackCandidates[(int)part].push_back( temp);
        }
      }
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
void PrHybridSeeding::ExtendCluster(PrHits::iterator& itBeg, PrHits::iterator& itEnd , unsigned int iCase,PrHits&myStereo , int &nLay){
  if(m_debug) always()<<"Pop counter"<<endmsg;
  int planes = 0;
  for( auto Hit = itBeg; itEnd!=Hit; ++Hit){  
    planes|= 1<<(*Hit)->planeCode();
  }
  nLay = __builtin_popcount( planes);
  float tolTy = m_tolTyOffset[iCase] + (*itBeg)->coord()*m_tolTySlope[iCase] ;
  while( itEnd != myStereo.end() && ((*itEnd)->coord() - (*itBeg)->coord() ) < tolTy && nLay<6){//as soon as you find a sixth element exit the loop (in other case extend up to tolerance
    if( m_debug) always()<<" Counting layers "<<endmsg;
    planes|= 1<<(*itEnd)->planeCode();
    ++itEnd;
    nLay = __builtin_popcount( planes);
  }
}


//==================
//Add Stereo void
//==================
bool PrHybridSeeding::CheckCluster( PrHits::iterator& itBeg, PrHits::iterator& itEnd, float tol){
  //float deltaCoord = (*(itEnd))->coord() - (*itBeg)->coord() ;
  return ( (*(itEnd-1))->coord() - (*itBeg)->coord() < tol);
}


void PrHybridSeeding::IncreaseIters( PrHits::iterator& itBeg, PrHits::iterator& itEnd, unsigned int minUV , PrHits& myStereo){
  if(m_debug){ always()<<"Increasing iterators for cluster searches"<<endmsg;}
  ++itBeg;
  itEnd = (itBeg + minUV <=myStereo.end()? itBeg+minUV : myStereo.end());
}
bool PrHybridSeeding::LineOK( float minChi2Low, float minChi2High, PrLineFitterY& line, PrHybridSeedTrack& xProje ){
  const int nHits = line.nHitsLine() + xProje.hits().size();
  const float Chi2DoFLineXProj = xProje.chi2PerDoF() + line.Chi2DoF();
  if( nHits > 10 && Chi2DoFLineXProj < minChi2High) return true;
  if( nHits < 11 && Chi2DoFLineXProj < minChi2Low ) return true;
  return false;
}

void PrHybridSeeding::removeClonesX(unsigned int part,unsigned int icase, bool xOnly){
  //unsigned int index = part*m_nCases+icase;
  std::sort(m_xCandidates[(int)part].begin(), m_xCandidates[(int)part].end(),PrHybridSeedTrack::GreaterBySize());
  //std::sort( m_xCandidates[(int)part].begin(), m_xCandidates[(int)part].end(), PrHybridSeedTrack::LowerBySize());
  unsigned int maxCommon = 0;
  if( ! m_xOnly ){
    for( auto itT1 = m_xCandidates[part].begin(); m_xCandidates[part].end() !=itT1; ++itT1){
      if(!(*itT1).valid() || xOnly) continue;
      for( auto itT2 = itT1 + 1; m_xCandidates[part].end() !=itT2; ++itT2 ){
        if(!(*itT2).valid())                continue;
        if(((*itT1).bloomfilter() & (*itT2).bloomfilter()).empty()){
          continue;
        }
        if(! (  std::fabs( (*itT1).xT1() - (*itT2).xT1() ) < 2.0 || 
                std::fabs( (*itT1).xT2() - (*itT2).xT2() ) < 2.0 ||
                std::fabs( (*itT1).xT3() - (*itT2).xT3() ) < 2.0 ) ) continue;
        //if((*itT1).type()%2 != (*itT2).type()%2 )continue;
        
        int Compare = (*itT1).size()+(*itT2).size(); // 36 , 30 ,30  ( 6 vs 6 6 vs 5 )
        //only hits in t1/t3
        if( Compare <10 ){
          maxCommon = 1;
        }else if( Compare ==10 ){
          maxCommon = 2;
        }else if( Compare >10){
          maxCommon = 3;
        }
        unsigned int nCommon = 0;
        auto itH1 = (*itT1).hits().begin();
        auto itH2 = (*itT2).hits().begin();
        auto itEnd1 = (*itT1).hits().end();
        auto itEnd2 = (*itT2).hits().end();
        //count number of common hits between track 1 and track 2 in T1/T3 ( skip T2 )
        while(nCommon < maxCommon &&  itH1 != itEnd1 && itH2 != itEnd2){
          if((*itH1)->id() == (*itH2)->id()){
            ++nCommon;
            ++itH1;
            ++itH2;
          }else if( (*itH1)->id() < (*itH2)->id() ){
            // while( itH1!=itEnd1 && (*itH1)->id() < (*itH2)->id() )
            ++itH1;
          }else{
            // while( itH2!=itEnd2 && (*itH2)->id() < (*itH1)->id() )
            ++itH2;
          }
        }
        if( nCommon>=maxCommon){
          (*itT1).setValid(false);
          break;
        }
      }
    }
  }else{
    unsigned int maxCommon = 0;
    for( auto itT1 = m_xCandidates[part].begin(); m_xCandidates[part].end() !=itT1; ++itT1){
      if( !(*itT1).valid()) continue;                                                                            
      for( auto itT2 = itT1 +1; m_xCandidates[part].end()!=itT2; ++itT2){
        if( (*itT1).type()%2 != (*itT2).type()%2 ) continue;
        if(!(*itT2).valid() ) continue;
        unsigned int nCommon = 0;                         
        auto itH1 = (*itT1).hits().begin();
        auto itH2 = (*itT2).hits().begin();
        auto itEnd1 = (*itT1).hits().end();
        auto itEnd2 = (*itT2).hits().end();
        int Compare = (*itT1).size()+(*itT2).size();
        if( Compare <10 ){
          maxCommon = 1;                                          
        }
        else if( Compare ==10 ){
          maxCommon = 2;
        }
        else if( Compare >10){
          maxCommon = 3;
        }
        while( itH1 != itEnd1 && itH2 != itEnd2 ){
          if((*itH1)->id() == (*itH2)->id()){
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
          if( nCommon >=maxCommon )  break;              
        }
        if(nCommon >= maxCommon){
          (*itT1).setValid(false);
          break;
        }
      }
    }
    if( icase == m_nCases-1){ // xOnly is automatic!
      for( auto itT1 = m_xCandidates[part].begin() ; m_xCandidates[part].end()!= itT1; ++itT1){
        if((*itT1).valid()) m_trackCandidates[(int)part].push_back( (*itT1)); 
      }
    }
  }
}

void PrHybridSeeding::removeClones( unsigned int part){
  //std::sort(m_trackCandidates[(int)part].begin(), m_trackCandidates[(int)part].end(),PrHybridSeedTrack::GreaterBySize());
  std::sort(m_trackCandidates[(int)part].begin(), m_trackCandidates[(int)part].end(),PrHybridSeedTrack::LowerBySize());
  for ( auto itT1 = m_trackCandidates[(int)part].begin(); m_trackCandidates[(int)part].end()!=itT1; ++itT1){
    if( !(*itT1).valid()) continue;
    //maxCommon = 8;
    //m_fracCommon*(*itT1).hits().size();
    for ( auto itT2 = itT1 + 1; m_trackCandidates[(int)part].end() !=itT2; ++itT2 ) {
      if ( !(*itT2).valid()) continue;
      if(((*itT1).bloomfilter() & (*itT2).bloomfilter()).empty()) continue;
      if(! (  std::fabs( (*itT1).xT1() - (*itT2).xT1() ) < 2.0 ||  
              std::fabs( (*itT1).xT2() - (*itT2).xT2() ) < 2.0 ||  
              std::fabs( (*itT1).xT3() - (*itT2).xT3() ) < 2.0 ) ) continue;
      unsigned int nCommon = 0;
      auto itH1 = (*itT1).hits().begin();
      auto itH2 = (*itT2).hits().begin();
      auto itEnd1 = (*itT1).hits().end();
      auto itEnd2 = (*itT2).hits().end();
      while( nCommon < m_nCommonTot && itH1 != itEnd1 && itH2 != itEnd2 ){
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
      if( nCommon>=m_nCommonTot ){
        //Due to sorting of tracks from lower quality to higher one
        (*itT1).setValid(false);
        break;
      }
    }
  }
}
void PrHybridSeeding::flagHits(unsigned int icase, unsigned int part){
  //always()<<"flagging hits on tracks"<<endmsg;
  std::sort( m_trackCandidates[(int)part].begin() , m_trackCandidates[(int)part].end() , PrHybridSeedTrack::LowerBySize()); //bigger size is in front
  for(auto track = m_trackCandidates[(int)part].begin(); m_trackCandidates[(int)part].end()!=track ; ++track){
    // important the sorting for this break
    if((*track).size() < m_SizeFlag[icase]) break;
    if(!(*track).valid()) continue;
    if((*track).Case()!=icase) continue;
    // Important the sorting of before for this break
    if(!( ((*track).size()==11
           && (*track).chi2PerDoF()< m_MaxChi2Flag[icase]
           && std::fabs((*track).X0())<m_MaxX0Flag[icase])
          || ((*track).size()==12 )
          ) ) continue;
    for(auto it = (*track).hits().begin();(*track).hits().end()!=it; ++it){
      (*it)->setUsed(true);
    }
  }
}
//=========================================================================
//  Convert to LHCb tracks
//=========================================================================
void PrHybridSeeding::makeLHCbTracks( LHCb::Tracks* result , unsigned int part ) {
  for( auto itT = m_trackCandidates[(int)part].begin();m_trackCandidates[(int)part].end() != itT; ++itT ){
    if ( !(*itT).valid() ) continue;
    if ( m_debug ) debug()<<"Creating LHCb Track"<<endmsg;
    LHCb::Track* tmp = new LHCb::Track;
    tmp->setType( LHCb::Track::Ttrack );
    tmp->setHistory( LHCb::Track::PrSeeding );
    float qOverP = m_geoTool->qOverP( *itT );
    // always()<<"qOverP = "<<qOverP<<endmsg;
    
    LHCb::State tState;
    float z = StateParameters::ZEndT;
    tState.setLocation( LHCb::State::AtT );
    tState.setState( (*itT).x( z ), (*itT).y( z ), z, (*itT).xSlope( z ), (*itT).ySlope( ), qOverP );
    //== overestimated covariance matrix, as input to the Kalman fit
    tState.setCovariance( m_geoTool->covariance( qOverP ) );
    tmp->addToStates( tState );
    //== LHCb ids
    // Overloading things
    tmp->setPatRecStatus( LHCb::Track::PatRecIDs );
    for ( auto itH = (*itT).hits().begin(); (*itT).hits().end() != itH; ++itH ) {
      tmp->addToLhcbIDs( (*itH)->id() );
    }
    tmp->setChi2PerDoF( (*itT).chi2PerDoF() );
    tmp->setNDoF( (*itT).nDoF() );
    if(m_debug) debug()<<"Inserting a track in container"<<endmsg;
    result->insert( tmp );
  }
}


void PrHybridSeeding::solveParabola(const PrHit* hit1,const PrHit* hit2,const PrHit* hit3,float& a1, float& b1,float& c1){
  const float z1 = hit1->z() - m_zReference;//->zReference();
  const float z2 = hit2->z() - m_zReference;//m_geoTool->zReference();
  const float z3 = hit3->z() - m_zReference;//m_geoTool->zReference();
  const float x1 = hit1->x();
  const float x2 = hit2->x();
  const float x3 = hit3->x();
  //const float e = m_dRatio;
  const float corrZ1 = 1.+m_dRatio*z1;
  const float corrZ2 = 1.+m_dRatio*z2;
  const float corrZ3 = 1.+m_dRatio*z3;
  const float det = (z1*z1)*corrZ1*z2 + z1*(z3*z3)*corrZ3 + (z2*z2)*corrZ2*z3 - z2*(z3*z3)*corrZ3 - z1*(z2*z2)*corrZ2 - z3*(z1*z1)*corrZ1;
  const float recdet = 1./det;
  if( std::fabs(det) < 1e-8 )
  {
    a1 = 0.0;
    b1 = 0.0;
    c1 = 0.0;
    return;
  }
  const float det1 = (x1)*z2 + z1*(x3) + (x2)*z3 - z2*(x3) - z1*(x2) - z3*(x1);
  const float det2 = (z1*z1)*corrZ1*x2 + x1*(z3*z3)*corrZ3 + (z2*z2)*corrZ2*x3 - x2*(z3*z3)*corrZ3 - x1*(z2*z2)*corrZ2 - x3*(z1*z1)*corrZ1;
  const float det3 = (z1*z1)*corrZ1*z2*x3 + z1*(z3*z3)*corrZ3*x2 + (z2*z2)*corrZ2*z3*x1 - z2*(z3*z3)*corrZ3*x1 - z1*(z2*z2)*corrZ2*x3 - z3*(z1*z1)*corrZ1*x2;
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
  float mat[15];
  float rhs[5];
  // unsigned int nHitsX = 0;
  // unsigned int nHitsStereo = 0;
  const float zRef = m_zReference;
  for (unsigned int loop = 0; 3 > loop ; ++loop ){
    if(loop ==1 && m_useCorrPos){
      float RadiusPosition = std::sqrt((0.0005*0.0005*track.ax()*track.ax() + 0.001*0.001*track.y(zRef)*track.y(zRef)));
      float dRatioPos = -1.*(2.633e-4 - 3.59957e-6*RadiusPosition + 4.7312e-5*RadiusPosition*RadiusPosition);
      track.setdRatio(dRatioPos);
    }
    std::fill(mat,mat+15,0.);
    std::fill(rhs,rhs+5,0.);
    for( auto itH = track.hits().begin(); track.hits().end() != itH; ++itH ){
      const float w = (*itH)->w();
      const float dxdy = (*itH)->dxDy();
      const float yOnTrack = track.yOnTrack( (*itH) ) ;
      const float   dz = 0.001*((*itH)->z( yOnTrack ) - zRef );
      const float dRatio = track.dRatio();
      const float deta = dz*dz*(1. + dz*dRatio);
      const float wdz = w * dz;
      const float weta = w * deta;
      const float wdxdy = w * dxdy;
      const float wdxdydz = wdxdy * dz;
      const float dist = track.distance(*itH);
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
    ROOT::Math::CholeskyDecomp<float, 5> decomp(mat);
    if (!decomp) return false;
    decomp.Solve(rhs);
    rhs[1]*=1.e-3;
    rhs[2]*=1.e-6;
    rhs[4]*=1.e-3;
    rhs[3]-=rhs[4]*zRef;
    //crazy values!
    if( (std::fabs(rhs[0]) > 1e4 || std::fabs(rhs[1]) > 5. ||
         std::fabs(rhs[2]) > 1e-3 || std::fabs(rhs[3]) > 1e4 || std::fabs(rhs[4]) > 1.)) return false;
    track.updateParameters(rhs[0],rhs[1],rhs[2],rhs[3],rhs[4]);
  }
  float maxChi2 = std::numeric_limits<float>::min();
  float chi2_onHit = std::numeric_limits<float>::max();
  for( auto itH = track.hits().begin(); track.hits().end() != itH; ++itH ){
    chi2_onHit = track.chi2( *itH);
    if ( chi2_onHit > maxChi2 ){
      maxChi2 = chi2_onHit;
    }
  }//Set Max Chi2DoF
  float X0 = track.ax() - track.bx()*m_zReference+track.cx()*m_ConstC;
  track.setX0(X0);
  if( (track.size()>10) && maxChi2<m_maxChi2HitFullFitHigh[iCase]) return true;
  if( std::fabs(track.y(0.))< m_maxY0Low[iCase] &&
      maxChi2<m_maxChi2HitFullFitLow[iCase] &&
      track.size()<11 &&
      std::fabs(track.y(zRef)) < m_maxYZrefLow[iCase] ) return true;
  return false;
}
//=======================================
//Fit Only X Projection
//=======================================
bool PrHybridSeeding::fitXProjection(PrHybridSeedTrack& track, unsigned int iCase ){
  if (m_debug) debug()<<"Fitting"<<endmsg;
  if(track.size()<m_minXPlanes) return false;
  float mat[6];
  float rhs[3];
  const float zRef = m_zReference;
  for(unsigned int loop = 0;3>loop;++loop){
    std::fill(mat,mat+6,0.);
    std::fill(rhs,rhs+3,0.);
    for( auto itH = track.hits().begin(); track.hits().end() != itH; ++itH ){
      const float dRatio = track.dRatio();
      float w = (*itH)->w();//squared
      const float dz= 0.001*((*itH)->z() - zRef);
      const float deta = dz*dz*(1. + dRatio*dz);
      const float dist = track.distance( *itH );
      if (m_debug) debug()<<"Loop \t"<<loop<<"\n Distance From Hit \t"<<dist<<endmsg;
      mat[0]+= w;
      mat[1]+= w * dz;   mat[2]+= w * dz * dz;
      mat[3]+= w * deta; mat[4]+= w * dz * deta;  mat[5]+= w * deta * deta;
      rhs[0]+= w * dist;
      rhs[1]+= w * dist * dz;
      rhs[2]+= w * dist * deta;
    }
    ROOT::Math::CholeskyDecomp<float,3> decomp(mat);
    if(!decomp){
      return false;
    }
    //Solve linear system
    decomp.Solve(rhs);
    rhs[1]*=1.e-3;
    rhs[2]*=1.e-6;
    if (m_debug) debug()<<"Loop \t"<<loop<<"\n a = \t"<<rhs[0]<<"\n b = \t"<<rhs[1]<<"\n c = \t"<<rhs[2]<<endmsg;
    // protect against unreasonable track parameter corrections
    // (check that out)
    if(std::fabs(rhs[0]) > 1.e4 || std::fabs(rhs[1]) > 5. ||
       std::fabs(rhs[2]) > 1.e-3 ) return false;
    //Small corrections
    track.updateParameters(rhs[0],rhs[1],rhs[2],0.,0.);
    if(loop==0){
      track.setdRatio(m_dRatio);
    }
    //Put back later faster maybe
    if(loop >0 && std::abs(rhs[0]) < 5e-5 && std::abs(rhs[1]) < 5e-8 &&
       std::abs(rhs[2]) < 5e-11){
      break;
    }
  }
  //Compute some values on the track
  float maxChi2 = 0.;
  for ( auto itH = track.hits().begin(); track.hits().end() != itH; ++itH )  //Loop over all hits in PrHybridSeedTrack
  {
    float chi2_onHit = track.chi2( *itH );
    if ( chi2_onHit > maxChi2 ){
      maxChi2 = chi2_onHit;
    }
  }
  // float X0 = track.ax() - track.bx()*m_geoTool->zReference()+track.cx()*m_ConstC;
  //float X0 = track.ax() - track.bx()*m_zReference+track.cx()*m_ConstC;
  return (maxChi2 < m_maxChi2HitsX[iCase]) ;
  return false;
}



bool PrHybridSeeding::removeWorstAndRefit(PrHybridSeedTrack& track, unsigned int iCase){
  //maybe useless?
  // bool removeX = false;
  // bool removeUV = false;
  // if( track.nx() == 4) removeUV = true;
  // if( track.ny() == 4) removeX = true;
  if(m_debug) debug()<<"Removing Worst UV layer"<<endmsg;
  auto worst= track.hits().begin();
  // auto worstUV = track.hits().begin();
  // auto worstDupl = track.hits().begin();
  float maxChi2 = std::numeric_limits<float>::min();
  float chi_hit = std::numeric_limits<float>::min();
  if( track.nx() <4 || track.ny()<4) return false;
  for( auto itH = track.hits().begin(); track.hits().end() != itH; ++itH ){
    // if( removeUV && (*itH)->isX() ) continue;
    // if( removeX && !(*itH)->isX() ) continue;
    chi_hit = track.chi2( (*itH));
    if( chi_hit  > maxChi2){
      maxChi2 = chi_hit;
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
  if(track.size()<=m_minXPlanes) return false;
  if (m_debug) debug()<<"Removing Worst and Refitting"<<endmsg;
  //Find maxChi2 contribution of an Hit
  float maxChi2 = 0.;
  auto worst = track.hits().begin();
  for ( auto itH = track.hits().begin(); track.hits().end() != itH; ++itH ){
    float chi2 = track.chi2( *itH );
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
  //for ( auto itH = track.hits().begin(); track.hits().end() != itH; ++itH )
  for (PrHit* hit : track.hits()){
    //const float d = track.distance( hit );
    if ( hit->dxDy() != 0 ) hasStereo = true;
    //hasStereo = hasStereo || (hit->dxDy() != 0);
    //const float w = hit->w();
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
  float chi2 = 0.;
  int   nDoF = -3;  // Fitted a parabola
  //bool hasStereo = false;
  //for ( auto itH = track.hits().begin(); track.hits().end() != itH; ++itH ) {
  for (PrHit* hit : track.hits()) {
    if(hit->dxDy() !=0 && m_debug) debug()<<"You were picking up Stereo Layers!!!"<<endmsg;
    const float Chi2Hit = track.chi2( hit);
    // const float d = track.distance( hit );
    // const float w = hit->w();
    //chi2 += w * d * d;
    chi2+=  Chi2Hit;
    nDoF += 1;
  }
  if (m_debug) debug()<<"Chi2 Set for track = \t"<<chi2<<endmsg;
  track.setChi2( chi2, nDoF );
}


//===============================================
//  Print the whole track
//=========================================================================
void PrHybridSeeding::printTrack ( PrHybridSeedTrack& track ) {
  for ( auto itH = track.hits().begin(); track.hits().end() != itH; ++itH ) {
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


void PrHybridSeeding::findXProjections(unsigned int part, unsigned int iCase){
  m_xCandidates[part].clear();
  PrHits parabolaSeedHits;
  parabolaSeedHits.reserve(12);

  std::vector<PrHits> xHitsLists; //vector of list of Hits
  xHitsLists.reserve(100);
  PrHits xHits;
  xHits.reserve(6);

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
  if (m_debug) debug()<<"\t Loading Case Hit in first and last Zone"<<endmsg;
  PrHitZone* fZone = m_zones[firstZoneId];
  PrHitZone* lZone = m_zones[lastZoneId];
  
  if (m_debug) debug()<<"Hits in last and first Zone Loaded"<<endmsg;
  //load hits in first zone and last one
  PrHits& fHits = fZone->hits();
  PrHits& lHits = lZone->hits();
  float zFirst     = fZone->z(0.); //First  Zone Z value
  float zLast      = lZone->z(0.); //First  Zone Z value
  float DeltaZ = zLast-zFirst;     //Delta First-Last
  std::vector<PrHitZone*> xZones;
  xZones.reserve(4);
  for (unsigned int xZoneId : {s_T1X1, s_T1X2, s_T2X1, s_T2X2, s_T3X1, s_T3X2}){
    xZoneId |= part;
    if (xZoneId != firstZoneId && xZoneId != lastZoneId){
      //should i have to remove the layers already used?
      xZones.push_back(m_zones[xZoneId]);
    }
  }
  //auto itLBeg = lHits.begin();
  if (m_debug) debug()<<"Hits in the InBetween Zones Loaded"<<endmsg;
  if (m_debug) debug()<<"Will Loop over Hits in first Zone"<<endmsg;
  //for(auto itF = fHits.begin(); fHits.end() != itF; ++itF){
  for (PrHit* fHit : fHits){
    if(  fHit->isUsed() && m_removeFlagged) continue; // Only for case 0 by construction!
    ////define search window as a function of the x in the first layer
    float tx_inf =  fHit->x()/zFirst;
    float xProjeInf = tx_inf*zLast;
    float tolHp = m_TolFirstLast[iCase];
    //From Branch ImproveIt we change the alphaCOrrection : case 1 : 1988.63; case 2 : 2354.0
    //For all cases except case = 0
    float maxXl = xProjeInf + tx_inf*m_alphaCorrection[iCase]  +tolHp;
    float minXl = xProjeInf + tx_inf*m_alphaCorrection[iCase]  -tolHp;;
    if(maxXl < minXl){ //should never happen!
      std::swap( minXl, maxXl);
    }
    if(m_debug) debug()<<"iCase "<<iCase<<"\t X last Min \t "<<minXl<<"\n\t\t\t Max X last \t "<<maxXl<<endmsg;
    if(m_debug) debug()<<"Will Loop over Last Layer"<<endmsg;
    auto itL = std::lower_bound( lHits.begin(), lHits.end(), minXl, lowerBoundX());
    auto itLEnd = std::upper_bound( lHits.begin(), lHits.end(), maxXl, upperBoundX());
    PrHit* lHit;
    //loop trough hits in last layer
    for (; itLEnd != itL; ++itL) {
      lHit = *itL;
      //should never happen in principle 
      // if(nullptr == lHit){
      //   if (m_debug) debug()<<"Not Picking Hits in Last layer in the good search Window"<<endmsg;
      //   break;
      // }
      if(lHit->isUsed() && m_removeFlagged) continue;
      float tx_pickedcombination = (lHit->x()-fHit->x())/DeltaZ;
      parabolaSeedHits.clear();
      float x0 = fHit->x()-tx_pickedcombination*zFirst;
      float CorrX0 = m_x0Corr[iCase]*x0;
      float x0new = x0*(1.+m_x0Corr[iCase]);
      if( m_debug ) debug() <<" x0 " << x0 << "CorrX0" << CorrX0 << "x0new" << x0new << "slope"<< m_x0Corr[iCase]<< endmsg;
        if (m_debug) debug()<<"Will loop over Parabola Seed Hits: n Layers"<<m_zones.size()<<endmsg;
        for(PrHitZone* xZone : {m_zones[s_T2X1 | part], m_zones[s_T2X2 | part]}) {
          float xProjected = x0 + xZone->z()*tx_pickedcombination;
          float xProjectedCorrected = xProjected+CorrX0;
          float xMax =0.;
          float xMin =0.;
          float max = 0.;
          float min = 0.;
          float slope = (m_tolAtX0Cut[iCase]-m_TolX0SameSign[iCase])/(m_x0Cut[iCase]-m_x0SlopeChange[iCase]);
          float slopeopp = (m_tolAtx0CutOppSig[iCase] -m_tolX0Oppsig[iCase])/(m_x0Cut[iCase]-m_x0SlopeChange2[iCase]);

          if(x0>0.){
            max = m_TolX0SameSign[iCase];
            //max= 1.2; //tight ?
            //min = x0>400.? -m_hp4_slope*x0 : -1.0; // <0
            min = x0 > m_x0SlopeChange[iCase]?  -slope*( x0 - m_x0SlopeChange[iCase]) - m_TolX0SameSign[iCase] : -m_TolX0SameSign[iCase];
            xMin = xProjectedCorrected + min;
            max = x0>m_x0SlopeChange2[iCase]? slopeopp*( x0 - m_x0SlopeChange2[iCase]) + m_tolX0Oppsig[iCase] : +m_tolX0Oppsig[iCase];
            xMax = xProjectedCorrected + max;
          }else{
            //max = x0<-400. ? -m_hp4_slope*: 1.0; // >0
            max = x0 <-m_x0SlopeChange[iCase]? -slope*( x0 + m_x0SlopeChange[iCase]) + m_TolX0SameSign[iCase]: m_TolX0SameSign[iCase];
            //min = -1.2;
            min = x0 < - m_x0SlopeChange2[iCase]? slopeopp*( x0 + m_x0SlopeChange2[iCase]) - m_tolX0Oppsig[iCase]: -m_tolX0Oppsig[iCase] ;
            xMin = xProjectedCorrected + min;
            xMax = xProjectedCorrected + max;
          }
          if(xMin > xMax) always()<<"Error xMin xMax"<<endmsg;
          if( xMax<xMin && m_debug) debug()<<"\t\t\t\t\t Wrong xMax/xMin"<<endmsg;
          if( m_debug) debug()<<"Lower bound the zones"<<endmsg;
          auto itH = std::lower_bound(xZone->hits().begin(), xZone->hits().end(), xMin, lowerBoundX());
          if( itH == xZone->hits().end()) continue; //next Zone
          PrHit* mHit = nullptr;;
          if(m_debug )debug()<<"Will Loop over xZones Hits"<<endmsg;
          for(; xZone->hits().end() != itH; ++itH){
            mHit = *itH;
            if( mHit == nullptr) break;
            if( mHit->x() > xMax ) break;
            // we can try to avoid this test
            if( mHit->isUsed() && m_removeFlagged) continue; //Not re use Hits in the middle
            if( m_debug) debug()<<"Filling Parabola Seed Hits"<<endmsg;
            parabolaSeedHits.push_back(mHit);
          }//Look for another Hit in last layer
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
                       // float lhsx0 =0 ;
                       // float rhsx0 =0 ;
                       // lhsx0 = x0new;
                       // rhsx0 = x0new;
                       return std::fabs( lhs->x() - ( x0new + lhs->z()*tx_pickedcombination)) < std::fabs( rhs->x() - (x0new + rhs->z()*tx_pickedcombination));} );
        }
        if(m_debug) debug()<<"The Lambda Function Sorting end"<<endmsg;
        unsigned int maxParabolaSeedHits = m_maxParabolaSeedHits;
        if(parabolaSeedHits.size()<m_maxParabolaSeedHits){
          maxParabolaSeedHits = parabolaSeedHits.size();
        }
         if( parabolaSeedHits.size()>m_maxParabolaSeedHits){
           maxParabolaSeedHits = parabolaSeedHits.size();
         }
         for(unsigned int i = 0; i<maxParabolaSeedHits;++i) //build a parabola for each 3 hit combination
        {
          float a = 0;
          float b = 0;
          float c = 0;
          //PrHits xHits;
          xHits.clear();
          solveParabola(fHit,parabolaSeedHits[i],lHit,a,b,c); //Extrapolation with dRatio
          if (m_debug){
            debug()<<"Parabola Par"
                   <<"\n a \t"<<a
                   <<"\n b \t"<<b
                   <<"\n c \t"<<c<<endmsg;
          }
          //===================================================
          // Look in all the other layers except the
          // 1st/last/zone except the parabolaSeedHit
          //===================================================
          //Loop on all the xZones
          //for ( std::vector<PrHitZone*>::iterator itZ = xZones.begin(); xZones.end() != itZ; ++itZ )
          for (PrHitZone* xZone : xZones){
            if (m_debug) debug()<<"Selecting ParSeedHits"<<endmsg;
            if((int)xZone->planeCode() == (int)parabolaSeedHits[i]->planeCode()) continue;
            // float dz   = xZone->z() - m_geoTool->zReference();
            float dz = xZone->z() - m_zReference;
            float xAtZ= a * dz * dz * (1. + m_dRatio* dz) + b * dz + c; //with Cubic Correction
            float xMaxAtZ = xAtZ + m_tolRemaining[iCase];
            float xMinAtZ = xAtZ - m_tolRemaining[iCase];
            PrHit* bestProj = nullptr;
            float  bestDist = 10.0; // 1 cm 
            if (xMinAtZ > xMaxAtZ){
              if (m_debug) debug()<<"Bad Settings!!!!!!"<<endmsg;
            }
            auto itH = std::lower_bound(xZone->hits().begin() ,xZone->hits().end(),xMinAtZ,lowerBoundX());
            PrHit* hit;
            for (; xZone->hits().end() != itH; ++itH){
              hit = *itH;
              if (hit->isUsed() && m_removeFlagged ) continue; 
              //try to remove it ? allow for the remaining layers to pick up flagged hits?
              if (hit->x() < xMinAtZ ) continue;
              if (hit->x() > xMaxAtZ ) break;
              //Find Hit with Best distance < 1 cm
              if(std::fabs(hit->x() - xAtZ)  <  bestDist){
                bestDist = std::fabs(hit->x() - xAtZ);
                if (m_debug) debug()<<"I found an Hit from projection"<<endmsg;
                bestProj = hit;
              }
            }
            if(bestProj != nullptr){
              xHits.push_back(bestProj);
            }
          }//end loop xZones
          //in xHits are not present the first layer and last + parabola seed hits
          if (m_debug) debug()<<"End Loop in between zones to pick up Projection of parabola"<<endmsg;
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
          //Still in the L0 loop (selection last layer) : 
          // By Construction here:
          //at this step we have 1 Hit in 1st Layer
          //at this step we have 1 Hit in last Layer
          //at this step we have 1 Hit in Middle Layer
          //at this step we have Hit in remaining X layers at the
          //best distance to extrapolated parabola All of them are
          //inside xHits i want to have at least min_HitXSize
          //1st Case keep only 6 Hits on found track
          //2nd Case keep tracks with 4/5/6 hits
          std::sort(xHits.begin(), xHits.end(), compLHCbID());
          bool isEqual = false;
          // Remove xHits in the xHitsLists which are basically the same
          for(PrHits& hits : xHitsLists){
            if(m_debug) debug()<<"looping on xHitsLists"<<endmsg;
            if(hits == xHits){
              isEqual = true;
              break;
            }
          }
          if(!isEqual){
            if (m_debug) debug()<<"Pushing Back xHits List"<<endmsg;
            xHitsLists.push_back( xHits);
          }
        }
         //End loop parabolaSeedHits
        if (m_debug) debug()<<"End Loop For pick up Parabola Hits and build the xHitsLists"<<endmsg;
        //End loop Parabola Seed Hits
        //-------- Remove Duplicates from search in parabolaSeedHits
        if(m_debug) debug()<<"xHitsLists size before removing duplicates: "<<xHitsLists.size()<<endmsg;
        if(xHitsLists.size() == 0){
          continue;
        }
        if(xHitsLists.size() > 1){
          //---Remove Duplicates in the HitsList
          std::stable_sort( xHitsLists.begin(), xHitsLists.end());
          xHitsLists.erase( std::unique(xHitsLists.begin(), xHitsLists.end()), xHitsLists.end());
        }
        if(m_debug) debug()<<"xHitsLists size after removing duplicates: "<<xHitsLists.size()<<endmsg;
        //Now let's fit the track
        for(PrHits& xHits : xHitsLists){
          if(m_debug) debug()<<"Fit Track"<<endmsg;
          //Create the track
          PrHybridSeedTrack temp_track( part , m_zReference , xHits);
          //Setters for it: usefull later to parametrise
          temp_track.setdRatio(m_dRatio);
          //-----------------------------------------------------
          //----------------O-_The Fit_-O------------------
          //-----------------------------------------------------
          int nIter = 0;
          if (m_debug ){ 
            debug()<<"Attempting to Fit the following Track"<<endmsg; printTrack(temp_track);
          }
          bool OK = false;
          if(temp_track.size()>m_minXPlanes){ //no 4 hits at first fit
            OK = fitXProjection(temp_track,iCase);
          }
          while(!OK && temp_track.size()>m_minXPlanes){
            if(temp_track.size() <=m_minXPlanes){
              OK = false;
              break;
            }
            nIter++;
            if( nIter==1 && temp_track.size() == 5){ OK = false; break;}
            if( temp_track.size() > m_minXPlanes){
              OK = removeWorstAndRefitX(temp_track,iCase);
            }
          }
          if( OK ){
            setChi2X(temp_track);
          }
          if( OK &&
              temp_track.size() >= m_minXPlanes &&
              (( temp_track.chi2PerDoF() < m_maxChi2DoFX[iCase]))){
            temp_track.setCase( iCase );
            // useful later in clone killing step
            temp_track.setXT1(temp_track.x( (m_zones[0]->z() )));
            temp_track.setXT2(temp_track.x( (m_zones[12]->z())));
            temp_track.setXT3(temp_track.x( (m_zones[23]->z())));  
            temp_track.updateIDs();//for bloom filter
            m_xCandidates[(int)part].push_back(temp_track); //The X Candidate is created
          }
        }//end Loop xHist:xHitsLists
    }//end loop Last Zone given a firsZone selected
  }//end loop first zone
}





