// $Id: $
#ifndef VERTEXFUNCTIONTOOLROOTMINI_H 
#define VERTEXFUNCTIONTOOLROOTMINI_H 1

// Include files
#include "VertexFunctionTool.h"


/** @class VertexFunctionToolRootMini VertexFunctionToolRootMini.h
 *   This tool implements IVertexFunctionTool. See the interface class for a description
 *   of the purpose of these classes.
 *   This class uses the Minuit maximum Vf finder algorithm.
 *
 *  @author Julien Cogan and Mathieu Perrin-Terrin
 *  @date   2012-10-15
 */

class VertexFunctionToolRootMini : public VertexFunctionTool{
public: 
  /// Standard constructor
  VertexFunctionToolRootMini( const std::string& type, 
                              const std::string& name,
                              const IInterface* parent);
  //VertexFunctionToolRootMini( ); 

  StatusCode finalize();

  void computeValueMax(LHCb::RecVertex & V, Gaudi::XYZPoint & PMax, double & Max);
  
  virtual ~VertexFunctionToolRootMini( ); ///< Destructor

protected:

private:
  ROOT::Math::Minimizer* m_min;

};


/////////////////////////////
//External tools to compute the max



class Vf_Max : public ROOT::Math::IMultiGenFunction {
public:
  /** constructor */
 Vf_Max(VertexFunctionTool *ptr) :
  m_ptr(ptr)
  {
  }

  /** define the NDim() virtual function
      Returns number of arguments */
  virtual unsigned int NDim() const
  {
    return 3;
  }
  
  
  /** define the actual function */
  double DoEval(const double* x) const
  {
    Gaudi::XYZPoint P;
    P.SetXYZ(x[0],x[1],x[2]);
    return -1.0*m_ptr->computeValueAt(P);
  }

  /** we are required to define the Clone() function */
  virtual Vf_Max * Clone() const {
    return new Vf_Max(m_ptr);
  };

 private:
  VertexFunctionTool *m_ptr;
};

#endif // VERTEXFUNCTIONTOOLROOTMINI_H
