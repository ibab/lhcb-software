#include "VPlot.h"
//_____________________________________________________________________________

VPlot::VPlot() :
  m_plottables(),
  m_title("Default Title"),
  m_xAxisTitle("x"),
  m_yAxisTitle("y"),
  m_zAxisTitle("z"),
  m_drawn(false),
  m_vcp(NULL),
  m_layout(NULL),
  m_multipleModules(false),
	m_yRangeSpecified(false),
	m_yLow(0),
	m_yUp(1),
	m_tip("Default help text"){}


//_____________________________________________________________________________

VPlot::VPlot(std::string title,
             VTabContent * tab, bool multipleModules, VPlotOps * plotOps) :
  m_plottables(),
  m_xAxisTitle("x"),
  m_yAxisTitle("y"),
  m_zAxisTitle("z"),
  m_drawn(false),
  m_vcp(NULL),
  m_layout(NULL),
  m_multipleModules(multipleModules),
  m_moduleNum(0),
	m_yRangeSpecified(false),
	m_yLow(0),
	m_yUp(1),
	m_tip("Default help text")
{
  m_title = title;
  m_tab = tab;
  m_tab->m_plots.push_back(this);
  m_plotOps = plotOps;
}


//_____________________________________________________________________________

void VPlot::draw() {
  setupLayout();
  m_vcp = setupPlot(false);
  m_layout->addWidget(m_vcp, 1, 1, 1, 1);
  m_vcp->makeSelected();
  if (m_yRangeSpecified) m_vcp->m_qcp->yAxis->setRange(QCPRange(m_yLow, m_yUp));
}


//_____________________________________________________________________________

void VPlot::moduleChanged()
{
  m_drawn = false;
  for (unsigned int i=0; i<m_plottables.size(); i++)
    m_plottables[i]->m_gotData = false;

  m_moduleNum = m_plotOps->currentModuleNum();
  getData();
  draw();
  m_plotOps->newSelection(this, true);
}


//_____________________________________________________________________________

void VPlot::setupLayout() {
  // Layout options.
  foreach (QWidget * w, findChildren<QWidget*>())
    if (! w->windowFlags() & Qt::Window) delete w;
  delete layout();
  m_layout = new QGridLayout(this);
  m_layout->setContentsMargins(0,0,0,0);
  m_layout->setVerticalSpacing(2);
  setLayout(m_layout);
}


//_____________________________________________________________________________

VCustomPlot * VPlot::setupPlot(bool isPopUp) {
	VCustomPlot * vcp;
	if (isPopUp) vcp = new VCustomPlot(NULL, m_title, isPopUp, this, m_plotOps);
	else vcp = new VCustomPlot(this, m_title, isPopUp, this, m_plotOps);
  setupPlottables(vcp);
  vcp->m_qcp->xAxis->setLabel(m_xAxisTitle.c_str());
  vcp->m_qcp->yAxis->setLabel(m_yAxisTitle.c_str());
  connect(vcp, SIGNAL(requestPopUp()), this, SLOT(makePopUp()));
  connect(vcp, SIGNAL(requestRefresh()), this, SLOT(refresh()));
  vcp->addPlotButtons();
  addStatsBox(vcp, isPopUp);
  return vcp;
}


//_____________________________________________________________________________

void VPlot::setupPlottables(VCustomPlot * vcp) {
  for (std::vector<VPlottable*>::iterator iplottable = m_plottables.begin();
       iplottable != m_plottables.end(); iplottable++) {
    (*iplottable)->getData();
    if ((*iplottable)->m_plottableDimension == 1) add1dPlot(vcp, (*iplottable));
    else if ((*iplottable)->m_plottableDimension == 2 && !(*iplottable)->m_isRef) addColzPlot(vcp, (*iplottable));
    if ((*iplottable)->m_isRefDiff && vcp->m_qcp->plottableCount() > 0) vcp->m_qcp->plottable()->setVisible(false);
    vcp->m_qcp->rescaleAxes(true);
  }
}


//_____________________________________________________________________________

