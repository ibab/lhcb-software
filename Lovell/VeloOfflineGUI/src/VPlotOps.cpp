#include "VPlotOps.h"
#include <QApplication>

//_____________________________________________________________________________

VPlotOps::VPlotOps(QGroupBox * opsHolder, std::string * dataDir) {
	m_waitSwitch = true;
	m_dataDir = dataDir;
  m_firstTime = true;
  m_opsHolder = opsHolder;
  m_selPlot = NULL;
  if (opsHolder->layout() != NULL) delete opsHolder->layout();
  m_layout = new QGridLayout();
  opsHolder->setLayout(m_layout);
  m_layout->setContentsMargins(6,6,6,6);

  m_textBrowser = new QTextBrowser();


  m_layout->addWidget(new QLabel("Notifications:"), m_layout->rowCount(), 0, 1, 1);
  m_layout->addWidget(m_textBrowser, m_layout->rowCount(), 0, 1, 1);

  // Buttons.
  QWidget * buttonBox = new QWidget();
  QGridLayout * l = new QGridLayout();
  buttonBox->setLayout(l);
  l->setContentsMargins(0,0,0,0);

  // Log boxes.
  b_logx = new QCheckBox("LogX");
  b_logx->setChecked(false);
  l->addWidget(b_logx, 1, 0, 1, 1);

  b_logy = new QCheckBox("LogY");
  b_logy->setChecked(false);
  l->addWidget(b_logy, 1, 1, 1, 1);

  b_logz = new QCheckBox("LogZ");
  b_logz->setChecked(false);
  l->addWidget(b_logz, 1, 2, 1, 1);

  b_toggleRef = new QCheckBox("Toggle ref");
  b_toggleRef->setChecked(true);
  l->addWidget(b_toggleRef, 2, 0, 1, 3);

  b_toggleRefDiff = new QCheckBox("Data - Ref");
  b_toggleRefDiff->setChecked(false);
  l->addWidget(b_toggleRefDiff, 3, 0, 1, 3);

  b_toggleRefRatio = new QCheckBox("Data/Ref");
  b_toggleRefRatio->setChecked(false);
  l->addWidget(b_toggleRefRatio, 4, 0, 1, 3);

  b_popout = new QPushButton(QIcon(":/popoutIcon.png"), "");
  l->addWidget(b_popout, 5,0,1,1);
  b_popout->setStyleSheet("background: lightGrey");

  b_ref = new QPushButton(QIcon(":/refreshIcon.png"), "");
  l->addWidget(b_ref, 5,1,1,1);
  b_ref->setStyleSheet("background: lightGrey");

  b_tip = new QPushButton("?");
  l->addWidget(b_tip, 5,2,1,1);
  b_tip->setStyleSheet("background: lightGrey");
  connect(b_tip, SIGNAL(clicked()), this, SLOT(showTip()));

  l->setAlignment(Qt::AlignCenter);
  m_layout->addWidget(buttonBox, m_layout->rowCount(), 0, 1, 1);
}


void VPlotOps::showTip() {
	std::cout<<m_selPlot->m_tip<<std::endl;
	QMessageBox tip(QMessageBox::Information, "Tip", QString(m_selPlot->m_tip.c_str()));
	tip.exec();
}

//_____________________________________________________________________________

