#ifndef MINTDALITZ_SCP_BINNING_HH
#define MINTDALITZ_SCP_BINNING_HH


#include "Mint/ScpBoxSet.h"
#include "Mint/ScpBox.h"



#include "Mint/IFastAmplitudeIntegrable.h"

#include "Mint/IDalitzEventList.h"
#include "Mint/IDalitzEvent.h"
#include "Mint/IGetRealEvent.h"
#include "Mint/IDalitzPdf.h"

#include "Mint/DalitzHistoStackSet.h"
#include "TH1D.h"
#include "TMath.h"

#include <vector>
#include <iostream>
#include <algorithm>

class ScpBinning {
protected:
  std::vector<ScpBoxSet> _boxSets;
  int _nData;
  int _nDataCC;
  double _totalMCWeight;
  double m_norm;
//
  static int* __colourPalette;
  static int __Ncol;
  static void makeColourPaletteBlueGrey();
  static void makeColourPaletteBlueWhite();
  static void makeColourPaletteRGB();
  static void makeColourPalette();
  static int* getColourPalette();

  void setHistoColours();

  ScpBoxSet splitBoxes(IDalitzEventList* events
			 , int maxPerBin
			 ) const;
  int mergeBoxes(ScpBoxSet& boxes, int minPerBin);

 public:
  ScpBinning();

  void resetEventCounts();
  virtual void fillData(IDalitzEventList* data);
  virtual void fillDataCC(IDalitzEventList* data);
  double normFactor() const;
  void setBoxesNormFactors();
  void sortByScp();
  int createBinning(IDalitzEventList* events
		    , int minPerBin = 10
		    , int maxPerBin = 100
		    );

  virtual double setEvents(IDalitzEventList* data
			 , IDalitzEventList* mc
			 );

  virtual double setBackgroundEvents(IDalitzEventList* data
			 , IDalitzEventList* mc
			 );


  void setFas(IFastAmplitudeIntegrable* fas);

  int numBins() const;

  double getScp_perBin() const;
  double getScpErr_perBin(double mean) const;
  double scp_ofBin(unsigned int i) const;
  double getMaxScp() const;
  double getMinScp() const;

  double Prob() const;
  double Prob(double chi2) const;

  double Chi2() const;

  int ndof() const;

  int Diff_ofBin(unsigned int i) const;

  void print(std::ostream& os = std::cout) const;

  void setNorm(double norm)
  {
	  m_norm = norm;
  }

  DalitzHistoStackSet getDataHistoStack();
  DalitzHistoStackSet getMCHistoStack();
  MINT::counted_ptr<TH1D> getScpDistribution() const;
  void drawScpDistribution(const std::string& fname = "scpDistribution.eps")const;

  void SubtractBackgroundData(IDalitzEventList* data);
  void SubtractBackgroundDataCC(IDalitzEventList* data);

  int NEntires();

  int SetBinning(const char*);

  void saveBinning(const char*);

  double getMinEntries() const;

  void saveAsNTuple(const char* tuplsFileName, IDalitzEventList* data);

  double Scp(const IDalitzEvent* Devt);

  friend class lessByScpBoxData;
  friend class lessByScpBoxScp;
  friend class lessByScpBoxSetScp;

};

class lessByScpBoxData{
 private:
	double m_norm;
 public:
	void SetNorm(double norm)
		{
			m_norm = norm;
		}
  bool operator()(const ScpBox& a, const ScpBox& b) const;
};

class lessByScpBoxScp{
	private:
	double m_norm;

 public:
	void SetNorm(double norm)
			{
				m_norm = norm;
			}
  bool operator()(const ScpBox& a, const ScpBox& b) const;
};

class lessByScpBoxSetScp{
 private:
	double m_norm;
 public:
	void SetNorm(double norm)
		{
			m_norm = norm;
		}
  bool operator()(const ScpBoxSet& a, const ScpBoxSet& b) const;
};

std::ostream& operator<<(std::ostream& os, const ScpBinning& c2b);

#endif
//
