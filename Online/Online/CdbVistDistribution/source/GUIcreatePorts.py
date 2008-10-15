# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'createport.ui'
#
# Created: Wed Aug 1 09:48:04 2007
#      by: The PyQt User Interface Compiler (pyuic) 3.13
#
# WARNING! All changes made in this file will be lost!


from qt import *
from qttable import QTable


class GUIcreatePort(QDialog):
    def __init__(self,parent = None,name = None,modal = 0,fl = 0):
        QDialog.__init__(self,parent,name,modal,fl)

        if not name:
            self.setName("GUIcreatePort")


        GUIcreatePortLayout = QGridLayout(self,1,1,11,6,"GUIcreatePortLayout")

        self.cancelbutton = QPushButton(self,"cancelbutton")

        GUIcreatePortLayout.addWidget(self.cancelbutton,4,4)

        self.okbutton = QPushButton(self,"okbutton")

        GUIcreatePortLayout.addWidget(self.okbutton,4,3)
        spacer2 = QSpacerItem(421,20,QSizePolicy.Expanding,QSizePolicy.Minimum)
        GUIcreatePortLayout.addMultiCell(spacer2,4,4,0,2)

        self.table = QTable(self,"table")
        self.table.setNumCols(self.table.numCols() + 1)
        self.table.horizontalHeader().setLabel(self.table.numCols() - 1,self.__tr("Device Name"))
        self.table.setNumCols(self.table.numCols() + 1)
        self.table.horizontalHeader().setLabel(self.table.numCols() - 1,self.__tr("Port Nr"))
        self.table.setNumCols(self.table.numCols() + 1)
        self.table.horizontalHeader().setLabel(self.table.numCols() - 1,self.__tr("Port Type"))
        self.table.setNumCols(self.table.numCols() + 1)
        self.table.horizontalHeader().setLabel(self.table.numCols() - 1,self.__tr("Port Way"))
        self.table.setNumCols(self.table.numCols() + 1)
        self.table.horizontalHeader().setLabel(self.table.numCols() - 1,self.__tr("Port Speed"))
        self.table.setNumCols(self.table.numCols() + 1)
        self.table.horizontalHeader().setLabel(self.table.numCols() - 1,self.__tr("PXI?"))
        self.table.setNumCols(self.table.numCols() + 1)
        self.table.horizontalHeader().setLabel(self.table.numCols() - 1,self.__tr("OK?"))
        self.table.setNumCols(self.table.numCols() + 1)
        self.table.horizontalHeader().setLabel(self.table.numCols() - 1,self.__tr("Phy"))
        self.table.setNumCols(self.table.numCols() + 1)
        self.table.horizontalHeader().setLabel(self.table.numCols() - 1,self.__tr("MAC Adress"))
        self.table.setNumCols(self.table.numCols() + 1)
        self.table.horizontalHeader().setLabel(self.table.numCols() - 1,self.__tr("IP Adress"))
        self.table.setNumCols(self.table.numCols() + 1)
        self.table.horizontalHeader().setLabel(self.table.numCols() - 1,self.__tr("Sub net"))
        self.table.setNumCols(self.table.numCols() + 1)
        self.table.horizontalHeader().setLabel(self.table.numCols() - 1,self.__tr("IP Name"))
        self.table.setNumCols(self.table.numCols() + 1)
        self.table.horizontalHeader().setLabel(self.table.numCols() - 1,self.__tr("Bia"))
        self.table.setNumCols(self.table.numCols() + 1)
        self.table.horizontalHeader().setLabel(self.table.numCols() - 1,self.__tr("PortID"))
        self.table.setNumCols(self.table.numCols() + 1)
        self.table.horizontalHeader().setLabel(self.table.numCols() - 1,self.__tr("Modified"))
        self.table.setNumCols(self.table.numCols() + 1)
        self.table.horizontalHeader().setLabel(self.table.numCols() - 1,self.__tr("Modify parameters"))
        self.table.setNumCols(self.table.numCols() + 1)
        self.table.horizontalHeader().setLabel(self.table.numCols() - 1,self.__tr("Old IP"))
        self.table.setNumCols(self.table.numCols() + 1)
        self.table.horizontalHeader().setLabel(self.table.numCols() - 1,self.__tr("Save Satus"))
        self.table.setNumRows(0)
        self.table.setNumCols(18)

        GUIcreatePortLayout.addMultiCellWidget(self.table,3,3,0,4)

        self.removebutton = QPushButton(self,"removebutton")

        GUIcreatePortLayout.addWidget(self.removebutton,2,0)
        spacer1 = QSpacerItem(351,20,QSizePolicy.Expanding,QSizePolicy.Minimum)
        GUIcreatePortLayout.addMultiCell(spacer1,2,2,1,3)

        self.addportbutton = QPushButton(self,"addportbutton")

        GUIcreatePortLayout.addMultiCellWidget(self.addportbutton,1,1,0,4)

        self.buttonGroup3 = QButtonGroup(self,"buttonGroup3")

        self.textLabel2 = QLabel(self.buttonGroup3,"textLabel2")
        self.textLabel2.setGeometry(QRect(11,11,126,16))

        self.textLabel2_2_2 = QLabel(self.buttonGroup3,"textLabel2_2_2")
        self.textLabel2_2_2.setGeometry(QRect(10,60,156,16))

        self.porttype = QLineEdit(self.buttonGroup3,"porttype")
        self.porttype.setGeometry(QRect(10,80,150,22))
        self.porttype.setMaximumSize(QSize(200,32767))

        self.textLabel2_2_3 = QLabel(self.buttonGroup3,"textLabel2_2_3")
        self.textLabel2_2_3.setGeometry(QRect(100,110,21,16))

        self.textLabel2_2_3_2 = QLabel(self.buttonGroup3,"textLabel2_2_3_2")
        self.textLabel2_2_3_2.setGeometry(QRect(10,110,56,16))

        self.textLabel2_2_2_2 = QLabel(self.buttonGroup3,"textLabel2_2_2_2")
        self.textLabel2_2_2_2.setGeometry(QRect(194,11,40,16))

        self.textLabel2_2 = QLabel(self.buttonGroup3,"textLabel2_2")
        self.textLabel2_2.setGeometry(QRect(190,60,80,16))

        self.admstatus = QCheckBox(self.buttonGroup3,"admstatus")
        self.admstatus.setGeometry(QRect(100,170,77,20))

        self.buttonGroup1 = QButtonGroup(self.buttonGroup3,"buttonGroup1")
        self.buttonGroup1.setGeometry(QRect(190,110,100,80))

        self.portwayOUT = QRadioButton(self.buttonGroup1,"portwayOUT")
        self.portwayOUT.setGeometry(QRect(30,40,40,20))

        self.portwayIN = QRadioButton(self.buttonGroup1,"portwayIN")
        self.portwayIN.setGeometry(QRect(30,20,30,20))

        self.portid = QLineEdit(self.buttonGroup3,"portid")
        self.portid.setGeometry(QRect(195,30,100,22))
        self.portid.setMaximumSize(QSize(200,32767))

        self.portnumber = QSpinBox(self.buttonGroup3,"portnumber")
        self.portnumber.setGeometry(QRect(189,79,90,22))
        self.portnumber.setMaxValue(10000)

        self.portspeed = QSpinBox(self.buttonGroup3,"portspeed")
        self.portspeed.setGeometry(QRect(10,130,80,22))
        self.portspeed.setMaximumSize(QSize(200,32767))
        self.portspeed.setMaxValue(100000)

        self.devicename = QLineEdit(self.buttonGroup3,"devicename")
        self.devicename.setGeometry(QRect(11,31,150,22))

        self.phy = QComboBox(0,self.buttonGroup3,"phy")
        self.phy.setGeometry(QRect(100,130,78,22))

        self.pxibooting = QCheckBox(self.buttonGroup3,"pxibooting")
        self.pxibooting.setGeometry(QRect(10,170,77,20))

        GUIcreatePortLayout.addMultiCellWidget(self.buttonGroup3,0,0,0,1)

        self.buttonGroup2 = QButtonGroup(self,"buttonGroup2")
        self.buttonGroup2.setSizePolicy(QSizePolicy(4,0,0,0,self.buttonGroup2.sizePolicy().hasHeightForWidth()))

        self.textLabel2_3_2_3_2 = QLabel(self.buttonGroup2,"textLabel2_3_2_3_2")
        self.textLabel2_3_2_3_2.setGeometry(QRect(10,140,50,16))

        self.textLabel2_3_2_3 = QLabel(self.buttonGroup2,"textLabel2_3_2_3")
        self.textLabel2_3_2_3.setGeometry(QRect(10,110,50,16))

        self.textLabel2_3_2_2 = QLabel(self.buttonGroup2,"textLabel2_3_2_2")
        self.textLabel2_3_2_2.setGeometry(QRect(10,80,80,16))

        self.textLabel2_3_2 = QLabel(self.buttonGroup2,"textLabel2_3_2")
        self.textLabel2_3_2.setGeometry(QRect(10,50,60,16))

        self.textLabel2_3 = QLabel(self.buttonGroup2,"textLabel2_3")
        self.textLabel2_3.setGeometry(QRect(10,20,70,16))

        self.macadress = QLineEdit(self.buttonGroup2,"macadress")
        self.macadress.setGeometry(QRect(90,20,180,22))

        self.ipadress = QLineEdit(self.buttonGroup2,"ipadress")
        self.ipadress.setGeometry(QRect(90,50,180,22))

        self.subnetadress = QLineEdit(self.buttonGroup2,"subnetadress")
        self.subnetadress.setGeometry(QRect(90,80,180,22))

        self.ipname = QLineEdit(self.buttonGroup2,"ipname")
        self.ipname.setGeometry(QRect(90,110,180,22))

        self.bia = QLineEdit(self.buttonGroup2,"bia")
        self.bia.setGeometry(QRect(90,140,180,22))

        GUIcreatePortLayout.addMultiCellWidget(self.buttonGroup2,0,0,2,4)

        self.languageChange()

        self.resize(QSize(645,548).expandedTo(self.minimumSizeHint()))
        self.clearWState(Qt.WState_Polished)

        self.setTabOrder(self.table,self.devicename)
        self.setTabOrder(self.devicename,self.portid)
        self.setTabOrder(self.portid,self.porttype)
        self.setTabOrder(self.porttype,self.portnumber)
        self.setTabOrder(self.portnumber,self.portspeed)
        self.setTabOrder(self.portspeed,self.phy)
        self.setTabOrder(self.phy,self.portwayIN)
        self.setTabOrder(self.portwayIN,self.portwayOUT)
        self.setTabOrder(self.portwayOUT,self.pxibooting)
        self.setTabOrder(self.pxibooting,self.admstatus)
        self.setTabOrder(self.admstatus,self.macadress)
        self.setTabOrder(self.macadress,self.ipadress)
        self.setTabOrder(self.ipadress,self.subnetadress)
        self.setTabOrder(self.subnetadress,self.ipname)
        self.setTabOrder(self.ipname,self.bia)
        self.setTabOrder(self.bia,self.addportbutton)
        self.setTabOrder(self.addportbutton,self.removebutton)
        self.setTabOrder(self.removebutton,self.okbutton)
        self.setTabOrder(self.okbutton,self.cancelbutton)


    def languageChange(self):
        self.setCaption(self.__tr("Create Port(s)"))
        self.cancelbutton.setText(self.__tr("Cancel"))
        self.okbutton.setText(self.__tr("Ok"))
        self.table.horizontalHeader().setLabel(0,self.__tr("Device Name"))
        self.table.horizontalHeader().setLabel(1,self.__tr("Port Nr"))
        self.table.horizontalHeader().setLabel(2,self.__tr("Port Type"))
        self.table.horizontalHeader().setLabel(3,self.__tr("Port Way"))
        self.table.horizontalHeader().setLabel(4,self.__tr("Port Speed"))
        self.table.horizontalHeader().setLabel(5,self.__tr("PXI?"))
        self.table.horizontalHeader().setLabel(6,self.__tr("OK?"))
        self.table.horizontalHeader().setLabel(7,self.__tr("Phy"))
        self.table.horizontalHeader().setLabel(8,self.__tr("MAC Adress"))
        self.table.horizontalHeader().setLabel(9,self.__tr("IP Adress"))
        self.table.horizontalHeader().setLabel(10,self.__tr("Sub net"))
        self.table.horizontalHeader().setLabel(11,self.__tr("IP Name"))
        self.table.horizontalHeader().setLabel(12,self.__tr("Bia"))
        self.table.horizontalHeader().setLabel(13,self.__tr("PortID"))
        self.table.horizontalHeader().setLabel(14,self.__tr("Modified"))
        self.table.horizontalHeader().setLabel(15,self.__tr("Modify parameters"))
        self.table.horizontalHeader().setLabel(16,self.__tr("Old IP"))
        self.table.horizontalHeader().setLabel(17,self.__tr("Save Satus"))
        self.removebutton.setText(self.__tr("Remove"))
        self.addportbutton.setText(self.__tr("Add Port"))
        self.buttonGroup3.setTitle(self.__tr("General :"))
        self.textLabel2.setText(self.__tr("Device Name(s):"))
        self.textLabel2_2_2.setText(self.__tr("Port Type(s):"))
        self.textLabel2_2_3.setText(self.__tr("Phy:"))
        self.textLabel2_2_3_2.setText(self.__tr("Port Speed:"))
        self.textLabel2_2_2_2.setText(self.__tr("Port ID:"))
        self.textLabel2_2.setText(self.__tr("Port Number(s):"))
        self.admstatus.setText(self.__tr("Adm Status"))
        self.buttonGroup1.setTitle(self.__tr("Port Way :"))
        self.portwayOUT.setText(self.__tr("Out"))
        self.portwayIN.setText(self.__tr("In"))
        self.pxibooting.setText(self.__tr("PXI Booting"))
        self.buttonGroup2.setTitle(self.__tr("Network :"))
        self.textLabel2_3_2_3_2.setText(self.__tr("Bia:"))
        self.textLabel2_3_2_3.setText(self.__tr("IP Name:"))
        self.textLabel2_3_2_2.setText(self.__tr("Subnet Adress:"))
        self.textLabel2_3_2.setText(self.__tr("IP Adress:"))
        self.textLabel2_3.setText(self.__tr("MAC Adress:"))


    def __tr(self,s,c = None):
        return qApp.translate("GUIcreatePort",s,c)
