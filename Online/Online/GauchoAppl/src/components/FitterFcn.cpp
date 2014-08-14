/*
 * FitterFcn.cpp
 *
 *  Created on: Aug 12, 2014
 *      Author: beat
 */
#include "FitterFcn.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/StatusCode.h"
#include "IDumAligWork.h"
using namespace LHCb;
//DECLARE_COMPONENT(LHCb::FitterFcn)

DECLARE_NAMESPACE_TOOL_FACTORY(LHCb,FitterFcn)

FitterFcn::FitterFcn(const std::string &  type, const std::string &  name, const IInterface *  parent  ) :
      AlgTool(type,name,parent),m_result(0.0)
{
  declareProperty("ParameterFileName",m_ParamFileName);
  declareProperty("DataFileName",m_DataFileName);
  const void *p = parent;
  m_Parent = (IDumAligWork*)p;
}
//void FitterFcn::setWorkerSvc(IDumAligWork *p)
//{
//  m_Parent = (DumAligWork*)m_parent;;
//}
FitterFcn::~FitterFcn()
{

}

StatusCode FitterFcn::finalize()
{
  return AlgTool::finalize();
}
StatusCode FitterFcn::initialize()
{
  StatusCode sc= AlgTool::initialize();
  printf ("Parameter File Name: %s\n",m_ParamFileName.c_str());
  printf ("Data File Name: %s\n",m_DataFileName.c_str());
  fflush(stdout);
//  init();
  return sc;
}
void FitterFcn::init()
{
  StatusCode sc;
  if ( !sc.isSuccess() )  {
//    return error("Cannot access monitoring service of type "+m_monitorSvcType+".");
  }
  FILE *f;
  m_MonSvc = m_Parent->getMonSvc();//service(m_monitorSvcType,m_MonSvc,true);
  m_SvcName = "Chi2";
  m_MonSvc->declareInfo(m_SvcName,m_result,"Chi Square",0);
  size_t ndat=0;
  f = fopen(m_DataFileName.c_str(),"r");
  std::fscanf(f,"%d",(int*)(&ndat));
  m_dat_x.reserve(ndat);
  m_dat_y.reserve(ndat);
  m_dat_dy.reserve(ndat);
  for (size_t i=0;i<ndat;i++)
  {
    double x,y,dy;
    fscanf(f,"%lf %lf %lf\n",&x,&y,&dy);
    m_dat_x.insert(m_dat_x.end(), x);
    m_dat_y.insert(m_dat_y.end(), y);
    m_dat_dy.insert(m_dat_dy.end(), dy);
    printf("%15f %15f %15f\n",m_dat_x[i],m_dat_y[i],m_dat_y[i]);
  }
  fclose(f);
  fflush(stdout);
  m_PartitionName = m_Parent->getPartitionName();
  m_RefFileName   = "/group/online/dataflow/options/"+m_PartitionName+"/Alignement_Reference_File.txt";

}

double FitterFcn::analyze()
{
  double result = 0.0;
  size_t i;
  for (i=0;i<m_dat_x.size();i++)
  {
    double x=m_dat_x[i];
    double y;
    size_t j;
    y = 0.0;
    for (j=0;j<m_params.size();j++)
    {
      y = x*y+m_params[j];
    }
    double chi = (y-m_dat_y[i])/m_dat_dy[i];
    result += chi*chi;
  }
  m_result = result;
  return result;
}

void FitterFcn::ReadParams()
{
  FILE *f;
  m_params.clear();
  f = fopen(m_ParamFileName.c_str(),"r");
  while (!feof(f))
  {
    double p;
    fscanf(f,"%lf",&p);
    if (feof(f)) break;
    m_params.insert(m_params.end(),p);
  }
  fclose(f);
  printf("Number of Parameters %ld\n",m_params.size());
  for (size_t i=0;i<m_params.size();i++)
  {
    printf ("Parameter %ld %lf\n",i,m_params[i]);
  }
  printf("\n");
  fflush(stdout);
}
void FitterFcn::PubResult(long reference)
{
  m_MonSvc->updatePerSvc(reference);
}
//void FitterFcn::run()
//{
//
//}
void FitterFcn::i_run()
{

}
StatusCode FitterFcn::queryInterface(const InterfaceID& riid, void** ppvIF)
{
  if (LHCb::IFitterFcn::interfaceID().versionMatch(riid))  {
    *ppvIF = (IFitterFcn*) this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return AlgTool::queryInterface(riid, ppvIF);
}
