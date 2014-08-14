/*
 *  Derivatives
 *
 *
 */
//from GAUDI
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "Derivatives.h"
#include <cmath>
#include <iostream>

using namespace std;
using namespace Gaudi;


/// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( Derivatives )



/// Standard Concstructor:
Derivatives::Derivatives( const std::string &type,
                          const std::string& name,
                          const IInterface* parent )
  : GaudiTool( type, name, parent ),
    m_MonteCarlo(false),
    m_sign(1.0), // 1.0 for data; -1.0 for MC data.
    m_n_elements(0),
    m_tr_pars(0),
    m_n_dofs(0)
{
  declareInterface<IDerivatives>(this);
  declareProperty("MonteCarlo", m_MonteCarlo = false );
}

/// Destructor:
Derivatives::~Derivatives(){}

StatusCode Derivatives::initialize( const int &n_tr_pars,
                                    const int &n_elements,
                                    const std::vector<bool> &dofs ) {
  if ( m_MonteCarlo ) {
    m_sign = 1.0;
    info() << "Trying to find misalignments in MC data!" << endreq;
  } else {
    info() << "Trying to find misalignments in data!" << endreq;
    m_sign = -1.0;
  }

  m_n_elements = n_elements;
  m_tr_pars = n_tr_pars;
  m_dofs = dofs;
  for ( unsigned int i = 0; i < m_dofs.size();i++ )
    if ( m_dofs[i] )
      m_n_dofs++;
  m_derLC.resize( m_tr_pars );
  m_derGB.resize( m_n_elements * m_n_dofs );
  info() << "Number of degrees of freedom: " << m_n_dofs << endreq;
  info() << "Number of local parameters: " << m_tr_pars << endreq;
  info() << "Number of objects to align: " << m_n_elements << endreq;
  return StatusCode::SUCCESS;
}

StatusCode Derivatives::SetGlobal( const Gaudi::XYZVector &slope, const
                                   Gaudi::XYZVector &origin,  /// Slope and origin vectors of track
                                   const struct Point predic, // prediction of hit x,y
                                   std::vector<double> &m_derGB, // vector containing global derivatives
                                   const int &station, // detector element number in list
//                                   const double &z,  // z-position of this det. element
                                   const double &sta ) { // stereo-angle of this det element

  double tx = slope.x();
  double ty = slope.y();
  double x0,y0;
  if(origin.z() == 0.){
    x0 = origin.x();
    y0 = origin.y();
  }else
  {
    x0 = origin.x() -tx*origin.z();
    y0 = origin.y() -ty*origin.z();
  }
  debug() << " -----------------------------------------------" << endreq;  
  debug() << " predicted hits = " << predic.x << " " << predic.y << endreq;
  debug() << " Derivatives::origin -- > "<< x0 << " and " << y0 << endreq;
  debug() << " Derivatives::slopes -- > "<< tx << " and " << ty << endreq;
  
  
  int dof_cnt = 0; // count the degrees of freedom
  // shift in co-ordinate along measurement:
  if ( m_dofs[0] ) {
    m_derGB[station] = m_sign;//-1.0 == real Data; +1 == sim
    dof_cnt++;
  }

  // shift in co-ordinate perpendicular to measurement:
  if ( m_dofs[1] ) {
    m_derGB[station+dof_cnt*m_n_elements] = 0.0;
    dof_cnt++;
  }

  // shift along z co-ordinate
  if ( m_dofs[2] ) {
    m_derGB[station+dof_cnt*m_n_elements] = (-m_sign) * (tx*cos(sta)+ty*sin(sta)); // +1 == real; -1==sim
    dof_cnt++;
  }


  // rotation around  x-axis
  if ( m_dofs[3] ) {
    m_derGB[station+dof_cnt*m_n_elements] = (-m_sign)*(sin(sta)*ty*(predic.y) + cos(sta)*tx*(predic.y)); //+1 ==real, -1==sim
    debug() << "  ALPHA m_derGB["<<station+dof_cnt*m_n_elements<<"]= " <<  m_derGB[station+dof_cnt*m_n_elements] << endreq;
    
    dof_cnt++;
  }

  // rotation around y-axis
  if ( m_dofs[4] ) {
    m_derGB[station+dof_cnt*m_n_elements] = (-m_sign)*(-sin(sta)*ty*(predic.x)-cos(sta)*tx*(predic.x));//+1 ==real; -1==sim

    dof_cnt++;
  }

  // rotation around z-axis
  if ( m_dofs[5] ) {
    m_derGB[station+dof_cnt*m_n_elements] = (m_sign)*(predic.y*cos(sta)-predic.x*sin(sta));// -1 == real; +1  == sim 
    dof_cnt++;
  }

  
  debug() << " -----------------------------------------------" << endreq;    

  return StatusCode::SUCCESS;
}

