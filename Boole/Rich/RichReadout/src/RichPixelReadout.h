
#ifndef RICHPIXELREADOUT_H
#define RICHPIXELREADOUT_H 1

class RichShape;
class RichShape_flat;
class RichFrontEndDigitiser;

class RichPixelReadout {

public:

  RichPixelReadout();
  ~RichPixelReadout() { cleanUp(); }

  const RichShape* Shape() const;
  const RichShape_flat* Shape_flat() const;
  const RichFrontEndDigitiser* ADC() const;

  double BaseLine() const;
  double SigmaElecNoise() const;
  int BinOfMaximum() const;  
  int FrameSize() const;

  void setBaseLine ( unsigned long index, double ) ;
  void setBaseLine ( double ) ;  //! same value for all index
  void setSigmaElecNoise ( double ) ;
  void setBinOfMaximum ( int ) ;
  void setFrameSize ( int ) ;

private: // methods

  void cleanUp();

private: // data

  RichShape * m_shape;
  RichShape_flat * m_shape_flat;
  RichFrontEndDigitiser * m_frontEnd;

  int m_frameSize;
  double m_baseline;
  double m_sigmaElecNoise;
 
};

// Getters

inline const RichShape* RichPixelReadout::Shape() const
{
  return m_shape;
}

inline const RichShape_flat* RichPixelReadout::Shape_flat() const
{
  return m_shape_flat;
}

inline const RichFrontEndDigitiser* RichPixelReadout::ADC() const
{
  return m_frontEnd;
}

inline int RichPixelReadout::FrameSize() const
{
  return m_frameSize;
}

inline double RichPixelReadout::BaseLine() const
{
  return m_baseline;
}

inline double RichPixelReadout::SigmaElecNoise() const
{
  return m_sigmaElecNoise;
}

// Setters

inline void RichPixelReadout::setBaseLine (unsigned long, double d)
{
  m_baseline = d ;
}

inline void RichPixelReadout::setBaseLine (double d)
{
  m_baseline = d ;
}

inline void RichPixelReadout::setSigmaElecNoise (double d)
{
  m_sigmaElecNoise = d ;
}

inline void RichPixelReadout::setFrameSize(int f)
{
  m_frameSize = f;
}

#endif //RICHPIXELREADOUT_H
