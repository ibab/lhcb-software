#include "VeloView.h"
#include "ui_VeloView.h"
#include <QStylePainter>

//_____________________________________________________________________________

class TestTabBar : public QTabBar
{
public:
    explicit TestTabBar(QWidget* parent=0) : QTabBar(parent)
    {
        setIconSize(QSize(120, 25));
    }

protected:
    QSize tabSizeHint(int) const
    {
        return QSize(120, 25);
    }
    void paintEvent(QPaintEvent *)
    {
        QStylePainter p(this);
        for (int index = 0; index < count(); index++)
        {
            QStyleOptionTabV3 tab;
            initStyleOption(&tab, index);

            QIcon tempIcon = tab.icon;
            QString tempText = tab.text;
            tab.icon = QIcon();
            tab.text = QString();

            p.drawControl(QStyle::CE_TabBarTab, tab);

            QPainter painter;
            painter.begin(this);
            QRect tabrect = tabRect(index);
            //tabrect.adjust(0, 8, 0, -8);
            painter.drawText(tabrect, Qt::AlignVCenter | Qt::AlignHCenter, tempText);
            tempIcon.paint(&painter, 0, tabrect.top(), tab.iconSize.width(), tab.iconSize.height(), Qt::AlignTop | Qt::AlignHCenter);    
            painter.end();
        }
    }
};

class TestTabWidget : public QTabWidget
{
public:
    explicit TestTabWidget(QWidget *parent = 0) : QTabWidget(parent)
    {
        setTabBar(new TestTabBar());
    }
};

//_____________________________________________________________________________

veloview::veloview(int runMode, std::vector<std::string> * ops, QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::veloview),
  m_printOption(0),
  m_content(NULL),
  m_welcomeTabSet(false),
  m_ran(false),
  m_runMode(runMode),
  m_logoText("Default GUI"),
  m_plotOps(NULL),
  m_runProxy(NULL),
  m_verbose(false),
  m_dataDir("/calib/velo/dqm/VeloView/VetraOutput")
{
  m_ops = ops;
  setOps();
  std::cout<<"Data directory: "<<m_dataDir<<std::endl;

  QFile stylesheet("styleSheet.qss");
  stylesheet.open(QFile::ReadOnly);
  QString setSheet = QLatin1String(stylesheet.readAll());
  this->setStyleSheet(setSheet);
  ui->setupUi(this);
  m_VVinterfaceScript = "veloview_configuration.py";
  ui->w_moduleSelector->setEnabled(false);
  loadOptionsFile();
  setOptionsWidg();

  // Logo settings.
  if (m_runMode == 0) m_logoText = "VeloView";

  ui->l_logo->setText(QString(m_logoText.c_str()));

  this->setStyleSheet("QWidget{font-size:11px}");
  ui->l_logo->setStyleSheet("QWidget{font-size:30px}");
  connect(ui->b_load, SIGNAL(clicked()), this, SLOT(setContent()));  

  if (m_runMode == 3) setContent();
  else if (m_runMode == 0) {
    QPixmap plogo(QString(":/Logos/veloLogo.png"));
    ui->m_logo->setPixmap(plogo);
  }
}


//_____________________________________________________________________________

void veloview::setOps(){
  for (std::vector<std::string>::iterator iOp = m_ops->begin();
  		iOp != m_ops->end(); iOp++) {
  	if ((*iOp).size() > 14) {
  		if ((*iOp).substr(0, 14) == "--run-data-dir")
  			m_dataDir = (*iOp).substr(15, (*iOp).size());

  		else std::cout<<"Unknown option: "<<(*iOp)<<std::endl;
  	}
  	else std::cout<<"Unknown option: "<<(*iOp)<<std::endl;
  }
}



//_____________________________________________________________________________

void veloview::loadOptionsFile(){
  std::cout<<"Loading options file."<<std::endl;
}


//_____________________________________________________________________________

void veloview::setOptionsWidg(){
  std::cout<<"Setting options."<<std::endl;
  if (m_runMode == 0 || m_runMode == 3) setVeloOptionsWidg();
}


//_____________________________________________________________________________

