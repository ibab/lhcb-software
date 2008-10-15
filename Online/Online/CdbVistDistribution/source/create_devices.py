# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'create_devices.ui'
#
# Created: Wed Jul 18 11:23:13 2007
#      by: The PyQt User Interface Compiler (pyuic) 3.13
#
# WARNING! All changes made in this file will be lost!


from qt import *


class Create_Devices(QDialog):
    def __init__(self,parent = None,name = None,modal = 0,fl = 0):
        QDialog.__init__(self,parent,name,modal,fl)

        if not name:
            self.setName("Create_Devices")


        Create_DevicesLayout = QGridLayout(self,1,1,11,6,"Create_DevicesLayout")

        self.__serialtxtbox = QLineEdit(self,"__serialtxtbox")

        Create_DevicesLayout.addMultiCellWidget(self.__serialtxtbox,1,1,8,10)

        self.__hwtypetxtbox = QLineEdit(self,"__hwtypetxtbox")

        Create_DevicesLayout.addMultiCellWidget(self.__hwtypetxtbox,3,3,8,10)

        self.__responsibletxtbox = QLineEdit(self,"__responsibletxtbox")

        Create_DevicesLayout.addMultiCellWidget(self.__responsibletxtbox,5,5,8,10)

        self.textLabel2_4_3 = QLabel(self,"textLabel2_4_3")

        Create_DevicesLayout.addMultiCellWidget(self.textLabel2_4_3,0,0,8,9)

        self.textLabel2_4_3_2 = QLabel(self,"textLabel2_4_3_2")

        Create_DevicesLayout.addMultiCellWidget(self.textLabel2_4_3_2,2,2,8,9)

        self.textLabel2_4_3_2_2_2 = QLabel(self,"textLabel2_4_3_2_2_2")

        Create_DevicesLayout.addMultiCellWidget(self.textLabel2_4_3_2_2_2,6,6,8,9)

        self.textLabel2_4_3_2_2 = QLabel(self,"textLabel2_4_3_2_2")

        Create_DevicesLayout.addMultiCellWidget(self.textLabel2_4_3_2_2,4,4,8,9)

        self.textLabel2_4_3_2_2_2_2 = QLabel(self,"textLabel2_4_3_2_2_2_2")

        Create_DevicesLayout.addMultiCellWidget(self.textLabel2_4_3_2_2_2_2,9,9,8,9)

        layout6 = QVBoxLayout(None,0,6,"layout6")

        self.textLabel2_3_2 = QLabel(self,"textLabel2_3_2")
        layout6.addWidget(self.textLabel2_3_2)

        self.__functiontxtbox = QComboBox(0,self,"__functiontxtbox")
        layout6.addWidget(self.__functiontxtbox)

        Create_DevicesLayout.addMultiCellLayout(layout6,13,13,0,6)

        layout11 = QVBoxLayout(None,0,6,"layout11")

        self.textLabel2_2 = QLabel(self,"textLabel2_2")
        layout11.addWidget(self.textLabel2_2)

        self.__sysnamescmbbox = QComboBox(0,self,"__sysnamescmbbox")
        layout11.addWidget(self.__sysnamescmbbox)

        Create_DevicesLayout.addMultiCellLayout(layout11,0,1,2,4)

        self.__delbutton = QPushButton(self,"__delbutton")
        self.__delbutton.setMaximumSize(QSize(21,21))

        Create_DevicesLayout.addWidget(self.__delbutton,1,6)

        self.__addbutton = QPushButton(self,"__addbutton")
        self.__addbutton.setMaximumSize(QSize(21,21))

        Create_DevicesLayout.addWidget(self.__addbutton,1,5)

        self.__chk_prommode = QCheckBox(self,"__chk_prommode")

        Create_DevicesLayout.addMultiCellWidget(self.__chk_prommode,11,11,0,1)

        layout5 = QVBoxLayout(None,0,6,"layout5")

        self.textLabel2_4_2_2 = QLabel(self,"textLabel2_4_2_2")
        layout5.addWidget(self.textLabel2_4_2_2)

        self.__startspinbox = QSpinBox(self,"__startspinbox")
        layout5.addWidget(self.__startspinbox)

        Create_DevicesLayout.addMultiCellLayout(layout5,10,10,0,1)

        layout4 = QVBoxLayout(None,0,6,"layout4")

        self.textLabel2_4_2_2_2 = QLabel(self,"textLabel2_4_2_2_2")
        layout4.addWidget(self.textLabel2_4_2_2_2)

        self.__endspinbox = QSpinBox(self,"__endspinbox")
        layout4.addWidget(self.__endspinbox)

        Create_DevicesLayout.addMultiCellLayout(layout4,10,10,2,6)

        self.__paddingchkbox = QCheckBox(self,"__paddingchkbox")

        Create_DevicesLayout.addWidget(self.__paddingchkbox,8,0)

        self.__paddingspinbox = QSpinBox(self,"__paddingspinbox")
        self.__paddingspinbox.setEnabled(0)

        Create_DevicesLayout.addMultiCellWidget(self.__paddingspinbox,8,8,1,3)

        self.textLabel2_4_2 = QLabel(self,"textLabel2_4_2")

        Create_DevicesLayout.addMultiCellWidget(self.textLabel2_4_2,8,8,4,5)
        spacer1 = QSpacerItem(310,20,QSizePolicy.Expanding,QSizePolicy.Minimum)
        Create_DevicesLayout.addMultiCell(spacer1,14,14,0,8)

        self.__okbutton = QPushButton(self,"__okbutton")

        Create_DevicesLayout.addWidget(self.__okbutton,14,9)

        self.__cancelbutton = QPushButton(self,"__cancelbutton")

        Create_DevicesLayout.addWidget(self.__cancelbutton,14,10)

        self.__portbutton = QPushButton(self,"__portbutton")

        Create_DevicesLayout.addMultiCellWidget(self.__portbutton,13,13,8,10)

        self.__commentstxtbox = QTextEdit(self,"__commentstxtbox")

        Create_DevicesLayout.addMultiCellWidget(self.__commentstxtbox,10,12,8,10)
        spacer4 = QSpacerItem(20,20,QSizePolicy.Fixed,QSizePolicy.Minimum)
        Create_DevicesLayout.addItem(spacer4,8,7)

        self.__locationtxtbox = QLineEdit(self,"__locationtxtbox")

        Create_DevicesLayout.addMultiCellWidget(self.__locationtxtbox,7,7,8,10)

        layout9 = QVBoxLayout(None,0,6,"layout9")

        self.textLabel2 = QLabel(self,"textLabel2")
        layout9.addWidget(self.textLabel2)

        self.__systemnametxtbox = QLineEdit(self,"__systemnametxtbox")
        self.__systemnametxtbox.setEnabled(0)
        layout9.addWidget(self.__systemnametxtbox)

        Create_DevicesLayout.addMultiCellLayout(layout9,0,1,0,0)

        layout7 = QVBoxLayout(None,0,6,"layout7")

        self.textLabel2_4 = QLabel(self,"textLabel2_4")
        layout7.addWidget(self.textLabel2_4)

        self.__prefixtxtbox = QLineEdit(self,"__prefixtxtbox")
        layout7.addWidget(self.__prefixtxtbox)

        Create_DevicesLayout.addMultiCellLayout(layout7,6,7,0,6)

        self.__chk_node = QCheckBox(self,"__chk_node")

        Create_DevicesLayout.addMultiCellWidget(self.__chk_node,12,12,0,4)

        layout8 = QVBoxLayout(None,0,6,"layout8")

        self.textLabel2_3 = QLabel(self,"textLabel2_3")
        layout8.addWidget(self.textLabel2_3)

        self.__devicetypetxtbox = QComboBox(0,self,"__devicetypetxtbox")
        layout8.addWidget(self.__devicetypetxtbox)

        Create_DevicesLayout.addMultiCellLayout(layout8,4,5,0,6)

        layout7_2_2 = QVBoxLayout(None,0,6,"layout7_2_2")

        self.textLabel2_4_4_2 = QLabel(self,"textLabel2_4_4_2")
        layout7_2_2.addWidget(self.textLabel2_4_4_2)

        self.__devicetxtbox = QLineEdit(self,"__devicetxtbox")
        layout7_2_2.addWidget(self.__devicetxtbox)

        Create_DevicesLayout.addMultiCellLayout(layout7_2_2,2,3,0,2)

        layout7_2 = QVBoxLayout(None,0,6,"layout7_2")

        self.textLabel2_4_4 = QLabel(self,"textLabel2_4_4")
        layout7_2.addWidget(self.textLabel2_4_4)

        self.__deviceidtxtbox = QLineEdit(self,"__deviceidtxtbox")
        layout7_2.addWidget(self.__deviceidtxtbox)

        Create_DevicesLayout.addMultiCellLayout(layout7_2,2,3,3,6)

        self.languageChange()

        self.resize(QSize(579,461).expandedTo(self.minimumSizeHint()))
        self.clearWState(Qt.WState_Polished)


    def languageChange(self):
        self.setCaption(self.__tr("Create Device(s)"))
        self.textLabel2_4_3.setText(self.__tr("Serial Number:"))
        self.textLabel2_4_3_2.setText(self.__tr("Hardware type:"))
        self.textLabel2_4_3_2_2_2.setText(self.__tr("Location:"))
        self.textLabel2_4_3_2_2.setText(self.__tr("Responsible:"))
        self.textLabel2_4_3_2_2_2_2.setText(self.__tr("Comments:"))
        self.textLabel2_3_2.setText(self.__tr("List of Functions :"))
        self.textLabel2_2.setText(self.__tr("Add to subsystem:"))
        self.__delbutton.setText(self.__tr("-"))
        self.__addbutton.setText(self.__tr("+"))
        self.__chk_prommode.setText(self.__tr("Promiscuous mode"))
        self.textLabel2_4_2_2.setText(self.__tr("Start nr:"))
        self.textLabel2_4_2_2_2.setText(self.__tr("Total:"))
        self.__paddingchkbox.setText(self.__tr("Pad with 0s?:"))
        self.textLabel2_4_2.setText(self.__tr("digits max."))
        self.__okbutton.setText(self.__tr("Ok"))
        self.__cancelbutton.setText(self.__tr("Cancel"))
        self.__portbutton.setText(self.__tr("Create/Modify ports ..."))
        self.textLabel2.setText(self.__tr("Create in:"))
        self.textLabel2_4.setText(self.__tr("Prefix: (%d - where to place nr)"))
        self.__chk_node.setText(self.__tr("This is a node (starts or ends a path)"))
        self.textLabel2_3.setText(self.__tr("Device Type:"))
        self.textLabel2_4_4_2.setText(self.__tr("Device Name:"))
        self.textLabel2_4_4.setText(self.__tr("Device ID:"))


    def __tr(self,s,c = None):
        return qApp.translate("Create_Devices",s,c)
