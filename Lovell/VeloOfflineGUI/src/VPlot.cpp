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
	m_zRangeSpecified(false),
	m_zLow(0),
	m_zUp(1),
	m_tip("Default help text."){}


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
	m_tip("Default help text."),
	m_zRangeSpecified(false),
	m_zLow(0),
	m_zUp(1)
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
  if (m_zRangeSpecified) m_colorScale->setDataRange(QCPRange(m_zLow, m_zUp));
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



  //Viridis time!
	QCPColorGradient * grad = new QCPColorGradient();
	grad->clearColorStops();
	grad->setColorInterpolation(QCPColorGradient::ciRGB);
	grad->setColorStopAt(0.0, QColor(68.08602255, 1.24295415, 84.00087345));
	grad->setColorStopAt(0.01171875, QColor(69.18274695, 5.0851743, 88.5534981));
	grad->setColorStopAt(0.0234375, QColor(70.1128671, 9.62671155, 92.95852365));
	grad->setColorStopAt(0.03515625, QColor(70.87506465, 14.3627322, 97.2036387));
	grad->setColorStopAt(0.046875, QColor(71.4679779, 18.7213962, 101.27668995));
	grad->setColorStopAt(0.05859375, QColor(71.8905129, 22.8648861, 105.16587855));
	grad->setColorStopAt(0.0703125, QColor(72.14217495, 26.87532975, 108.8600151));
	grad->setColorStopAt(0.08203125, QColor(72.2233491, 30.79813755, 112.3489302));
	grad->setColorStopAt(0.09375, QColor(72.13539195, 34.65961275, 115.6239717));
	grad->setColorStopAt(0.10546875, QColor(71.8811238, 38.47477485, 118.6782087));
	grad->setColorStopAt(0.1171875, QColor(71.4649434, 42.2516436, 121.5068931));
	grad->setColorStopAt(0.12890625, QColor(70.8931518, 45.9935442, 124.1077401));
	grad->setColorStopAt(0.140625, QColor(70.1737458, 49.700877, 126.4812444));
	grad->setColorStopAt(0.15234375, QColor(69.3161706, 53.3722803, 128.63070315));
	grad->setColorStopAt(0.1640625, QColor(68.3319573, 57.00502305, 130.562142));
	grad->setColorStopAt(0.17578125, QColor(67.234116, 60.5958489, 132.28421565));
	grad->setColorStopAt(0.1875, QColor(66.03595005, 64.14189675, 133.80770295));
	grad->setColorStopAt(0.19921875, QColor(64.7534199, 67.6397292, 135.14559615));
	grad->setColorStopAt(0.2109375, QColor(63.40039245, 71.08764795, 136.31168055));
	grad->setColorStopAt(0.22265625, QColor(61.9938762, 74.4833427, 137.32148055));
	grad->setColorStopAt(0.234375, QColor(60.5475519, 77.8265661, 138.189957));
	grad->setColorStopAt(0.24609375, QColor(59.0767425, 81.116979, 138.9327822));
	grad->setColorStopAt(0.2578125, QColor(57.5951415, 84.35531325, 139.56495015));
	grad->setColorStopAt(0.26953125, QColor(56.11451205, 87.5432544, 140.1003252));
	grad->setColorStopAt(0.28125, QColor(54.64588035, 90.68286285, 140.552022));
	grad->setColorStopAt(0.29296875, QColor(53.1989721, 93.77663505, 140.9320893));
	grad->setColorStopAt(0.3046875, QColor(51.78108795, 96.8276922, 141.25088775));
	grad->setColorStopAt(0.31640625, QColor(50.3973075, 99.8395431, 141.51710775));
	grad->setColorStopAt(0.328125, QColor(49.05108345, 102.8158317, 141.73807545));
	grad->setColorStopAt(0.33984375, QColor(47.74386165, 105.76034475, 141.91952835));
	grad->setColorStopAt(0.3515625, QColor(46.47518055, 108.67693275, 142.0656255));
	grad->setColorStopAt(0.36328125, QColor(45.2428599, 111.569436, 142.1789883));
	grad->setColorStopAt(0.375, QColor(44.0432838, 114.441603, 142.2607566));
	grad->setColorStopAt(0.38671875, QColor(42.87223455, 117.2969451, 142.31090745));
	grad->setColorStopAt(0.3984375, QColor(41.72448465, 120.1388589, 142.3278471));
	grad->setColorStopAt(0.41015625, QColor(40.59450315, 122.970537, 142.308564));
	grad->setColorStopAt(0.421875, QColor(39.4777944, 125.79479715, 142.24911585));
	grad->setColorStopAt(0.43359375, QColor(38.37139275, 128.6141052, 142.1445684));
	grad->setColorStopAt(0.4453125, QColor(37.2759663, 131.4303558, 141.98979105));
	grad->setColorStopAt(0.45703125, QColor(36.19349385, 134.2453977, 141.77769735));
	grad->setColorStopAt(0.46875, QColor(35.1314724, 137.06049315, 141.50109375));
	grad->setColorStopAt(0.48046875, QColor(34.10446245, 139.8763179, 141.1529754));
	grad->setColorStopAt(0.4921875, QColor(33.1328385, 142.6933131, 140.7252027));
	grad->setColorStopAt(0.50390625, QColor(32.2456119, 145.5114558, 140.2094805));
	grad->setColorStopAt(0.515625, QColor(31.48301655, 148.33008555, 139.5984699));
	grad->setColorStopAt(0.52734375, QColor(30.89275785, 151.14841695, 138.8834907));
	grad->setColorStopAt(0.5390625, QColor(30.5330778, 153.96507045, 138.05699745));
	grad->setColorStopAt(0.55078125, QColor(30.46805025, 156.7783401, 137.11150845));
	grad->setColorStopAt(0.5625, QColor(30.7627512, 159.58622415, 136.0395267));
	grad->setColorStopAt(0.57421875, QColor(31.4781129, 162.38629245, 134.83467465));
	grad->setColorStopAt(0.5859375, QColor(32.66219265, 165.17594655, 133.4901846));
	grad->setColorStopAt(0.59765625, QColor(34.34641665, 167.95222845, 132.000444));
	grad->setColorStopAt(0.609375, QColor(36.54224205, 170.71199655, 130.35994995));
	grad->setColorStopAt(0.62109375, QColor(39.24298275, 173.45187465, 128.56390335));
	grad->setColorStopAt(0.6328125, QColor(42.427716, 176.16830805, 126.60791565));
	grad->setColorStopAt(0.64453125, QColor(46.0665507, 178.8575661, 124.4882919));
	grad->setColorStopAt(0.65625, QColor(50.12551065, 181.5158034, 122.2013754));
	grad->setColorStopAt(0.66796875, QColor(54.57003825, 184.13899605, 119.7448737));
	grad->setColorStopAt(0.6796875, QColor(59.3678199, 186.72307425, 117.11557125));
	grad->setColorStopAt(0.69140625, QColor(64.48912005, 189.2638152, 114.31230525));
	grad->setColorStopAt(0.703125, QColor(69.9080511, 191.75695785, 111.3332295));
	grad->setColorStopAt(0.71484375, QColor(75.60214245, 194.1981621, 108.17696955));
	grad->setColorStopAt(0.7265625, QColor(81.551193, 196.58307765, 104.84379825));
	grad->setColorStopAt(0.73828125, QColor(87.73889805, 198.90728025, 101.33216265));
	grad->setColorStopAt(0.75, QColor(94.149621, 201.16642215, 97.6431669));
	grad->setColorStopAt(0.76171875, QColor(100.7693904, 203.35622955, 93.7781013));
	grad->setColorStopAt(0.7734375, QColor(107.58657315, 205.4724006, 89.73707295));
	grad->setColorStopAt(0.78515625, QColor(114.58874565, 207.51092925, 85.5229863));
	grad->setColorStopAt(0.796875, QColor(121.7636373, 209.46809505, 81.13979895));
	grad->setColorStopAt(0.80859375, QColor(129.0991815, 211.34048355, 76.59233805));
	grad->setColorStopAt(0.8203125, QColor(136.5833805, 213.1250526, 71.8866216));
	grad->setColorStopAt(0.83203125, QColor(144.201888, 214.8196245, 67.03359165));
	grad->setColorStopAt(0.84375, QColor(151.9390317, 216.4228911, 62.0488389));
	grad->setColorStopAt(0.85546875, QColor(159.77770365, 217.93458465, 56.9549079));
	grad->setColorStopAt(0.8671875, QColor(167.69870235, 219.3557889, 51.78598395));
	grad->setColorStopAt(0.87890625, QColor(175.68059505, 220.68918645, 46.59476025));
	grad->setColorStopAt(0.890625, QColor(183.69974325, 221.93928825, 41.46369615));
	grad->setColorStopAt(0.90234375, QColor(191.7304557, 223.11261465, 36.5232114));
	grad->setColorStopAt(0.9140625, QColor(199.74541425, 224.21778975, 31.9783719));
	grad->setColorStopAt(0.92578125, QColor(207.7169667, 225.26528895, 28.1384289));
	grad->setColorStopAt(0.9375, QColor(215.6179428, 226.26721965, 25.42405845));
	grad->setColorStopAt(0.94921875, QColor(223.4229012, 227.23684185, 24.2888673));
	grad->setColorStopAt(0.9609375, QColor(231.10935855, 228.18794085, 25.0218648));
	grad->setColorStopAt(0.97265625, QColor(238.6556322, 229.135452, 27.5733897));
	grad->setColorStopAt(0.984375, QColor(246.04785015, 230.09239305, 31.60483005));
	grad->setColorStopAt(0.99609375, QColor(253.27821195, 231.06992535, 36.703731));

	colorMap->setGradient(*grad);


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
  else if (plottable->m_isFit) vcp->m_qcp->graph()->setName("Fit");
  else vcp->m_qcp->graph()->setName("Data");
  vcp->m_qcp->graph()->setData(plottable->m_xs, plottable->m_ys);
  vcp->m_qcp->xAxis->rescale();
  vcp->m_qcp->yAxis->rescale();
  if (!vcp->m_isPopUp) plottable->m_qcpGraph = vcp->m_qcp->graph();

  if (plottable->m_plottableStyle == 0) {
    // Like a TGraph with scatter points (red line and discs).
    QCPScatterStyle ss(QCPScatterStyle::ssCross, Qt::red, 6);
    vcp->m_qcp->graph()->setScatterStyle(ss);
    vcp->m_qcp->graph()->setLineStyle(QCPGraph::lsNone);
//    QPen pen(Qt::red);
//    pen.setWidthF(1.5);
//    vcp->m_qcp->graph()->setPen(pen);
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

  if (plottable->m_plottableStyle == 5) {
    // Like a TH1F (blue highlighted bins) - note, points should show center
    // of bins.
  	QPen pen("black");
  	pen.setWidthF(2);
    vcp->m_qcp->graph()->setPen(pen);
  }

  if (m_plottables.size() > 1) vcp->m_qcp->legend->setVisible(true);
}

//_____________________________________________________________________________

void VPlot::makePopUp() {
  VCustomPlot * vcp = setupPlot(true);
  if (m_yRangeSpecified) vcp->m_qcp->yAxis->setRange(QCPRange(m_yLow, m_yUp));
  if (m_zRangeSpecified) m_colorScale->setDataRange(QCPRange(m_zLow, m_zUp));
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
  if (m_zRangeSpecified) m_colorScale->setDataRange(QCPRange(m_zLow, m_zUp));
}


//_____________________________________________________________________________

void VPlot::getData() {
  for (unsigned int i=0; i<m_plottables.size(); i++) m_plottables[i]->getData();
}


