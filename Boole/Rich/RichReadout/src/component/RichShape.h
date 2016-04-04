
#ifndef RICHREADOUT_RICHSHAPE_H
#define RICHREADOUT_RICHSHAPE_H 1

#include <math.h>
#include <map>
#include <memory>

// RichKernel
#include "RichKernel/RichMap.h"

// richdet
#include "RichDet/Rich1DTabFunc.h"

namespace Rich
{
  namespace MC
  {
    namespace Digi
    {

      /// Rich Shape Utility class
      class RichShape
      {

      public:

        RichShape ( const double tpeak,
                    const double alpha )
          : m_alpha   ( alpha ),
            m_tpeak   ( tpeak ),
            m_maxTime ( 200   ),
            m_minTime ( 0     )
        {

          // Initialise interpolator
          Rich::Map<double,double> data;
          data[-1] = 0; // Just to extend interpolator range to cover 0 safetly.
          for ( int i = 0; i < 101; ++i )
          {
            const double time = m_minTime + ((m_maxTime-m_minTime)*i)/100;
            data[time] = responseFunction(time);
          }
          m_tabFunc.reset( new Rich::TabulatedFunction1D(data) );

        }

        inline double getTpeak () const
        {
          return m_tpeak;
        }

        inline double getPower () const
        {
          return m_alpha;
        }

        inline double operator[] ( const double time ) const
        {
          // return interpolator (faster than function)
          return ( time < m_minTime || time > m_maxTime ? 0 : m_tabFunc->value(time) );
          //return ( time < m_minTime || time > m_maxTime ? 0 : responseFunction(time) );
        }

      private: // methods

        inline double responseFunction( const double time ) const
        {
          return ( pow ( time/getTpeak(), getPower() ) *
                   exp ( -(time-getTpeak()) / (getTpeak()*getPower()) ) );
        }

      private: // data

        // Response function parameters
        double m_alpha;
        double m_tpeak;

        /// Max time
        double m_maxTime;

        /// Min time
        double m_minTime;

        std::unique_ptr<Rich::TabulatedFunction1D> m_tabFunc;

      };

    }
  }
}

#endif // RICHREADOUT_RICHSHAPE_H
