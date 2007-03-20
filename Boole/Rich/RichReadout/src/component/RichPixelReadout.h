
#ifndef RICHPIXELREADOUT_H
#define RICHPIXELREADOUT_H 1

namespace Rich
{
  namespace MC
  {
    namespace Digi
    {

      class RichShape;
      class RichFrontEndDigitiser;

      /// Pixel Readout utility class
      class RichPixelReadout
      {

      public:

        RichPixelReadout();

        ~RichPixelReadout() { cleanUp(); }

        // Getters

        inline const RichShape* Shape() const
        {
          return m_shape;
        }

        inline const RichFrontEndDigitiser* ADC() const
        {
          return m_frontEnd;
        }

        inline double BaseLine() const
        {
          return m_baseline;
        }

        inline double SigmaElecNoise() const
        {
          return m_sigmaElecNoise;
        }

        inline int FrameSize() const
        {
          return m_frameSize;
        }

        // Setters

        inline void setBaseLine (const unsigned long, const double d)
        {
          m_baseline = d ;
        }

        inline void setBaseLine (const double d)
        {
          m_baseline = d ;
        }

        inline void setSigmaElecNoise (double d)
        {
          m_sigmaElecNoise = d ;
        }

        inline void setFrameSize(int f)
        {
          m_frameSize = f;
        }

      private: // methods

        void cleanUp();

      private: // data

        RichShape * m_shape;
        RichFrontEndDigitiser * m_frontEnd;

        int m_frameSize;
        double m_baseline;
        double m_sigmaElecNoise;

      };

    }
  }
}

#endif //RICHPIXELREADOUT_H
