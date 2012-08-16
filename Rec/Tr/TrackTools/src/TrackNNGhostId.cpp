// Include files 
#include "Event/Track.h"
//#include "Kernel/HitPattern.h"


// from Gaudi
#include "GaudiKernel/ToolFactory.h"

#include "Event/OTTime.h"
#include "Event/STCluster.h"
#include "Event/VeloCluster.h"

#include "Event/KalmanFitResult.h"

#include "TMVA/TMVAClassification_MLP_fittedVelo.C"
#include "TMVA/TMVAClassification_MLP_fittedUpstream.C"
#include "TMVA/TMVAClassification_MLP_fittedDownstream.C"
#include "TMVA/TMVAClassification_MLP_fittedTtrack.C"
#include "TMVA/TMVAClassification_MLP_fittedLong.C"
#include "TMVA/TMVAClassification_MLP_fittedLong_MC2012.C"

// local
#include "TrackNNGhostId.h"


#include "Kernel/HitPattern.h"

#include "TrackInterfaces/IHitExpectation.h"
#include "TrackInterfaces/IVeloExpectation.h"

//boost
#include <boost/assign/list_of.hpp>

//#include "Event/RecSummary.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackNNGhostId
//
// 2009-10-06 : Johannes Albrecht
// 2012-08-16 : JA: implement new tuning on MC2012
//
//  Tool to calculate a track quality variable using a trained (TMVA) netork
//
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TrackNNGhostId )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackNNGhostId::TrackNNGhostId( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent ),
  m_otdecoder(0),
  m_readerVelo(0),
  m_readerUpstream(0),
  m_readerDownstream(0),
  m_readerTtrack(0),
  m_readerLong(0),
  m_readerLong12(0),
  m_ttExpectation(0),
  m_itExpectation(0),
  m_otExpectation(0),
  m_veloExpectation(0)
{
  declareInterface<ITrackManipulator>(this);
  declareProperty("IsMC2012Tuning",m_tuningMC12=true);
  
}
//=============================================================================
// Destructor
//=============================================================================
TrackNNGhostId::~TrackNNGhostId() {} 

//=============================================================================

