// $Id: $
#ifndef VERTEXFUNCTIONTOOL_H 
#define VERTEXFUNCTIONTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "IncTopoVert/IVertexFunctionTool.h"            // Interface
#include "Event/Track.h"
#include "Event/RecVertex.h"
#include "Kernel/IDistanceCalculator.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "LHCbMath/MatrixTransforms.h"


//new includes for the ROOT minimizer
#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"

/** @class VertexFunctionTool VertexFunctionTool.h
 *   This tool implements IVertexFunctionTool. See the interface class for a description
 *   of the purpose of these classes.
 *   This class uses the memory maximum Vf finder algorithm. This algorithm memorises each
 *   steps in order to infers the direction where the maximum should be. 
 *
 *  @author Julien Cogan and Mathieu Perrin-Terrin
 *  @date   2012-07-27
 */


//enum containing the key to read the extra info in the RecVertex
namespace VertexFunctionInfo{
    enum Type {  VfAt      = 0,
               VfMax     = 1,
               VfMaxPosX = 2,
               VfMaxPosY = 3,
               VfMaxPosZ = 4,

    };
  } 


using namespace VertexFunctionInfo;

class VertexFunctionTool : public GaudiTool, virtual public IVertexFunctionTool {

public: 
  /// Standard constructor
  VertexFunctionTool( const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);

  StatusCode initialize();

  void   setTracks(std::vector<const LHCb::Track*> & vTr) {m_vTr = vTr;}
  void   getTracks(std::vector<const LHCb::Track*> & vTr) {vTr = m_vTr;}
  void   sortByValueAt(std::vector<LHCb::RecVertex *> & vVtx);
  void   sortByValueMax(std::vector<LHCb::RecVertex *> & vVtx);

  double valueAt(std::vector<const LHCb::Track*> & vTr, LHCb::RecVertex & V);
  double valueAt(LHCb::RecVertex & V);
  double computeValueAt(Gaudi::XYZPoint & P);
  double computeValueAt2(const double *xx){
    Gaudi::XYZPoint P;
    P.SetXYZ(xx[0],xx[1],xx[2]);
    return computeValueAt(P);
  };

  Gaudi::XYZVector computeGradientAt(Gaudi::XYZPoint & P);
  double computeSumGaussTubeAt(Gaudi::XYZPoint & P);

  double valueMax(LHCb::RecVertex & V);
  Gaudi::XYZPoint positionMax(LHCb::RecVertex & V);
  void valueMax(LHCb::RecVertex & V, Gaudi::XYZPoint & PMax, double & Max);
  void computeValueMax(LHCb::RecVertex & V, Gaudi::XYZPoint & PMax, double & Max);
  /*
  void computeValueMax2(LHCb::RecVertex & V, Gaudi::XYZPoint & PMax, double & Max, double &step, int& NumberOfIterations,Gaudi::XYZVector & Previous_gradient);
  void computeValueMax(LHCb::RecVertex & V, Gaudi::XYZPoint & PMax, double & Max, double &step, int& NumberOfIterations){
    Gaudi::XYZVector* Previous_gradient = new Gaudi::XYZVector(0,0,0);
    computeValueMax(V, PMax, Max, step, NumberOfIterations,*Previous_gradient);
    delete Previous_gradient;
    
};
  
  void computeValueMax(LHCb::RecVertex & V, Gaudi::XYZPoint & PMax, double & Max, double &step, int& NumberOfIterations,Gaudi::XYZVector & Previous_gradient){
    PMax = V.position();
    Max = valueAt(V);
  };


  void computeValueMax(LHCb::RecVertex & V, Gaudi::XYZPoint & PMax, double & Max) {
    int MyNumberOfIterations=0;
    Double_t* step = new Double_t(m_step);
    return computeValueMax(V,PMax,Max,*step,MyNumberOfIterations);
    delete step;
  };
  */
  
  
  bool   areResolved(LHCb::RecVertex & V1,LHCb::RecVertex & V2);
  void printParam();
  void setParam(TString name, double value);
  IDistanceCalculator*  m_Geom;
  ITrackExtrapolator *  m_Trans;

  virtual ~VertexFunctionTool( ); ///< Destructor


protected:

  std::vector<const LHCb::Track*> m_vTr;
  double m_step;
  double m_max_finder_min_gradient_mag;
  int    m_max_finder_max_iteration;
  double m_max_finder_min_step;
  double m_max_finder_max_jump;
  double m_resolver_cut; 
  double m_resolver_min_step;
  double m_resolver_max_iteration;
  void setVfMax(LHCb::RecVertex & V, Gaudi::XYZPoint & PositionMax, double & MaxValue);
  void setVfAt(LHCb::RecVertex & V,double & VfValue);
  bool  areResolveDicotomy(Gaudi::XYZPoint P1,Gaudi::XYZVector Dir,double &Vmin,int nMeshPoints
                          ,double step);
  Gaudi::XYZVector impactParam(const LHCb::Track &Tr ,const LHCb::VertexBase & V);

  double ParabolicExtrapolator(Gaudi::XYZVector &gradient, Gaudi::XYZPoint &initialPoint, double &deltaR);
  
};


/////////////////////////////
//External tools to sort

class orderValueMax
{
public :
	orderValueMax(VertexFunctionTool *ptr) :
		m_ptr(ptr)
	{
	}

	bool operator ()(LHCb::RecVertex *a,LHCb::RecVertex  *b)
	{
		double va = m_ptr->valueMax(*a);
		double vb = m_ptr->valueMax(*b);
		return (va<vb);  
	};
private:
	VertexFunctionTool *m_ptr;

};


class orderValueAt
{
public :
	orderValueAt(VertexFunctionTool *ptr) :
		m_ptr(ptr)
	{
	}

	bool operator ()(LHCb::RecVertex *a,LHCb::RecVertex  *b)
	{
		double va = m_ptr->valueAt(*a);
		double vb = m_ptr->valueAt(*b);
		return (va<vb);  
	};
private:
	VertexFunctionTool *m_ptr;
 
};


#endif // VERTEXFUNCTIONTOOL_H

