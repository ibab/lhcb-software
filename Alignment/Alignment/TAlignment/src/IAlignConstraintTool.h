#ifndef TALIGNMENT_IALIGNCONSTRAINTTOOL_H
#define TALIGNMENT_IALIGNCONSTRAINTTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "IGetElementsToBeAligned.h"
#include "SolvKernel/AlEquations.h"
#include "SolvKernel/AlVec.h"
#include "SolvKernel/AlSymMat.h"


namespace Al
{
  /// Static ID object
  static const InterfaceID IID_IAlignConstraintTool( "Al::IAlignConstraintTool", 0, 0 );
  
  class IAlignConstraintTool : virtual public IAlgTool
  {
  public:
    typedef IGetElementsToBeAligned::Elements Elements ;

    // Retrieve interface ID
    static const InterfaceID& interfaceID() { return IID_IAlignConstraintTool ; }
    
    virtual size_t addConstraints(const Elements& elements,
				  const Al::Equations& equations,
				  AlVec& halfDChi2DAlpha, AlSymMat& halfD2Chi2DAlpha2) const = 0 ;
      
    virtual void printConstraints( const AlVec& parameters, const AlSymMat& covariance,
				   std::ostream& logmessage) const = 0 ;
    
  } ;
}

#endif
