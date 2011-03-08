
// STL
#include <exception>
#include <iostream>

// local
#include "RichHPDImageAnalysis/HPDFit.h"
#include "RichHPDImageAnalysis/HPDBoundaryFcn.h"
#include "RichHPDImageAnalysis/SimpleChi2Fit.h"
#include "RichHPDImageAnalysis/CleanHPDImage.h"
#include "RichHPDImageAnalysis/SobelFilter.h"
#include "RichHPDImageAnalysis/FullImageFit.h"
#include "RichHPDImageAnalysis/SobelPeakPixelFinder.h"
#include "RichHPDImageAnalysis/BoundaryOutlierRejection.h"

// Minuit2
#include "Minuit2/FunctionMinimum.h"
#include "Minuit2/MnUserParameterState.h"
#include "Minuit2/MnMigrad.h"

// RichRec
#include "RichRecBase/FastRingFitter.h"

using namespace Rich::HPDImage;

//-----------------------------------------------------------------------------
// Implementation file for class : Rich::HPDImage::HPDFit
//
// 2011-03-02 : Chris Jones
//-----------------------------------------------------------------------------

HPDFit::Result HPDFit::fit ( const TH2D& hist,
                             const Params& params ) const
{
  Result result;
  Pixel::List & boundary = m_boundaryPixels;
  boundary.clear();

  const TH2D * hToUse = &hist;
  if ( params.cleanHistogram )
  {
    Clean cleaner(&hist);
    hToUse = cleaner.filter();
  }

  // Sobel image filter fit
  if ( "Sobel" == params.type )
  {

    // Sobel filter the cleaned histogram
    SobelFilter sobel(hToUse);
    TH2D * sobelH = sobel.filter();

    // Get image boundary pixels
    const SobelPeakPixelFinder boundaryFn( sobelH );
    boundaryFn.findBoundary( boundary );

    // If enough hits on boundary, then fit
    if ( !boundary.empty() )
    {

      // Fitter parameters
      ROOT::Minuit2::MnUserParameters par_pre;
      par_pre.Add ( "Col0",   16.0 , 0.5 );
      par_pre.Add ( "Row0",   16.0 , 0.5 );
      par_pre.Add ( "Radius", 16.0 , 0.5 );

      // The function to minimise
      SimpleChi2Fit FCN_pre( sobelH, boundary );

      // Make the minimiser
      ROOT::Minuit2::MnMigrad migrad_pre( FCN_pre, par_pre );

      // minimise
      const ROOT::Minuit2::FunctionMinimum min_pre = migrad_pre();

      // fill results
      if ( min_pre.IsValid() )
      {
        result.setOK( true );

        // Make a copy of original boundary pixels
        const Pixel::List originalBoundary = boundary;

        // Outlier rejection
        BoundaryOutlierRejection::FitResult outlier_result;
        outlier_result.row    = min_pre.UserParameters().Value("Row0");
        outlier_result.col    = min_pre.UserParameters().Value("Col0");
        outlier_result.radius = min_pre.UserParameters().Value("Radius");
        const BoundaryOutlierRejection outliers( sobelH,
                                                 originalBoundary,
                                                 outlier_result );

        // Get new pixel list
        outliers.reject(boundary);
        if ( !boundary.empty() )
        {

          // Fitter parameters
          ROOT::Minuit2::MnUserParameters par;
          par.Add ( "Col0",   16.0 , 0.5 );
          par.Add ( "Row0",   16.0 , 0.5 );
          par.Add ( "Radius", 16.0 , 0.5 );

          // The function to minimise
          SimpleChi2Fit FCN( sobelH, boundary );

          // Make the minimiser
          ROOT::Minuit2::MnMigrad migrad( FCN, par );

          // minimise
          const ROOT::Minuit2::FunctionMinimum min = migrad();

          // Fill results object
          if ( min.IsValid() )
          {
            // Use final fit results
            result.setRowAndErr( min.UserParameters().Value("Row0"),
                                 min.UserParameters().Error("Row0") );
            result.setColAndErr( min.UserParameters().Value("Col0"),
                                 min.UserParameters().Error("Col0") );
            result.setRadAndErr( min.UserParameters().Value("Radius"),
                                 min.UserParameters().Error("Radius") );
          }
          else
          {
            // use prefit results (most likely almost as good)
            result.setRowAndErr( min_pre.UserParameters().Value("Row0"),
                                 min_pre.UserParameters().Error("Row0") );
            result.setColAndErr( min_pre.UserParameters().Value("Col0"),
                                 min_pre.UserParameters().Error("Col0") );
            result.setRadAndErr( min_pre.UserParameters().Value("Radius"),
                                 min_pre.UserParameters().Error("Radius") );
          }

        } // outlier rejected not empty

      } // pre fit OK

    } // boundary OK

    // clean up
    delete sobelH;

  }
  // Original Fit from Tom/David
  else if ( "SimpleChi2" == params.type )
  {

    // Get image boundary pixels
    const HPDBoundaryFcn boundaryFn( hToUse );
    boundaryFn.findBoundary(boundary);

    // If enough hits on boundary, then fit
    if ( !boundary.empty() )
    {

      // Fitter parameters
      ROOT::Minuit2::MnUserParameters par;
      par.Add ( "Col0",   16.0 , 0.5 );
      par.Add ( "Row0",   16.0 , 0.5 );
      par.Add ( "Radius", 16.0 , 0.5 );

      // The function to minimise
      SimpleChi2Fit FCN( hToUse, boundary );

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
  // test fit using FastRingFitter
  else if ( "FastRingFit" == params.type )
  {

    // Get image boundary pixels
    const HPDBoundaryFcn boundaryFn( hToUse );
    boundaryFn.findBoundary(boundary);

    // If enough hits on boundary, then fit
    if ( !boundary.empty() )
    {

      // Make a fitter
      Rich::Rec::FastRingFitter fitter;

      // Add the points on the boundary
      for ( Pixel::List::const_iterator iP = boundary.begin();
            iP != boundary.end(); ++iP )
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

  // cleanup
  if ( params.cleanHistogram ) { delete hToUse; }

  return result;
}
