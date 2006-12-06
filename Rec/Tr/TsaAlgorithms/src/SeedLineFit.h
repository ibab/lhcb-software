// $Id: SeedLineFit.h,v 1.1 2006-12-06 14:35:00 mneedham Exp $
#ifndef SEEDLINEFIT_H
#define SEEDLINEFIT_H 1

class SeedTrack;

class SeedLineFit{

 public:

  SeedLineFit(double z0, double sth, unsigned int nIter = 10, double outlierCut = 3.5);

  ~SeedLineFit(){}

  int fit( SeedTrack* seed) const;

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

};

inline SeedLineFit::SeedLineFit(double z0, double sth,  unsigned int nIter, double outlierCut):
  m_z0(z0),
  m_sth(sth),
  m_scth(1.0/sth), 
  m_maxIter(nIter), 
  m_outlierCut(outlierCut){
  reset();
}

inline void SeedLineFit::reset() const {

  m_a = 0.;
  m_b = 0.;
  m_c = 0.;
  m_d = 0.;
  m_e = 0.;
}

#endif
