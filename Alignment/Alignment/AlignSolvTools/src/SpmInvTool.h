// $Id: SpmInvTool.h,v 1.2 2007-11-21 14:12:10 janos Exp $
#ifndef ALIGNSOLVTOOLS_SPMINVTOOL_H 
#define ALIGNSOLVTOOLS_SPMINVTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// Interface
#include "AlignmentInterfaces/IAlignSolvTool.h" 

/** @class SpmInvTool SpmInvTool.h AlignSolvTools/SpmInvTool.h
 *  
 *
 *  @author Adlene Hicheur
 *  @date   2007-06
 */

class SpmInvTool : public GaudiTool, virtual public IAlignSolvTool {
public: 
  /// Standard constructor
  SpmInvTool( const std::string& type, 
	      const std::string& name,
	      const IInterface* parent);
  
  virtual ~SpmInvTool( ); ///< Destructor

  // Call method to compute the solution, get symetric matrix and 
  // vector in input and replaces them by inverse matrix and solution vector in output.
  virtual bool compute(AlSymMat& ,AlVec&) const; 




protected:

private:

  //parameters
  
  int SolvSpmInv(AlSymMat& M, AlVec& B);
  
  void Precond(AlSymMat& M, AlVec& r, AlVec& c, bool equIn);

  


};
#endif // ALIGNSOLVTOOLS_ALIGNSOLVTOOL_H
