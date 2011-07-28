#ifndef MINT_SYM_POLY_TERM_HH
#define MINT_SYM_POLY_TERM_HH

#include <vector>
#include <iostream>

class symPolyTerm{
  std::vector<int> _powers;

  std::vector< std::vector<int> > _allPermutations;

  std::string _name;

  bool init();
  void makeName();
  void makeAllPermutations();

 public:
  symPolyTerm(const std::vector<int>& pos);
  symPolyTerm(int a);
  symPolyTerm(int a, int b);
  symPolyTerm(int a, int b, int c);
  symPolyTerm(int a, int b, int c, int d);
  symPolyTerm(int a, int b, int c, int d, int e);
  symPolyTerm(int a, int b, int c, int d, int e, int f);

  symPolyTerm(const symPolyTerm& other);
  
  ~symPolyTerm(){}

  void print(std::ostream& os = std::cout) const;

  double eval(double x, double y)const;
  double eval(double x, double y, double z)const;
  double eval(double x, double y, double z
	      , double u)const;
  double eval(double x, double y, double z
	      , double u, double v)const;
  double eval(double x, double y, double z
	      , double u, double v, double w)const;
  double eval(const std::vector<double>& x)const;
  double eval(const std::vector<double* >& x)const;
  double eval(const std::vector<const double* >& x)const;

  const std::string& name() const{return _name;}
};

std::ostream& operator<<(std::ostream& os, const symPolyTerm& spt);

#endif
//
