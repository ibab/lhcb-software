#include "OTDet/WalkRelation.h"
#include "vdt/exp.h"

namespace OTDet
{

  WalkRelation::WalkRelation() :
    m_upperend(0.f), m_foff(0.f), m_fdptfactor(0.f),
    m_approx(0., 1., [] (double) { return 0.;} ), // dummy walk relation
    m_famp(0.f), m_fovertau(0.f),
    m_off(0.0), m_amp(0.0), m_tau(0.0), m_dpt(0.0), m_dptfactor(0.0), m_overtau(0.0)
  {
  }

  WalkRelation::WalkRelation(double off, double amp, double tau, double dpt) :
    m_upperend(9.1 * std::abs(tau)),
    m_foff(off), m_fdptfactor((0. != tau) ? (1e-3 * dpt) : 0.),
    m_approx(0., 9.1 * std::abs(tau), [off, amp, tau, dpt] (double l) {
	return off + amp * (std::tanh(((0. != tau) ?
	      (1. / std::abs(tau)) : 0.) * l) - 1.0) +
       	((0. != tau) ? (1e-3 * dpt) : 0.) * l; }),
    m_famp(amp), m_fovertau((0. != tau) ? (1. / std::abs(tau)) : 0.),
    m_off(off), m_amp(amp), m_tau(tau), m_dpt(dpt),
    m_dptfactor((0. != tau) ? (1e-3 * m_dpt) : 0.),
    m_overtau((0. != tau) ? (1. / std::abs(tau)) : 0.)
  {
  }

  inline double WalkRelation::fastTanh(const double x) noexcept
  {
    //table of tanh
    static const double tantable[101] = {0.0000000000e+00, 4.9958374958e-02, 9.9667994625e-02, 1.4888503362e-01, 1.9737532022e-01, 2.4491866240e-01, 2.9131261245e-01, 3.3637554434e-01, 3.7994896226e-01, 4.2189900525e-01, 4.6211715726e-01, 5.0052021119e-01, 5.3704956700e-01, 5.7166996609e-01, 6.0436777712e-01, 6.3514895239e-01, 6.6403677027e-01, 6.9106946983e-01, 7.1629787020e-01, 7.3978305127e-01, 7.6159415596e-01, 7.8180635761e-01, 8.0049902176e-01, 8.1775407797e-01, 8.3365460701e-01, 8.4828363996e-01, 8.6172315931e-01, 8.7405328789e-01, 8.8535164820e-01, 8.9569287384e-01, 9.0514825364e-01, 9.1378549012e-01, 9.2166855441e-01, 9.2885762145e-01, 9.3540907060e-01, 9.4137553850e-01, 9.4680601285e-01, 9.5174595716e-01, 9.5623745813e-01, 9.6031938853e-01, 9.6402758008e-01, 9.6739500126e-01, 9.7045193661e-01, 9.7322616434e-01, 9.7574313003e-01, 9.7802611474e-01, 9.8009639627e-01, 9.8197340269e-01, 9.8367485769e-01, 9.8521691731e-01, 9.8661429815e-01, 9.8788039702e-01, 9.8902740220e-01, 9.9006639670e-01, 9.9100745368e-01, 9.9185972457e-01, 9.9263152020e-01, 9.9333038539e-01, 9.9396316735e-01, 9.9453607847e-01, 9.9505475369e-01, 9.9552430296e-01, 9.9594935922e-01, 9.9633412212e-01, 9.9668239784e-01, 9.9699763549e-01, 9.9728296010e-01, 9.9754120276e-01, 9.9777492793e-01, 9.9798645836e-01, 9.9817789761e-01, 9.9835115063e-01, 9.9850794233e-01, 9.9864983454e-01, 9.9877824128e-01, 9.9889444273e-01, 9.9899959779e-01, 9.9909475555e-01, 9.9918086567e-01, 9.9925878772e-01, 9.9932929974e-01, 9.9939310594e-01, 9.9945084369e-01, 9.9950308984e-01, 9.9955036646e-01, 9.9959314604e-01, 9.9963185619e-01, 9.9966688387e-01, 9.9969857928e-01, 9.9972725935e-01, 9.9975321085e-01, 9.9977669332e-01, 9.9979794161e-01, 9.9981716825e-01, 9.9983456555e-01, 9.9985030754e-01, 9.9986455170e-01, 9.9987744052e-01, 9.9988910295e-01, 9.9989965567e-01, 1.0 };

    const double overdx = 100.0/5.0;
    const double xoverdx = x*overdx;
    const unsigned i = unsigned(xoverdx);
    if (i > 99)
      return 1.0;
    const double fraction = xoverdx-i;
    return tantable[i] + fraction*(tantable[i+1]-tantable[i]);
  }

  inline double vdt_tanh(const double x) noexcept
  {
    const double _e = 2.0 * x;
    const double exptwox = ( _e >  100 ? 1e43 :
                             _e < -100 ? 0.0  :
                             vdt::fast_exp(_e) );
    return (exptwox-1.0)/(exptwox+1.0);
  }
}
