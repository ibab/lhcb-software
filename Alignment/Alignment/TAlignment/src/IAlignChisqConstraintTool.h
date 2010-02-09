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
    struct SurveyData
    {
      SurveyData() {
	err[0] = err[1] = err[2] = err[3] = err[4] = err[5] = -1 ;
	par[0] = par[1] = par[2] = par[3] = par[4] = par[5] = 0 ;
      }
      std::string name ;
      double par[6] ;
      double err[6] ;
    } ;

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

    virtual SurveyData surveyData( const AlignmentElement& element ) const = 0 ;
  } ;
}

#endif

//  LocalWords:  TALIGNMENT
