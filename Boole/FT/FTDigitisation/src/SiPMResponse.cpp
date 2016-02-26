// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/PhysicalConstants.h"

// boost
#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_of.hpp>

// std
#include <algorithm>

// local
#include "SiPMResponse.h"

using namespace boost;

//-----------------------------------------------------------------------------
// Implementation file for class : SiPMResponse
//
//   This class describes the SiPM response to a single pulse
//
// 2013-11-12 : Maurizio Martinelli
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( SiPMResponse )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SiPMResponse::SiPMResponse( const std::string& type,
                            const std::string& name,
                            const IInterface* parent)
  : GaudiTool ( type, name , parent ), m_responseSpline(0)
{
  declareInterface<SiPMResponse>(this);
  // properties
  declareProperty("samplingDt",  m_samplingDt = 0.1 * Gaudi::Units::ns, "Sampling time step" );
  declareProperty("times",  m_times, "Times for user-defined function" );
  declareProperty("values", m_values, "Values for user-defined function" );
  declareProperty("splineType", m_splineType = "Cspline", "The spline type" );
  declareProperty("useNewResponse", m_useNewResponse = 1, "Use the new, data-driven electronics response shape" ); 
}

//=============================================================================
// Destructor
//=============================================================================
SiPMResponse::~SiPMResponse() 
{
  if (0 != m_responseSpline) delete m_responseSpline;
} 
//=============================================================================
// Initialize the pulse shape
//=============================================================================
StatusCode 
SiPMResponse::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);
  
  // If times and values are empty set default
  if (m_times.size() == 0 && m_values.size() == 0) {
    // prev spill


    if( m_useNewResponse == 1 ) {

    // output of tools/getSiPMResponseShape.py here:

      std::vector<double> values =
        boost::assign::list_of
        (-0.0215125706032) (-0.0231673837265) (-0.0173118911363) (-0.00789218566507) (-0.00992887873992) 
        (-0.0238038503124) (-0.0299139295369) (-0.0304231028057) (-0.0314414493431) (-0.0146387314755) 
        (-0.0132385049866) (-0.0237184706484) (-0.0192942516977) (-0.0318326434398) (-0.00268247380591) 
        (-0.0175121452572) (-0.0155390988409) (-0.0308779435609) (-0.0392156558361) (-0.0525069409786) 
        (0.0155111564055) (0.0332049274933) (0.165208097407) (0.330233016627) (0.54020380902) 
        (0.689767092038) (0.809849242799) (0.90066665994) (0.930192344859) (0.962308448783) 
        (0.964784303803) (0.919832065963) (0.882841559405) (0.737141628563) (0.604256253866) 
        (0.443628387341) (0.334849883146) (0.251434572399) (0.21305563727) (0.165002410035) 
        (0.119049522534) (0.0919996926332) (0.0692778355169) (0.0307522026013) (0.0363531085571) 
        (-5.27801559059e-05) (-0.029330243107) (-0.0162190314376) (-0.0213107641247) (-0.00832684577251) 
        (-0.00578097942894) (0.0105125651699) (0.00834857877787) (0.0250240033283) ;

      std::vector<double> times = 
        boost::assign::list_of
        (-45.2) (-43.06) (-40.92) (-38.78) (-36.64) 
        (-34.5) (-32.36) (-30.22) (-28.08) (-25.94) 
        (-23.8) (-21.66) (-19.52) (-17.38) (-15.24) 
        (-13.1) (-10.96) (-8.82) (-6.68) (-4.54) 
        (-2.4) (-0.26) (1.88) (4.02) (6.16) 
        (8.3) (10.44) (12.58) (14.72) (16.86) 
        (19.0) (21.14) (23.28) (25.42) (27.56) 
        (29.7) (31.84) (33.98) (36.12) (38.26) 
        (40.4) (42.54) (44.68) (46.82) (48.96) 
        (51.1) (53.24) (55.38) (57.52) (59.66) 
        (61.8) (63.94) (66.08) (68.22) ; 
    
        m_times  = times;
        m_values = values;


    } else if ( m_useNewResponse == 2 ) {

      // Use f(t) = 1
      info() << "WARNING: Using flat SiPMResponse!" << endmsg;

      std::vector<double> values =
        boost::assign::list_of
        (1.) (1.) (1.) (1.) (1.) 
        (1.) (1.) (1.) (1.) (1.) 
        (1.) (1.) (1.) (1.) (1.) 
        (1.) (1.) (1.) (1.) (1.) 
        (1.) (1.) (1.) (1.) (1.) 
        (1.) (1.) (1.) (1.) (1.) 
        (1.) (1.) (1.) (1.) (1.) 
        (1.) (1.) (1.) (1.) (1.) 
        (1.) (1.) (1.) (1.) (1.) 
        (1.) (1.) (1.) (1.) (1.) 
        (1.) (1.) (1.) (1.) ;
      
      std::vector<double> times = 
        boost::assign::list_of
        (-45.2) (-43.06) (-40.92) (-38.78) (-36.64) 
        (-34.5) (-32.36) (-30.22) (-28.08) (-25.94) 
        (-23.8) (-21.66) (-19.52) (-17.38) (-15.24) 
        (-13.1) (-10.96) (-8.82) (-6.68) (-4.54) 
        (-2.4) (-0.26) (1.88) (4.02) (6.16) 
        (8.3) (10.44) (12.58) (14.72) (16.86) 
        (19.0) (21.14) (23.28) (25.42) (27.56) 
        (29.7) (31.84) (33.98) (36.12) (38.26) 
        (40.4) (42.54) (44.68) (46.82) (48.96) 
        (51.1) (53.24) (55.38) (57.52) (59.66) 
        (61.8) (63.94) (66.08) (68.22) ; 
    
        m_times  = times;
        m_values = values;

    } else if ( m_useNewResponse == 3 ) {

      // Herve's new shape v1
      
      std::vector<double> values = 
        boost::assign::list_of
      (-0.004869155713) (-0.00458774837201) (-0.00264495986224) (0.0751562685088) (0.239124605441) 
      (0.429799717187) (0.603980435177) (0.742914410283) (0.843973702721) (0.91506540563) 
      (0.96026826976) (0.98651928819) (0.991199696832) (0.995581196178) (0.885083968824) 
      (0.62457297529) (0.404369960247) (0.243516225207) (0.130321819046) (0.0483476593722) 
      (-0.00869890357073) (-0.0495013706019) (-0.0843704970139) (-0.105420128293) (-0.117638221173) 
      (-0.119785887601) (-0.127815112694) (-0.132538056271) (-0.134706422797) (-0.135974686934) 
      (-0.133936796355) (-0.130751443479) (-0.12907193259) (-0.125790749319) (-0.119873735352) 
      (-0.116414231815) (-0.111949922223) (-0.112164968681) (-0.108055809597) (-0.105467281054) 
      (-0.0994356069024) (-0.0945087418386) (-0.0901510648241) (-0.0861436550559) (-0.081491837112) 
      (-0.0779460920812) (-0.0736685107274) (-0.0720503416521) (-0.0688854566965) (-0.0648134122739) 
      (-0.064624872288) (-0.0622951044581) (-0.0621881728599) (-0.0573041085531) (-0.0570813502507) 
      (-0.0554269166805) (-0.0523248625131) (-0.0514483997044) (-0.0498139954378) (-0.0473739892834) 
      (-0.047329516693) (-0.0465311495108) ;

      std::vector<double> times = 
        boost::assign::list_of
      (-8) (-6) (-4) (-2) (0) 
      (2) (4) (6) (8) (10) 
      (12) (14) (16) (18) (20) 
      (22) (24) (26) (28) (30) 
      (32) (34) (36) (38) (40) 
      (42) (44) (46) (48) (50) 
      (52) (54) (56) (58) (60) 
      (62) (64) (66) (68) (70) 
      (72) (74) (76) (78) (80) 
      (82) (84) (86) (88) (90) 
      (92) (94) (96) (98) (100) 
      (102) (104) (106) (108) (110) 
      (112) (114) ; 

      m_times = times;
      m_values = values;

    } else if ( m_useNewResponse == 4 ) {

      // Herve's new shape v2
      
      std::vector<double> values = 
        boost::assign::list_of
      (-0.000629890317322) (-0.00417290949048) (0.0982995589966) (0.350062744828) (0.562620082078) 
      (0.713573363265) (0.817195956444) (0.887416931875) (0.934124765348) (0.964353824168) 
      (0.982257923463) (0.990811990483) (0.993470038566) (0.997294262036) (0.89262223573) 
      (0.632344630466) (0.411097659514) (0.249848377303) (0.13512664194) (0.0529708663533) 
      (-0.00635897070887) (-0.0493174261614) (-0.0803965798988) (-0.101697568176) (-0.116173121342) 
      (-0.119956787331) (-0.128294261725) (-0.132857879785) (-0.133792775886) (-0.133220159255) 
      (-0.13173598583) (-0.129523411434) (-0.126558535531) (-0.122463311824) (-0.117946238201) 
      (-0.113158301946) (-0.107984422642) (-0.101822394348) (-0.0958740169387) (-0.0910938487292) 
      (-0.0868869696091) (-0.0828540668475) (-0.0787617046219) (-0.0745873580922) (-0.0702813205894) 
      (-0.066296308351) (-0.0625394098693) (-0.0586297154686) (-0.0552694322032) (-0.0522803644448) 
      (-0.0520436271235) (-0.0493421093184) (-0.0471967235778) (-0.0455192850154) (-0.0436512144117) 
      (-0.0417143311206) (-0.0397082109403) (-0.0377472769556) (-0.0358715642284) (-0.0340734208293) 
      (-0.0324188040746) (-0.0309765919704) ;
      
      std::vector<double> times = 
        boost::assign::list_of
      (-6) (-4) (-2) (0) (2) 
      (4) (6) (8) (10) (12) 
      (14) (16) (18) (20) (22) 
      (24) (26) (28) (30) (32) 
      (34) (36) (38) (40) (42) 
      (44) (46) (48) (50) (52) 
      (54) (56) (58) (60) (62) 
      (64) (66) (68) (70) (72) 
      (74) (76) (78) (80) (82) 
      (84) (86) (88) (90) (92) 
      (94) (96) (98) (100) (102) 
      (104) (106) (108) (110) (112) 
      (114) (116) ; 

      m_times = times;
      m_values = values;

    } else {
    
      // use old SiPMResponse
      info() << "WARNING: Using old SiPMResponse!" << endmsg;

      std::vector<double> Tprev =  
        boost::assign::list_of
        (3.22188688e-05)(3.22145863e-05)(3.22145863e-05)(3.22145863e-05)(3.22145863e-05)
        (3.22145863e-05)(3.22145863e-05)(3.22145863e-05)(3.22145863e-05)(3.22145863e-05)
        (3.22145863e-05)(3.22145863e-05)(3.22145863e-05)(3.22145863e-05)(3.22145863e-05)
        (3.22145863e-05)(3.22145863e-05)(3.22145863e-05)(3.22145863e-05)(3.22145863e-05)
        (3.22145863e-05)(3.22145863e-05)(3.22145863e-05)(3.22145863e-05)(3.22145863e-05)(3.22145863e-05);
      // current spill
      std::vector<double> Tcurr =  
        boost::assign::list_of
        (2.88048286e-02)(2.76166647e-02)(2.84853231e-02)(2.86509678e-02)(2.87964850e-02)
        (2.90245415e-02)(2.92078012e-02)(2.93951976e-02)(2.95711670e-02)(2.96709382e-02)
        (2.97279647e-02)(2.96909134e-02)(2.94095934e-02)(2.89146629e-02)(2.80610778e-02)
        (2.67112946e-02)(2.47312816e-02)(2.20768793e-02)(1.86479251e-02)(1.44897527e-02)
        (9.82891757e-03)(5.23039367e-03)(1.56929340e-03)(-2.17283810e-04)(-1.82921036e-05)(6.66654783e-05);
      // next spill
      std::vector<double> Tnext =  
        boost::assign::list_of
        (-0.00136548)(-0.00089388)(-0.0012617)(-0.00137867)(-0.00145512)
        (-0.00160822)(-0.00173438)(-0.00187304)(-0.00201678)(-0.00213902)
        (-0.0022567)(-0.00234672)(-0.0023399)(-0.00226133)(-0.00202669)
        (-0.00154593)(-0.00067318)(0.00060853)(0.00250173)(0.00515046)
        (0.00876429)(0.01327495)(0.01883291)(0.02372474)(0.02788753)(0.0281931);
        // nextnext spill
      std::vector<double> Tnext2 =  
        boost::assign::list_of
        (-0.00034101)(-0.00025578)(-0.00030324)(-0.00031672)(-0.00032477)
        (-0.00034131)( -0.00035811)(-0.00037248)(-0.00038796)(-0.00040745)
        (-0.00042273)(-0.00044454)(-0.00046555)(-0.00048982)(-0.00051403)
        (-0.00054482)(-0.00056937)(-0.00060258)(-0.00064924)(-0.00068913)
        (-0.00070694)(-0.00073142)(-0.00099868)(-0.00083826)(-0.00119224)(-0.00104176);
      // reverse the vectors
      std::reverse(Tprev.begin(), Tprev.end());
      std::reverse(Tcurr.begin(), Tcurr.end());
      std::reverse(Tnext.begin(), Tnext.end());
      std::reverse(Tnext2.begin(), Tnext2.end());
      // set times
      for (int i = -25; i < 76; ++i) m_times.push_back(i);
      // set values
      for (int i = 0; i < 26; ++i) m_values.push_back(Tprev[i]-Tprev[0]);
      for (int i = 1; i < 26; ++i) m_values.push_back(Tcurr[i]-Tcurr[0]+m_values[25]);
      for (int i = 1; i < 26; ++i) m_values.push_back(Tnext[i]-Tnext[0]+m_values[50]);
      for (int i = 1; i < 26; ++i) m_values.push_back(Tnext2[i]-Tnext2[0]+m_values[75]);
    }

  }


  // Check if the data is provided and the times and values are of equal length
  if (m_times.size() == 0) return Error("No data !", StatusCode::FAILURE);
  if (m_times.size() != m_values.size()) {
    return Error("inconsistant data !", StatusCode::FAILURE);
  }
  
  // Store the first and last entry of the vector of times
  m_tMin = m_times.front();
  m_tMax = m_times.back();

  // Set maximum to 1
  double maxVal(-99999.);
  for (std::vector<double>::iterator it = m_values.begin(); 
       it != m_values.end(); ++it)
    if (*it > maxVal) maxVal = *it;

  if ( msgLevel(MSG::DEBUG) ){
    debug() << "SiPMResponse before reweighting:"<<endmsg;
    debug() << "----------------------------" <<endmsg;
    std::vector<double> vals(m_values);
    for (unsigned int i =0; i < m_values.size(); ++i)
      debug() << "\t t = " << m_times[i] << "\t val = " << vals[i] << " / " << maxVal << " = " << vals[i] /  maxVal << endmsg;
  }

  for (unsigned int i =0; i < m_values.size(); ++i)
    m_values[i] = m_values[i] / maxVal;

  if ( msgLevel(MSG::DEBUG) ){
    debug() << "SiPMResponse max = " << maxVal << endmsg;
    debug() << "SiPMResponse after reweighting:" <<endmsg;
    debug() << "----------------------------" <<endmsg;
    for (unsigned int i =0; i < m_values.size(); ++i)
      debug() << "\t t = " << m_times[i] << "\t val = " << m_values[i] << endmsg;
    
  }
  
  // Fit the spline to the data
  m_responseSpline = new GaudiMath::SimpleSpline( m_times, m_values,
						  typeFromString() );
  /*
  // dump to screen 
  if (m_printToScreen == true) {
    printToScreen();
  }
  if (m_printForRoot == true) {
    printForRoot();
  }
  */
  
  return sc;
}

double 
SiPMResponse::response(const double time) const
{
  return ((time>m_tMin)&&(time<m_tMax) ? m_responseSpline->eval(time) : 0.0);
}

GaudiMath::Interpolation::Type 
SiPMResponse::typeFromString() const
{  
  GaudiMath::Interpolation::Type aType;
  using namespace GaudiMath::Interpolation;
  if      (m_splineType == "Cspline")          aType = Cspline;
  else if (m_splineType == "Linear")           aType = Linear;
  else if (m_splineType == "Polynomial")       aType = Polynomial;
  else if (m_splineType == "Akima")            aType = Akima;
  else if (m_splineType == "Akima_Periodic")   aType = Akima_Periodic;
  else if (m_splineType == "Cspline_Periodic") aType = Cspline_Periodic;
  else aType = Cspline; // default to cubic
  
  return aType;
}

void 
SiPMResponse::sample(std::vector<double>& times, std::vector<double>& values) const
{
  double t = m_tMin;
  while(t < m_tMax) {
  times.push_back(t);
  values.push_back(response(t));
  t += m_samplingDt; 
  } // loop times
}

//=============================================================================
