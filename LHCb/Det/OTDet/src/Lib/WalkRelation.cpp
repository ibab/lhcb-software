
#include "OTDet/WalkRelation.h"
#include "vdt/exp.h"

#include "GaudiKernel/Kernel.h" // for (UN)LIKELY

#include "LHCbMath/ChebyshevApprox.h"

namespace OTDet
{

  WalkRelation::WalkRelation() :
    m_walk_fast(&WalkRelation::walk_fast_zero),
    m_upperend(0.f),
    m_approx(0., 1., [] (double) { return 0.;} ), // dummy walk relation
    m_off(0.0), m_amp(0.0), m_tau(0.0), m_dpt(0.0), m_dptfactor(0.0), m_overtau(0.0)
  {
  }

  WalkRelation::WalkRelation(double off, double amp, double tau, double dpt) :
    m_upperend(9.* std::abs(tau)),
    m_approx(0., 9. * std::abs(tau), [off, amp, tau, dpt] (double l) {
	return off + amp * (std::tanh(((0. != tau) ?
	      (1. / std::abs(tau)) : 0.) * l) - 1.0) +
       	((0. != tau) ? (1e-3 * dpt) : 0.) * l; }),
    m_off(off), m_amp(amp), m_tau(tau), m_dpt(dpt),
    m_dptfactor((0. != m_tau) ? (1e-3 * m_dpt) : 0.),
    m_overtau((0. != m_tau) ? (1. / std::abs(m_tau)) : 0.)
  {
    m_walk_fast = (0. == m_tau ||
	m_approx.error_estimate() > 0.5f * 25.f / 64.f) ?
      &WalkRelation::walk_fast_exact : &WalkRelation::walk_fast_chebyshev;
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

  inline double WalkRelation::padeTanh(const double x) noexcept
  {
    typedef double T;
    // for very large |x| > 20, tanh(x) is x/|x| anyway (at least to double
    // precision)
    const T x2 = x * x;
    if (x2 > T(400)) return (x > T(0)) ? T(1) : T(-1);
    // strategy for large arguments: tanh(2x) = 2 tanh(x)/(1 + tanh^2(x))
    // idea is to use this "argument halving" a couple of times, and use a
    // very short Padé approximation for the rest of the way
    constexpr unsigned N = 3;
    const T xx = x / T(1u << N);
    const T xx2 = xx * xx;
    const T numer = T(135135) + xx2 * (T(17325) + xx2 * (T( 378) + xx2 * T( 1)));
    const T denom = T(135135) + xx2 * (T(62370) + xx2 * (T(3150) + xx2 * T(28)));

    T f = xx * numer / denom;
    for (unsigned i = N; i--; f = T(2) * f / (T(1) + f * f));

    return f;
  }

  inline float WalkRelation::padeTanh(const float x) noexcept
  {
    if (std::abs(x) > 9.1f) return (x > 0.f) ? 1.f : -1.f;
    const auto xx = x * 0.125f;
    const auto xx2 = xx * xx;
    auto tanh = xx * (xx2 + 15.f) / (6.f * xx2 + 15.f);
    tanh = 2.f * tanh / (1.f + tanh * tanh);
    tanh = 2.f * tanh / (1.f + tanh * tanh);
    return 2.f * tanh / (1.f + tanh * tanh);
  }

  inline double vdt_tanh(const double x) noexcept
  {
    const double _e = 2.0 * x;
    const double exptwox = ( _e >  100 ? 1e43 :
                             _e < -100 ? 0.0  :
                             vdt::fast_exp(_e) );
    return (exptwox-1.0)/(exptwox+1.0);
  }

  double WalkRelation::walk(double l) const noexcept
  {
    l *= (l > 0);//only positive values allowed
    const double tanh = padeTanh(l * m_overtau);
    return (m_amp * (tanh - 1.0)) + (l * m_dptfactor + m_off);
  }

  float WalkRelation::walk_fast_exact(float l) const noexcept
  {
    l *= (l > 0);//only positive values allowed
    const float tanh = padeTanh(l * m_overtau);
    return (m_amp * (tanh - 1.0f) + (l * m_dptfactor + m_off));
  }

  float WalkRelation::walk_fast_chebyshev(float l) const noexcept
  {
    // if l > 9 * m_tau, then tanh(x) is 1 to float precision anyway, so we can
    // get by with a linear approximation; typical values for l and m_tau will
    // not produce such a large result, and we make do with a Chebyshev
    // approximation of the full monty...
    if (UNLIKELY(l > m_upperend)) return m_dptfactor * l + m_off;
    else return m_approx((l *= (l > 0)));
  }

  float WalkRelation::walk_fast_zero(float) const noexcept
  { return 0.f; }
}
