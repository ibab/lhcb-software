#ifndef RichPmtQE_h
#define RichPmtQE_h 1
// class with Pmt Quantum efficiencies.
// There would be one instance of this class for
// each pmt in each Pmt in each of the two rich detectors.
// SE 1-3-02.
// stores the photon energy in eV and Qe in absolute values
// in its member variables.
// the standard unit in Gaudi is MeV and hence it needs to be
// multiplied by 1000000 before using with variables of this class.
//
#include <vector>
#include <string>

class RichPmtQE {

public:

  RichPmtQE();
  RichPmtQE(int pmtnum, int richdetnum );
  virtual ~RichPmtQE();

  int curPmtNum() const {return  m_curPmtNum; }
  int curRichDetNum() const {return m_curRichDetNum; }
  int numPmtQEbins() const {return  m_numPmtQEbins; }

  const std::vector<double> & PmtQEff() const { return m_PmtQEff; }
  const std::vector<double> & PhotEnergyPmtQE() const { return m_PhotEnergyPmtQE ; }

  double getPmtQEffBin(const int curqbin) { return m_PmtQEff[curqbin]; }
  double getPmtPhotEnergyFromBin(const int curwbin) { return m_PhotEnergyPmtQE[curwbin];}

  std::vector<double> PhotWavelengthPmtQE();
  double getPmtPhotonWaveLengthFromBin(int curmbin);

  double getPmtPhotonEnergyFromWavelen(double photonwlen);

  double getPmtPhotonWavelenFromEnergy(double photonenergy);

  double getPmtQEffFromPhotEnergy(double photonenergy );
  double getMcpQEffFromPhotEnergy(double photonenergy ) 
  { return ( getPmtQEffFromPhotEnergy(photonenergy ) ) ;}
  

  double getPmtQEffFromPhotWavelen(double photwavelen );

  void setCurrentPmtQEwl(int ipmtnum, int irichdetnum, int inumQEbins,
                         const std::vector<double> & qeffvect, 
                         const std::vector<double> & qeffwavelen );

  void setCurrentPmtQEen(int jpmtnum, int jrichdetnum, int jnumQEbins,
                         const std::vector<double> & jqeffvect,
                         const std::vector<double> & qeffener );

  void setAnPmtQEwl(int inumQEbins,
                    const std::vector<double> & qeffvect, 
                    const std::vector<double> & qeffwavelen );

  void setAnPmtQEen(int jnumQEbins,
                    const std::vector<double> & jqeffvect, 
                    const std::vector<double> & qeffener );

  //additions by RWL 11.10.06
  //
  // void setCurrentPmtPCRIen(int jpmtnum, int jrichdetnum, int jnumPCRIbins,
  //                       const std::vector<double> & aPCRIvect, 
  //                       const std::vector<double> & aPCRIener );
  //
  //  void setCurrentPmtQWRIen(int jpmtnum, int jrichdetnum, int jnumQWRIbins,
  //                       const std::vector<double> & aQWRIvect,
  //                       const std::vector<double> & aQWRIener );
  //
  //  void setAnPmtPCRIen(int jnumPCRIbins,
  //                  const std::vector<double> & aPCRIvect, 
  //                  const std::vector<double> & aPCRIener );

  //  void setAnPmtQWRIen(int jnumQWRIbins,
  //                  const std::vector<double> & aQWRIvect, 
  //                  const std::vector<double> & aQWRIener );

  //  double getPmtQWRIFromPhotEnergy(double photonenergy );

  //  double getPmtPCRIFromPhotEnergy(double photonenergy );

  //  double getPmtCorrectedQEFromPhotEnergy(double photonenergy, double originalQE);


  //end of additions

  //  void getPmtQEDataFromDB();

private:

  int m_curPmtNum;
  int m_curRichDetNum;
  int m_numPmtQEbins;
  std::vector<double> m_PmtQEff;
  std::vector<double> m_PhotEnergyPmtQE;


  //  int m_numPmtQWRIbins;                    //added by RWL 11.10.06
  // std::vector<double> m_PmtQWRI;
  // std::vector<double> m_PhotEnergyPmtQWRI;
  //  int m_numPmtPCRIbins;
  //  std::vector<double> m_PmtPCRI;
  // std::vector<double> m_PhotEnergyPmtPCRI;  //end of additions



  double m_WaveEnerCnv;
  int PmtQEVerboseLevel;

};

#endif

