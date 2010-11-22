#include "MinuitInterface.cxx"

#ifdef __MAKECINT__
#pragma link C++ class MinuitInterface+;
#endif

class PrsMinimizer: public MinuitInterface {
private:
  virtual double fcn( const double* );
  virtual void fillMassPlot( TH1F*, const double*, int );
public:
  ClassDef( PrsMinimizer , 1 );
};