void veloview::setVeloOptionsWidg() {
  QGridLayout * l;
  l = (QGridLayout*)ui->w_settings->layout();
  addModuleSelector();

  // Run number selected.
  l->addWidget(new QLabel("RunView #:"), l->rowCount(), 0, 1, 1);
  QStringListModel * runBoxModel = new QStringListModel;
  FILE * in;
  char buff[512];
  std::string command;
  std::cout<<"Retriving run list..."<<std::endl;
  if (m_runMode == 0) command = "" + m_VVinterfaceScript + " run_list --run-data-dir="
  		+ m_dataDir;
  else command = "dummyDataGetter.py run_list";
  std::cout<<"Proccessing..."<<std::endl;
  in = popen(command.c_str(), "r");
  while(fgets(buff, sizeof(buff), in)!=NULL) {
    std::string line(buff);
    line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
    std::cout<<"Available runs:\t"<<line<<std::endl;
    runBoxModel->insertRow(runBoxModel->rowCount());
    runBoxModel->setData(runBoxModel->index(runBoxModel->rowCount() - 1, 0),
                         QString(line.c_str()), Qt::DisplayRole);
  }

  b_veloRunNumber = new QComboBox;
  if (m_runProxy != NULL) delete m_runProxy;
  m_runProxy = new QSortFilterProxyModel;
  m_runProxy->setSourceModel(runBoxModel);
  b_veloRunNumber->setModel(m_runProxy);
  b_veloRunNumber->setEditable(true);
  QCompleter * completer = new QCompleter(runBoxModel, this);
  completer->setCaseSensitivity(Qt::CaseInsensitive);
  b_veloRunNumber->setCompleter(completer);
  connect(b_veloRunNumber, SIGNAL(editTextChanged(QString)), this, SLOT(filterWildcard(QString)));
  l->addWidget(b_veloRunNumber, l->rowCount(), 0, 1, 1);
}

void veloview::filterWildcard(QString s) {
	m_runProxy->setFilterWildcard(s.left(s.size()-1));
}


//_______________________ ______________________________________________________

void veloview::setContent() {
	QApplication::setOverrideCursor(Qt::WaitCursor);
  if (!m_ran) {
    ui->w_plotOps->setEnabled(true);
    m_plotOps = new VPlotOps(ui->w_plotOps, &m_dataDir);
    m_plotOps->b_moduleSelector1 = ui->b_selector1;
    m_plotOps->b_moduleSelector2 = ui->b_selector2;
    m_plotOps->b_veloRunNumber = b_veloRunNumber;
    m_plotOps->m_moduleSelector = ui->w_moduleSelector;
    m_plotOps->notify("Loading run " + b_veloRunNumber->currentText().toStdString() + "...");
    	qApp->processEvents();
  }
  else {
  	m_plotOps->notify("\nLoading run " + b_veloRunNumber->currentText().toStdString() + "...");
    m_plotOps->m_firstTime = true;
    qApp->processEvents();
    delete m_plotOps->m_statsBox;
  }


  // Creates the contents instance, as outlined by the relevant function in
  // VContentGetter (which may call other sources/databases).
  if (m_ran) {
    QLayoutItem* item;
    if (ui->m_contentHolder->layout() != NULL) {
      while ((item = ui->m_contentHolder->layout()->takeAt(0)) != NULL){
        delete item->widget(); delete item;
      }
    }
  }

  if (m_runMode == 0) {
    m_content = VContentGetter::veloFileConfigs(m_plotOps, m_VVinterfaceScript);
    if (!m_ran) delete ui->m_logo;
  }
  else if (m_runMode == 3) {
		m_content = VContentGetter::veloFileConfigs(m_plotOps, "dummyDataGetter.py");
  }

  else std::cout<<"Unknown run mode"<<std::endl;

  VPrint("Details for this GUI: \n");
  VPrintContentDetails(m_content);
  
  // Setup all the tabs in the GUI - includes setting a welcome tab.
  if (ui->m_contentHolder->layout() != NULL)
    delete ui->m_contentHolder->layout();

  QGridLayout * lay = new QGridLayout();
  lay->setContentsMargins(0,0,0,0);
  ui->m_contentHolder->setLayout(lay);
  completeTabs(m_content->m_subContents, ui->m_contentHolder, lay);
  m_ran = true;
  ui->b_load->setText("Reload");
  m_plotOps->m_waitSwitch = false;

  QApplication::restoreOverrideCursor();
}


