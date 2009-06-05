// $Id: RichLocalDefaultTemplateData.cpp,v 1.2 2009-06-05 17:21:34 jonrob Exp $
// Include files 

// local
#include "RichLocalDefaultTemplateData.h"

using namespace Rich::Rec::TemplateRings;

//-----------------------------------------------------------------------------
// Implementation file for class : RichLocalDefaultTemplateData
//
// 2008-11-26 : Sajan EASO
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichLocalDefaultTemplateData* RichLocalDefaultTemplateData::m_RichLocalDefaultTemplateDataInstance=0;

RichLocalDefaultTemplateData::RichLocalDefaultTemplateData() {
  
  DefineLocalDefaultTemplateData() ;
  
}
//=============================================================================
// Destructor
//=============================================================================
RichLocalDefaultTemplateData::~RichLocalDefaultTemplateData() {} 

//=============================================================================
void RichLocalDefaultTemplateData::DefineLocalDefaultTemplateData() 
{
  // First for aerogel
  m_aerogelDefaultTemplate_X.clear(); m_aerogelDefaultTemplate_Y.clear();
    m_aerogelDefaultTemplate_X.resize(138);
    m_aerogelDefaultTemplate_Y.resize(138);
    
 m_aerogelDefaultTemplate_X[0]=-141.99;    m_aerogelDefaultTemplate_Y[0]=  -307.167;
 m_aerogelDefaultTemplate_X[1]=208.375;    m_aerogelDefaultTemplate_Y[1]=-286.38;
 m_aerogelDefaultTemplate_X[2]=-17.5108;   m_aerogelDefaultTemplate_Y[2]=-340.026;
 m_aerogelDefaultTemplate_X[3]=12.8199;    m_aerogelDefaultTemplate_Y[3]=-340.066;
 m_aerogelDefaultTemplate_X[4]=-94.3834;   m_aerogelDefaultTemplate_Y[4]=-339.898;
 m_aerogelDefaultTemplate_X[5]=215.033;    m_aerogelDefaultTemplate_Y[5]=266.39;
 m_aerogelDefaultTemplate_X[6]=-254.046;   m_aerogelDefaultTemplate_Y[6]=233.041;
 m_aerogelDefaultTemplate_X[7]=-251.315;   m_aerogelDefaultTemplate_Y[7]=233.082;
 m_aerogelDefaultTemplate_X[8]=-332.373;   m_aerogelDefaultTemplate_Y[8]=111.396;
 m_aerogelDefaultTemplate_X[9]=331.198;    m_aerogelDefaultTemplate_Y[9]=-120.576;
 m_aerogelDefaultTemplate_X[10]=-301.569;  m_aerogelDefaultTemplate_Y[10]=-172.347;
 m_aerogelDefaultTemplate_X[11]=-266.922;  m_aerogelDefaultTemplate_Y[11]=-221.436;
 m_aerogelDefaultTemplate_X[12]=89.4225;   m_aerogelDefaultTemplate_Y[12]=-327.371;
 m_aerogelDefaultTemplate_X[13]= -352.497; m_aerogelDefaultTemplate_Y[13]=-2.02254;
 m_aerogelDefaultTemplate_X[14]=291.937;   m_aerogelDefaultTemplate_Y[14]=-189.942;
 m_aerogelDefaultTemplate_X[15]=-256.344;  m_aerogelDefaultTemplate_Y[15]=-231.663;
 m_aerogelDefaultTemplate_X[16]=97.6759;   m_aerogelDefaultTemplate_Y[16]=-329.783;
 m_aerogelDefaultTemplate_X[17]=33.4963;   m_aerogelDefaultTemplate_Y[17]=-334.675;
 m_aerogelDefaultTemplate_X[18]=200.55;    m_aerogelDefaultTemplate_Y[18]=274.3;
 m_aerogelDefaultTemplate_X[19]=185.147;   m_aerogelDefaultTemplate_Y[19]=287.151;
 m_aerogelDefaultTemplate_X[20]=307.744;   m_aerogelDefaultTemplate_Y[20]=-175.039;
 m_aerogelDefaultTemplate_X[21]=255.538;   m_aerogelDefaultTemplate_Y[21]=-237.595;
 m_aerogelDefaultTemplate_X[22]=309.227;   m_aerogelDefaultTemplate_Y[22]=147.321;
 m_aerogelDefaultTemplate_X[23]=-314.709;  m_aerogelDefaultTemplate_Y[23]=142.313;
 m_aerogelDefaultTemplate_X[24]=336.476;   m_aerogelDefaultTemplate_Y[24]=-107.791;
 m_aerogelDefaultTemplate_X[25]=-305.567;  m_aerogelDefaultTemplate_Y[25]=166.119;
 m_aerogelDefaultTemplate_X[26]=347.514;   m_aerogelDefaultTemplate_Y[26]=68.0866;
 m_aerogelDefaultTemplate_X[27]=-339.81;   m_aerogelDefaultTemplate_Y[27]=-99.7022;
 m_aerogelDefaultTemplate_X[28]=-305.852;  m_aerogelDefaultTemplate_Y[28]=-169.468;
 m_aerogelDefaultTemplate_X[29]=-250.273;  m_aerogelDefaultTemplate_Y[29]=-239.173;
 m_aerogelDefaultTemplate_X[30]=31.8113;   m_aerogelDefaultTemplate_Y[30]=-334.449;
 m_aerogelDefaultTemplate_X[31]=149.683;   m_aerogelDefaultTemplate_Y[31]=300.336;
 m_aerogelDefaultTemplate_X[32]=-172.947;  m_aerogelDefaultTemplate_Y[32]=282.261;
 m_aerogelDefaultTemplate_X[33]=-162.296;  m_aerogelDefaultTemplate_Y[33]=290.272;
 m_aerogelDefaultTemplate_X[34]=-118.78;   m_aerogelDefaultTemplate_Y[34]=308.067;
 m_aerogelDefaultTemplate_X[35]=-220.464;  m_aerogelDefaultTemplate_Y[35]=246.208;
 m_aerogelDefaultTemplate_X[36]=-326.169;  m_aerogelDefaultTemplate_Y[36]=132.827;
 m_aerogelDefaultTemplate_X[37]=-303.124;  m_aerogelDefaultTemplate_Y[37]=-179.911;
 m_aerogelDefaultTemplate_X[38]=-209.724;  m_aerogelDefaultTemplate_Y[38]=-265.022;
 m_aerogelDefaultTemplate_X[39]=121.32;    m_aerogelDefaultTemplate_Y[39]=-317.286;
 m_aerogelDefaultTemplate_X[40]=-132.004;  m_aerogelDefaultTemplate_Y[40]=-317.138;
 m_aerogelDefaultTemplate_X[41]=-165.875;  m_aerogelDefaultTemplate_Y[41]=287.333;
 m_aerogelDefaultTemplate_X[42]=254.618;   m_aerogelDefaultTemplate_Y[42]=232.479;
 m_aerogelDefaultTemplate_X[43]=-345.831;  m_aerogelDefaultTemplate_Y[43]=-71.6848;
 m_aerogelDefaultTemplate_X[44]=223.452;   m_aerogelDefaultTemplate_Y[44]=-268.313;
 m_aerogelDefaultTemplate_X[45]=272.813;   m_aerogelDefaultTemplate_Y[45]=-221.585;
 m_aerogelDefaultTemplate_X[46]=-197.934;  m_aerogelDefaultTemplate_Y[46]=-270.388;
 m_aerogelDefaultTemplate_X[47]=-264.46;   m_aerogelDefaultTemplate_Y[47]=-232.188;
 m_aerogelDefaultTemplate_X[48]=135.679;   m_aerogelDefaultTemplate_Y[48]=-309.754;
 m_aerogelDefaultTemplate_X[49]=332.5;     m_aerogelDefaultTemplate_Y[49]=-122.934;
 m_aerogelDefaultTemplate_X[50]=142.18;    m_aerogelDefaultTemplate_Y[50]=-309.069;
 m_aerogelDefaultTemplate_X[51]=240.152;   m_aerogelDefaultTemplate_Y[51]=240.823;
 m_aerogelDefaultTemplate_X[52]=-258.645;  m_aerogelDefaultTemplate_Y[52]=220.495;
 m_aerogelDefaultTemplate_X[53]=-326.342;  m_aerogelDefaultTemplate_Y[53]=114.479;
 m_aerogelDefaultTemplate_X[54]=-326.348;  m_aerogelDefaultTemplate_Y[54]=117.208;
 m_aerogelDefaultTemplate_X[55]=-308.575;  m_aerogelDefaultTemplate_Y[55]=158.249;
 m_aerogelDefaultTemplate_X[56]=-303.241;  m_aerogelDefaultTemplate_Y[56]=160.936;
 m_aerogelDefaultTemplate_X[57]=331.984;   m_aerogelDefaultTemplate_Y[57]=-122.967;
 m_aerogelDefaultTemplate_X[58]=-267.059;  m_aerogelDefaultTemplate_Y[58]=212.971;
 m_aerogelDefaultTemplate_X[59]=325.254;   m_aerogelDefaultTemplate_Y[59]=135.274;
 m_aerogelDefaultTemplate_X[60]=-326.978;  m_aerogelDefaultTemplate_Y[60]=117.249;
 m_aerogelDefaultTemplate_X[61]=133.889;   m_aerogelDefaultTemplate_Y[61]=307.754;
 m_aerogelDefaultTemplate_X[62]=250.6;     m_aerogelDefaultTemplate_Y[62]=232.671;
 m_aerogelDefaultTemplate_X[63]=-308.466;  m_aerogelDefaultTemplate_Y[63]=-172.37;
 m_aerogelDefaultTemplate_X[64]=-313.457;  m_aerogelDefaultTemplate_Y[64]=-154.605;
 m_aerogelDefaultTemplate_X[65]=-139.58;   m_aerogelDefaultTemplate_Y[65]=-309.707;
 m_aerogelDefaultTemplate_X[66]=-144.627;  m_aerogelDefaultTemplate_Y[66]=-307.159;
 m_aerogelDefaultTemplate_X[67]=197.819;   m_aerogelDefaultTemplate_Y[67]=277.873;
 m_aerogelDefaultTemplate_X[68]=116.119;   m_aerogelDefaultTemplate_Y[68]=310.917;
 m_aerogelDefaultTemplate_X[69]=-165.107;  m_aerogelDefaultTemplate_Y[69]=290.821;
 m_aerogelDefaultTemplate_X[70]=250.383;   m_aerogelDefaultTemplate_Y[70]=230.898;
 m_aerogelDefaultTemplate_X[71]=103.495;   m_aerogelDefaultTemplate_Y[71]=-324.13;
 m_aerogelDefaultTemplate_X[72]=108.383;   m_aerogelDefaultTemplate_Y[72]=315.56;
 m_aerogelDefaultTemplate_X[73]=305.12;    m_aerogelDefaultTemplate_Y[73]=170.931;
 m_aerogelDefaultTemplate_X[74]=-194.809;  m_aerogelDefaultTemplate_Y[74]=-280.7;
 m_aerogelDefaultTemplate_X[75]=-311.029;  m_aerogelDefaultTemplate_Y[75]=155.299;
 m_aerogelDefaultTemplate_X[76]=342.546;   m_aerogelDefaultTemplate_Y[76]=85.3042;
 m_aerogelDefaultTemplate_X[77]=227.285;   m_aerogelDefaultTemplate_Y[77]=-260.19;
 m_aerogelDefaultTemplate_X[78]=-217.59;   m_aerogelDefaultTemplate_Y[78]=-262.738;
 m_aerogelDefaultTemplate_X[79]=121.296;   m_aerogelDefaultTemplate_Y[79]=-317.431;
 m_aerogelDefaultTemplate_X[80]=280.57;    m_aerogelDefaultTemplate_Y[80]=210.032;
 m_aerogelDefaultTemplate_X[81]=-256.566;  m_aerogelDefaultTemplate_Y[81]=230.628;
 m_aerogelDefaultTemplate_X[82]=341.748;   m_aerogelDefaultTemplate_Y[82]=80.4198;
 m_aerogelDefaultTemplate_X[83]=-343.364;  m_aerogelDefaultTemplate_Y[83]=-86.90;
 m_aerogelDefaultTemplate_X[84]=-155.376;  m_aerogelDefaultTemplate_Y[84]=295.043;
 m_aerogelDefaultTemplate_X[85]=322.562;   m_aerogelDefaultTemplate_Y[85]=144.958;
 m_aerogelDefaultTemplate_X[86]=-319.445;  m_aerogelDefaultTemplate_Y[86]=140.004;
 m_aerogelDefaultTemplate_X[87]=344.326;   m_aerogelDefaultTemplate_Y[87]=60.0312;
 m_aerogelDefaultTemplate_X[88]=-88.7648;  m_aerogelDefaultTemplate_Y[88]=318.56;
 m_aerogelDefaultTemplate_X[89]=284.257;   m_aerogelDefaultTemplate_Y[89]=204.664;
 m_aerogelDefaultTemplate_X[90]=235.108;   m_aerogelDefaultTemplate_Y[90]=243.405;
 m_aerogelDefaultTemplate_X[91]=326.295;   m_aerogelDefaultTemplate_Y[91]=127.233;
 m_aerogelDefaultTemplate_X[92]=-351.714;  m_aerogelDefaultTemplate_Y[92]=0.603577;
 m_aerogelDefaultTemplate_X[93]=243.109;   m_aerogelDefaultTemplate_Y[93]=-247.157;
 m_aerogelDefaultTemplate_X[94]=151.653;   m_aerogelDefaultTemplate_Y[94]=295.08;
 m_aerogelDefaultTemplate_X[95]=120.524;   m_aerogelDefaultTemplate_Y[95]=310.247;
 m_aerogelDefaultTemplate_X[96]=344.259;   m_aerogelDefaultTemplate_Y[96]=70.2536;
 m_aerogelDefaultTemplate_X[97]=339.329;   m_aerogelDefaultTemplate_Y[97]=-97.2234;
 m_aerogelDefaultTemplate_X[98]=257.292;   m_aerogelDefaultTemplate_Y[98]=-234.75;
 m_aerogelDefaultTemplate_X[99]=46.2835;   m_aerogelDefaultTemplate_Y[99]=-340.011;
 m_aerogelDefaultTemplate_X[100]=15.9074;  m_aerogelDefaultTemplate_Y[100]=-337.4;
 m_aerogelDefaultTemplate_X[101]=43.6203;  m_aerogelDefaultTemplate_Y[101]=-334.854;
 m_aerogelDefaultTemplate_X[102]=43.5705;  m_aerogelDefaultTemplate_Y[102]=-329.808;
 m_aerogelDefaultTemplate_X[103]=257.869;  m_aerogelDefaultTemplate_Y[103]=-237.22;
 m_aerogelDefaultTemplate_X[104]=-281.529; m_aerogelDefaultTemplate_Y[104]=200.127;
 m_aerogelDefaultTemplate_X[105]=307.649;  m_aerogelDefaultTemplate_Y[105]=-174.449;
 m_aerogelDefaultTemplate_X[106]=-316.072; m_aerogelDefaultTemplate_Y[106]=-149.031;
 m_aerogelDefaultTemplate_X[107]=-70.3356; m_aerogelDefaultTemplate_Y[107]=-334.512;
 m_aerogelDefaultTemplate_X[108]=-147.29;  m_aerogelDefaultTemplate_Y[108]=-306.783;
 m_aerogelDefaultTemplate_X[109]=116.345;  m_aerogelDefaultTemplate_Y[109]=315.53;
 m_aerogelDefaultTemplate_X[110]=-126.804; m_aerogelDefaultTemplate_Y[110]=305.551;
 m_aerogelDefaultTemplate_X[111]=319.922;  m_aerogelDefaultTemplate_Y[111]=137.65;
 m_aerogelDefaultTemplate_X[112]=304.781;  m_aerogelDefaultTemplate_Y[112]=181.691;
 m_aerogelDefaultTemplate_X[113]=-314.446; m_aerogelDefaultTemplate_Y[113]=145.31;
 m_aerogelDefaultTemplate_X[114]=249.831;  m_aerogelDefaultTemplate_Y[114]=-247.28;
 m_aerogelDefaultTemplate_X[115]=-269.292; m_aerogelDefaultTemplate_Y[115]=-221.437;
 m_aerogelDefaultTemplate_X[116]=141.016;  m_aerogelDefaultTemplate_Y[116]=-311.954;
 m_aerogelDefaultTemplate_X[117]=184.957;  m_aerogelDefaultTemplate_Y[117]=282.423;
 m_aerogelDefaultTemplate_X[118]=292.653;  m_aerogelDefaultTemplate_Y[118]=-189.786;
 m_aerogelDefaultTemplate_X[119]=-305.927; m_aerogelDefaultTemplate_Y[119]=-166.989;
 m_aerogelDefaultTemplate_X[120]=24.2301;  m_aerogelDefaultTemplate_Y[120]=-337.072;
 m_aerogelDefaultTemplate_X[121]=6.2193;   m_aerogelDefaultTemplate_Y[121]=-332.202;
 m_aerogelDefaultTemplate_X[122]=192.418;  m_aerogelDefaultTemplate_Y[122]=282.722;
 m_aerogelDefaultTemplate_X[123]=89.6789;  m_aerogelDefaultTemplate_Y[123]=318.757;
 m_aerogelDefaultTemplate_X[124]=259.96;   m_aerogelDefaultTemplate_Y[124]=225.614;
 m_aerogelDefaultTemplate_X[125]=-284.504; m_aerogelDefaultTemplate_Y[125]=197.474;
 m_aerogelDefaultTemplate_X[126]=-269.21;  m_aerogelDefaultTemplate_Y[126]=205.268;
 m_aerogelDefaultTemplate_X[127]=8.22862;  m_aerogelDefaultTemplate_Y[127]=-342.385;
 m_aerogelDefaultTemplate_X[128]=344.942;  m_aerogelDefaultTemplate_Y[128]=-77.1921;
 m_aerogelDefaultTemplate_X[129]=276.278;  m_aerogelDefaultTemplate_Y[129]=-221.273;
 m_aerogelDefaultTemplate_X[130]=95.6937;  m_aerogelDefaultTemplate_Y[130]=-322.247;
 m_aerogelDefaultTemplate_X[131]=-345.423; m_aerogelDefaultTemplate_Y[131]=75.5759;
 m_aerogelDefaultTemplate_X[132]=334.686;  m_aerogelDefaultTemplate_Y[132]=-117.682;
 m_aerogelDefaultTemplate_X[133]=-94.2024; m_aerogelDefaultTemplate_Y[133]=-324.613;
 m_aerogelDefaultTemplate_X[134]=178.809;  m_aerogelDefaultTemplate_Y[134]=289.926;
 m_aerogelDefaultTemplate_X[135]=241.968;  m_aerogelDefaultTemplate_Y[135]=235.362;
 m_aerogelDefaultTemplate_X[136]=228.598;  m_aerogelDefaultTemplate_Y[136]=248.732;
 m_aerogelDefaultTemplate_X[137]=304.216;  m_aerogelDefaultTemplate_Y[137]=-169.79;
  

  m_aerogelTrackDefaultTemplate_X=0.0;
  m_aerogelTrackDefaultTemplate_Y=0.0;
 


 // Now for the Rich1Gas

 m_rich1GasDefaultTemplate_X.clear();
 m_rich1GasDefaultTemplate_Y.clear(); 
 m_rich1GasDefaultTemplate_X.resize(24);
 m_rich1GasDefaultTemplate_Y.resize(24);
 m_rich1GasDefaultTemplate_X[0]=6.41405;     m_rich1GasDefaultTemplate_Y[0]=-143.203;
 m_rich1GasDefaultTemplate_X[1]= -6.27691;   m_rich1GasDefaultTemplate_Y[1]=-130.073;
 m_rich1GasDefaultTemplate_X[2]= -100.782;   m_rich1GasDefaultTemplate_Y[2]=-125.038;
 m_rich1GasDefaultTemplate_X[3]= -93.2157;   m_rich1GasDefaultTemplate_Y[3]=-122.457;
 m_rich1GasDefaultTemplate_X[4]= -75.7114;   m_rich1GasDefaultTemplate_Y[4]=-120.024;
 m_rich1GasDefaultTemplate_X[5]= -67.921;    m_rich1GasDefaultTemplate_Y[5]=-114.995;
 m_rich1GasDefaultTemplate_X[6]= 20.4088;    m_rich1GasDefaultTemplate_Y[6]=-205.303;
 m_rich1GasDefaultTemplate_X[7]= 20.5099;    m_rich1GasDefaultTemplate_Y[7]=-197.601;
 m_rich1GasDefaultTemplate_X[8]= 23.171;     m_rich1GasDefaultTemplate_Y[8]=-195.043;
 m_rich1GasDefaultTemplate_X[9]= 20.1483;    m_rich1GasDefaultTemplate_Y[9]=-171.761;
 m_rich1GasDefaultTemplate_X[10]= -125.259;  m_rich1GasDefaultTemplate_Y[10]=-220.747;
 m_rich1GasDefaultTemplate_X[11]= -130.489;  m_rich1GasDefaultTemplate_Y[11]=-207.562;
 m_rich1GasDefaultTemplate_X[12]= -133.023;  m_rich1GasDefaultTemplate_Y[12]=-195;
 m_rich1GasDefaultTemplate_X[13]= -130.555;  m_rich1GasDefaultTemplate_Y[13]=-195.005;
 m_rich1GasDefaultTemplate_X[14]= -133.018;  m_rich1GasDefaultTemplate_Y[14]=-190.078;
 m_rich1GasDefaultTemplate_X[15]= -125.133;  m_rich1GasDefaultTemplate_Y[15]=-158.607;
 m_rich1GasDefaultTemplate_X[16]= -8.95158;  m_rich1GasDefaultTemplate_Y[16]=-246.985;
 m_rich1GasDefaultTemplate_X[17]= -8.99129;  m_rich1GasDefaultTemplate_Y[17]=-244.30; 
 m_rich1GasDefaultTemplate_X[18]= -6.44374;  m_rich1GasDefaultTemplate_Y[18]=-241.641;
 m_rich1GasDefaultTemplate_X[19]= -54.3343;  m_rich1GasDefaultTemplate_Y[19]=-262.165;
 m_rich1GasDefaultTemplate_X[20]= -83.2231;  m_rich1GasDefaultTemplate_Y[20]=-257.473;
 m_rich1GasDefaultTemplate_X[21]= -78.1481;  m_rich1GasDefaultTemplate_Y[21]=-257.408;
 m_rich1GasDefaultTemplate_X[22]= -106.169;  m_rich1GasDefaultTemplate_Y[22]=-249.484;
 m_rich1GasDefaultTemplate_X[23]= -95.8568;  m_rich1GasDefaultTemplate_Y[23]=-249.669;
 

 m_rich1GasTrackDefaultTemplate_X=-55.9503 ;
 m_rich1GasTrackDefaultTemplate_Y=-187.615;
 
 


 // Now for Rich2 Gas

 m_rich2GasDefaultTemplate_X.clear();
 m_rich2GasDefaultTemplate_Y.clear();
 m_rich2GasDefaultTemplate_X.resize(18);
 m_rich2GasDefaultTemplate_Y.resize(18);
 
 m_rich2GasDefaultTemplate_X[0]=-420.108;  m_rich2GasDefaultTemplate_Y[0]=-294.591;
 m_rich2GasDefaultTemplate_X[1]=-435.214;  m_rich2GasDefaultTemplate_Y[1]=-269.22;
 m_rich2GasDefaultTemplate_X[2]=-432.658;  m_rich2GasDefaultTemplate_Y[2]= -269.262;
 m_rich2GasDefaultTemplate_X[3]=-295.58;   m_rich2GasDefaultTemplate_Y[3]=-371.423;
 m_rich2GasDefaultTemplate_X[4]=-298.302;  m_rich2GasDefaultTemplate_Y[4]=-368.795;
 m_rich2GasDefaultTemplate_X[5]=-292.971;  m_rich2GasDefaultTemplate_Y[5]=-368.893;
 m_rich2GasDefaultTemplate_X[6]=-264.98;   m_rich2GasDefaultTemplate_Y[6]=-363.988;
 m_rich2GasDefaultTemplate_X[7]=-257.216;  m_rich2GasDefaultTemplate_Y[7]=-358.635;
 m_rich2GasDefaultTemplate_X[8]=-249.543;  m_rich2GasDefaultTemplate_Y[8]=-125.942; 
 m_rich2GasDefaultTemplate_X[9]=-295.523;  m_rich2GasDefaultTemplate_Y[9]=-115.702;
 m_rich2GasDefaultTemplate_X[10]=-285.071; m_rich2GasDefaultTemplate_Y[10]=-115.636;
 m_rich2GasDefaultTemplate_X[11]=-295.511; m_rich2GasDefaultTemplate_Y[11]=-113.15;
 m_rich2GasDefaultTemplate_X[12]=-176.973; m_rich2GasDefaultTemplate_Y[12]=-273.264;
 m_rich2GasDefaultTemplate_X[13]=-179.706; m_rich2GasDefaultTemplate_Y[13]=-267.865;
 m_rich2GasDefaultTemplate_X[14]=-182.38;  m_rich2GasDefaultTemplate_Y[14]=-262.581;
 m_rich2GasDefaultTemplate_X[15]=-179.899; m_rich2GasDefaultTemplate_Y[15]=-257.477;
 m_rich2GasDefaultTemplate_X[16]=-179.706; m_rich2GasDefaultTemplate_Y[16]=-224.385;
 m_rich2GasDefaultTemplate_X[17]=-195.026; m_rich2GasDefaultTemplate_Y[17]=-170.403;
 
     
  m_rich2GasTrackDefaultTemplate_X= -294.835 ;
  m_rich2GasTrackDefaultTemplate_Y= -241.207 ;
  
}

RichLocalDefaultTemplateData* RichLocalDefaultTemplateData::getRichLocalDefaultTemplateDataInstance() 
{
  if( m_RichLocalDefaultTemplateDataInstance == 0 ) {
    m_RichLocalDefaultTemplateDataInstance=  new RichLocalDefaultTemplateData();
    
  }
  
  return m_RichLocalDefaultTemplateDataInstance;
  
}
