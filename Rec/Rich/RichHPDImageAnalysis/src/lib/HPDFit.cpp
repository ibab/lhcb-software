
// local
#include "RichHPDImageAnalysis/HPDFit.h"
#include "RichHPDImageAnalysis/HPDBoundaryFcn.h"

// Minuit2
#include "Minuit2/FunctionMinimum.h"
#include "Minuit2/MnUserParameterState.h"
#include "Minuit2/MnMigrad.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HPDFit
//
// 2011-03-02 : Chris Jones
//-----------------------------------------------------------------------------

const Rich::HPDImage::HPDFit::Result 
Rich::HPDImage::HPDFit::fit ( const TH2D& hist,
                              const Rich::HPDImage::HPDFit::Params& params ) const
{
  HPDFit::Result result;
  
  HPDBoundaryFcn FCN( &hist, params.cutFraction );
  if ( FCN.findBoundary() >= params.minBoundary )
  {
    
    ROOT::Minuit2::MnUserParameters par;
    par.Add("Col0",   16. , 0.5 );
    par.Add("Row0",   16. , 0.5 );
    par.Add("Radius", 16. , 0.5 );

    // Make the minimiser
    ROOT::Minuit2::MnMigrad migrad( FCN, par );
    
    // minimise
    const ROOT::Minuit2::FunctionMinimum min = migrad();

    // fill results
    result.OK = min.IsValid();
    if ( result.OK )
    {
      result.col    = min.UserParameters().Value("Col0");
      result.colErr = min.UserParameters().Error("Col0");
      result.row    = min.UserParameters().Value("Row0");
      result.rowErr = min.UserParameters().Error("Row0");
      result.rad    = min.UserParameters().Value("Radius");
      result.radErr = min.UserParameters().Error("Radius");
    }

  }

  // return result
  return result;
}
