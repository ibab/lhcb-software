#ifndef TALIGNMENT_IALIGNCHISQCONSTRAINTTOOL_H
#define TALIGNMENT_IALIGNCHISQCONSTRAINTTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "IGetElementsToBeAligned.h"
#include "AlignKernel/AlEquations.h"
#include "AlignKernel/AlVec.h"
#include "AlignKernel/AlSymMat.h"
#include "Event/ChiSquare.h"

namespace Al
{
  /// Static ID object
  static const InterfaceID IID_IAlignChisqConstraintTool( "Al::IAlignChisqConstraintTool", 0, 0 );
  
  class IAlignChisqConstraintTool : virtual public IAlgTool
  {
  public:

    typedef IGetElementsToBeAligned::Elements Elements ;

    // Retrieve interface ID
    static const InterfaceID& interfaceID() { return IID_IAlignChisqConstraintTool ; }
    virtual LHCb::ChiSquare addConstraints(const Elements& elements,
					   AlVec& halfDChi2DAlpha, AlSymMat& halfD2Chi2DAlpha2,
					   std::ostream& logmessage) const = 0 ;
    virtual LHCb::ChiSquare addConstraints(const Elements& inputelements,
					   Al::Equations& equations,
					   std::ostream& logmessage) const = 0 ;
    virtual LHCb::ChiSquare chiSquare( const AlignmentElement& element, bool activeonly=true) const = 0 ;
    // this returns the survey in the AlignmentFrame of the element, taking correlations into account.
    virtual const AlParameters* surveyParameters( const AlignmentElement& element ) const = 0 ;
  } ;
}

#endif

//  LocalWords:  TALIGNMENT
