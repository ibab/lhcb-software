/*
 * AligOnlIterator.cpp
 *
 *  Created on: Oct 6, 2014
 *      Author: beat
 */

#include <stdio.h>
#include <stdlib.h>
#include "AligOnlIterator.h"


DECLARE_TOOL_FACTORY(AligOnlIterator)

static AligOnlIterator *IteratorInstance;

extern "C"
{
  int AligOnlIteratorThreadFunction(void *t)
  {
  	AligOnlIterator *f = (AligOnlIterator*)t;
    IteratorInstance = f;
    f->i_run();
    return 1;
  }
}

AligOnlIterator::AligOnlIterator(const std::string &  type, const std::string &  name, const IInterface *  parent  ):AlgTool(type,name,parent)
{
  declareProperty("PartitionName",   m_PartitionName= "LHCbA");
  declareProperty("ParamFileName",   m_ParamFileName= "/home/beat/aligparams.dat");
  declareProperty("CounterNames",m_CounterNames={"aaa/Chi2"});
  declareProperty("CounterDNS",m_CntDNS="mona08");
  declareProperty("CounterTask",m_CntTask="LHCbA_AligWrk_00");
  m_Minuit = 0;
  IInterface *p=(IInterface*)parent;
  StatusCode sc = p->queryInterface(IAlignDrv::interfaceID(),(void**)(&m_parent));
  m_cntTask=0;
}

StatusCode AligOnlIterator::initialize()
{
  int npar;
  m_Minuit = new TMinuit(3);
  m_Minuit->SetFCN(&Chi2);
  read_params(npar,m_params);
  for (size_t i=0;i<m_params.size();i++)
  {
    char nam[2];
    char ii;
    ii = char(i);
    nam[0] = 'a'+ii;
    nam[1] = 0;
    m_Minuit->DefineParameter(i,nam,m_params[i],10.0,0.0,0.0);
  };
  return StatusCode::SUCCESS;
}

StatusCode AligOnlIterator::i_run()
{
  int res=m_Minuit->Migrad();
  double par[100],dpar[100];
  printf("MIGRAD has finished with return code: %d\nParamters:\n",res);
  for (unsigned int i=0;i<m_params.size();i++)
  {
    m_Minuit->GetParameter(i,par[i],dpar[i]);
    printf("Param #%d %15g +- %15g\n",i,par[i],dpar[i]);
  }
  fflush(stdout);
  m_parent->doStop();
  return StatusCode::SUCCESS;
}
StatusCode AligOnlIterator::i_start()
{
  ::lib_rtl_start_thread(AligOnlIteratorThreadFunction,this,&m_thread);
  return StatusCode::SUCCESS;
}

StatusCode AligOnlIterator::stop()
{
  ::lib_rtl_delete_thread(m_thread);
  return StatusCode::SUCCESS;
}

StatusCode AligOnlIterator::finalize()
{
  if (m_Minuit)
  {
    delete m_Minuit;
    m_Minuit = 0;
  }
  return StatusCode::SUCCESS;
}

double AligOnlIterator::getIterationResult()
{
  std::vector<CntrDescr*> cdesc;
  if(m_cntTask == 0)
  {
    m_cntTask = new CounterTask(m_CntTask,m_CntDNS);
  }
  m_cntTask->Counters(m_CounterNames,cdesc);
  if (cdesc.size() > 0)
  {
    return cdesc[0]->d_data;
  }
  else
  {
    printf("No Counters Found...\n");
  }
  return nan("");
}
void AligOnlIterator::write_params(int npar, std::vector<double> &params)
{
  FILE *f;
  f = fopen(m_ParamFileName.c_str(),"w");
  for (int i=0;i<npar;i++)
  {
    fprintf(f,"%15lf ",params[i]);
  }
  fprintf(f,"\n");
  fclose(f);
}

void AligOnlIterator::write_params(int npar, double *params)
{
  FILE *f;
  f = fopen(m_ParamFileName.c_str(),"w");
  for (int i=0;i<npar;i++)
  {
    fprintf(f,"%15lf ",params[i]);
  }
  fprintf(f,"\n");
  fclose(f);
}

void AligOnlIterator::read_params(int &npar, std::vector<double> &params)
{
  FILE *f;
  params.clear();
  f = fopen(m_ParamFileName.c_str(),"r");
  while (!feof(f))
  {
    double p;
    fscanf(f,"%lf",&p);
    if (feof(f)) break;
    params.insert(params.end(),p);
  }
  fclose(f);
  npar = params.size();
}
StatusCode AligOnlIterator::queryInterface(const InterfaceID& riid, void** ppvIF)
{
  if (LHCb::IAlignIterator::interfaceID().versionMatch(riid))  {
    *ppvIF = (IAlignIterator*) this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return AlgTool::queryInterface(riid, ppvIF);
}