StatusCode Derivatives::SetLocal( std::vector<double> &m_derLC,
//                                  const int &rank,
                                  const double &z,
                                  const double &stereo_angle
//                                  const Gaudi::XYZVector orig,
//                                  const Gaudi::XYZVector slope
				) 
 
{
//  double tilt = 0.003601; 

 if(m_tr_pars ==4 ){
     m_derLC[0] =  cos( stereo_angle );
     m_derLC[1] =  z *cos(stereo_angle);
     m_derLC[2] =  sin(stereo_angle  );
     m_derLC[3] =  z *sin(stereo_angle);
  
     
  } else if ( m_tr_pars == 5 ) {
    double dRatio = 2.2*1.e-3; //number from MC studies
//     m_derLC[0] = cos( stereo_angle ); //x0
//     m_derLC[1] = (z + (orig.y()+z*slope.y()) *tilt) *cos(stereo_angle);
//     m_derLC[2] = sin( stereo_angle ); //y0    
//     m_derLC[3] = (z+  (orig.y()+z*slope.y()) *tilt) *sin(stereo_angle);
//     m_derLC[4] = z * z * cos( stereo_angle )+ dRatio*z*z*z*cos( stereo_angle ); //curvature in x
    m_derLC[0] = cos( stereo_angle ); //x0
    m_derLC[1] = z * cos(stereo_angle);
    m_derLC[2] = sin( stereo_angle ); //y0    
    m_derLC[3] = z * sin(stereo_angle);
    m_derLC[4] = z * z * cos( stereo_angle )+ dRatio*z*z*z*cos( stereo_angle ); //curvature in x
  }

   
    return StatusCode::SUCCESS;
}


double Derivatives::GetGlobalDerivative_X(){
  return  m_globalderiv_X;
}

double Derivatives::GetGlobalDerivative_Y(){
  return  m_globalderiv_Y;
}

double Derivatives::GetGlobalDerivative_Z(){
  return  m_globalderiv_Z;
}

double Derivatives::GetGlobalDerivative_RotZ(){
  return m_globalderiv_RotZ;
}

void Derivatives::SetTrueSlope(double slope[]){
  for ( int i=0 ;i<3  ;i++  ) {
    m_TrackSlope[i] = slope[i];
  }
}
void Derivatives::SetTrueOrigin(double origin[]){
  for ( int i=0 ;i<3  ;i++  ) {
    m_TrackOrigin[i] = origin[i];
  }
}

void Derivatives::SetModulePos(double modulepos[]){
  for ( int i=0 ;i<3  ;i++  ) {
    m_ModulePos[i] = modulepos[i];
  }
}

void Derivatives::SetModuleRot(double modulerot[]){
  for ( int i=0 ;i<3  ;i++  ) {
    m_ModuleRot[i] = modulerot[i];
  }
}

void Derivatives::SetModuleOffset(double moduleoff[]){
  for ( int i=0 ;i<3  ;i++  ) {
    m_ModuleOffset[i] = moduleoff[i];
  }
}
void Derivatives::SetStereo(double ster){
  m_Stereo = ster;
}
