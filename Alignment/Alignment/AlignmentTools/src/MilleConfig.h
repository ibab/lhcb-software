#ifndef ALIGNMENTTOOLS_MILLECONFIG_H 
#define ALIGNMENTTOOLS_MILLECONFIG_H 1

// Include files
#include "GaudiKernel/MsgStream.h"

/** @class MilleConfig MilleConfig.h AlignmentTools/MilleConfig.h
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

// Is it internal or box alignment ?

  inline void   SetInternal(bool i_align) {m_i_align = i_align;} //
  inline bool   isInternal() {return m_i_align;}

// Do we align left or right box ?

  inline void   SetLeft(int right) {m_right = right;} //
  inline int    isLeft() {return m_right;}

// What are the DOFs to align ?

  inline void   DefineAlign(bool DOF[]){for (int j=0; j<6; j++) {m_DOF[j] = DOF[j];}} //
  inline bool   GetDOF(int nDOF) {return m_DOF[nDOF];}


// What are the constraints to define ?

  inline void   DefineConstraint(double Sigm[],bool Const[])
  {
    for (int j=0; j<9; j++) {m_Cons[j] = Const[j];}
    for (int j=0; j<6; j++) {m_Sigm[j] = Sigm[j];}
  } 

  inline bool   GetCons(int nCONS) {return  m_Cons[nCONS];}
  inline double GetDOFCons(int nDOF) {return  m_Sigm[nDOF];}

// What are the stations to consider ?

  void          DefineMap(double VELOmap[]); 
  inline bool   GetVELOmap(int station) {return  m_VELO_map[station];}


// Number of global params/DOF

  inline void   SetNgb(int nglo) {m_nglo = nglo;} //
  inline int    GetNgb() {return m_nglo;}

// The first station to align 

  inline void   SetFirst(int nfirst) {m_nfirst = nfirst;} //
  inline int    GetFirst() {return m_nfirst;}

// The last station to align

  inline void   SetLast(int nlast) {m_nlast = nlast;} //
  inline int    GetLast() {return m_nlast;}

// Number of local params

  inline void   SetNlc(int nloc) {m_nloc = nloc;} //
  inline int    GetNlc() {return m_nloc;}

// Start factor for Chi^2 local fit cut

  inline void   SetStartf(double startfact) {m_startfact = startfact;} //
  inline double GetStartf() {return m_startfact;}

// Numb. of standard deviations

  inline void   SetNstd(int nstd) {m_nstd = nstd;} //
  inline int    GetNstd() {return m_nstd;}

// Parameters for constraint equation (internal alignment only)

  inline void   Set_zmoy(double zmoy) {m_zmoy = zmoy;} //
  inline double Get_zmoy() {return m_zmoy;} // 

  inline void   Set_szmoy(double szmoy) {m_szmoy = szmoy;} //
  inline double Get_szmoy() {return m_szmoy;} //   


private:

  bool m_i_align;
  bool m_right;
  bool m_DOF[6];
  bool m_Cons[9];

  double m_Sigm[6];
  double m_VELO_map[21];
  double m_zmoy;
  double m_szmoy;
  double m_startfact;

  int    m_nglo;
  int    m_nloc;
  int    m_nlast;
  int    m_nfirst;
  int    m_nstd;
};
#endif // ALIGNMENTTOOLS_MILLECONFIG_H
