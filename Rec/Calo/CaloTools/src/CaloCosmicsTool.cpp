// Include files 

// from LHCb
#include "GaudiKernel/Plane3DTypes.h"
#include "LHCbMath/LineTypes.h"
#include "LHCbMath/GeomFun.h"
#include "CaloDet/DeCalorimeter.h"

// local
#include "CaloCosmicsTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloCosmicsTool
//
// 2008-04-02 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CaloCosmicsTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloCosmicsTool::CaloCosmicsTool( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : GaudiTupleTool ( type, name , parent )
  , m_calo(NULL)
{
  declareInterface<ICaloCosmicsTool>(this);


  declareProperty( "Detector"           , m_det);     // Detector name
  declareProperty( "ReadoutTool"        , m_readoutTool  = "CaloDataProvider" ); // Name of calo bank decoding tool
  declareProperty( "TrajectorySlots"    , m_seq);     // Vector of time-slots to be used for trajectory 
  declareProperty( "AsymmetrySlots"     , m_asy);     // Pair of time-slots to be used for asymmetry 
  declareProperty( "RemoveSlotForKernel", m_kern);     // Vector of time-slots to be used for trajectory 

  // Cosmics track reconstruction
  declareProperty( "ZeroSuppression"    , m_zSup=0);     // Zero suppression threshold for hits ADC (sum over BX)
  declareProperty( "MaxSuppression"     , m_zInf=99999); // Remove largest ADC
  declareProperty( "MinCosmicsDeposit"  , m_minD=0);     // minimal ADC sum over hits in cosmics track
  declareProperty( "MaxCosmicsDeposit"  , m_maxD=99999); // maximal ADC sum over hits in cosmics track
  declareProperty( "MinCosmicsMult"     , m_minM=0);      // minimal multiplicity of hits in cosmics track 
  declareProperty( "MaxCosmicsMult"     , m_maxM=6156);   // minimal multiplicity of hits in cosmics track 
  declareProperty( "MaxDistance2Line"   , m_tol=2.0);       // maximal distance between hit and cosmics track (cellSize unit)
  // Timing
  declareProperty( "MinR"               , m_minR= 0. );    // minimal asymmetry range to compute time (absolute value)
  declareProperty( "MaxR"               , m_maxR= 0.8);    // maximal asymmetry range to compute time (absolute value)
  declareProperty( "RtoTime"            , m_par);          // parameters to convert R to time 
  declareProperty( "TRes"               , m_tRes=0);        // time resolution parameter
  // Tupling setup
  declareProperty( "Ntupling"           , m_tuple=false);    // produce ntuple
  declareProperty( "MaxArraySize"       , m_maxAdc=500);     // ntuple max array (# cosmics ADCs)
  declareProperty( "AllDigits"          , m_full=false);     // fill digit vector with all 0-sup ADC


  // default sequence
  m_seq.push_back("Prev1"); 
  m_seq.push_back("T0"); 
  m_seq.push_back("Next1");

  m_kern.push_back("T0");
  


  // default asymmetry (first-second)/(first+second)
  std::vector<std::string> prevT0;
  prevT0.push_back("Prev1"); 
  prevT0.push_back("T0");
  m_asy["-25."] =  prevT0;


  std::vector<std::string> t0Next;
  t0Next.push_back("T0"); 
  t0Next.push_back("Next1");
  m_asy["0."]=t0Next;


  //  std::vector<std::string> nextNext;
  //  nextNext.push_back("Next1");
  //  nextNext.push_back("Next2"); 
  //  m_asy["25."]=nextNext;

  //  std::vector<std::string> prevNext;
  //  prevNext.push_back("Prev1"); 
  //  prevNext.push_back("Next1");
  //  m_asy["9999"]=prevNext;

  // set default detectorName
  int index = name.find_last_of(".") +1 ; // return 0 if '.' not found --> OK !!
  m_det = name.substr( index, 4 ); 
  if ( name.substr(index,3) == "Prs" ) m_det = "Prs";
  if ( name.substr(index,3) == "Spd" ) m_det = "Spd";

  //
  if(m_det == "Ecal"){ m_zSup = 100;}
  else if(m_det == "Hcal"){ m_zSup = 10;  }
  else { m_zSup = 0;  }

  // Jacques's fit parameter for T=f(R) 
  m_par.push_back(+1.4);
  m_par.push_back(-0.7);
  m_par.push_back(+25.0);
  m_par.push_back(+0.19);
}
//=============================================================================
CaloCosmicsTool::~CaloCosmicsTool() {} 

