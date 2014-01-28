// $Id: $
#ifndef VERTEXFUNCTIONTOOLSTARMAX_H 
#define VERTEXFUNCTIONTOOLSTARMAX_H 1

// Include files
#include "VertexFunctionTool.h"

/** @class VertexFunctionToolStarMax VertexFunctionToolStarMax.h
 *   This tool implements IVertexFunctionTool. See the interface class for a description
 *   of the purpose of these classes.
 *   This class uses the Star maximum Vf finder algorithm. This algorithm trie twenty six
 *   different directions disposed as to form a regular polyhedra and finally walk in the
 *   direction in which the vertex function increases the most.
 *
 *  @author Giampiero Mancinelli, Mathieu Perrin-Terrin
 *  @date   2012-10-16
 */

class VertexFunctionToolStarMax  : public VertexFunctionTool{
public: 
  /// Standard constructor
  VertexFunctionToolStarMax( const std::string& type, 
                             const std::string& name,
                             const IInterface* parent);
  /// Standard constructor
  void computeValueMax(LHCb::RecVertex & V, Gaudi::XYZPoint & PMax, double & Max){
    double step = m_step;
    int NumberOfIterations = 0;
    computeValueMax(V, PMax, Max, step, NumberOfIterations);
  };
  void computeValueMax(LHCb::RecVertex & V, Gaudi::XYZPoint & PMax, double & Max, double &step, int &NumberOfIterations);
  virtual ~VertexFunctionToolStarMax( ); ///< Destructor

protected:

private:


};
#endif // VERTEXFUNCTIONTOOLSTARMAX_H
