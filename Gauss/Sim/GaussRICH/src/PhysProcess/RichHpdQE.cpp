#include "RichHpdQE.h"
#include "G4ios.hh"

// initialize the QE vectors with 20 bins are adjust the
//size after getting the correct size from the db.
RichHpdQE::RichHpdQE() : 
  m_curHpdNum(0), 
  m_curRichDetNum(0),
  m_numHpdQEbins(20),
  m_HpdQEff(std::vector<double>(20)),
  m_PhotEnergyHpdQE(std::vector<double>(20)),
  m_WaveEnerCnv(1243.125) { }

RichHpdQE::RichHpdQE(const int hpdnum, const int richdetnum) :
  m_curHpdNum(hpdnum), 
  m_curRichDetNum(richdetnum),
  m_numHpdQEbins(20), 
  m_HpdQEff(std::vector<double>(20)),
  m_PhotEnergyHpdQE(std::vector<double>(20)), 
  m_WaveEnerCnv(1243.125) { }

RichHpdQE::~RichHpdQE() { ; }

double RichHpdQE::getHpdPhotonWaveLengthFromBin(int curmbin)
{
  double photwa(0);
  const double photener = getHpdPhotEnergyFromBin(curmbin);

  if ( photener > 0.0 ) {
    photwa = m_WaveEnerCnv/photener ;
  }else {
    G4cout << " RichHpdQE: Inadmissible photonenergy=  " << photener
           << "  Bin num =  " << curmbin << G4endl;
    photwa=0.0 ;
  }
  return photwa;
}

std::vector<double>RichHpdQE::PhotWavelengthHpdQE()
{
  std::vector<double> photwVect(m_numHpdQEbins);
  for (int ii=0 ; ii<m_numHpdQEbins; ++ii ){
    photwVect[ii] = getHpdPhotonWaveLengthFromBin(ii);
  }
  return photwVect;
}

double RichHpdQE::getHpdPhotonEnergyFromWavelen(double photonwlen )
{
  double photenergy;
  if(photonwlen > 0.0 ) {
    photenergy=m_WaveEnerCnv/photonwlen;
  }
  else
  {
    G4cout << " RichHpdQE: Inadmissible photonwavelength=  " << photonwlen
           << G4endl;
    photenergy=0.0;
  }
  return photenergy;
}

double RichHpdQE::getHpdPhotonWavelenFromEnergy(double  photonenerg ){
  double photonwlength;
  if( photonenerg > 0.0 ) {
    photonwlength=m_WaveEnerCnv/photonenerg;
  }else {

    G4cout <<" RichHpdQE: Inadmissible photonenergy=  "<<photonenerg<<G4endl;
    photonwlength=0.0;

  }
  return  photonwlength;
}

double  RichHpdQE::getHpdQEffFromPhotWavelen(double photwavelen) 
{
  const double photonener = getHpdPhotonEnergyFromWavelen( photwavelen);
  return getHpdQEffFromPhotEnergy(photonener);
}

double RichHpdQE::getHpdQEffFromPhotEnergy(double  photonenergy ){
  //if the energy is between 2 bins, interpolate.
  // assumes that energies are stored in increasing order in the class.
  // this means in the db they should be in the increasing order.

  double hq1,hq2, e1, e2,aslope,aintc;
  double qeff=0.0;

  for (int ibinq=0 ; ibinq<m_numHpdQEbins-1 ; ibinq++ ){
    e1 = m_PhotEnergyHpdQE[ibinq];
    e2 = m_PhotEnergyHpdQE[ibinq+1];
    if( photonenergy >= e1 &&  photonenergy <= e2 ) {
      hq1 =  m_HpdQEff[ibinq];
      hq2 =  m_HpdQEff[ibinq+1];
      if(e1 != e2 ){
        aslope = (hq2-hq1)/(e2-e1);
      }else{
        G4cout <<"RichHpdQE: Two Identical QE energy bins for same hpd, check db  "
               <<e1<<G4endl;
        aslope=0.0;
      }
      aintc =  hq1 - (aslope * e1 );
      qeff= aintc + aslope * photonenergy  ;
    }
  }
  return qeff;
}

void RichHpdQE::setCurrentHpdQEwl(int ihpdnum, 
                                  int irichdetnum,
                                  int inumQEbins,
                                  const std::vector<double> & qeffvect,
                                  const std::vector<double> & qeffwavelen ) 
{
  m_curHpdNum = ihpdnum;
  m_curRichDetNum = irichdetnum;
  setAnHpdQEwl(inumQEbins,qeffvect,qeffwavelen);
}

void RichHpdQE::setAnHpdQEwl(int inumQEbins,
                             const std::vector<double> & qeffvect,
                             const std::vector<double> & qeffwavelen) {

  m_numHpdQEbins= inumQEbins;
  if ( m_numHpdQEbins != (int) m_HpdQEff.size() ) m_HpdQEff.resize(m_numHpdQEbins);
  if ( m_numHpdQEbins != (int) m_PhotEnergyHpdQE.size() ) m_PhotEnergyHpdQE.resize(m_numHpdQEbins);
  m_HpdQEff= qeffvect;
  for (int ij=0; ij<m_numHpdQEbins; ++ij) {
    m_PhotEnergyHpdQE[ij]=getHpdPhotonEnergyFromWavelen(qeffwavelen[ij]);
  }

}

void RichHpdQE::setCurrentHpdQEen(int jhpdnum, int jrichdetnum,
                                  int jnumQEbins, 
                                  const std::vector<double> & jqeffvect,
                                  const std::vector<double> & qeffener) {
  //   RichHpdQElog << MSG::INFO
  //  <<"set hpd QEen: hpdnum detnum numbins "<<jhpdnum<<"  "<<jrichdetnum
  //    <<"   "<<jnumQEbins<<endreq;
  m_curHpdNum= jhpdnum;
  m_curRichDetNum=jrichdetnum;
  setAnHpdQEen(jnumQEbins,jqeffvect,qeffener);

}

void RichHpdQE::setAnHpdQEen(int jnumQEbins, 
                             const std::vector<double> & jqeffvect,
                             const std::vector<double> & qeffener) {

  //   RichHpdQElog << MSG::INFO
  //   <<"set hpd QEen: NumQEbins "<<jnumQEbins<<endreq;

  m_numHpdQEbins= jnumQEbins;
  if(m_numHpdQEbins != (int) m_HpdQEff.size())m_HpdQEff.resize(m_numHpdQEbins);
  if(m_numHpdQEbins != (int) m_PhotEnergyHpdQE.size())
    m_PhotEnergyHpdQE.resize(m_numHpdQEbins);

  m_HpdQEff = jqeffvect;
  m_PhotEnergyHpdQE =qeffener;

}






