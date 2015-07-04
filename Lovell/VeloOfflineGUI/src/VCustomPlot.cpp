#include "VCustomPlot.h"

//_____________________________________________________________________________

VCustomPlot::VCustomPlot(QWidget * parent, std::string title, bool isPopUp, VPlot * plot,
		VPlotOps * plotOps) : QWidget(parent) {
  m_xLogged = false;
  m_yLogged = false;
  m_zLogged = false;
  m_colormap = NULL;

  setStyleSheet("background-color:white;");
  m_plotOps = plotOps;
  m_plot = plot;
  m_isPopUp = isPopUp;
  m_qcp = new QCustomPlot(this);
  m_buttonLayout = NULL;
  m_buttonBox = NULL;
  m_RubberBand = new QRubberBand(QRubberBand::Rectangle, this);
  QPalette pal;
  pal.setBrush(QPalette::Highlight, QBrush(QColor(100, 100, 100, 100)));
  m_RubberBand->setPalette(pal);


  // Setup the additional plot interactions.
  m_qcp->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                  QCP::iSelectLegend | QCP::iSelectPlottables);
  connect(m_qcp, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
  connect(m_qcp, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));
  connect(m_qcp->xAxis, SIGNAL(rangeChanged(QCPRange)), m_qcp->xAxis2, SLOT(setRange(QCPRange)));
  connect(m_qcp->yAxis, SIGNAL(rangeChanged(QCPRange)), m_qcp->yAxis2, SLOT(setRange(QCPRange)));
  connect(m_qcp, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress(QMouseEvent*)));
  connect(m_qcp, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(mouseMove(QMouseEvent*)));
  connect(m_qcp, SIGNAL(mouseRelease(QMouseEvent*)), this, SLOT(mouseRelease(QMouseEvent*)));

  m_qcp->setContextMenuPolicy(Qt::CustomContextMenu);
  //connect(m_qcp, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));


  // Esthetics.
  m_qcp->axisRect()->setupFullAxesBox();
  m_layout = new QGridLayout(this);
  setLayout(m_layout);
  m_layout->addWidget(m_qcp, 1, 1, 1, 1);
  m_qcp->setContentsMargins(0,0,0,0);
  m_layout->setContentsMargins(3, 1, 3, 3);
  m_layout->setVerticalSpacing(0);
  m_qcp->xAxis->setLabelPadding(1);
  m_qcp->yAxis->setLabelPadding(1);

  m_qcp->xAxis->setPadding(1);
  m_qcp->yAxis->setPadding(1);


  // Fonts.
  int size = 9;
  QFont ftick(font().family(), size);
  QFont faxistitle(font().family(), size);
  QFont flegend(font().family(), size);
  QFont ftitle(font().family(), size);
  m_qcp->legend->setFont(flegend);
  m_qcp->xAxis->setLabelFont(faxistitle);
  m_qcp->yAxis->setLabelFont(faxistitle);
  m_qcp->xAxis->setTickLabelFont(ftick);
  m_qcp->yAxis->setTickLabelFont(ftick);

  m_qcp->plotLayout()->insertRow(0);
  QCPPlotTitle * t = new QCPPlotTitle(m_qcp, title.c_str());
  t->setFont(ftitle);
  m_qcp->plotLayout()->addElement(0, 0, t);

  m_qcp->legend->setBrush(QColor(255,255,255,100));


  // Axes ticks.
  if (m_plot->m_tab->m_plots.size() > 2) {
    m_qcp->xAxis->setAutoTickCount(4);
    m_qcp->yAxis->setAutoTickCount(4);
  }
}


//_____________________________________________________________________________

void VCustomPlot::selectionChanged() {
  if (m_qcp->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || m_qcp->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      m_qcp->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || m_qcp->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels)) {
    m_qcp->xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    m_qcp->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }

  if (m_qcp->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || m_qcp->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      m_qcp->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || m_qcp->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels)) {
    m_qcp->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    m_qcp->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }

  for (int i=0; i<m_qcp->graphCount(); ++i) {
    QCPGraph *graph = m_qcp->graph(i);
    QCPPlottableLegendItem *item = m_qcp->legend->itemWithPlottable(graph);
    if (item->selected() || graph->selected()) {
      item->setSelected(true);
      graph->setSelected(true);
    }
  }
}


