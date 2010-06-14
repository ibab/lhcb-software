// $Id: Bs2DsHMvaUtilityTool.h,v 1.1 2010/06/10 12:42:22 seaso Exp $
#ifndef BS2DSHMVAUTILITYTOOL_H 
#define BS2DSHMVAUTILITYTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "IBs2DsHMvaUtilityTool.h"            // Interface


/** @class Bs2DsHMvaUtilityTool Bs2DsHMvaUtilityTool.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2010-02-11
 */
class Bs2DsHMvaUtilityTool : public GaudiTool, virtual public IBs2DsHMvaUtilityTool {
public: 
  /// Standard constructor
  Bs2DsHMvaUtilityTool( const std::string& type, 
                        const std::string& name,
                        const IInterface* parent);

  virtual ~Bs2DsHMvaUtilityTool( ); ///< Destructor
  virtual StatusCode initialize();
  virtual void initializeDataSelectionPathStrings();
  virtual void initializeStdParamForMvaSel();
  virtual std::string getcurrentBsLocation(int aChannelNumA)
   {return  m_currentBsLocation[aChannelNumA]; }
  virtual std::string 
     getparticle2RefittedSortedPVRelationsLoc(int aChannelNumB) 
   {return m_particle2RefittedSortedPVRelationsLoc[aChannelNumB];}
  virtual std::string 
     getparticle2RefittedPVRelationsLoc(int aChannelNumC) 
   {return m_particle2RefittedPVRelationsLoc[aChannelNumC];}
  
  virtual std::string getcurrentStdPVLocation()
  {  return  m_currentStdPVLocation;}


  virtual int pdgDsValue(){return m_pdgDsValue;}
  virtual double massStdDsValue()  
  {  return m_massStdDsValue;}
  virtual int pdgKaonValue() 
  {  return  m_pdgKaonValue;}
  virtual double massStdBsValue()
  {  return m_massStdBsValue;}
  virtual int pdgPionValue() 
  {  return m_pdgPionValue;}

  virtual bool getL0Decision ();
  virtual int FindBestCandidateInEv(std::vector<int> aCandIndex,
               std::vector<double> aIps_Bs, std::vector<double>aprob_Bs, 
              int SelOption );
  
  virtual std::string fisherWeightsFullFileName(int channel );
  
  
protected:

private:

 int m_num_Bs_Phys_Channels_Used;
  
 std::string  m_basicPhysStringPath;
 std::string  m_particleLocation;
 std::string m_P2RefitPVSortedRel;
 std::string m_P2RefitPVRel;
  
 std::string   m_bs2DsK_Recon_Dir;
 std::string   m_bs2DsPi_Recon_Dir;
 std::string  m_bs2DsK_PvRefitAlgName;
 std::string  m_bs2DsPi_PvRefitAlgName;
  
 
 std::vector<std::string>  m_bsParticleDir;  
 std::vector<std::string>  m_particle2RefittedSortedPVRelationsLoc;
 std::vector<std::string>  m_particle2RefittedPVRelationsLoc;
  

 std::vector<std::string>  m_currentBsLocation;

 std::string  m_currentStdPVLocation;

  std::string m_pathtoWeightsFile_Bs2DsK;
  std::string m_pathtoWeightsFile_Bs2DsPi;
  
  std::string m_fisherWeightsLocalFileName_Bs2DsK;
  std::string m_fisherWeightsLocalFileName_Bs2DsPi;
  
  std::vector<std::string> m_fisherWeightsFullFileName;

  

  int m_pdgDsValue;
  double m_massStdDsValue;
  int m_pdgKaonValue;
  double m_massStdBsValue;
  int m_pdgPionValue;
};
#endif // BS2DSHMVAUTILITYTOOL_H
