#ifndef __VPLOTTABLE_H_INCLUDED__
#define __VPLOTTABLE_H_INCLUDED__

#include <functional> // Lambdas.
#include <iostream>
#include <QtCore/QVector>
#include "qcustomplot.h"
#include "sstream"
#include <iomanip>
#include "VPlot.h"
#include <mutex>

// VPlottable - a class to store data and settings of Qt plottables (ie plot 
// curves).
// Author: Daniel Saunders

class VPlot;

class VPlottable {
public:
  int m_plottableDimension; // Must be set.
  int m_plottableStyle; // Unique to type, 0 for default. TODO
  std::string m_name;
  bool m_gotData;
  QCPGraph * m_qcpGraph;
  std::vector< std::string > m_statsTitles;
  std::vector< std::string > m_statsValues;
  QVector<double> m_xs;
  QVector<double> m_ys;
  QVector<QVector<double> > m_zs;
  VPlot * m_plot;
  std::string m_retrivalCommand;
  int m_dataFileType;
  std::mutex m_mtx;
  bool m_isRef;
  bool m_isRefDiff;
  bool m_isFit;


  // Methods __________________________________________________________________
  VPlottable();
  VPlottable(VPlot*);
  VPlottable(VPlot*, int);
  void reset();
  void draw();
  void getData() {
  	m_mtx.lock();
    if (!m_gotData) {
			reset();

			getPlotData();
			m_gotData = true;
    }
    m_mtx.unlock();
  }

  virtual void getPlotData() {
    std::cout << "getData() not set in plot: "<< m_name << std::endl;
  }


  //___________________________________________________________________________
};

#endif
