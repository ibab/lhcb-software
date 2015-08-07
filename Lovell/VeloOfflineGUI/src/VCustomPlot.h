#ifndef VCUSTOMPLOT_H
#define VCUSTOMPLOT_H

#include "qcustomplot.h"
#include <QtGui/QWidget>
#include <iostream>
#include <QtGui/QRubberBand>
#include <QtCore/QPoint>
#include "VPlotOps.h"
#include <QtGui/QGridLayout>
#include <QtGui/QMenu>
#include <QtGui/QAction>

class VPlot;
class VPlotOps;

class VCustomPlot : public QWidget{
  Q_OBJECT
public:
  VCustomPlot(QWidget * parent, std::string, bool, VPlot*, VPlotOps*);
  QCustomPlot * m_qcp;
  QGridLayout * m_layout;
  bool m_isPopUp;
  std::vector<QCPColorMap*> m_colormaps;
  QGridLayout * m_buttonLayout;
  QWidget * m_buttonBox;
  QRubberBand * m_RubberBand;
  QPoint m_Origin;
  VPlot * m_plot;
  VPlotOps * m_plotOps;
  bool m_xLogged;
  bool m_yLogged;
  bool m_zLogged;
  bool m_refToggled; //true for on. Only supported for 1d histos.
  bool m_refToggledDiff; //true for on. Only supported for 1d histos.
  bool m_refToggledRatio;

  void addPlotButtons();
  void makeSelected();

private slots:
  void selectionChanged();
  void mousePress(QMouseEvent*);
  void mouseWheel();
  void mouseMove(QMouseEvent *);
  void mouseRelease(QMouseEvent *);
  void removeSelectedGraph();
  void removeAllGraphs();
  void contextMenuRequest(QPoint pos);
  void moveLegend();
  void doLogX(int);
  void doLogY(int);
  void doLogZ(int);
  void doToggleRef(int);
  void doToggleRefDiff(int);
  void doToggleRefRatio(int);
  void refreshClicked() {emit requestRefresh();}
  void popoutClicked() {emit requestPopUp();}
  void popoutRefreshClicked() {
    emit requestPopUp();
    delete this;
  }


signals:
   void requestPopUp();
   void requestRefresh();
};

#endif // VCUSTOMPLOT_H