void VPlotOps::newSelection(VPlot * selPlot, bool forceConnect) {
  if (selPlot != m_selPlot || forceConnect) {
  	// Disconnections (only needed if previously connected).
    if (!m_firstTime) {
      m_selPlot->setFrameStyle(0);
      disconnect(b_logx, SIGNAL(stateChanged(int)), m_selPlot->m_vcp, SLOT(doLogX(int)));
      disconnect(b_logy, SIGNAL(stateChanged(int)), m_selPlot->m_vcp, SLOT(doLogY(int)));
      disconnect(b_logz, SIGNAL(stateChanged(int)), m_selPlot->m_vcp, SLOT(doLogZ(int)));
      disconnect(b_toggleRef, SIGNAL(stateChanged(int)), m_selPlot->m_vcp, SLOT(doToggleRef(int)));
      disconnect(b_toggleRefDiff, SIGNAL(stateChanged(int)), m_selPlot->m_vcp, SLOT(doToggleRefDiff(int)));
      disconnect(b_toggleRefRatio, SIGNAL(stateChanged(int)), m_selPlot->m_vcp, SLOT(doToggleRefRatio(int)));
      if (m_selPlot->m_multipleModules) {
				disconnect(b_moduleSelector1, SIGNAL(currentIndexChanged(int)), m_selPlot, SLOT(moduleChanged()));
				disconnect(b_moduleSelector2, SIGNAL(currentIndexChanged(int)), m_selPlot, SLOT(moduleChanged()));
				m_moduleSelector->setEnabled(false);
      }

      disconnect(b_popout, SIGNAL(clicked()), m_selPlot->m_vcp, SLOT(popoutClicked()));
      disconnect(b_ref, SIGNAL(clicked()), m_selPlot->m_vcp, SLOT(refreshClicked()));
      delete m_statsBox;
    }

    if (selPlot->m_plottables[0]->m_plottableDimension==2) b_toggleRef->setEnabled(false);
    else {
    	if (b_showAllRefs->isChecked()) b_toggleRef->setEnabled(true);
    }


    m_firstTime = false;
    if (selPlot->m_vcp->m_xLogged) b_logx->setChecked(true);
    else b_logx->setChecked(false);
    if (selPlot->m_vcp->m_yLogged) b_logy->setChecked(true);
    else b_logy->setChecked(false);
    if (selPlot->m_vcp->m_zLogged) b_logz->setChecked(true);
    else b_logz->setChecked(false);
    if (selPlot->m_vcp->m_refToggled) b_toggleRef->setChecked(true);
    else b_toggleRef->setChecked(false);
    if (selPlot->m_vcp->m_refToggledDiff) b_toggleRefDiff->setChecked(true);
    else b_toggleRefDiff->setChecked(false);
    if (selPlot->m_vcp->m_refToggledRatio) b_toggleRefRatio->setChecked(true);
    else b_toggleRefRatio->setChecked(false);



    m_selPlot = selPlot;
    m_selPlot->setFrameStyle(3);

    connect(b_logx, SIGNAL(stateChanged(int)), selPlot->m_vcp, SLOT(doLogX(int)));
    connect(b_logy, SIGNAL(stateChanged(int)), selPlot->m_vcp, SLOT(doLogY(int)));
    connect(b_logz, SIGNAL(stateChanged(int)), selPlot->m_vcp, SLOT(doLogZ(int)));
    connect(b_toggleRef, SIGNAL(stateChanged(int)), selPlot->m_vcp, SLOT(doToggleRef(int)));
    connect(b_toggleRefDiff, SIGNAL(stateChanged(int)), selPlot->m_vcp, SLOT(doToggleRefDiff(int)));
    connect(b_popout, SIGNAL(clicked()), selPlot->m_vcp, SLOT(popoutClicked()));
    connect(b_ref, SIGNAL(clicked()), selPlot->m_vcp, SLOT(refreshClicked()));
    connect(b_toggleRefRatio, SIGNAL(stateChanged(int)), m_selPlot->m_vcp, SLOT(doToggleRefRatio(int)));
    if (m_selPlot->m_multipleModules) {
      m_moduleSelector->setEnabled(true);
      b_moduleSelector1->setCurrentIndex(m_selPlot->m_moduleNum / b_moduleSelector2->count());
      b_moduleSelector2->setCurrentIndex(m_selPlot->m_moduleNum % b_moduleSelector2->count());
      connect(b_moduleSelector1, SIGNAL(currentIndexChanged(int)), m_selPlot, SLOT(moduleChanged()));
      connect(b_moduleSelector2, SIGNAL(currentIndexChanged(int)), m_selPlot, SLOT(moduleChanged()));
    }

    std::cout<<"Selection made"<<std::endl;
    m_statsBox = selPlot->exportStatsBox();
    m_layout->addWidget(m_statsBox, m_layout->rowCount(), 0, 1, m_layout->columnCount());
  }
  QScrollBar * sb = m_textBrowser->verticalScrollBar();
	sb->setValue(sb->maximum());
}


//_____________________________________________________________________________

void VPlotOps::notify(std::string message) {
	std::cout<<"NOTE: "<<message<<std::endl;
	m_textBrowser->append(QString(message.c_str()));
	QScrollBar * sb = m_textBrowser->verticalScrollBar();
	sb->setValue(sb->maximum());
}
