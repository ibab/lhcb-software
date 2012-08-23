
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

HPDFit::HPDFit()
{ 
  // Helps prevent some strange crashes when running in python ....
  m_boundaryPixels.reserve( Rich::DAQ::NumPixelColumns * 
                            Rich::DAQ::NumPixelRows ); 
}

HPDFit::~HPDFit() { }

HPDFit::Result HPDFit::fit ( const TH2D& hist,
                             const Params& params,
                             const unsigned int nEvents ) const
{
  // Make a results object to return
  Result result;

  // Clear working boundary list
  Pixel::List & boundary = m_boundaryPixels;
  boundary.clear();

  // Clean the HPD image if requested
  const TH2D * hToUse = &hist;
  if ( params.cleanHistogram )
  {
    const Clean cleaner(&hist,nEvents);
    hToUse = cleaner.filter();
  }

  // Sobel image filter fit
  if ( "Sobel" == params.type )
  {

    // Sobel filter the cleaned histogram
    const SobelFilter sobel(hToUse);
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
      par_pre.Add ( "Radius", 12.5 , 0.5 );

      // The function to minimise
      SimpleChi2Fit FCN_pre( sobelH, boundary );

      // Make the minimiser
      ROOT::Minuit2::MnMigrad migrad_pre( FCN_pre, par_pre );

      // minimise
      const ROOT::Minuit2::FunctionMinimum min_pre = migrad_pre();

      // If pre fit is OK, do second fit with outlier rejection
      if ( min_pre.IsValid() )
      {
        result.setOK( true );

        // Outlier rejection
        BoundaryOutlierRejection::FitResult outlier_result;
        outlier_result.row    = min_pre.UserParameters().Value("Row0");
        outlier_result.col    = min_pre.UserParameters().Value("Col0");
        outlier_result.radius = min_pre.UserParameters().Value("Radius");
        const BoundaryOutlierRejection outliers( sobelH, boundary, outlier_result );
        
        // Get new pixel list
        outliers.reject(boundary);
        if ( !boundary.empty() )
        {

          // Fitter parameters
          ROOT::Minuit2::MnUserParameters par;
          par.Add ( "Col0",   16.0 , 0.5 );
          par.Add ( "Row0",   16.0 , 0.5 );
          par.Add ( "Radius", 12.5 , 0.5 );

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
      par.Add ( "Radius", 12.5 , 0.5 );

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

  // Sanity checks ...
  result.setOK( result.OK()                             &&
                fabs(result.x()) < params.maxImageShift &&
                fabs(result.y()) < params.maxImageShift );

  // cleanup
  if ( params.cleanHistogram ) { delete hToUse; }

  return result;
}

std::ostream& HPDFit::Params::fillStream ( std::ostream& os ) const
{
  os << "{" 
     << " Fit Type = " << type
     << " | Max. Image Shift = " << maxImageShift;
  if ( cleanHistogram ) { os << " | HPD Cleaning";    }
  else                  { os << " | No HPD Cleaning"; }
  return os << " }";
}

std::ostream& HPDFit::Result::fillStream ( std::ostream& os ) const
{
  os << "{ OK = " << OK()
     << " | (col,row) = (" << col() << "+-" << colErr() << "," 
     << row() << "+-" << rowErr() << ")"
     << " | (x,y) = (" << x() << "+-" << xErr() << "," << y() << "+-" << yErr() << ")"
     << " | radius =" << radInMM() << "+-" << radErrInMM()
     << " }";
  return os;
}
