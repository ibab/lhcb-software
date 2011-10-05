/*
 * MonHist.h
 *
 *  Created on: Mar 26, 2010
 *      Author: beat
 */

#ifndef MONHIST_H_
#define MONHIST_H_

#include "Gaucho/MonObj.h"
#include "MonTypes.h"
#include <string>
#include "GaudiKernel/IMessageSvc.h"
#include "TObject.h"

class TObject;
class TAxis;
class CntrMgr;
namespace AIDA { class IBaseHistogram; }
class StatEntity;
class MonHist : public MonObj
{
private:
  static std::string optsep;
  MONTYPE m_type;
  TObject *m_rootobj;
  TObject *m_rootdeser;
  CntrMgr *m_cntrmgr;
  int m_xmitbuffersize;
  int m_hdrlen;
  int m_buffersize;
  std::string m_name;
  std::string m_bookopts;
  int m_namelen;
  std::string m_title;      /** Pointer to Histogram Title **/
  int m_titlen;
  int m_nx;           /** Number of x-bins **/
  double m_xmin;     /** Minimum x **/
  double m_xmax;     /** Maximum y **/
  int m_ny;           /* Number of y bins */
  double m_ymin;     /* Minimum y */
  double m_ymax;     /* Maximum y */
  int m_nz;           /* Number of y bins */
  double m_zmin;     /* Minimum y */
  double m_zmax;     /* Maximum y */
  int m_addoff;

  double *m_hentries;
  double *m_hsumw2;
  double *m_hsumw;
  double *m_hbinsumw2;
  int m_blocksize;
  TAxis *m_Xaxis;
  TAxis *m_Yaxis;
  TAxis *m_Zaxis;
//  char **m_Xlabels;
//  char **m_Ylabels;
  std::vector<std::string> m_Xlabels;
  std::vector<std::string> m_Ylabels;
  std::vector<std::string> m_Zlabels;
  int m_xlablen;
  int m_ylablen;
  int m_zlablen;
  IMessageSvc* m_msgsvc;
  void _clear();
  void setup(IMessageSvc* msgSvc, const std::string& source, const AIDA::IBaseHistogram *aidahist);
  void setup(IMessageSvc* msgSvc);
  void setup(IMessageSvc* msgSvc, const std::string& source,const std::string& desc, const StatEntity *se);
  void cpyBinLabels(char *,std::vector<std::string>&, int);
public:
  MonHist();
  MonHist(IMessageSvc* msgSvc, const std::string& source, const AIDA::IBaseHistogram *aidahist);
  MonHist(IMessageSvc* msgSvc, const std::string& source, const std::string& desc,const StatEntity *se);
  MonHist(IMessageSvc* msgSvc, const std::string& source, CntrMgr *);
  virtual ~MonHist();
  int datasize()  const {return m_buffersize;};
  int xmitbuffersize();
  int hdrlen() const {return m_hdrlen;};
  int serialize(void* &ptr);
  const char *name() const {return m_name.c_str();};
  void clear(void);
  int namelength() const {return m_namelen+1;};
  int titlen() const {return m_titlen+1;};
  int type() const {return m_type;};
  void *cpyName(void*) const;
  void *cpytitle(void *ptr) const;
  int GetBinLabels(TAxis *ax, std::vector<std::string>&);
  static void SetBinLabels(TAxis *ax, char *l);
  void makeCounters();
  void List();
  static void *de_serialize(void *ptr, char *nam =0);
  void resetup(void);
  void FreeDeser(){if (m_rootdeser != 0){delete m_rootdeser; m_rootdeser=0;};};
  int BlockSize(void){return m_blocksize;};

};
#endif /* MONHIST_H_ */
