#include "../headers/VTable.h"

//_____________________________________________________________________________

VTable::VTable(std::string fileName, VTabContent * tab) : QWidget() {
  m_drawn = false;
  m_fileName = fileName;
  m_tab = tab;
  tab->m_tables.push_back(this);
}


//_____________________________________________________________________________

void VTable::draw() {
  if (!m_drawn) {
    setupLayout();
    m_table = getTable(false);
    m_layout->addWidget(m_table, 1, 1, 1, 1);
    addButtons();
    m_drawn = true;
  }
}


//_____________________________________________________________________________

void VTable::setupLayout() {
  // Layout options.
  if (layout() != NULL) delete layout();
  m_layout = new QGridLayout();
  m_layout->setContentsMargins(0,0,0,0);
  setLayout(m_layout);
}


//_____________________________________________________________________________

QTableWidget * VTable::getTable(bool isPopUp) {
  // Converts a .csv file into a QTableWidget. Start by opening the .csv file.
  std::ifstream myfile;
  myfile.open(m_fileName.c_str());
  if (!myfile) std::cout<<"Can't open file: " << m_fileName << std::endl;

  QTableWidget * table = new QTableWidget(10,10);
  fillTable(table, myfile);
  return table;
}


//_____________________________________________________________________________

void VTable::fillTable(QTableWidget * table, std::ifstream & myfile) {
  int row = 0;
  while (!myfile.eof()) {
    std::string line;
    std::getline(myfile, line);
    std::stringstream lineStream(line);
    std::string cell;
    int column = 0;
    while(std::getline(lineStream, cell, ',')) {
      QTableWidgetItem * item = new QTableWidgetItem(cell.c_str());
      if (row == 0 && column != 0) table->setHorizontalHeaderItem(column-1, item);
      else {
        if (column == 0 && row != 0) table->setVerticalHeaderItem(row-1, item);
        else if (column != 0 && row != 0) table->setItem(row-1, column-1, item);
      }
      column++;
    }
    row++;
  }
}


//_____________________________________________________________________________

void VTable::makePopUp() {
  QTableWidget * table = getTable(true);
  table->resize(600, 500);
  table->show();
}


//_____________________________________________________________________________

void VTable::addButtons() {
  QPushButton * popout = new QPushButton(QIcon("popoutIcon.png"), "", this);
  m_layout->addWidget(popout,m_layout->rowCount()+1,1,1,1);
  connect(popout, SIGNAL(clicked()), this, SLOT(makePopUp()));
  popout->setStyleSheet("background: lightGrey");
}


//_____________________________________________________________________________