//_____________________________________________________________________________

void VCustomPlot::addPlotButtons() {
  if (m_isPopUp) {
    m_buttonBox = new QWidget(this);
    QWidget * buttonBox = m_buttonBox;
    m_buttonLayout = new QGridLayout();
    QGridLayout * l = m_buttonLayout;
    buttonBox->setLayout(l);
    l->setContentsMargins(0, 10, 20, 10);

    // Log boxes.
    QCheckBox * logxBox = new QCheckBox("LogX");
    logxBox->setChecked(false);
    l->addWidget(logxBox, 1, 1, 1, 1);
    connect(logxBox, SIGNAL(stateChanged(int)), this, SLOT(doLogX(int)));

    QCheckBox * logyBox = new QCheckBox("LogY");
    logyBox->setChecked(false);
    l->addWidget(logyBox, 2, 1, 1, 1);
    connect(logyBox, SIGNAL(stateChanged(int)), this, SLOT(doLogY(int)));

    QCheckBox * logzBox = new QCheckBox("LogZ");
    logzBox->setChecked(false);
    l->addWidget(logzBox, 3, 1, 1, 1);
    connect(logzBox, SIGNAL(stateChanged(int)), this, SLOT(doLogZ(int)));


    if (!m_isPopUp) {
    QPushButton * popout = new QPushButton(QIcon(":/popoutIcon.png"), "", this);
    l->addWidget(popout, 4,1,1,1);
    connect(popout, SIGNAL(clicked()), this, SLOT(popoutClicked()));
    popout->setStyleSheet("background: lightGrey");

    QPushButton * ref = new QPushButton(QIcon(":/refreshIcon.png"), "", this);
    l->addWidget(ref, 5,1,1,1);
    connect(ref, SIGNAL(clicked()), this, SLOT(refreshClicked()));
    ref->setStyleSheet("background: lightGrey");
    }

    else {
    QPushButton * ref = new QPushButton(QIcon(":/refreshIcon.png"), "", this);
    l->addWidget(ref, 4,1,1,1);
    connect(ref, SIGNAL(clicked()), this, SLOT(popoutRefreshClicked()));
    ref->setStyleSheet("background: lightGrey");
    }
    l->setAlignment( Qt::AlignCenter );

    m_layout->addWidget(buttonBox, 1, 2, 2, 1);
    m_layout->setHorizontalSpacing(0);
    buttonBox->setMaximumWidth(70);
    buttonBox->setMinimumWidth(70);
  }
}


//_____________________________________________________________________________

void VCustomPlot::doLogX(int logOrNot) {
  if (logOrNot==2) {
    m_qcp->xAxis->setScaleType(QCPAxis::stLogarithmic);
    m_qcp->xAxis2->setScaleType(QCPAxis::stLogarithmic);
    m_qcp->xAxis->setRangeLower(0.1);
    m_xLogged = true;
  }

  else {
    m_qcp->xAxis->setScaleType(QCPAxis::stLinear);
    m_qcp->xAxis2->setScaleType(QCPAxis::stLinear);
    m_xLogged = false;
  }

  m_qcp->xAxis->rescale();
  m_qcp->yAxis->rescale();
  m_qcp->replot();
}


//_____________________________________________________________________________

void VCustomPlot::doLogZ(int logOrNot) {
  if (m_colormap != NULL) {
    if (logOrNot) {
      m_colormap->colorScale()->setDataScaleType(QCPAxis::stLogarithmic);
      m_colormap->colorScale()->axis()->setRangeLower(0.1);
      m_zLogged = true;
    }

    else {
      m_colormap->colorScale()->setDataScaleType(QCPAxis::stLinear);
      m_zLogged = false;
    }
  }
  m_qcp->replot();
}


//_____________________________________________________________________________

void VCustomPlot::doLogY(int logOrNot) {
  if (logOrNot==2) {
    m_qcp->yAxis->setScaleType(QCPAxis::stLogarithmic);
    m_qcp->yAxis2->setScaleType(QCPAxis::stLogarithmic);
    m_qcp->yAxis->setRangeLower(0.1);
    m_yLogged = true;
  }

  else {
    m_qcp->yAxis->setScaleType(QCPAxis::stLinear);
    m_qcp->yAxis2->setScaleType(QCPAxis::stLinear);
    m_yLogged = false;
  }

  m_qcp->xAxis->rescale();
  m_qcp->yAxis->rescale();
  m_qcp->replot();
}


