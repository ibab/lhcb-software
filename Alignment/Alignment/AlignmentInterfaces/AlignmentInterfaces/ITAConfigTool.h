#ifndef ITACONFIGTOOL_H
#define ITACONFIGTOOL_H
//-----------------------------------------------------------------------------
/** @file TAConfigTool.h
 *
 *  Header file for Tstation alignment : ITAConfigTool
 *
 *  CVS Log :-
 *  $Id: ITAConfigTool.h,v 1.14 2010-02-06 11:44:25 jblouw Exp $
 *
 *  @author J. Blouw johan.blouw@cern.ch
 *  @date   12/04/2007
 */
//-----------------------------------------------------------------------------


#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/Transform3DTypes.h"
#include "GaudiKernel/Point3DTypes.h"
#include <string>
// geometry
#include "DetDesc/DetectorElement.h"
#include "OTDet/DeOTDetector.h"

static const InterfaceID 
IID_ITAConfigTool("ITAConfig",0,0);

//Forward declarations
struct Point
{
  double x;
  double y;
};


namespace LHCb {
	class Track;
  	class Trajectory;
 	class Measurement;
	class LHCbID;
  	class MCHit;
  	class State;
}


class ITAConfigTool : virtual public IAlgTool {
  
 public:
  // Retrieve interface ID
  static const InterfaceID & interfaceID() {
    return IID_ITAConfigTool; 
  }

  virtual StatusCode Initialize( std::vector<std::string> & ) = 0;
  virtual StatusCode CalcResidual( const LHCb::LHCbID &,
                                   int ,
                                   struct Point  &,
                                   bool, double &, double & ,
                                   double &,
                                   struct Point  &
                                   ) = 0;


  //MD  virtual StatusCode FillMatrix( const int &,
			// 	 const double [], 
// 				 const double &, 
// 				 const double &, 
// 				 const double & ) = 0;
  virtual   StatusCode FillMatrix( int ,
                                   const  struct Point ,
                                   const double ,
                                   const double,
                                   const double,
                                   double ) = 0;
  

  virtual int InvMatrix( double [][4],
                         double [],
                         const int & ) = 0;
  virtual int InvMatrixP( double [][5],
                         double [],
                         const int & ) = 0;

  virtual StatusCode ConfMatrix( double rmeasI, 
                                 double wght,
                                 unsigned int rank, 
                                 const double,
                                 double local_vec[],
                                 double local_mat[][4],
                                 double stereo_angle
                                 ) =0;
  virtual StatusCode ConfMatrixP( double rmeasI, 
                                 double wght,
                                 unsigned int rank, 
                                 const double,
                                 double local_vec[],
                                 double local_mat[][5],
                                 double stereo_angle
                                 ) =0;
  
  
  virtual StatusCode ZeroMatrVec( double [][4], double [] ) = 0;
  virtual StatusCode ZeroMatrVecP( double [][5], double [] ) = 0;
  virtual StatusCode CacheDetElements() = 0;
  virtual StatusCode ResetGlVars() = 0;
  virtual bool AlignDetector( std::string & ) = 0;
  virtual StatusCode LocalTrackFit( int &,
                                    std::vector< std::vector< double> >,
                                    std::vector<double> &,
                                    const int ,
                                    std::vector<double> &,
                                    double &,
                                    double &,
//                                    unsigned int,
                                    int) = 0;

  //write xml
  virtual StatusCode PrintParameters( std::vector<double> & , int)=0;


  virtual StatusCode GetHalflayerReference(DeOTLayer* , Gaudi::XYZPoint& ,Gaudi::XYZPoint& , int )=0;
  
   
  virtual int NumTrPars() = 0;
  virtual std::vector<bool> NumDOF() = 0; //MD

  virtual unsigned int GetNoFixedObj() =0; //MD
  

  //MD useless..  virtual Gaudi::XYZVector Residual( LHCb::Track &, 
  //				     LHCb::LHCbID & ) = 0;
  virtual StatusCode GlobalFit( std::vector<double> &, 
                                std::vector<double> &,
                                std::vector<double> ) = 0;
  virtual StatusCode Rank( LHCb::LHCbID &, int & ) = 0;

  virtual void ArraytoVector( const double [], std::vector<double> & , int ) =0;
  
  virtual int NumAlignPars() = 0;
  //MD
  virtual std::vector< std::vector<double> > getMPHits() = 0;
  virtual void clearMPHits() = 0;
  virtual std::vector<Gaudi::Transform3D> GetDetMap() = 0;
  
  virtual void SetTrackPar(const std::vector<double>) = 0;
  virtual StatusCode SetAlignmentPar(const std::vector<double> & )=0;
  virtual void MakeAlignParVec() =0;
  virtual void MakeTrackParVec() =0;
  virtual DetectorElement* GetDetEl()=0;
  virtual void CheckLChi2(const double &, const int &,const int &, bool & , double )=0;
  virtual void SetMeanZ(double )  = 0;
  virtual double GetMeanZ()       = 0;
  virtual double GetDetEl_meanZ() = 0;
  virtual std::vector<double> GetDetEl_Z()      = 0;
  virtual std::vector<double> GetStereoAngle()  = 0;
  virtual std::vector<double> GetMisalInput_X() = 0;
  virtual std::vector<double> GetMisalInput_Y() = 0;
  virtual std::vector<double> GetMisalInput_Z() = 0;
  virtual std::vector<double> GetMisalInput_A() = 0;
  virtual std::vector<double> GetMisalInput_B() = 0;
  virtual std::vector<double> GetMisalInput_C() = 0;
  virtual double GetChi2Scale()   = 0;
  virtual double GetMinChi2()     = 0;
  virtual double GetOutlierMax()  = 0;
  virtual StatusCode NewMeasurement(double &, const int  ,const struct Point, 
                            double )=0;
  virtual std::map<std::string, int> GetCMap() = 0;  
  virtual std::multimap<int, std::pair<double, double> > GetRnkZAngleMap() = 0;
  virtual int nRanks() = 0;
  
    
};

#endif // ITACONFIGTOOL_H
