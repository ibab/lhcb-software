/*
 * HLTPerfFormatter.h
 *
 *  Created on: Aug 24, 2012
 *      Author: Beat Jost
 */

#ifndef HLTPERFFORMATTER_H_
#define HLTPERFFORMATTER_H_
#include <map>
#include <set>
#include <math.h>

class HLTp_Node
{
  public:
    std::string m_name;
    int m_samples;
    double m_perfsum;
    double m_perfsum2;
    float m_perf;
    float m_rms;
    float m_scalefactor;
    HLTp_Node(std::string &n)
    {
      m_name = n;
      m_samples = 0;
      m_perfsum = 0.0;
      m_perfsum2 = 0.0;
      m_perf = 0.0;
    }
    void fill(std::string &p)
    {
      float f;
      sscanf(p.c_str(),"%f",&f);
      if (f > 0.0)
      {
        m_perfsum += f;
        m_perfsum2 += f*f;
        m_samples++;
      }
    }
    void calc()
    {
      if (m_samples != 0)
      {
        m_perf = m_perfsum/m_samples;
        m_rms = sqrt(m_perfsum2/m_samples-m_perf*m_perf);
      }
    }
    void Normalize(float mx)
    {
      m_scalefactor = m_perf/mx;
      if (m_scalefactor >1.0)m_scalefactor =1.0;
    }
};
typedef std::map<std::string,HLTp_Node*> nodemap;
class HLTPerfFormatter
{
  public:
    HLTPerfFormatter()
    {
    }
    void Extract(nodemap &);
    void Histogram(nodemap&);
};


#endif /* HLTPERFFORMATTER_H_ */