StatusCode TrackNNGhostId::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }
  m_veloExpectation = tool<IVeloExpectation>("VeloExpectation");
  m_ttExpectation = tool<IHitExpectation>("TTHitExpectation");
  m_itExpectation = tool<IHitExpectation>("ITHitExpectation");
  m_otExpectation = tool<IHitExpectation>("OTHitExpectation");


  /**
   *   initialize input variable names as used for training
   *   and class readers for all track types
   *
   */
  //velo tracks
  m_inNames.clear();
  m_inNames.push_back( "track_probChi2" );
  m_inNames.push_back( "nVeloHits+nTTHits+nITHits+nOTHits" );
  m_inNames.push_back( "track_veloHits" );
  m_readerVelo = new ReadMLP_fittedVelo( m_inNames );

  //upstream tracks
  m_inNames.clear();
  m_inNames.push_back( "track_probChi2" );
  m_inNames.push_back( "track_ttHits" );
  m_inNames.push_back( "nVeloHits+nTTHits+nITHits+nOTHits" );
  m_inNames.push_back( "track_veloHits" );
  m_readerUpstream = new ReadMLP_fittedUpstream( m_inNames );

  //downstream tracks
  m_inNames.clear();
  m_inNames.push_back( "track_probChi2" );
  m_inNames.push_back( "track_ttHits" );
  m_inNames.push_back( "nVeloHits+nTTHits+nITHits+nOTHits" );
  m_inNames.push_back( "track_itHits" );
  m_inNames.push_back( "track_otHits" );
  m_readerDownstream = new ReadMLP_fittedDownstream( m_inNames );

  //Ttracks
  m_inNames.clear();
  m_inNames.push_back( "track_probChi2" );
  m_inNames.push_back( "nVeloHits+nTTHits+nITHits+nOTHits" );
  m_inNames.push_back( "track_itHits" );
  m_inNames.push_back( "track_otHits" );
  m_readerTtrack = new ReadMLP_fittedTtrack( m_inNames );

  //long tracks
  m_inNames.clear();


  // MC09 tuning
  ///m_inNames.push_back( "track_probChi2" );
  ///m_inNames.push_back( "track_fitMatchChi2" );
  ///m_inNames.push_back( "track_ttHits" );
  ///m_inNames.push_back( "track_nCandCommonHits" );
  ///m_inNames.push_back( "nVeloHits+nTTHits+nITHits+nOTHits" );
  ///m_inNames.push_back( "track_veloHits" );
  ///m_inNames.push_back( "track_itHits" );
  ///m_inNames.push_back( "track_otHits" );
  ///m_readerLong = new ReadMLP_fittedLong( m_inNames );

  // first studies on MC11
  ///m_inNames.push_back("track_Chi2Dof");
  ///m_inNames.push_back( "track_MatchChi2");
  ///m_inNames.push_back( "Vchi");
  ///m_inNames.push_back( "Tchi");
  ///m_inNames.push_back( "(observedV-expectedV)");
  ///m_inNames.push_back( "(observedIT-expectedIT)");
  ///m_inNames.push_back( "(observedTT-expectedTT)");
  ///m_inNames.push_back( "(observedOT-expectedOT)");
  ///m_inNames.push_back( "OTbaddrifttime");
  ///m_inNames.push_back( "eta");
  ///m_inNames.push_back( "pt" );
  ///m_readerLong = new ReadBDT( m_inNames );

  //MC2012 tuning for long tracks
  m_inNames.clear();
  m_inNames.push_back("track_Chi2");
  m_inNames.push_back("track_Dof");
  m_inNames.push_back("track_MatchChi2");
  m_inNames.push_back("Vchi*Vdof");
  m_inNames.push_back("Vdof");
  m_inNames.push_back("Tchi*Tdof");
  m_inNames.push_back("Tdof");
  m_inNames.push_back("observedV");
  m_inNames.push_back("observedIT");
  m_inNames.push_back("observedTT");
  m_inNames.push_back("observedOT");
  m_inNames.push_back("(observedV-expectedV)");
  m_inNames.push_back("(observedIT-expectedIT)");
  m_inNames.push_back("(observedTT-expectedTT)");
  m_inNames.push_back("(observedOT-expectedOT)");
  m_inNames.push_back("OThits");
  m_inNames.push_back("IThits");
  m_inNames.push_back("TThits");
  m_inNames.push_back("VELOhits");
  m_inNames.push_back("(OTbaddrifttime+OToutliers)");
  m_inNames.push_back("eta");
  m_inNames.push_back("pt");
  m_readerLong12 = new ReadMLP_fittedLong12( m_inNames );
  
  
  m_inputVec = new std::vector<double>;
  
  m_otdecoder = tool<IOTRawBankDecoder>("OTRawBankDecoder");

  m_veloExpectation = tool<IVeloExpectation>("VeloExpectation");
  m_ttExpectation = tool<IHitExpectation>("TTHitExpectation");
  m_itExpectation = tool<IHitExpectation>("ITHitExpectation");
  m_otExpectation = tool<IHitExpectation>("OTHitExpectation");
  
  m_transformationMap = boost::assign::list_of(0.999956)(0.999956)(0.999956)(0.999956)(0.999956)(0.999956)(0.999953)(0.999953)(0.999953)(0.999951)(0.999951)(0.999951)(0.999951)(0.999948)(0.999946)(0.999941)(0.999938)(0.999938)(0.999933)(0.999933)(0.99993)(0.99993)(0.999928)(0.99992)(0.99991)(0.99991)(0.999907)(0.999904)(0.999902)(0.999897)(0.999894)(0.999891)(0.999886)(0.999873)(0.999868)(0.999863)(0.99986)(0.99986)(0.999858)(0.999855)(0.99985)(0.999837)(0.999827)(0.999817)(0.999814)(0.999811)(0.999806)(0.999796)(0.999788)(0.999775)(0.999767)(0.99976)(0.999744)(0.999742)(0.999736)(0.999729)(0.999716)(0.999695)(0.99969)(0.999672)(0.999664)(0.999649)(0.999638)(0.999628)(0.99961)(0.999594)(0.999584)(0.999555)(0.999535)(0.999527)(0.999512)(0.999491)(0.999478)(0.99946)(0.999429)(0.999406)(0.999375)(0.999346)(0.999313)(0.999276)(0.999235)(0.999204)(0.999165)(0.999116)(0.999062)(0.999021)(0.998995)(0.99894)(0.998894)(0.99886)(0.998796)(0.998734)(0.998669)(0.998586)(0.998522)(0.998462)(0.99839)(0.998318)(0.998237)(0.998157)(0.998064)(0.997958)(0.997868)(0.997777)(0.997659)(0.99755)(0.99746)(0.997361)(0.997245)(0.997095)(0.996958)(0.996808)(0.996661)(0.996542)(0.996359)(0.996165)(0.996012)(0.995824)(0.99563)(0.995431)(0.995252)(0.995028)(0.994834)(0.994568)(0.994338)(0.994064)(0.993808)(0.993547)(0.993301)(0.993043)(0.99279)(0.992492)(0.992174)(0.991846)(0.991523)(0.991185)(0.990856)(0.990464)(0.990081)(0.989683)(0.989246)(0.988804)(0.988321)(0.98789)(0.987473)(0.986951)(0.986481)(0.985962)(0.985406)(0.984832)(0.984155)(0.983499)(0.982858)(0.98215)(0.981369)(0.980568)(0.97979)(0.978898)(0.978053)(0.977131)(0.976048)(0.974931)(0.973807)(0.972618)(0.971298)(0.969987)(0.968579)(0.967072)(0.965346)(0.963508)(0.961531)(0.959446)(0.957246)(0.954623)(0.952008)(0.949098)(0.945888)(0.942285)(0.938435)(0.934222)(0.929565)(0.924391)(0.918739)(0.91273)(0.906078)(0.898495)(0.890367)(0.881389)(0.871186)(0.859998)(0.848151)(0.834971)(0.820801)(0.805757)(0.790034)(0.773173)(0.756181)(0.738821)(0.720857)(0.702746)(0.684898)(0.666603)(0.648796)(0.631561)(0.615259)(0.599967)(0.58498)(0.570334)(0.556859)(0.544087)(0.531894)(0.520127)(0.509063)(0.498871)(0.489337)(0.480364)(0.4719)(0.463888)(0.456461)(0.44933)(0.442802)(0.436711)(0.431017)(0.425487)(0.42022)(0.415297)(0.410851)(0.406634)(0.402646)(0.398875)(0.395118)(0.391794)(0.388437)(0.385369)(0.382364)(0.379518)(0.376779)(0.374207)(0.371726)(0.369183)(0.366948)(0.364795)(0.3625)(0.360484)(0.358409)(0.356385)(0.354398)(0.352527)(0.350687)(0.348852)(0.347118)(0.345358)(0.343763)(0.342047)(0.340359)(0.338755)(0.337173)(0.335661)(0.334064)(0.332588)(0.331035)(0.329549)(0.328081)(0.326476)(0.325153)(0.323796)(0.322447)(0.321033)(0.319684)(0.318263)(0.316953)(0.315632)(0.314399)(0.313053)(0.311704)(0.310355)(0.309161)(0.307876)(0.30668)(0.30547)(0.304245)(0.303028)(0.301873)(0.300733)(0.299562)(0.298376)(0.297187)(0.296063)(0.294965)(0.29391)(0.292887)(0.29169)(0.290561)(0.289344)(0.288206)(0.287033)(0.285958)(0.284896)(0.283875)(0.282808)(0.281709)(0.280701)(0.279696)(0.278654)(0.277566)(0.276582)(0.275633)(0.274703)(0.273721)(0.272806)(0.271873)(0.270971)(0.270129)(0.269188)(0.268221)(0.267286)(0.266361)(0.26542)(0.264513)(0.263551)(0.262626)(0.261597)(0.26075)(0.259827)(0.258928)(0.258039)(0.257119)(0.256188)(0.25524)(0.254364)(0.253532)(0.252661)(0.251697)(0.250766)(0.249888)(0.248988)(0.248187)(0.247422)(0.246582)(0.245688)(0.244889)(0.244099)(0.243272)(0.242421)(0.241625)(0.240806)(0.239989)(0.239188)(0.238493)(0.237728)(0.236924)(0.236201)(0.23534)(0.234523)(0.233766)(0.233017)(0.232303)(0.231554)(0.230753)(0.230008)(0.229298)(0.228489)(0.227778)(0.226977)(0.226233)(0.225486)(0.22484)(0.224132)(0.223408)(0.222651)(0.222023)(0.221296)(0.220552)(0.21994)(0.21927)(0.218531)(0.217864)(0.217236)(0.216588)(0.215952)(0.215303)(0.214613)(0.213952)(0.213362)(0.212685)(0.212021)(0.211403)(0.210781)(0.210124)(0.209496)(0.208775)(0.208124)(0.207498)(0.206904)(0.206328)(0.205658)(0.20501)(0.20441)(0.20381)(0.203151)(0.202567)(0.202004)(0.201428)(0.20088)(0.20028)(0.19966)(0.199058)(0.198437)(0.197828)(0.19728)(0.196659)(0.196083)(0.195476)(0.194835)(0.194246)(0.193651)(0.193093)(0.192566)(0.191958)(0.191333)(0.19085)(0.190348)(0.189821)(0.189242)(0.18873)(0.188136)(0.187609)(0.187025)(0.186518)(0.18596)(0.185412)(0.184885)(0.184358)(0.18381)(0.183277)(0.18274)(0.182156)(0.181605)(0.181037)(0.180504)(0.179962)(0.17945)(0.178938)(0.178512)(0.178005)(0.177496)(0.177018)(0.17655)(0.175992)(0.175581)(0.175041)(0.174563)(0.174064)(0.173594)(0.173048)(0.172568)(0.17209)(0.171606)(0.171182)(0.170733)(0.170221)(0.169751)(0.169293)(0.168782)(0.168327)(0.1679)(0.167422)(0.166939)(0.166502)(0.166117)(0.165722)(0.165295)(0.164812)(0.164352)(0.163889)(0.163463)(0.163003)(0.162607)(0.162166)(0.16178)(0.161328)(0.160884)(0.160447)(0.160015)(0.159607)(0.1591)(0.158684)(0.15825)(0.157757)(0.157335)(0.156948)(0.15649)(0.156046)(0.155609)(0.155188)(0.154699)(0.154291)(0.153908)(0.153508)(0.153097)(0.152689)(0.152208)(0.151815)(0.151474)(0.151032)(0.150626)(0.150236)(0.149794)(0.149378)(0.148988)(0.148616)(0.148218)(0.147794)(0.147445)(0.14708)(0.14669)(0.146359)(0.145995)(0.145556)(0.14509)(0.144703)(0.144253)(0.143824)(0.143452)(0.143031)(0.142674)(0.142304)(0.141953)(0.141568)(0.141193)(0.140774)(0.140351)(0.139976)(0.139583)(0.139195)(0.138818)(0.138495)(0.138128)(0.13774)(0.137412)(0.137004)(0.136658)(0.136254)(0.135869)(0.135458)(0.13512)(0.134753)(0.134422)(0.134091)(0.13367)(0.133272)(0.13289)(0.132535)(0.132163)(0.131835)(0.13145)(0.131078)(0.130708)(0.130349)(0.129969)(0.129607)(0.129246)(0.12891)(0.128566)(0.128199)(0.127796)(0.127408)(0.127031)(0.126702)(0.126359)(0.126)(0.125633)(0.125247)(0.124839)(0.124475)(0.124103)(0.123741)(0.123387)(0.123069)(0.122702)(0.122348)(0.121963)(0.121642)(0.121299)(0.120921)(0.120552)(0.120167)(0.119849)(0.119464)(0.119117)(0.118766)(0.118435)(0.118135)(0.11781)(0.117419)(0.117081)(0.116678)(0.116313)(0.115902)(0.115582)(0.115181)(0.114876)(0.114463)(0.114054)(0.113726)(0.113344)(0.112959)(0.112548)(0.112191)(0.111855)(0.111449)(0.11109)(0.110731)(0.110348)(0.110028)(0.109684)(0.109333)(0.108981)(0.108643)(0.108278)(0.107924)(0.107601)(0.10725)(0.106883)(0.106521)(0.106175)(0.105702)(0.105324)(0.104939)(0.104577)(0.104205)(0.103903)(0.103567)(0.103259)(0.10296)(0.102657)(0.102321)(0.101985)(0.101639)(0.101311)(0.100936)(0.100626)(0.100313)(0.0999462)(0.099618)(0.0992614)(0.0989202)(0.0985946)(0.0982121)(0.097796)(0.0973877)(0.0970724)(0.0967002)(0.0963178)(0.0959456)(0.0956148)(0.0952375)(0.0948214)(0.0944777)(0.0941262)(0.0937437)(0.0934284)(0.0931054)(0.0927513)(0.0923688)(0.0920432)(0.091671)(0.0912963)(0.0909396)(0.0905416)(0.0902289)(0.0898826)(0.0894588)(0.0891409)(0.0887274)(0.0883682)(0.0880425)(0.0877117)(0.0874145)(0.0870294)(0.0866702)(0.0863317)(0.0860267)(0.0856907)(0.0852721)(0.0849025)(0.0845665)(0.0842641)(0.0839256)(0.0835638)(0.0831916)(0.0828789)(0.0825507)(0.0821785)(0.0818348)(0.0815014)(0.0811138)(0.0807959)(0.0804134)(0.0800438)(0.0796949)(0.0793486)(0.0789816)(0.0786534)(0.0782864)(0.0779014)(0.0775163)(0.077139)(0.076834)(0.0764308)(0.0760845)(0.0757382)(0.0754307)(0.0751102)(0.0747898)(0.0744435)(0.0740842)(0.073725)(0.0733528)(0.0729652)(0.0726163)(0.0722984)(0.0719107)(0.0715799)(0.0711768)(0.0708175)(0.0704919)(0.0701327)(0.0697347)(0.0693289)(0.0689439)(0.0685149)(0.0681737)(0.0677421)(0.0673234)(0.0669048)(0.0665662)(0.0661811)(0.065809)(0.065411)(0.0650388)(0.0646331)(0.0642584)(0.0638759)(0.0634934)(0.0631367)(0.0627361)(0.0623537)(0.0619738)(0.0615887)(0.0612114)(0.060741)(0.0603482)(0.0599166)(0.0595367)(0.0591568)(0.0587458)(0.0583478)(0.0580119)(0.0576371)(0.0572572)(0.0568308)(0.0564225)(0.0560839)(0.0556575)(0.0552259)(0.0548537)(0.0544764)(0.0540087)(0.0536623)(0.0532127)(0.0528431)(0.0524606)(0.0520678)(0.0516078)(0.0512382)(0.0508376)(0.0503698)(0.0499615)(0.0495712)(0.0491422)(0.0487494)(0.0483592)(0.0479302)(0.0474831)(0.0470566)(0.0466561)(0.0462529)(0.0458678)(0.0454466)(0.0449891)(0.0445679)(0.0441466)(0.0437434)(0.0433765)(0.0429862)(0.04249)(0.0420558)(0.0416966)(0.0412366)(0.0407817)(0.0403501)(0.039903)(0.039443)(0.0390218)(0.0386083)(0.038218)(0.037789)(0.0373497)(0.0368664)(0.0363598)(0.0358274)(0.0353545)(0.0348919)(0.034419)(0.0339512)(0.0334524)(0.0329924)(0.0325659)(0.0321137)(0.0315761)(0.0310696)(0.0305656)(0.0301418)(0.0295991)(0.0291494)(0.0286738)(0.0281983)(0.0276168)(0.0270586)(0.0265314)(0.0260662)(0.0255597)(0.0249756)(0.0244251)(0.0238979)(0.0233242)(0.022753)(0.0221663)(0.0215745)(0.020962)(0.0203573)(0.0196957)(0.0190392)(0.0183621)(0.0177083)(0.0170699)(0.016491)(0.0158061)(0.0151187)(0.0144804)(0.0138007)(0.0130357)(0.0124154)(0.0117331)(0.0109914)(0.0103066)(0.00952349)(0.00883604)(0.00801937)(0.0072208)(0.00651009)(0.00578904)(0.00510159)(0.0045201)(0.00395154)(0.00346826)(0.00299531)(0.00261799)(0.00224325)(0.00194088)(0.00163851)(0.00142917)(0.00127411)(0.00116298)(0.00106477)(0.000953641)(0.000876109)(0.0008115)(0.000752058)(0.000679695)(0.00064093)(0.000586657)(0.000542723)(0.000516879)(0.000483282)(0.000452269)(0.000410919)(0.000390243)(0.000361815)(0.000333387)(0.000304958)(0.000292036)(0.000284283)(0.000250686)(0.000240349)(0.000227427)(0.000204167)(0.000186076)(0.000167986)(0.000160232)(0.00014731)(0.000136973)(0.000131804)(0.000124051)(0.000113713)(9.82069e-05)(8.78694e-05)(8.78694e-05)(7.75318e-05)(7.49474e-05)(6.71942e-05)(6.71942e-05)(5.68567e-05)(5.68567e-05)(5.42723e-05)(5.42723e-05)(5.42723e-05)(5.16879e-05)(4.91035e-05)(4.91035e-05)(4.65191e-05)(4.13503e-05)(3.61815e-05)(3.35971e-05)(2.84283e-05)(2.84283e-05)(2.84283e-05)(2.32595e-05)(2.32595e-05)(1.80908e-05)(1.80908e-05)(1.80908e-05)(1.80908e-05)(1.80908e-05)(1.55064e-05)(1.55064e-05)(1.55064e-05)(1.55064e-05)(1.2922e-05)(1.2922e-05)(1.2922e-05)(1.2922e-05)(1.2922e-05)(1.2922e-05)(1.2922e-05)(1.2922e-05)(1.03376e-05)(1.03376e-05)(7.75318e-06)(7.75318e-06)(7.75318e-06)(7.75318e-06)(5.16879e-06)(2.58439e-06)(2.58439e-06)(2.58439e-06)(2.58439e-06)(2.58439e-06)(2.58439e-06)(2.58439e-06)(2.58439e-06)(2.58439e-06)(2.58439e-06)(2.58439e-06)(2.58439e-06)(2.58439e-06)(2.58439e-06)(2.58439e-06)(2.58439e-06)(2.58439e-06)(2.58439e-06)(2.58439e-06)(2.58439e-06)(2.58439e-06)(2.58439e-06)(2.58439e-06)(2.58439e-06)(2.58439e-06)(2.58439e-06)(2.58439e-06)(2.58439e-06)(2.58439e-06)(2.58439e-06)(2.58439e-06)(2.58439e-06)(2.58439e-06)(2.58439e-06)(2.58439e-06)(2.58439e-06)(2.58439e-06)(2.58439e-06)(2.58439e-06)(2.58439e-06)(2.58439e-06)(2.58439e-06)(0)(0)(0)(0);
    
  

  return StatusCode::SUCCESS;
}