void VPlot::addStatsBox(VCustomPlot * vcp, bool isPopUp) {
  if (isPopUp) {
    m_statsBox = new QWidget();
    QWidget * statsBox = m_statsBox;
    QGridLayout * lay = new QGridLayout();
    statsBox->setLayout(lay);
    lay->setContentsMargins(10,0,10,0);
    lay->setVerticalSpacing(0);
    lay->setHorizontalSpacing(0);
    QLabel * lab;
    QFont f(font().family(), 10);
    QPalette p;
    p.setColor(QPalette::WindowText, Qt::black);

    int irow = 0;
    for (std::vector<VPlottable*>::iterator iplottable = m_plottables.begin();
       iplottable!= m_plottables.end(); iplottable++) {
    	 if ((*iplottable)->m_isRef || (*iplottable)->m_isRefDiff) continue;
       lab = new QLabel((*iplottable)->m_name.c_str());
       lab->setFont(f);
       lay->addWidget(lab, irow, 1, 1, 1);
       lab->setPalette(p);
       lab->setWordWrap(true);

      for (unsigned int istat = 0; istat < (*iplottable)->m_statsTitles.size(); istat++) {
        lab = new QLabel(((*iplottable)->m_statsTitles[istat] + " " + (*iplottable)->m_statsValues[istat]).c_str());
        lab->setFont(f);
        lab->setPalette(p);
        lay->addWidget(lab, irow, 2+istat, 1, 1);
      }
      irow++;
    }
    statsBox->setMaximumHeight(15*m_plottables.size());
    vcp->m_layout->addWidget(statsBox, 2, 1, 1, 2);
  }
}

//_____________________________________________________________________________

QWidget * VPlot::exportStatsBox() {
  QWidget * statsBox = new QWidget();
  QGridLayout * lay = new QGridLayout();
  statsBox->setLayout(lay);
  lay->setContentsMargins(0,0,0,0);
  lay->setVerticalSpacing(0);
  lay->setHorizontalSpacing(0);
  QLabel * lab;
  QFont f(font().family(), 13);
  //f.setBold(true);
  QPalette p;
  p.setColor(QPalette::WindowText, Qt::black);

  QLabel * labTit;
  QLabel * labVal;


  int irow = 0;
  for (std::vector<VPlottable*>::iterator iplottable = m_plottables.begin();
     iplottable!= m_plottables.begin() + 1; iplottable++) {
  	if ((*iplottable)->m_isRef || (*iplottable)->m_isRefDiff) continue;
    lab = new QLabel((*iplottable)->m_name.c_str());
    lab->setFont(f);
    lab->setWordWrap(true);
    lay->addWidget(lab, lay->rowCount(), 0, 2, 2);
    lab->setPalette(p);

    for (unsigned int istat = 0; istat < (*iplottable)->m_statsTitles.size(); istat++) {
      labTit = new QLabel((*iplottable)->m_statsTitles[istat].c_str());
      labVal = new QLabel((*iplottable)->m_statsValues[istat].c_str());
      lab->setFont(f);
      lab->setPalette(p);
      lay->addWidget(labTit, lay->rowCount(), 0, 1, 1);
      lay->addWidget(labVal, lay->rowCount()-1, 1, 1, 1);
    }
    irow++;
  }
  return statsBox;
}


//_____________________________________________________________________________

void VPlot::addColzPlot(VCustomPlot * vcp, VPlottable * plottable) {
  int nbinsx = plottable->m_xs.size();
  int nbinsy = plottable->m_ys.size();

  //Make a QCPColorMap:
  QCPColorMap * colorMap = new QCPColorMap(vcp->m_qcp->xAxis, vcp->m_qcp->yAxis);
  vcp->m_colormaps.push_back(colorMap);
  vcp->m_qcp->addPlottable(colorMap);
  vcp->m_qcp->plottable(vcp->m_qcp->plottableCount()-1)->setName(plottable->m_name.c_str());
  colorMap->data()->setSize(nbinsx, nbinsy);
  colorMap->data()->setRange(QCPRange(plottable->m_xs[0], plottable->m_xs[nbinsx-1]),
                                    QCPRange(plottable->m_ys[0], plottable->m_ys[nbinsy-1]));
  for (int ix=0; ix<nbinsx; ++ix){
    for (int iy=0; iy<nbinsy; ++iy){
      /* double x = plottable->m_xs[ix]; */
      /* double y = plottable->m_ys[iy]; */
      colorMap->data()->setCell(ix, iy, plottable->m_zs[ix][iy]);
    }
  }

  if (!plottable->m_isRef && !plottable->m_isRefDiff) {
		//Add a color scale:
		m_colorScale = new QCPColorScale(vcp->m_qcp);
		int colorScaleRow = 1;
		m_colorScale->setBarWidth(25);
		m_colorScale->axis()->setPadding(0);
	  m_colorScale->axis()->setLabel(m_zAxisTitle.c_str());
		m_colorScale->axis()->setLabelFont(vcp->m_qcp->xAxis->labelFont());
		m_colorScale->axis()->setTickLabelFont(vcp->m_qcp->xAxis->tickLabelFont());
		vcp->m_qcp->plotLayout()->addElement(colorScaleRow, 1, m_colorScale); // add it to the right of the main axis rect

		//Make sure the axis rect and color scale synchronize their bottom and top margins (so they line up):
		QCPMarginGroup * marginGroup = new QCPMarginGroup(vcp->m_qcp);
		vcp->m_qcp->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
		m_colorScale->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
  }
  colorMap->setColorScale(m_colorScale);
  if (!plottable->m_isRef && !plottable->m_isRefDiff) colorMap->rescaleDataRange(true);
  colorMap->setGradient(QCPColorGradient::gpJet);
  //Set the color gradient of the color map to one of the presets:

  //Rescale all axes.
  vcp->m_qcp->xAxis->rescale();
  vcp->m_qcp->yAxis->rescale();
  colorMap->setInterpolate(false);
}


