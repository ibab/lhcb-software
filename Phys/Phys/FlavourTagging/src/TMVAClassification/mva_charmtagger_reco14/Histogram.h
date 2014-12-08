#ifndef MVA_CHARMTAGGER_RECO14_HISTOGRAM_H 
#define MVA_CHARMTAGGER_RECO14_HISTOGRAM_H 1


class Histogram {
public:
  Histogram(int,double,double);
  ~Histogram();
  void SetBinContent(int,double);
  int FindBin(double);
  double GetBinContent(int);
  double GetPurityAtBDT(double);
  int NumBins();
  
private:
  int numBins;
  double min;
  double max;
  double* binLefts;
  double* vals;
};

#endif // MVA_CHARMTAGGER_RECO14_HISTOGRAM_H
