#include "VPlottable.h"

//_____________________________________________________________________________
VPlottable::VPlottable() : m_plottableDimension(),
               m_plottableStyle(0),
               m_name(""),
               m_gotData(false),
               m_qcpGraph(),
               m_xs(),
               m_ys(),
               m_zs(){}


//_____________________________________________________________________________

VPlottable::VPlottable(VPlot * plot) :
             m_plottableDimension(),
             m_plottableStyle(0),
             m_name(""),
             m_gotData(false),
             m_qcpGraph(),
             m_xs(),
             m_ys(),
             m_zs(){
               m_plot = plot;
               m_plot->m_plottables.push_back(this);
             }


//_____________________________________________________________________________

VPlottable::VPlottable(VPlot * plot,
           int style) :
             m_plottableDimension(),
             m_name(""),
             m_gotData(false),
             m_qcpGraph(),
             m_xs(),
             m_ys(),
             m_zs(){
               m_plot = plot;
               m_plot->m_plottables.push_back(this);
               m_plottableStyle = style;
             }


//_____________________________________________________________________________

void VPlottable::reset() {
    m_xs.clear();
    m_ys.clear();
    for (std::size_t i=0; i<m_zs.size(); i++) m_zs[i].clear();
    m_zs.clear();
    m_statsTitles.clear();
    m_statsValues.clear();
}


//_____________________________________________________________________________
