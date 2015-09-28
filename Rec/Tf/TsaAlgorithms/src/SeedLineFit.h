// $Id: SeedLineFit.h,v 1.2 2007-11-07 17:28:39 mschille Exp $
#ifndef SEEDLINEFIT_H
#define SEEDLINEFIT_H 1

#include <iostream>
#include "GaudiKernel/MsgStream.h"

namespace Tf
{
  namespace Tsa
  {

    class SeedTrack;

    class SeedLineFit{

    public:

      SeedLineFit( MsgStream & msg, double z0, double sth, double, unsigned int nIter = 10);

      ~SeedLineFit() {}

      int fit( SeedTrack* seed ) const;

      MsgStream & msgStream() const { return *m_msg; }

    private:

      void reset() const;
      double m_z0;
      double m_sth;
      double m_scth;
      unsigned int m_maxIter;
      double m_outlierCut;
      mutable double m_a;
      mutable double m_b;
      mutable double m_c;
      mutable double m_d;
      mutable double m_e;
      mutable MsgStream * m_msg;

    };

    inline SeedLineFit::SeedLineFit(MsgStream & msg,double z0, double sth, double outlierCut, unsigned int nIter)
      : m_z0(z0),
        m_sth(sth),
        m_scth(1.0/sth),
        m_maxIter(nIter),
        m_outlierCut(outlierCut),
        m_msg(&msg)
    {
      msgStream() << MSG::DEBUG
                  << "SeedLineFit::fit : z0=" << m_z0 << " sth=" << m_sth << " scth=" << m_scth
                  << " maxIter=" << m_maxIter << " outlierCut=" << m_outlierCut << endmsg;
      reset();
    }

    inline void SeedLineFit::reset() const
    {
      m_a = 0.;
      m_b = 0.;
      m_c = 0.;
      m_d = 0.;
      m_e = 0.;
    }

  }
}

#endif