//_____________________________________________________________________________

void VCustomPlot::mousePress(QMouseEvent* e) {
  if (!m_isPopUp) makeSelected();
  if (e->button() == Qt::RightButton){
    if (m_qcp->selectedGraphs().size() > 0) contextMenuRequest(e->pos());
    else {
      m_Origin = e->pos();
      m_RubberBand->setGeometry(QRect(m_Origin, QSize(1,1)));
      m_RubberBand->show();
    }
  }

  else {
    if (m_qcp->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
      m_qcp->axisRect()->setRangeDrag(m_qcp->xAxis->orientation());
    else if (m_qcp->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
      m_qcp->axisRect()->setRangeDrag(m_qcp->yAxis->orientation());
    else
      m_qcp->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
  }
}


//_____________________________________________________________________________

void VCustomPlot::mouseWheel() {
  if (!m_isPopUp) makeSelected();
  if (m_qcp->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    m_qcp->axisRect()->setRangeZoom(m_qcp->xAxis->orientation());
  else if (m_qcp->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    m_qcp->axisRect()->setRangeZoom(m_qcp->yAxis->orientation());
  else
    m_qcp->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}


//_____________________________________________________________________________

void VCustomPlot::removeSelectedGraph() {
  if (m_qcp->selectedGraphs().size() > 0) {
    m_qcp->removeGraph(m_qcp->selectedGraphs().first());
    m_qcp->replot();
  }
}


//_____________________________________________________________________________

void VCustomPlot::removeAllGraphs() {
  m_qcp->clearGraphs();
  m_qcp->replot();
}


//_____________________________________________________________________________

void VCustomPlot::contextMenuRequest(QPoint pos) {
  QMenu * menu = new QMenu(this);
  menu->setAttribute(Qt::WA_DeleteOnClose);

  if (m_qcp->legend->selectTest(pos, false) >= 0) {
    menu->addAction("Move to top left", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignLeft));
    menu->addAction("Move to top center", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignHCenter));
    menu->addAction("Move to top right", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignRight));
    menu->addAction("Move to bottom right", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignRight));
    menu->addAction("Move to bottom left", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignLeft));
  }

  else {
    if (m_qcp->selectedGraphs().size() > 0)
      menu->addAction("Remove selected graph", this, SLOT(removeSelectedGraph()));
  }

  menu->popup(m_qcp->mapToGlobal(pos));
}


//_____________________________________________________________________________

void VCustomPlot::moveLegend() {
  if (QAction* contextAction = qobject_cast<QAction*>(sender())) {
    bool ok;
    int dataInt = contextAction->data().toInt(&ok);
    if (ok) {
      m_qcp->axisRect()->insetLayout()->setInsetAlignment(0, (Qt::Alignment)dataInt);
      m_qcp->replot();
    }
  }
}


//_____________________________________________________________________________

void VCustomPlot::mouseRelease(QMouseEvent * e) {
  if (e->button() == Qt::RightButton){
    if (m_RubberBand->isVisible()) {
    const QRect & zoomRect = m_RubberBand->geometry();
    int xp1, yp1, xp2, yp2;
    zoomRect.getCoords(&xp1, &yp1, &xp2, &yp2);
    double x1 = m_qcp->xAxis->pixelToCoord(xp1);
    double x2 = m_qcp->xAxis->pixelToCoord(xp2);
    double y1 = m_qcp->yAxis->pixelToCoord(yp1);
    double y2 = m_qcp->yAxis->pixelToCoord(yp2);

    m_qcp->xAxis->setRange(x1, x2);
    m_qcp->yAxis->setRange(y1, y2);

    m_RubberBand->hide();
    m_qcp->replot();
    }
  }
}


//_____________________________________________________________________________

void VCustomPlot::mouseMove(QMouseEvent * event) {
  if (m_RubberBand->isVisible())
    m_RubberBand->setGeometry(QRect(m_Origin, event->pos()).normalized());
}


//_____________________________________________________________________________

void VCustomPlot::makeSelected() {
  m_plotOps->newSelection(m_plot, false);
}


//_____________________________________________________________________________