//_____________________________________________________________________________

void VPlot::add1dPlot(VCustomPlot * vcp, VPlottable* plottable) {
  vcp->m_qcp->addGraph();
  if (plottable->m_isRef) vcp->m_qcp->graph()->setName("Ref");
  else if (plottable->m_isRefDiff) {
  	if (plottable->m_plottableStyle == 3) {
  		vcp->m_qcp->graph()->setName("Data - Ref");
  	}
  	else vcp->m_qcp->graph()->setName("Data/Ref");
  }
  else vcp->m_qcp->graph()->setName("Data");
  vcp->m_qcp->graph()->setData(plottable->m_xs, plottable->m_ys);
  vcp->m_qcp->xAxis->rescale();
  vcp->m_qcp->yAxis->rescale();
  if (!vcp->m_isPopUp) plottable->m_qcpGraph = vcp->m_qcp->graph();

  if (plottable->m_plottableStyle == 0) {
    // Like a TGraph with scatter points (red line and discs).
    QCPScatterStyle ss(QCPScatterStyle::ssCross, Qt::red, 6);
    vcp->m_qcp->graph()->setScatterStyle(ss);
    vcp->m_qcp->graph()->setPen(QPen(Qt::red));
  }

  if (plottable->m_plottableStyle == 2) {
    // Like a TGraph with scatter points (blue line and discs).
    QCPScatterStyle ss(QCPScatterStyle::ssDisc, Qt::blue, 6);
    vcp->m_qcp->graph()->setScatterStyle(ss);
    vcp->m_qcp->graph()->setPen(QPen(Qt::blue));
    vcp->m_qcp->graph()->setLineStyle(QCPGraph::lsNone);
  }

  if (plottable->m_plottableStyle == 1) {
    // Like a TH1F (blue highlighted bins) - note, points should show center
    // of bins.
    vcp->m_qcp->graph()->setLineStyle(QCPGraph::lsStepCenter);
    vcp->m_qcp->graph()->setBrush(QBrush(QColor(0, 0, 255, 80)));
    vcp->m_qcp->graph()->setPen(QPen(Qt::black));
  }

  if (plottable->m_plottableStyle == 3) {
    // Like a TH1F (blue highlighted bins) - note, points should show center
    // of bins.
    vcp->m_qcp->graph()->setLineStyle(QCPGraph::lsStepCenter);
    vcp->m_qcp->graph()->setBrush(QBrush(QColor(255, 0, 0, 80)));
    vcp->m_qcp->graph()->setPen(QPen(Qt::black));
  }

  if (plottable->m_plottableStyle == 4) {
    // Like a TH1F (blue highlighted bins) - note, points should show center
    // of bins.
    vcp->m_qcp->graph()->setLineStyle(QCPGraph::lsStepCenter);
    vcp->m_qcp->graph()->setBrush(QBrush(QColor(0, 255, 0, 200)));
    vcp->m_qcp->graph()->setPen(QPen(Qt::black));
  }

  if (m_plottables.size() > 1) vcp->m_qcp->legend->setVisible(true);
}

//_____________________________________________________________________________

void VPlot::makePopUp() {
  VCustomPlot * vcp = setupPlot(true);
  if (m_yRangeSpecified) vcp->m_qcp->yAxis->setRange(QCPRange(m_yLow, m_yUp));
  vcp->resize(800, 600);
  vcp->show();
}


//_____________________________________________________________________________

void VPlot::refresh() {
  delete m_vcp;
  m_vcp = setupPlot(false);
  m_layout->addWidget(m_vcp, 1, 1, 1, 1);
  m_plotOps->newSelection(this, true);
  if (m_yRangeSpecified) m_vcp->m_qcp->yAxis->setRange(QCPRange(m_yLow, m_yUp));
}


//_____________________________________________________________________________

void VPlot::getData() {
  for (unsigned int i=0; i<m_plottables.size(); i++) m_plottables[i]->getData();
}


//_____________________________________________________________________________


