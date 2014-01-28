// $Id: $
#ifndef IVERTEXFUNCTIONTOOL_H 
#define IVERTEXFUNCTIONTOOL_H 1

// Include files
// from STL
#include <string>

// from LHCb
#include "Event/Track.h"
#include "Event/RecVertex.h"
#include "TString.h"
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

//static const InterfaceID IID_IVertexFunctionTool ( "IVertexFunctionTool", 1, 0 );

/** @class IVertexFunctionTool IVertexFunctionTool.h IVertexFunctionTool.h
 *  Interface for the vertex function tool. The vertex function tool provides
 *  a function which returns at each point the probability to have a vertex.
 *  The tool contains also some tool to manipulate the vertex function.
 *  
 *  @author Julien Cogan and Mathieu Perrin-Terrin
 *  @date   2012-07-27
 */
class GAUDI_API IVertexFunctionTool : virtual public IAlgTool {
public: 

  DeclareInterfaceID(IVertexFunctionTool, 2, 0);
  

  // Return the interface ID
  //static const InterfaceID& interfaceID() { return IID_IVertexFunctionTool; }

  ///initialise the tracks used to compute the vertex function
  virtual void   setTracks(std::vector<const LHCb::Track*> & vTr) =0;

  ///get the tracks  used to compute the vertex function
  virtual void   getTracks(std::vector<const LHCb::Track*> & vTr) =0;

  ///sort vertex list by Vf value at the vertex fitted position
  virtual void   sortByValueAt(std::vector<LHCb::RecVertex *> & vVtx)=0;

  ///sort vertex list by Vf max value. The maximum being the one closest to the vertex fitted position.
  virtual void   sortByValueMax(std::vector<LHCb::RecVertex *> & vVtx)=0;

  ///evalate, at the position of the vertex given in V, the vertex function defined with the list of tracks given in vTr.
  virtual double valueAt(std::vector<const LHCb::Track*> & vTr, LHCb::RecVertex & V)=0;

  ///evalate, at the position of the vertex given in V, the vertex function defined with the default list of tracks.
  virtual double valueAt(LHCb::RecVertex & V)=0;

  ///evalate, at the position P, the vertex function defined with the default list of tracks.
  virtual double computeValueAt(Gaudi::XYZPoint & P)=0; 

  ///evalate, at the position P, the sum of the Gaussian tubes describing the tracks of the default list.
  virtual double computeSumGaussTubeAt(Gaudi::XYZPoint & P) =0; 
 
  ///return the maximum closest to the vertex fitted position. The first time valueMax is called the mas finder algo is run then the mas value and position is stored in the vertex extra-info.
  virtual double valueMax(LHCb::RecVertex & V)=0;

  ///return the position of the maximum closest to the vertex fitted position. The first time valueMax is called the mas finder algo is run then the mas value and position is stored in the vertex extra-info.
  virtual Gaudi::XYZPoint positionMax(LHCb::RecVertex & V)=0;

  ///return the position and the value of the maximum closest to the vertex fitted position. The first time valueMax is called the mas finder algo is run then the mas value and position is stored in the vertex extra-info.
  virtual void valueMax(LHCb::RecVertex & V, Gaudi::XYZPoint & PMax, double & Max)=0;

  ///return the position of the maximum closest to the vertex fitted position.
  virtual void computeValueMax(LHCb::RecVertex &V, Gaudi::XYZPoint &PMax, double &Max)=0;

  ///check the two vertices are resolved.
  virtual bool   areResolved(LHCb::RecVertex & V1,LHCb::RecVertex & V2)=0;

  ///print the paramters used to defined the vertex function
  virtual void printParam()=0;

  ///set the paramters defining the vertex function 
  virtual void setParam(TString name, double value)=0;

protected:

private:



};
#endif // IVERTEXFUNCTIONTOOL_H
