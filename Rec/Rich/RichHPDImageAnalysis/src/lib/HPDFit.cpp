
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
    
    // Max 'reasonable' shift in mm
    // Maybe should make a tunable parameter ..
    const double maxShift = 3.0;

    ROOT::Minuit2::MnUserParameters par;
    par.Add("Col0",   16. , 0.5 );
    par.Add("Row0",   16. , 0.5 );
    par.Add("Radius", 16. , 0.5 );

    // Make the minimiser
    ROOT::Minuit2::MnMigrad migrad( FCN, par );
    
    // minimise
    const ROOT::Minuit2::FunctionMinimum min = migrad();

    // fill results
    result.setOK( min.IsValid() );
    if ( result.OK() )
    {
      result.setRowAndErr( min.UserParameters().Value("Row0"),
                           min.UserParameters().Error("Row0") );
      result.setColAndErr( min.UserParameters().Value("Col0"),
                           min.UserParameters().Error("Col0") );
      result.setRadAndErr( min.UserParameters().Value("Radius"),
                           min.UserParameters().Error("Radius") );
      // Sanity checks ...
      if ( fabs(result.x()) > maxShift ||
           fabs(result.y()) > maxShift )
      {
        result.setOK( false );
      }
    }

  }

  // return result
  return result;
}
