#ifndef _TsaParabola_H
#define _TsaParabola_H

#include "TsaKernel_Tf/Line.h"

namespace Tf
{
  namespace Tsa
  {

    /** @class Parabola Parabola.h TsaKernel/Parabola.h
     *
     *  a parabola
     *
     *  @author M.Needham
     *  @date   31/05/2004
     */

    class Parabola {

    public:

      Parabola(const double a, const double b, const double c);

      ~Parabola(){}

      double value(const double z) const;

      double derivative(const double z) const;

      double curvature() const;

      Line tangent(const double z) const;

      double b() const;

      double c() const;

    private:

      double m_a;
      double m_b;
      double m_c;

    };

    inline Parabola::Parabola(const double a,
                              const double b,
                              const double c):
      m_a(a),
      m_b(b),
      m_c(c)
    {
      // constructer
    }

    inline double Parabola::value(const double z) const{
      return z*((m_a*z)+m_b)+m_c;
    }

    inline Line Parabola::tangent(const double z) const{
      const double slope = derivative(z);
      return Line(slope,value(z) - (slope*z));
    }

    inline double Parabola::derivative(const double z) const{
      return ((2.0*m_a*z) +m_b);
    }

    inline double Parabola::curvature() const{
      return m_a;
    }

    inline double Parabola::b() const{
      return m_b;
    }

    inline double Parabola::c() const{
      return m_c;
    }

  }
}

#endif // _TsaParabola_H
