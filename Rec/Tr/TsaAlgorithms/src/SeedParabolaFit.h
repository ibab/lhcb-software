// $Id: SeedParabolaFit.h,v 1.1 2006-12-06 14:35:00 mneedham Exp $
#ifndef SEEDPARABOLAFIT_H
#define SEEDPARABOLAFIT_H 1

class SeedTrack;

class SeedParabolaFit{

 public:

  SeedParabolaFit(double z0, unsigned int nIter = 5, double outlierCut = 3.1);

  ~SeedParabolaFit(){}

  int fit( SeedTrack* seed, const double &csth) const;

  int refit( SeedTrack* seed, const double &csth) const; 

 private:

  void reset() const;

  double x0() const;

  double s(const double x0) const;

  double t(const double x0, const double s) const;

  mutable double m_a;
  mutable double m_b;
  mutable double m_c;
  mutable double m_d;
  mutable double m_e;
  mutable double m_f;
  mutable double m_g;
  mutable double m_h;
  double m_z0;
  unsigned int m_maxIter;
  double m_outlierCut;
  double m_otResolution;
  double m_otResolution2;
  double m_itResolution;

};

inline SeedParabolaFit::SeedParabolaFit(double z0, unsigned int nIter, double outlierCut):
m_z0(z0),
m_maxIter(nIter),
m_outlierCut(outlierCut){
 
  m_otResolution = 0.2;
  m_otResolution2 = m_otResolution * m_otResolution ;
  m_itResolution = 0.05;

  reset();
}

inline void SeedParabolaFit::reset() const {

  m_a = 0.;
  m_b = 0.;
  m_c = 0.;
  m_d = 0.;
  m_e = 0.;
  m_f = 0.;
  m_g = 0.;
  m_h = 0.;

}

inline double SeedParabolaFit::x0() const{
  return ((m_f*m_f-m_h*m_d)*(m_f*m_a-m_d*m_e)-(m_d*m_d-m_f*m_c)*(m_h*m_e-m_f*m_g)) /
         ((m_d*m_d-m_f*m_c)*(m_f*m_d-m_h*m_c)-(m_f*m_f-m_h*m_d)*(m_d*m_c-m_f*m_b));
}

inline double SeedParabolaFit::s(const double x0) const {
 return ((m_f*m_a-m_d*m_e) + (m_d*m_c-m_f*m_b)*x0)/(m_f*m_c-m_d*m_d);
}

inline double SeedParabolaFit::t(const double x0, const double s) const{
  return  ( m_a - m_b*x0 - m_c*s ) / m_d;
}


#endif
