#include "VTree.h"

//_____________________________________________________________________________

VTree::VTree(std::string title, std::string fileName, VTabContent * tab) : QWidget() {
	m_title = title;
  m_drawn = false;
  m_fileName = fileName;
  m_tab = tab;
  tab->m_trees.push_back(this);
}


//_____________________________________________________________________________

void VTree::draw() {
  if (!m_drawn) {
    setupLayout();

    QFile file(m_fileName.c_str());
    file.open(QIODevice::ReadOnly);
    TreeModel * model = new TreeModel(file.readAll());
    file.close();
    QTreeView * view = new QTreeView();
    view->setModel(model);
    m_layout->addWidget(view, 1, 1, 1, 1);
    m_drawn = true;
  }
	std::cout<<"VTree::draw"<<std::endl;
}


//_____________________________________________________________________________

void VTree::setupLayout() {
  // Layout options.
  if (layout() != NULL) delete layout();
  m_layout = new QGridLayout();
  m_layout->setContentsMargins(0,0,0,0);
  setLayout(m_layout);
}


//_____________________________________________________________________________

