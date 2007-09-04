#ifndef MILLECONFIG_H 
#define MILLECONFIG_H 1

// Include files

#include "AlignmentInterfaces/IMillepede.h"
#include "VeloTrack.h"

/** @class MilleConfig MilleConfig.h VeloAlignment/MilleConfig.h
 *  
 *
 *  @author Sebastien Viret
 *  @date   2005-07-28
 */

class MilleConfig {

public: 

  /// Standard constructor
  MilleConfig();
  virtual ~MilleConfig( ); ///< Destructor

  void InitMilleTool(IMillepede *my_millepede, int right,
		     std::vector<bool> DOF, std::vector<double> Sigm,
		     std::vector<bool> Cons, int nloc, double starfact, 
		     int nstd, std::vector<double> res_cut, double VELOmap[],
		     int n_objects); 

  void InitBox(IMillepede *my_millepede,
	       std::vector<bool> DOF, std::vector<double> Sigm,
	       std::vector<bool> Cons, double starfact, 
	       std::vector<double> res_cut, double zmoy,
	       int n_objects); 

  StatusCode PutTrack(VeloTrack& atrack, IMillepede *my_millepede);

  StatusCode PutOverlapTrack(VeloTrack& atrack, IMillepede *my_millepede);

  StatusCode PutPVTrack(VeloTracks& aPV, IMillepede* my_millepede, int nPV,
			double zmoyl, double zmoyr, double VELO_open, int n_min);

  StatusCode correcTrack(VeloTrack& mistrack, 
			 VeloTrack& correctrack, 
			 std::vector<double> left_constants, 
			 std::vector<double> right_constants,
			 double VELOmap[]);

// Is it internal or box alignment ?

  inline bool   isInternal() {return m_i_align;}

// Do we align left or right box ?

  inline int    isLeft() {return m_right;}

// What are the parameter to align ?

  inline bool   GetDOF(int nDOF) {return m_DOF[nDOF];}

// What are the constraints to define ?

  inline bool   GetCons(int nCONS) {return  m_Cons[nCONS];}
  inline double GetDOFCons(int nDOF) {return  m_Sigm[nDOF];}

// Number of global params/DOF

  inline int    GetNstations() {return m_nglo;}

// The first station to align 

  inline int    GetFirst() {return m_nfirst;}

// The last station to align

  inline int    GetLast() {return m_nlast;}

// Number of local params

  inline int    GetNlocal() {return m_nloc;}

// Parameters for constraint equation (internal only)

  inline double Get_zmoy() {return m_zmoy;} // 
  inline double Get_szmoy() {return m_szmoy;} //   

  inline int    Get_CorrectStation(int Nvalue) {return CorrectVELOmap[Nvalue];}

// Value of the alignment constant

  inline double GetAliConstant(int iParam) {return m_aliConstants[iParam];} //   

private:

  std::vector<double>   ftx;
  std::vector<double>   fty;
  std::vector<double>   ftz;
  std::vector<double>   frotx; 
  std::vector<double>   froty; 
  std::vector<double>   frotz; 
  std::vector<double>   fscaz;
  std::vector<double>   shearx; 
  std::vector<double>   sheary;

  bool m_i_align;
  bool m_right;
  bool m_DOF[6];
  bool m_Cons[9];

  double m_Sigm[6];
  double m_zmoy;
  double m_szmoy;
  double m_startfact;

  int    CorrectVELOmap[21];

  std::vector<double>   m_aliConstants;

  int    m_nglo;
  int    m_nloc;
  int    m_nlast;
  int    m_nfirst;
  int    m_nstd;
};
#endif // VELOALIGNMENT_MILLECONFIG_H
