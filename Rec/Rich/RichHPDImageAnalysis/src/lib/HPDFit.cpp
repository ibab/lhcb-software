
// STL
#include <exception>
#include <iostream>

// local
#include "RichHPDImageAnalysis/HPDFit.h"
#include "RichHPDImageAnalysis/HPDBoundaryFcn.h"
#include "RichHPDImageAnalysis/SimpleChi2Fit.h"

// Minuit2
#include "Minuit2/FunctionMinimum.h"
#include "Minuit2/MnUserParameterState.h"
#include "Minuit2/MnMigrad.h"

// RichRec
#include "RichRecBase/FastRingFitter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Rich::HPDImage::HPDFit
//
// 2011-03-02 : Chris Jones
//-----------------------------------------------------------------------------

const Rich::HPDImage::HPDFit::Result
Rich::HPDImage::HPDFit::fit ( const TH2D& hist,
                              const Rich::HPDImage::HPDFit::Params& params ) const
{
  HPDFit::Result result;

  // Original Fit from Tom/David
  if ( "SimpleChi2" == params.type )
  {

    // Get image boundary pixels
    const HPDBoundaryFcn boundaryFn( &hist, params.cutFraction );
    result.setBoundaryPixels( boundaryFn.boundary() );

    // If enough hits on boundary, then fit
    if ( boundaryFn.boundary().size() >= params.minBoundary )
    {

      ROOT::Minuit2::MnUserParameters par;
      par.Add ( "Col0",   16.0 , 0.5 );
      par.Add ( "Row0",   16.0 , 0.5 );
      par.Add ( "Radius", 16.0 , 0.5 );

      // The function to minimise
      SimpleChi2Fit FCN( &hist, boundaryFn.boundary() );

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
      }

    } // Enough hits on boundary

  }
  else if ( "FastRingFit" == params.type )
  {

    // Get image boundary pixels
    const HPDBoundaryFcn boundaryFn( &hist, params.cutFraction );
    result.setBoundaryPixels( boundaryFn.boundary() );

    // If enough hits on boundary, then fit
    if ( boundaryFn.boundary().size() >= params.minBoundary )
    {

      // Make a fitter
      Rich::Rec::FastRingFitter fitter;

      // Add the points on the boundary
      for ( Pixel::List::const_iterator iP = boundaryFn.boundary().begin();
            iP != boundaryFn.boundary().end(); ++iP )
      {
        fitter.addPoint( (double)iP->col, (double)iP->row );
      }

      // Fit
      fitter.fit();

      // Extract the results
      result.setOK( fitter.result().Status == 0 );
      if ( result.OK() )
      {
        result.setRowAndErr( fitter.result().YCenter, 0.1 ); // no error
        result.setColAndErr( fitter.result().XCenter, 0.1 ); // no error
        result.setRadAndErr( fitter.result().Radius,  0.1 ); // no error
      }

    }

  }
  else
  {
    std::cerr << "Unknown fit type " + params.type << std::endl;
    throw std::exception( );
  }

  // Max 'reasonable' shift in mm
  // Maybe should make a tunable parameter ..
  const double maxShift = 3.0;

  // Sanity checks ...
  result.setOK( result.OK() &&
                fabs(result.x()) < maxShift &&
                fabs(result.y()) < maxShift );

  // return result
  return result;
}
