
#include "RichReadout/PixelReadout.h"
#include "RichReadout/RichShape.h"
#include "RichReadout/RichCoder.h"
#include "RichReadout/RichNoisifier.h"
#include "RichReadout/RichFrontEndDigitiser.h"

PixelReadout::PixelReadout()
  :  coder_(0),
     noisifier_(0),
     frontEnd_(0)
{

  shape_ = new RichShape(25.0, 2.7);
  coder_ = new RichCoder();
  noisifier_ = new RichNoisifier();

  frameSize_ = 100;
  peakTime_ = 25.0;
  sigmaElecNoise_ = 0.9;
  gain_ = 15.0; //kV
  offset_ = 50;
  calib_ = 4.42;
  threshold_ = 8.8;

  frontEnd_ = new RichFrontEndDigitiser( threshold_*sigmaElecNoise_,
                                         calib_, offset_ );
  
}

PixelReadout::~PixelReadout() { cleanUp(); }

void PixelReadout::cleanUp() {
  
  if ( shape_ ) delete shape_;
  if ( coder_ ) delete coder_;
  if ( noisifier_ ) delete noisifier_;
  if ( frontEnd_ ) delete frontEnd_;

}

// Getters

const RichShape* PixelReadout::Shape() const
{
  return shape_;
}

const RichNoisifier* PixelReadout::Noisifier() const
{
  return noisifier_;
}

const RichFrontEndDigitiser* PixelReadout::ADC() const
{
  return frontEnd_;
}

const RichCoder* PixelReadout::Coder() const
{
  return coder_;
}

double PixelReadout::Gain() const
{
  return gain_;
}

int PixelReadout::FrameSize() const
{
  return frameSize_;
}

double PixelReadout::PeakTime() const
{
  return peakTime_;
}

double PixelReadout::BaseLine() const
{
  return baseline_;
}

double PixelReadout::SigmaElecNoise() const
{
  return sigmaElecNoise_ ;
}

int PixelReadout::BinOfMaximum() const
{
  return binOfMaximum_;
}

// Setters

void PixelReadout::setShape ( RichShape* p)
{
  shape_ = p;
}

void PixelReadout::setNoisifier (RichNoisifier* p)
{
  noisifier_ = p;
}

void PixelReadout::setADC (RichFrontEndDigitiser* p )
{
  frontEnd_ = p;
}

void PixelReadout::setGain (unsigned int, double d)
{
  gain_ = d;
}

void PixelReadout::setBaseLine (unsigned int, double d)
{
  baseline_ = d ;
}


void PixelReadout::setBaseLine (double d)
{
  baseline_ = d ;
}

void PixelReadout::setSigmaElecNoise (double d)
{
  sigmaElecNoise_ = d ;
}

void PixelReadout::setPeakTime (double d)
{
  peakTime_ = d ;
}

void PixelReadout::setFrameSize(int f)
{
  frameSize_ = f;
}

void PixelReadout::setBinOfMaximum(int b)
{
  binOfMaximum_ = b;
}

