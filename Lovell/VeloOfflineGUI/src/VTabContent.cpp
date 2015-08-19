#include "VTabContent.h"

//_____________________________________________________________________________
void VTabContent::drawPlots() {
  // Drawn in real time when requested to be viewed, including layout.

  QLayoutItem *child;
  while ((child = layout()->takeAt(0)) != 0) {
    delete child;
  }

  // Set the layout.
  if (layout()!=NULL) delete layout();
  m_plotLayout = new QGridLayout(this);
  m_plotLayout->setContentsMargins(2,2,2,2);
  m_plotLayout->setHorizontalSpacing(2);
  m_plotLayout->setVerticalSpacing(2);
  setLayout(m_plotLayout);


  // Add the plots.
  for (unsigned int i = 0; i<m_plots.size(); i++) {
    unsigned int row, column;
    if (m_plotFillDirection == 0) {row = i / m_layoutX; column = i % m_layoutX;}
    else {row = i % m_layoutX; column = i / m_layoutX;}
    m_plotLayout->addWidget(m_plots[i], row, column, 1, 1);
    m_plots[i]->draw();
  }
}


//_____________________________________________________________________________

void VTabContent::drawTables() {
  if (layout()!=NULL) delete layout();
  m_plotLayout = new QGridLayout(this);
  m_plotLayout->setContentsMargins(2,2,2,2);
  m_plotLayout->setHorizontalSpacing(2);
  m_plotLayout->setVerticalSpacing(2);
  setLayout(m_plotLayout);

  // Add the tables.
  for (unsigned int i = 0; i<m_tables.size(); i++) {
    unsigned int row, column;
    if (m_plotFillDirection == 0) {row = i / m_layoutX; column = i % m_layoutX;}
    else {row = i % m_layoutX; column = i / m_layoutX;}
    m_plotLayout->addWidget(m_tables[i], row, column, 1, 1);
    m_tables[i]->draw();
  }
}


//_____________________________________________________________________________

void VTabContent::drawTrees() {
  if (layout()!=NULL) delete layout();
  m_plotLayout = new QGridLayout(this);
  m_plotLayout->setContentsMargins(2,2,2,2);
  m_plotLayout->setHorizontalSpacing(2);
  m_plotLayout->setVerticalSpacing(2);
  setLayout(m_plotLayout);

  // Add the trees.
  for (unsigned int i = 0; i<m_trees.size(); i++) {
    unsigned int row, column;
    if (m_plotFillDirection == 0) {row = i / m_layoutX; column = i % m_layoutX;}
    else {row = i % m_layoutX; column = i / m_layoutX;}
    m_plotLayout->addWidget(m_trees[i], row, column, 1, 1);
    m_trees[i]->draw();
  }
}


//_____________________________________________________________________________