//_____________________________________________________________________________

void veloview::completeTabs(std::vector<VTabContent*> & tabsContents,
  QWidget * tabsHolder, QGridLayout * topLay) {
  // Completes a set of tabs (for one particular level) in the GUI for the
  // given contents (belonging in that level). Called recusively. By ensuring
  // the use of a dummy top level tab, this function is always safe.

  // tabsHolder is passed as a parameter but not used in the function body
  Q_UNUSED(tabsHolder);
  QTabWidget * tabPages;
  if (tabsContents[0]->m_depth == 0) {
    tabPages = new TestTabWidget();
    tabPages->setTabPosition(QTabWidget::West);
  }
  else tabPages = new QTabWidget();
  
  topLay->addWidget(tabPages, 1, 1, 1, 1);

  // Complete the rest of the tabs.
  QGridLayout * lay;
  for (std::vector<VTabContent*>::iterator itabPage = tabsContents.begin();
     itabPage != tabsContents.end(); itabPage++) {
    lay = new QGridLayout();
    lay->setContentsMargins(2,2,2,2);
    (*itabPage)->setLayout(lay);
    tabPages->addTab((*itabPage), QString((*itabPage)->m_title.c_str()));
    (*itabPage)->m_qtab = tabPages;
    (*itabPage)->m_qtabID = tabPages->count()-1;
    tabPages->setTabEnabled(tabPages->count()-1, false);
    if ((*itabPage)->m_subContents.size() > 0) {
      completeTabs((*itabPage)->m_subContents, (*itabPage), lay);
    }
    if ((*itabPage)->m_title == "Pedestals")
      tabPages->setCurrentIndex(tabPages->count()-1);
  }
}


//_____________________________________________________________________________

void veloview::VPrintContentDetails(VTabContent* tabPage) {
  VPrint("Tab: ");
  if (tabPage != NULL) VPrint(tabPage->m_title);
  VPrint("\tParent Tab: ");
  if (tabPage->m_parentTab != NULL) VPrint(tabPage->m_parentTab->m_title);
  VPrint("\n");

  for (std::size_t i=0; i<tabPage->m_subContents.size(); i++)
      VPrintContentDetails(tabPage->m_subContents[i]);
}


//_____________________________________________________________________________

void veloview::VPrint(std::string x) {
  // Method that allows optional printing.
  if (m_printOption == 0) std::cout<<x;
}


void veloview::VPrint(int x) {
  // Method that allows optional printing.
  if (m_printOption == 0) std::cout<<x;
}


void veloview::VPrint(double x) {
  // Method that allows optional printing.
  if (m_printOption == 0) std::cout<<x;
}

//_____________________________________________________________________________

veloview::~veloview() {
  delete ui;
}


//_____________________________________________________________________________

void veloview::on_b_quit_clicked() {
  this->close();
}


//_____________________________________________________________________________

void veloview::addModuleSelector() {
  std::cout<<"Adding module selector."<<std::endl;
  ui->b_selector1->addItem("rL");
  ui->b_selector1->addItem("phiL");
  ui->b_selector1->addItem("rR");
  ui->b_selector1->addItem("phiR");

  for (int i=0; i<21; i++) {
    std::stringstream ss;
    ss<<i;
    ui->b_selector2->addItem(QString(ss.str().c_str()));
  }
}


//_____________________________________________________________________________

void veloview::on_b_selector3_clicked() {
	QApplication::setOverrideCursor(Qt::WaitCursor);
  ui->b_selector2->setCurrentIndex(ui->b_selector2->currentIndex()-1);
  moduleChanged();
  QApplication::restoreOverrideCursor();
}

void veloview::on_b_selector4_clicked() {
	QApplication::setOverrideCursor(Qt::WaitCursor);
  ui->b_selector2->setCurrentIndex(ui->b_selector2->currentIndex()+1);
  moduleChanged();
  QApplication::restoreOverrideCursor();
}

//_____________________________________________________________________________

void veloview::moduleChanged() {
  std::cout<<"veloview::moduleChanged"<<std::endl;
  std::cout<<ui->b_selector1->currentText().toStdString()<<"\t"<<ui->b_selector2->currentText().toStdString()<<std::endl;
}


//_____________________________________________________________________________