//=============================================================================
StatusCode CaloCosmicsTool::initialize(){
  StatusCode sc = GaudiTupleTool::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);


  
  // Fill vector with requested time slots from sequence and asymmetry settings
  for(std::vector<std::string>::iterator iseq = m_seq.begin(); iseq != m_seq.end() ;  ++iseq){
    m_slots.push_back( *iseq );
  }
  int k =0;
  for(std::map<std::string,std::vector<std::string> >::iterator iasy = m_asy.begin();m_asy.end() != iasy ; iasy++){
    std::vector<std::string> asy = (*iasy).second;
    if( asy.size() != 2){
      Error("AsymmetrySlot must be a vector of pairs of entries").ignore();
      return StatusCode::FAILURE;
    }else{
      std::string a = *(asy.begin());
      std::string b = *(asy.begin()+1);
 
      std::stringstream str("");
      str << (*iasy).first;
      double offset;
      str>> offset;

      info() << "Asymmetry ( id = '" << (*iasy).first
             << "') is define as R = [ADC("<< a <<")-ADC("<< b <<")]/[ADC("<< a <<")+ADC("<< b <<")]"<<endmsg;
      info() << "Corresponding Time offset : "<< offset << " ns" << endmsg;
    }
    for(std::vector<std::string>::iterator islot = asy.begin(); islot != asy.end();islot++){
      bool isReg = false;
      for(std::vector<std::string>::iterator is = m_slots.begin(); is != m_slots.end() ;  ++is){
        if( *is == *islot){
          isReg = true;
          continue;
        }
      }
      if( !isReg )m_slots.push_back( *islot );
    }
    k++;
  }
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "Requested time slots : " << m_slots << endmsg;
  

  // get detector element
  if ( "Ecal" == m_det ) {
    m_calo     = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );
  } else if ( "Hcal" == m_det ) {
    m_calo     = getDet<DeCalorimeter>( DeCalorimeterLocation::Hcal );
  } else if ( "Prs" == m_det ) {
    m_calo     = getDet<DeCalorimeter>( DeCalorimeterLocation::Prs );
  } else {
    error() << "Unknown detector name " << m_det << endmsg;
    return StatusCode::FAILURE;
  }

  // some detector geometry
  m_plane = m_calo->plane(CaloPlane::Middle);
  m_delta = m_calo->plane(CaloPlane::Front).HesseDistance()-
    m_calo->plane(CaloPlane::Back).HesseDistance();
  m_delta *= m_calo->plane(CaloPlane::Front).Normal().Z();
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << " m_det : " << m_det << " m_delta = " << m_delta << " cos(tilt) = " 
            <<  m_calo->plane(CaloPlane::Front).Normal().Z() << endmsg;

  // get the decoding tool for each time slot
  for(std::vector<std::string>::iterator islot = m_slots.begin(); islot != m_slots.end() ;  ++islot){
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "Loading : decoding tool for slot " << *islot << " : '"
              << m_det << "ReadoutTool" <<  *islot << "'" <<endmsg;
    this->setProperty("RootInTES","");
    if(*islot != "T0")this->setProperty("RootInTES",*islot);
     m_daqs[*islot] = tool<ICaloDataProvider>( m_readoutTool , m_det + "ReadoutTool" + *islot , this );
  }
  this->setProperty("RootInTES","");// reset

  //
  return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode CaloCosmicsTool::processing(){

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << " Process " << name() << endmsg;

  // default
  m_phi      = -999;
  m_sPhi     = 999;
  m_refPoint = Gaudi::XYZPoint();
  m_eRefPoint = Gaudi::XYZPoint();
  m_cosmicsADCs.clear();
  m_zsupADCs.clear();
  m_maxLoc = "none";
  m_offset = -99999;
  m_bound = std::make_pair(Gaudi::XYZPoint(),Gaudi::XYZPoint());
  m_time     = -999;
  m_stime    = -999;
  m_timed   = false;
  m_tracked   = false;
  for(std::map<std::string,std::vector<std::string> >::iterator iasy = m_asy.begin(); iasy != m_asy.end() ; iasy++){
    std::string id = (*iasy).first;
    m_Rmean[id]    = -999.;
    m_Rmean2[id]   = -999.;
    m_R[id]        = -999.;
    m_slotSum[id]   = -999.;
    m_td[id] = -999.;
  }
  m_kernel = -1.;

  // 1) get banks
  if( getBanks().isFailure() )return StatusCode::FAILURE;

  // 2) apply 0-suppression
  if( zSup().isFailure() )return StatusCode::SUCCESS;

  // 3) reconstruct 2D trajectory
  if( fit2D().isFailure() )return StatusCode::SUCCESS;

  // 4) reconstruct time 
  if( timing().isFailure() )return StatusCode::SUCCESS;

  // 5) produce ntuple
  if(m_tuple){
    
    unsigned int unit=100;
    if(m_det == "Prs")unit = 100;
    else if(m_det == "Ecal")unit = 200;
    else if(m_det == "Hcal")unit = 300;
    if( tupling(unit).isFailure())return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


//=============================================================================
StatusCode CaloCosmicsTool::getBanks(){

  // get rawBank from TES
  for(std::vector<std::string>::iterator islot = m_slots.begin(); islot != m_slots.end() ;  ++islot){
    if(!m_daqs[*islot]->ok()) return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
//=============================================================================
StatusCode CaloCosmicsTool::zSup(){

  // apply 0-suppression

  m_max = LHCb::CaloAdc(LHCb::CaloCellID(), -9999 );
  m_zsupADCs.clear();

  std::string slot = *(m_slots.begin());
  const CaloVector<LHCb::CaloAdc>& adcs = m_daqs[slot]->adcs() ;
  for(CaloVector<LHCb::CaloAdc>::const_iterator iadc = adcs.begin(); iadc!= adcs.end(); ++iadc){
    long sum = 0;
    LHCb::CaloCellID cell = (*iadc).cellID();
    if(cell.area() == m_calo->pinArea())continue;
    for(std::vector<std::string>::iterator islot = m_seq.begin(); islot != m_seq.end() ;  ++islot){
      int adc = m_daqs[*islot]->adc(cell);
      if(adc>0)sum += adc;
    }
    if( sum <= m_zSup )continue;

    // Collect 0-suppresed data
    LHCb::CaloAdc sumADC(cell , sum);
    m_zsupADCs.push_back( sumADC );                // vector of ADC sum over consecutive BX (after Z-sup)
    if( sum > m_max.adc() )m_max = sumADC;    // maximal ADC
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << name() 
              << " : cell = " << sumADC.cellID() 
              << " ADCsum =  " << sumADC.adc() 
              << " max =  " << m_max.adc() 
              << endmsg;
    
  }

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "Zero suppression : " << m_zsupADCs.size() << " among "
            << adcs.size() << " digits " << endmsg;

  if( m_zsupADCs.size() == 0){
    info() << "No digit left after Zero-suppression" <<endmsg;
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}
//=============================================================================
StatusCode CaloCosmicsTool::fit2D(){

  m_refPoint  = Gaudi::XYZPoint(0.,0.,-999);
  m_eRefPoint = Gaudi::XYZVector();
  m_cosmicsADCs.clear();

  // get the position of highest deposit
  Gaudi::XYZPoint  ref = m_calo->cellCenter( m_max.cellID() );

  //  Best 2D slope fit  (minimize   Chi2 = Sum_i  [ADC_i (phi_i - phi)^2/sigma_i^2)
  double swa  = 0;
  double sw   = 0;
  {for(std::vector<LHCb::CaloAdc>::iterator id = m_zsupADCs.begin(); id != m_zsupADCs.end(); ++id){
    LHCb::CaloCellID cell = (*id).cellID();
    if( m_max.cellID() == cell)continue;    
    Gaudi::XYZPoint pos = m_calo->cellCenter( cell );
    double size = m_calo->cellSize( cell );
    double sx2  = size*size/12;
    double dx = pos.X()-ref.X();
    double dy = pos.Y()-ref.Y();
    double a = acos(0.);                    // a = pi/2
    if(dx != 0 )a =  atan(dy/dx) ;             // tan( phi_i )= y_i/x_i
    if(a<0)a += acos(-1.);
    double s2 = sx2;
      ///(dx*dx + dy*dy);          // sigma(phi_i)
    int val =  (*id).adc();
    if( val < 0)val = 0;
    s2 = 1;
    Gaudi::XYZVector vec = pos-ref;
    double adc = (double) val/vec.rho() ;
    double w = adc  / s2;   // weight = ADC_i/sigma_i^2    
    sw  += w;
    swa += w*a;
  }}

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "Point : " << ref.X()  << " " << ref.Y() << endmsg;


  if( sw<=0 ){
    Warning( "Weight = 0 - fit failed ").ignore();
    return StatusCode::FAILURE;
  }
  else{
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "1st pass " << swa/sw << endmsg;
  }
  
  // Built the 2D line
  // best phi = swa/sw (assuming the 2D line passing through the highest deposit)
  double norm = sqrt(1.+tan(swa/sw)*tan(swa/sw));
  Gaudi::XYZVector vec(1./norm, tan(swa/sw)/norm , 0.);
  Gaudi::Math::XYZLine   line(ref, vec);
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "Line parameters " << vec.X() << " " << vec.Y() << " " << vec.Z() << endmsg;
  // Collect data along the 2D cosmics line (+- tolerance)
  double x   = 0;
  double y   = 0;
  double ssx2= 0;
  m_adcSum = 0;
  double kernel = 0.;
  {for(std::vector<LHCb::CaloAdc>::iterator id =m_zsupADCs.begin(); id != m_zsupADCs.end(); ++id){
    LHCb::CaloCellID cell = (*id).cellID();
    double size = m_calo->cellSize( cell );
    double sx2  = size*size/12;
    Gaudi::XYZPoint pos = m_calo->cellCenter( cell );
    pos.SetZ( ref.Z() );
    double ip = Gaudi::Math::impactParameter<Gaudi::XYZPoint, Gaudi::Math::XYZLine>(pos, line)/size;
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "--> " << cell << " ip = " << ip << " x = "<<  pos.X()
              << " y = " << pos.Y() << " z=  " << pos.Z() << endmsg;
    if(fabs(ip) < m_tol){
      m_cosmicsADCs.push_back( *id );
      m_adcSum += (*id).adc();
      kernel+= (*id).adc();
      for(std::vector<std::string>::iterator ik = m_kern.begin();ik!=m_kern.end();++ik){
        kernel -= m_daqs[*ik]->adc( cell );
      }    
      x += pos.X();
      y += pos.Y();
      ssx2 += sx2;
    }
  }}

  if( 0 == m_adcSum ) {
    m_kernel = 0;
  }
  else {
    m_kernel = kernel / (double) m_adcSum;  
  }
  
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << " m_cosmicsADCs.size() " << m_cosmicsADCs.size() << endmsg;
  if(m_cosmicsADCs.size()== 0){
    return Warning( "Empty 2D segment - fit failed", StatusCode::FAILURE );
  }

  // ADC sum thresholds
  if( m_adcSum < m_minD || m_adcSum > m_maxD){
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "ADC sum " << m_adcSum << " outside the requested range ["
              << m_minD << "," << m_maxD <<"]" <<endmsg;
    return StatusCode::FAILURE;
  }

  // Multiplicity thresholds
  if( (int) m_cosmicsADCs.size() < m_minM || (int) m_cosmicsADCs.size() > m_maxM ){
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "Segment #digits " << m_cosmicsADCs.size()
              << " outside the requested range " 
              << m_minM << "," << m_maxM << "]" << endmsg;
    return StatusCode::FAILURE;
  }  

  // average position
  double N = (double) m_cosmicsADCs.size();
  x /= N;
  y /= N;
  // Z (middle of active area) - take the tilt into account for the fun
  double z  = ( -m_plane.Normal().X()*x -m_plane.Normal().Y()*y - m_plane.HesseDistance() )/m_plane.Normal().Z();  
  double ex = ssx2/N/N;   // error on x
  double ey = ssx2/N/N;   // error on y
  double ez =  m_delta*m_delta/12./N; // error on z
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) {
    debug() << "Delta = " << m_delta << " z  = " << z <<  "+-" << sqrt(ez) << endmsg;
    debug() << "x = " << x << " +- " << sqrt(ex) << endmsg;
    debug() << "y = " << y << " +- " << sqrt(ey) << endmsg;  
  }
  
  m_refPoint.SetX(x);
  m_refPoint.SetY(y);
  m_refPoint.SetZ(z);
  m_eRefPoint.SetX(ex);
  m_eRefPoint.SetY(ey);
  m_eRefPoint.SetZ(ez);


  // (re-)Evaluate the parameters wrt the average (x,y)
  swa  = 0;
  sw   = 0;
  double sws2 = 0;
  std::vector<LHCb::CaloAdc>::iterator id = m_cosmicsADCs.begin();
  LHCb::CaloCellID idMin = (*id).cellID();
  LHCb::CaloCellID idMax = (*id).cellID();
  for(id = m_cosmicsADCs.begin() ; id != m_cosmicsADCs.end(); ++id){
    LHCb::CaloCellID cell = (*id).cellID();
    Gaudi::XYZPoint pos = m_calo->cellCenter( cell );
    if( pos.Y() < m_calo->cellCenter( idMin ).Y() )idMin = cell;
    if( pos.Y() > m_calo->cellCenter( idMax ).Y() )idMax = cell;
    double adc  = (double) (*id).adc();
    if(adc<0)adc = 0;
    vec = pos-m_refPoint;
    if(vec.rho()!=0)adc /= vec.rho();
    double size = m_calo->cellSize( cell );
    double sx2  = size*size/12;
    double dx = pos.X()-m_refPoint.X();
    double dy = pos.Y()-m_refPoint.Y();
    double a = acos(0.);
    if(dx != 0 )a =  atan(dy/dx) ;         // tan( phi_i )= y_i/x_i
    double s2;
    if(dx*dx+dy*dy != 0){
      s2 = sx2/(dx*dx + dy*dy);           // sigma(phi_i)
    }else{
      a = 0;
      s2= acos(-1.);
    }
    if(a<0)a += acos(-1.);
    double w = adc / s2;
    sw  += w;
    swa += w*a;
    sws2 += w*w*s2;
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "a = " << a << " w = " << w << " sw = " << sw << " swa = " << swa << endmsg;
  }
  m_phi   = swa / sw;          // best phi
  if(m_phi<0)m_phi += acos(-1.);
  m_sPhi = sws2 /sw/sw; //  phi variance

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "2nd pass " << m_phi << " +- " << sqrt(m_sPhi) << endmsg;
  m_bound = std::make_pair(m_calo->cellCenter( idMin), m_calo->cellCenter( idMax) );   // extrema of the track segment
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "END OF TRAJECTORY RECONSTRUCTION" << endmsg;
  m_tracked = true;
  return StatusCode::SUCCESS;
}






//=============================================================================
StatusCode CaloCosmicsTool::timing(){

  // Compute the asymmetries
  double maxSum = -99999;
  m_maxLoc = "none";


  // compute asymmetries
  
  for(std::map<std::string,std::vector<std::string> >::iterator iasy = m_asy.begin(); iasy != m_asy.end() ; iasy++){
    std::vector<std::string> asym = (*iasy).second;
    std::string id = (*iasy).first;
    std::string a1 = *(asym.begin());
    std::string a2 = *(asym.begin()+1);
    double sb1 = 0;
    double sb2 = 0;
    double rm  = 0;
    double rm2 = 0;
    double td  = 0;
    double tdm = 0;
    double dl  = 0;
    int N = 0;
    for(std::vector<LHCb::CaloAdc>::iterator it = m_cosmicsADCs.begin();it!=m_cosmicsADCs.end();it++){
      LHCb::CaloCellID cell = (*it).cellID();
      double b1 = (double) m_daqs[a1]->adc( cell );
      double b2 = (double) m_daqs[a2]->adc( cell );
      if(b1<0)b1=0;
      if(b2<0)b2=0;
      if(b1+b2 == 0 )continue;
      N++;
      double asy = ( b1 -  b2 ) / ( b1 + b2 );
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
        debug() << "R = " << asy << " b1 = " << b1 << "b2 = " << b2 << endmsg;
      sb1 += b1;
      sb2 += b2;
      rm  += asy;
      rm2 += asy*asy;
      // timing dispersion
      Gaudi::XYZPoint  pos = m_calo->cellCenter( cell );
      Gaudi::XYZVector vec = pos - m_refPoint;
      double sign = 0;
      if(vec.Y() != 0)sign = vec.Y()/fabs(vec.Y());
      td += asy * vec.rho() * sign;
      tdm+= vec.rho() * sign;
      dl += vec.Perp2();
    }

    if( N != 0) m_Rmean[id]   = rm / (double) N;
    if( N != 0) m_Rmean2[id]   = rm2 / (double) N;

    tdm *= m_Rmean[id];
    m_td[id] = (dl>0) ?  (td-tdm)/dl : 0.;
    m_Rmean2[id] -= m_Rmean[id]*m_Rmean[id];
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) {
      debug() << "<R>(  " << id << ") = "<< m_Rmean[id] << endmsg;
      debug() << "sigma(R)^2("<< id << ") = " << m_Rmean2[id] << endmsg;
    }
    if(sb1+sb2!=0)m_R[id]      = (sb1-sb2)/(sb1+sb2)      ;  // asymmetry of the average
    m_slotSum[id] = sb1+sb2;
    if(  sb1+sb2 >= maxSum){
      maxSum=sb1+sb2;
      m_maxLoc = id;
    } 
  }

  
  // convert asymmetry into time  (@todo - apply Yasmine's method)
  m_time = -999;
  if(m_maxLoc == "none")return StatusCode::SUCCESS;
  std::stringstream str("");
  str << m_maxLoc;
  double offset;
  str>> offset;
  m_offset = offset;
  if(fabs(offset)>999.)return StatusCode::SUCCESS;
  double R = m_Rmean[m_maxLoc];
  if(fabs(R) > m_minR && fabs(R) < m_maxR ){
    m_timed = true;
    double u = R / ( 1+ m_par[3]*sqrt(1-R*R));
    m_time = m_offset + m_par[0] + m_par[1]*R*R + m_par[2]/acos(-1.) * acos( u );
    //    double d = -2*m_par[1] + m_par[2]/acos(-1.) 
    //      * ( - 1/sqrt(1-u*u) * ( 1+ m_par[3]*sqrt(1-R*R) - R * 0.5 *  m_par[3] / sqrt(1-R*R)))*u*u/R/R;
    //m_stime = d*d* m_Rmean2[m_maxLoc] + m_tRes*m_tRes;
  }
  return StatusCode::SUCCESS;
}



//=============================================================================
StatusCode CaloCosmicsTool::tupling(unsigned int unit){


  Tuple ntp = nTuple(unit, m_det + "Cosmics" ,CLID_ColumnWiseTuple);
  StatusCode sc;
  // Ecal
  sc = ntp->column("deposit"    , deposit() );
  sc = ntp->column("phi"    , phi() );
  sc = ntp->column("phiVar" , phiVariance()  );
  sc = ntp->column("refX" , referencePoint().X() );
  sc = ntp->column("refY" , referencePoint().Y() );
  std::vector<LHCb::CaloAdc>& eAdcs = m_cosmicsADCs;
  if(m_full) eAdcs = m_zsupADCs;
  std::vector<float> x;
  std::vector<float> y;
  std::vector<int> a;
  std::vector<int> ids;
  for(std::vector<LHCb::CaloAdc>::iterator iadc = eAdcs.begin();iadc!=eAdcs.end();iadc++){
    if(iadc-eAdcs.begin() > m_maxAdc){
      Warning("Ntupling :  digits vector too large - will be truncated").ignore();
      break;
    }
    
    LHCb::CaloCellID id = (*iadc).cellID();
    ids.push_back(  id.all()  );
    x.push_back( (float)det()->cellCenter( id ).X() );
    y.push_back( (float)det()->cellCenter( id ).Y() );
    a.push_back( (*iadc).adc() );
  }
  sc = ntp->farray("id"   , ids      ,"NAdcs", m_maxAdc);
  sc = ntp->farray("x"   , x      ,"NAdcs", m_maxAdc);
  sc = ntp->farray("y"   , y      ,"NAdcs", m_maxAdc);
  sc = ntp->farray("adc" , a      ,"NAdcs", m_maxAdc);

  std::vector<double> rMean;
  std::vector<double> rSig;
  std::vector<double> rGlob;
  std::vector<double> sSum;
  std::vector<double> td;
  for(std::map<std::string,std::vector<std::string> >::iterator iasy = m_asy.begin(); iasy != m_asy.end() ; iasy++){
    std::string id = (*iasy).first;
    rMean.push_back( m_Rmean[id] );
    rSig.push_back(  m_Rmean2[id] );
    rGlob.push_back( m_R[id] );
    sSum.push_back( m_slotSum[id] );
    td.push_back(m_td[id]);
  }
  sc = ntp->farray("RMean" , rMean , "NAsymmetries" , m_Rmean.size() );
  sc = ntp->farray("RVar"  , rSig  , "NAsymmetries" , m_Rmean.size() );
  sc = ntp->farray("RGlob" , rGlob , "NAsymmetries" , m_Rmean.size() );
  sc = ntp->farray("Sum"   , sSum  , "NAsymmetries" , m_Rmean.size() );
  sc = ntp->farray("dTime" , td    , "NAsymmetries" , m_Rmean.size() );


  std::stringstream str("");
  double offset = -9999.;
  if(m_maxLoc != "none"){
    str << m_maxLoc; 
    double off;
    str>> off;
    offset = off;
  }
  
  sc = ntp->column("slot"               , offset);
  sc = ntp->column("time"               , m_time);
  sc = ntp->column("timeVariance"       , m_stime);
  sc = ntp->column("istimed"            , (int)m_timed);
  sc = ntp->column("istracked"          , (int)m_tracked);

  sc=ntp->write();
  if(sc.isFailure())Warning( name() + " Ntupling failed").ignore();
  return StatusCode::SUCCESS;
}
  
//=============================================================================
//  Finalize
//=============================================================================
StatusCode CaloCosmicsTool::finalize() {

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Finalize" << endmsg;

  return GaudiTupleTool::finalize();  // must be called after all other actions
}