StatusCode TrackNNGhostId::execute(LHCb::Track& aTrack) const{
  
  bool isDebug   = msgLevel(MSG::DEBUG);
  double retval = 0;
  if ( UNLIKELY( isDebug ) ) debug() << "==> Execute" << endmsg;
  //always()<<endmsg;
  //always()<<"--> execute, m_tuningMC12 = "<<m_tuningMC12<<endmsg;

  int veloHits = 0, ttHits=0,itHits=0,otHits=0;  
  const std::vector<LHCb::LHCbID> ids = aTrack.lhcbIDs();
  for(unsigned int i = 0; i < ids.size(); ++i){
    if(ids[i].isVelo()) ++veloHits;
    if(ids[i].isTT()) ++ttHits;
    if(ids[i].isIT()) ++itHits;
    if(ids[i].isOT()) ++otHits;
  }

  
  int nVeloCont = 0;
  int nTTCont = 0;
  int nITCont = 0;


  if (exist<LHCb::VeloClusters>(LHCb::VeloClusterLocation::Default)){
    LHCb::VeloClusters* veloCont = get<LHCb::VeloClusters>(LHCb::VeloClusterLocation::Default);
    nVeloCont = veloCont->size();
  }
  if (exist<LHCb::STClusters>(LHCb::STClusterLocation::TTClusters)){
    LHCb::STClusters *ttCont = get<LHCb::STClusters>(LHCb::STClusterLocation::TTClusters);
    nTTCont = ttCont->size();
  }
  if (exist<LHCb::STClusters>(LHCb::STClusterLocation::ITClusters)){
    LHCb::STClusters *itCont = get<LHCb::STClusters>(LHCb::STClusterLocation::ITClusters);
    nITCont = itCont->size();
  }

  int nHitMult = nVeloCont + nTTCont + nITCont + m_otdecoder->totalNumberOfHits();


  
  /**
   *   now evaluate the MVA value for different track types
   *
   */
  if( aTrack.checkType(LHCb::Track::Velo) ){
    m_inputVec->clear();
    m_inputVec->reserve(3);
    m_inputVec->push_back( aTrack.probChi2() );
    m_inputVec->push_back( nHitMult );
    m_inputVec->push_back( veloHits );
    // retrive the classifier responses            
    retval = m_readerVelo->GetMvaValue( *m_inputVec );
  }//end evaluate velo track

  if( aTrack.checkType(LHCb::Track::Upstream) ){
    m_inputVec->clear();
    m_inputVec->reserve(4);
    m_inputVec->push_back( aTrack.probChi2() );
    m_inputVec->push_back( ttHits );
    m_inputVec->push_back( nHitMult );
    m_inputVec->push_back( veloHits );
    // retrive the classifier responses            
    retval = m_readerUpstream->GetMvaValue( *m_inputVec );
  }//end evaluate upstream track

  if( aTrack.checkType(LHCb::Track::Downstream) ){
    m_inputVec->clear();
    m_inputVec->reserve(5);
    m_inputVec->push_back( aTrack.probChi2() );
    m_inputVec->push_back( ttHits );
    m_inputVec->push_back( nHitMult );
    m_inputVec->push_back( itHits );
    m_inputVec->push_back( otHits );
    // retrive the classifier responses            
    retval = m_readerDownstream->GetMvaValue( *m_inputVec );
  }//end evaluate downstream track
  
  if( aTrack.checkType(LHCb::Track::Ttrack) ){ 
    m_inputVec->clear();
    m_inputVec->reserve(4);
    m_inputVec->push_back( aTrack.probChi2() );
    m_inputVec->push_back( nHitMult );
    m_inputVec->push_back( itHits );
    m_inputVec->push_back( otHits );
    // retrive the classifier responses            
    retval = m_readerTtrack->GetMvaValue( *m_inputVec );
  }//end evaluate Ttrack track
  
  if( !m_tuningMC12 && aTrack.checkType(LHCb::Track::Long) ){
    if ( UNLIKELY( isDebug ) ) debug()<<"run mc09 tuning"<<endmsg;
    
    aTrack.setGhostProbability(-888.);
    m_inputVec->clear();
    ///m_inputVec->reserve(8);
    ///m_inputVec->push_back( aTrack.probChi2() );
    ///m_inputVec->push_back( aTrack.info(LHCb::Track::FitMatchChi2,-60) );
    ///m_inputVec->push_back( ttHits);
    ///m_inputVec->push_back( aTrack.info(LHCb::Track::NCandCommonHits,-60));
    ///m_inputVec->push_back( nHitMult);
    ///m_inputVec->push_back( veloHits);
    ///m_inputVec->push_back( itHits);
    ///m_inputVec->push_back( otHits);
    ///if(-60 != (*m_inputVec)[1]
    ///   && -60 != (*m_inputVec)[3] )
    ///  // retrive the classifier responses            
    if (!aTrack.hasInfo(21)) {
      aTrack.setGhostProbability(-999.);
      return Warning("Input variable 21 missing",StatusCode::SUCCESS, 1);
      if ( UNLIKELY( isDebug ) ) debug()<<"MC09:  NN output:  "<<retval<<endmsg;
    } else {
      if( UNLIKELY( isDebug ) ) debug()<<"warning: extra info of track not propetly filled ....  "<<endmsg;
    }
  }//end evaluate long track for Mc09 tuning

  /*
   *   transform MC09 classifier 
   *   MC2012 classifier gets transformed separately
   */
  //normalize the output to the interval [0,1]
  if(retval<-1) retval=-1;
  if(retval>1) retval=1;
  retval = -0.5*double(retval-1.);
  
  if( UNLIKELY ( isDebug ) ) {
    for(unsigned int i=0;i<m_inputVec->size();i++)  debug()<<"input variable [ "<<i<<" ] :"<<(*m_inputVec)[i]<<endmsg;
    debug() << "direct NN output is: "<<retval<<endmsg;
    debug() << "normlized NN output is: "<<retval<<endmsg;
  } 


  //MC2012 tuning
  if( m_tuningMC12 && aTrack.checkType(LHCb::Track::Long) ){
    
    aTrack.setGhostProbability(-888.);
    m_inputVec->clear();
    if (!aTrack.hasInfo(21)) {
      aTrack.setGhostProbability(-999.);
      if ( UNLIKELY( isDebug ) ) debug()<<"21.quit"<<endmsg;
      return Warning("Input variable 21 missing",StatusCode::SUCCESS, 1);
    }

    if (!aTrack.hasInfo(17)) {
      aTrack.setGhostProbability(-999.);
      if ( UNLIKELY( isDebug ) ) debug()<<"17.quit"<<endmsg;
      return Warning("Input variable 17 missing",StatusCode::SUCCESS, 1);
    }
    if (!aTrack.hasInfo(18)) {
      aTrack.setGhostProbability(-999.);
      if ( UNLIKELY( isDebug ) ) debug()<<"18.quit"<<endmsg;
      return Warning("Input variable 18 missing",StatusCode::SUCCESS, 1);
    }

    LHCb::HitPattern observedpattern(aTrack.lhcbIDs());
    LHCb::HitPattern expectedpattern;
    std::vector<LHCb::LHCbID> ids;
    {
      std::vector<LHCb::LHCbID> ids;
      ids.reserve(2 * aTrack.nLHCbIDs());

      std::bitset<23> velo[4];

      m_veloExpectation->expectedInfo(aTrack, velo);
      m_ttExpectation->collect(aTrack, ids);
      m_itExpectation->collect(aTrack, ids);
      m_otExpectation->collect(aTrack, ids);

      expectedpattern = LHCb::HitPattern(ids);
      expectedpattern.setVeloRA(velo[0]);
      expectedpattern.setVeloRC(velo[1]);
      expectedpattern.setVeloPhiA(velo[2]);
      expectedpattern.setVeloPhiC(velo[3]);
      
    }
    float expectedV;
    {
      std::bitset< 23 > ar = expectedpattern.veloRA()  .to_ulong();
      std::bitset< 23 > cr = expectedpattern.veloRC()  .to_ulong();
      std::bitset< 23 > ap = expectedpattern.veloPhiA().to_ulong();
      std::bitset< 23 > cp = expectedpattern.veloPhiC().to_ulong();
      float ret = 0.;
      for (int i = 0 ; i < 23 ; ++i) {
        if (ar[i]) ret+=1.;
        if (ap[i]) ret+=1.;
        if (cr[i]) ret+=1.;
        if (cp[i]) ret+=1.;
      }
      expectedV = ret;
      if ( UNLIKELY( isDebug ) ) debug() << "expectedV = " << ret << endmsg;
    }
    float observedV;
    {
      std::bitset< 23 > ar = observedpattern.veloRA()  .to_ulong();
      std::bitset< 23 > cr = observedpattern.veloRC()  .to_ulong();
      std::bitset< 23 > ap = observedpattern.veloPhiA().to_ulong();
      std::bitset< 23 > cp = observedpattern.veloPhiC().to_ulong();
      float ret = 0.;
      for (int i = 0 ; i < 23 ; ++i) {
        if (ar[i]) ret+=1.;
        if (ap[i]) ret+=1.;
        if (cr[i]) ret+=1.;
        if (cp[i]) ret+=1.;
      }
      observedV = ret;
      if ( UNLIKELY( isDebug ) ) debug() << "observedV = " << ret << endmsg;
    }


    float observedTT;
    {
      std::bitset< 4 > obs = observedpattern.tt().to_ulong();
      observedTT =  (obs[0]
                    +obs[1]
                    +obs[2]
                    +obs[3]);
      if ( UNLIKELY( isDebug ) ) debug() << "observedTT = " << observedTT << endmsg;
    }

    float expectedTT;
    {
      std::bitset< 4 > exp = expectedpattern.tt().to_ulong();
      expectedTT =  (exp[0]
                    +exp[1]
                    +exp[2]
                    +exp[3]);
      if ( UNLIKELY( isDebug ) ) debug() << "expectedTT = " << expectedTT << endmsg;
    }

  
    float observedIT;
    {
      std::bitset<12> dump = observedpattern.itAC().to_ulong();
      float ret = 0.;
      for (int i = 0 ; i < 12 ; ++i)
        if (dump[i]) ret+=1.;
      dump = observedpattern.itTopBottom().to_ulong();
      for (int i = 0 ; i < 12 ; ++i)
        if (dump[i]) ret+=1.;
      observedIT = ret;
      if ( UNLIKELY( isDebug ) ) debug() << "observedIT = " << ret << endmsg;
    }
  
    float expectedIT;
    {
      std::bitset<12> dump = expectedpattern.itAC().to_ulong();
      float ret = 0.;
      for (int i = 0 ; i < 12 ; ++i)
        if (dump[i]) ret+=1.;
      dump = expectedpattern.itTopBottom().to_ulong();
      for (int i = 0 ; i < 12 ; ++i)
        if (dump[i]) ret+=1.;
      expectedIT=  ret;
      if ( UNLIKELY( isDebug ) ) debug() << "expectedIT = " << ret << endmsg;
    }

    float observedOT;
    {
      std::bitset<12> dump = observedpattern.ot1stMonoLayer().to_ulong();
      float ret = 0.;
      for (int i = 0 ; i < 12 ; ++i)
        if (dump[i]) ret+=1.;
      dump = observedpattern.ot2ndMonoLayer().to_ulong();
      for (int i = 0 ; i < 12 ; ++i)
        if (dump[i]) ret+=1.;
      observedOT =  ret;
      if ( UNLIKELY( isDebug ) ) debug() << "observedOT = " << ret << endmsg;
    }
 
    float expectedOT;
    {
      std::bitset<12> dump = expectedpattern.ot1stMonoLayer().to_ulong();
      float ret = 0.;
      for (int i = 0 ; i < 12 ; ++i)
        if (dump[i]) ret+=1.;
      dump = expectedpattern.ot2ndMonoLayer().to_ulong();
      for (int i = 0 ; i < 12 ; ++i)
        if (dump[i]) ret+=1.;
      expectedOT =  ret;
      if ( UNLIKELY( isDebug ) ) debug() << "expectedOT = " << ret << endmsg;
    }

    const LHCb::KalmanFitResult* kalfit =static_cast<const LHCb::KalmanFitResult*>(aTrack.fitResult()) ;
    if( !kalfit ) {
      aTrack.setGhostProbability(-999.);
      if ( UNLIKELY( isDebug ) ) debug()<<"fit result not avialable... quit"<<endmsg;
      return Warning("kalman fit result not available ... end loop and set ghostProb = -999.",StatusCode::SUCCESS, 1);
    }

    float nMeas = kalfit->nMeasurements( LHCb::Measurement::OT );
    float nOTBad = nMeas-nMeas*aTrack.info(25 , 0 );// info 25 : FitFracUsedOTTimes

    if ( UNLIKELY( isDebug ) ) {
      debug()<<"nOTMeas: "<< nMeas<<endmsg;
      debug()<<"nOTBad: "<<nOTBad<<endmsg;
    }

    /*
     *    now fill the input vector to evaluate the NN
     */
    m_inputVec->reserve(22);
    m_inputVec->push_back( aTrack.chi2() );
    m_inputVec->push_back( aTrack.nDoF() );
    m_inputVec->push_back( aTrack.info(21,-1.) );
    m_inputVec->push_back( aTrack.info(17,-1.) );
    m_inputVec->push_back( aTrack.info(18,-1.) );
    m_inputVec->push_back( aTrack.info(19,-1.) );
    m_inputVec->push_back( aTrack.info(20,-1.) );
    m_inputVec->push_back( observedV );
    m_inputVec->push_back( observedIT );
    m_inputVec->push_back( observedTT );
    m_inputVec->push_back( observedOT );
    m_inputVec->push_back( (observedV-expectedV));
    m_inputVec->push_back( (observedIT-expectedIT));
    m_inputVec->push_back( (observedTT-expectedTT));
    m_inputVec->push_back( (observedOT-expectedOT));
    m_inputVec->push_back( m_otdecoder->totalNumberOfHits() );
    m_inputVec->push_back( nITCont );
    m_inputVec->push_back( nTTCont );
    m_inputVec->push_back( nVeloCont );
    m_inputVec->push_back( nOTBad );
    m_inputVec->push_back( aTrack.pseudoRapidity() );
    m_inputVec->push_back( aTrack.pt() );

    if ( UNLIKELY( isDebug ) ) {
      debug()<<"***   print all ***"<<endmsg;
      debug()<<"aTrack.chi2()           :"<< aTrack.chi2()<<endmsg;
      debug()<<"aTrack.nDoF()           :"<< aTrack.nDoF()<<endmsg;
      debug()<<"aTrack.info(21,-1.)     :"<< aTrack.info(21,-1.)<<endmsg;
      debug()<<"aTrack.info(17,-1.)     :"<< aTrack.info(17,-1.)<<endmsg;
      debug()<<"aTrack.info(18,-1.)     :"<<aTrack.info(18,-1.) <<endmsg;
      debug()<<"aTrack.info(19,-1.)     :"<<aTrack.info(19,-1.) <<endmsg;
      debug()<<"aTrack.info(20,-1.)     :"<< aTrack.info(20,-1.)<<endmsg;
      debug()<<"observedV               :"<< observedV<<endmsg;
      debug()<<"observedIT              :"<< observedIT<<endmsg;
      debug()<<"observedTT              :"<< observedTT<<endmsg;
      debug()<<"observedOT              :"<< observedOT<<endmsg;
      debug()<<"(observedV-expectedV)   :"<<(observedV-expectedV) <<endmsg;
      debug()<<"(observedIT-expectedIT) :"<< (observedIT-expectedIT)<<endmsg;
      debug()<<"(observedTT-expectedTT) :"<< (observedTT-expectedTT)<<endmsg;
      debug()<<"(observedOT-expectedOT) :"<< (observedOT-expectedOT)<<endmsg;
      debug()<<"OThits                  :"<<m_otdecoder->totalNumberOfHits() <<endmsg;
      debug()<<"IThits                  :"<<nITCont <<endmsg;
      debug()<<"TThits                  :"<<nTTCont <<endmsg;
      debug()<<"VELOhits                :"<<nVeloCont <<endmsg;
      debug()<<"nOTBad                  :"<<nOTBad <<endmsg;
      debug()<<"aTrack.pseudoRapidity() :"<<aTrack.pseudoRapidity() <<endmsg;
      debug()<<"aTrack.pt()             :"<<aTrack.pt() <<endmsg;
      debug()<<"***"<<endmsg;
    }

    retval = m_readerLong12->GetMvaValue( *m_inputVec );
  
    if ( UNLIKELY( isDebug ) ) {
      debug()<<" NN output:  "<<retval<<endmsg;
      debug()<<"now tranform"<<endmsg;
    }
    

    int index = (retval + 0.3) / (1.5) * 1000.;
    if (index >= 1000)
      return 0;
    if (index < 0)
      return 1;
    
    retval = m_transformationMap[index];

    if ( UNLIKELY( isDebug ) ) debug()<<"transformed value: "<<retval<<endmsg;
    

  }//end evaluate long track for MC2012 tuning  
  
    
  
  


  aTrack.setGhostProbability(retval);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode TrackNNGhostId::finalize()
{
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Finalize" << endmsg;

  return GaudiTool::finalize();  // must be called after all other actions
}
