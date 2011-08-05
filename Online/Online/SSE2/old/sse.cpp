#include <xmmintrin.h>
#include <iostream>
#include <cmath>

static const __m128d sse2d_null = _mm_setzero_pd();
static const __m128d sse2d_negative = _mm_set1_pd(-1.);

//template <class T = double> 
class CartesianD2D { 
public:
  typedef double Scalar;
  union SSEData {
    __m128d sse;
    double d[2];
    SSEData() {}
    SSEData(const __m128d& v) { sse = v; }
  } f;
public:
  CartesianD2D()  { f.sse=sse2d_null; }
  CartesianD2D(Scalar x, Scalar y) { f.sse = _mm_set_pd(x,y); } 
  //template <class CoordSystem> 
  //explicit Cartesian2D(const CoordSystem & v) { f.sse=_mm_set_pd(v.X(),v.Y()); }
  CartesianD2D(const CartesianD2D& v) { f.sse=v.f.sse; } 
  void SetCoordinates(Scalar x, Scalar y) {
    f.sse=_mm_set_pd(x,y); 
  }
  Scalar X()     const { return f.d[1];}
  Scalar Y()     const { return f.d[0];}
  Scalar Mag2()  const { 
    Scalar r;
    __m128d a = _mm_mul_pd(f.sse,f.sse); 
    __m128d b = _mm_shuffle_pd(a,a,_MM_SHUFFLE2(1,0));
    _mm_storel_pd(&r,_mm_add_pd(a,b));
    return r;
  }
  Scalar _Mag2()  const { return f.d[1]*f.d[1] + f.d[0]*f.d[0];   }
  Scalar _R()     const { return std::sqrt( _Mag2());}
  Scalar _Phi()   const { return (f.d[1]==0 && f.d[0]==0) ? 0.0 : atan2(f.d[0],f.d[1]);}
  Scalar R()      const {
    Scalar r;
    __m128d a, b;
    a = _mm_mul_pd(f.sse,f.sse);
    b = _mm_shuffle_pd(a,a,_MM_SHUFFLE2(1,0));
    _mm_storel_pd(&r,_mm_sqrt_pd(_mm_add_pd(a,b)));
    return r;
  }
  Scalar Phi()    const { 
    return (f.d[0]==0. && f.d[1]==0.) ? 0. : atan2(f.d[0],f.d[1]);
  }
  void Scale(Scalar a)  {     f.sse = _mm_mul_pd(f.sse,_mm_set1_pd(a));  }
  void Negate()         {     f.sse = _mm_mul_pd(f.sse,sse2d_negative);   }

  void _Scale(Scalar a) {    f.d[0] *= a; f.d[1] *= a; }
  void _Negate()        {     f.d[0] *= -1.; f.d[1] *= -1.; }

  void Rotate(Scalar angle) { 
    Scalar s = std::sin(angle);
    Scalar c = std::cos(angle);
    SetCoordinates( c*f.d[1] - s*f.d[0], s*f.d[1] + c * f.d[0] );
  }
  /*
  template <class CoordSystem> 
  Cartesian2D & operator = (const CoordSystem & v) { 
    f.sse=_mm_set_pd(v.X(),v.Y());
    return *this;
  }
  */
  bool operator == (const CartesianD2D & rhs) const {
    SSEData r(_mm_cmpeq_pd(f.sse,rhs.f.sse));
    return r.d[0]==0 && r.d[1]==0;
  }
};

std::ostream& operator<<(std::ostream& os, CartesianD2D& c) {
  CartesianD2D::Scalar r[2];
  _mm_storer_pd(r,c.f.sse);
  os << "SSE: " << r[0] << " " << r[1] << " R:" << c.R() << " Phi:" << c.Phi() << std::endl
     << "DBLE:" << c.f.d[1] << " " << c.f.d[0] << " R:" << c._R() << " Phi:" << c._Phi() << std::endl;
  c.Negate();
  os << "Negate:" << c.f.d[1] << " " << c.f.d[0] << " R:" << c.R() << " Phi:" << c.Phi() << std::endl;
  return os;
}

#include <time.h>
#include <sys/times.h>
#include <unistd.h>

static const double ttclk = sysconf(_SC_CLK_TCK);
inline double uclock_sec() {
  struct tms t;
  return ((double)times(&t))/ttclk;
}

bool prt = false;

void work(CartesianD2D& p) {
  if ( prt ) std::cout << p;
}
void work(CartesianD2D::Scalar p) {
  if ( prt ) std::cout << p;
}

int main(int argc, char**) {
  CartesianD2D c(0.,0.);
  CartesianD2D d(1.,2.);
  CartesianD2D::Scalar r;
  std::cout << c;
  std::cout << d;
  //_set_SSE2_enable(1);

  prt = argc>3;
  double cnt=0., delta;
  clock_t t0, t1;
  t0=clock();
  for(cnt=0; cnt<1e8; cnt += 1.)  {
    d.Negate();
    d.Scale(1.000001);
    r = d.R();
    d.Scale(r);
    d.Scale(1./r);
    work(d.Mag2());
    work(d);
  }
  t1=clock();
  delta = ((double)(t1-t0))/(double)CLOCKS_PER_SEC;
  printf("\nSSEDirect: %6.1f millions evaluations per second in %f seconds  %e\n",cnt/1e6/(delta),delta,(delta)/cnt);
  std::cout << d;

  t0=clock();
  for(cnt=0.; cnt<1e8; cnt += 1.)  {
    d.Negate();
    d.Scale(1./1.000001);
    r = d.R();
    d.Scale(r);
    d.Scale(1./r);
    work(d);
    work(d.Mag2());
  }
  t1=clock();
  delta = ((double)(t1-t0))/(double)CLOCKS_PER_SEC;
  printf("\nSSEDirect: %6.1f millions evaluations per second in %f seconds  %e\n",cnt/1e6/(delta),delta,(delta)/cnt);
  std::cout << d;

  t0=clock();
  for(cnt=0.; cnt<1e8; cnt += 1.)  {
    d._Negate();
    d._Scale(1.000001);
    r = d._R();
    d.Scale(r);
    d.Scale(1./r);
    work(d);
    work(d._Mag2());
  }
  t1=clock();
  delta = ((double)(t1-t0))/(double)CLOCKS_PER_SEC;
  printf("\nConventional: %6.1f millions evaluations per second in %f seconds  %e\n",cnt/1e6/(delta),delta,(delta)/cnt);
  std::cout << d;
  t0=clock();
  for(cnt=0.; cnt<1e8; cnt += 1.)  {
    d._Negate();
    d._Scale(1./1.000001);
    r = d._R();
    d.Scale(r);
    d.Scale(1./r);
    work(d);
    work(d._Mag2());
  }
  t1=clock();
  delta = ((double)(t1-t0))/(double)CLOCKS_PER_SEC;
  printf("\nConventional: %6.1f millions evaluations per second in %f seconds  %e\n",cnt/1e6/(delta),delta,(delta)/cnt);
  std::cout << d;
  return 1;
}
