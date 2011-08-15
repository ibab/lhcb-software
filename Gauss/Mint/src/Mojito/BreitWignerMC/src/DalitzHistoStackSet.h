#ifndef DALITZ_HISTO_STACK_SET_HH
#define DALITZ_HISTO_STACK_SET_HH

#include "DalitzSet.h"
#include "THStack.h"
#include "DalitzHistogram.h"
#include "DalitzHistoSet.h"
#include "TH1.h"
#include "TH2F.h"
#include "counted_ptr.h"

class TPaletteAxis;

class DalitzHistoStackSet : public DalitzSet<THStack>{

  std::vector< MINT::counted_ptr<TH1*> > _histograms;
  MINT::counted_ptr<THStack> makeHStack(const DalitzCoordSet& coord) const;

  int  _ncol;
  int* _palette;
  double _max;

  mutable MINT::counted_ptr<TH2F> _h2ForPalette;

  void makeH2ForPalette() const;
  MINT::counted_ptr<TH2F> getH2ForPalette() const;
  TPaletteAxis* getPaletteAxis() const;

 public:
  DalitzHistoStackSet();
  DalitzHistoStackSet(const DalitzHistoStackSet& other);

  virtual void add(const DalitzCoordSet& coord, const MINT::counted_ptr<TH1>& histo);
  virtual void add(const DalitzHistogram& histo);
  virtual void add(const DalitzHistoSet& hL);

  bool draw(const std::string& baseName=""
	    , const std::string& drawOpt="HIST"
	    , const std::string& format="eps") const;

  void setColourPalette(int nCol, int* pal, double max);

  virtual ~DalitzHistoStackSet();
};

#endif
//
//
