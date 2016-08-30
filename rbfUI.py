### TEST UI
import functools
import maya.cmds as mc
import pymel.core as pm

from PySide import QtCore as qc
from PySide import QtGui as qg
#from PySide.QtCore import Slot
from shiboken import wrapInstance

import maya.OpenMaya as om
import maya.OpenMayaUI as omui

def maya_main_window():
    '''
    Return the Maya main window as a Python object
    '''
    main_window_ptr = omui.MQtUtil.mainWindow()

class TestUI(qg.QDialog):
    
    def __init__(self, parent = maya_main_window()):
        super(TestUI, self).__init__(parent)
        self.pose = []
        self.targets = []
        
    def create_ui(self):
        mc.undoInfo(openChunk=True)
        
        self.setWindowTitle('RBF')
        self.setMinimumSize(500,600)
        self.setMaximumWidth(500)
        
        self.create_elements()
        
        mc.undoInfo(closeChunk=True)
        
    def create_elements(self):
        ### Create tabs and top level layouts
        tab_widget = qg.QTabWidget()
        create_tab_page = qg.QWidget()
        edit_tab_page = qg.QWidget()
        header_lo = qg.QVBoxLayout()
        create_lo = qg.QVBoxLayout(create_tab_page)
        edit_lo = qg.QVBoxLayout(edit_tab_page)
        tab_widget.addTab(create_tab_page, 'Create')
        tab_widget.addTab(edit_tab_page, 'Edit')
        main_lo = qg.QVBoxLayout()
        
        ###### Create header ######################################################################
        header_lbl = qg.QLabel('RBF Network Builder')
        header_lbl.setStyleSheet('font-size: 16pt' )
        credit_lbl = qg.QLabel('by James Sumner III')
        credit_lbl.setStyleSheet('color: rgb(140,140,140)')
        ##website_lbl = qg.QLabel('<a href="http://jamessumneriii.com/">www.jamessumneriii.com</a>')
        website_lbl = qg.QLabel('www.jamessumneriii.com')
        website_lbl.setStyleSheet('color: rgb(140,140,140); link-decoration: none;')
        header_lo.addWidget(header_lbl)
        header_lo.addWidget(credit_lbl)
        header_lo.addWidget(website_lbl)
        
       
        ###### Create tab page ######################################################################
        ### Pose add
        addPose_btn = qg.QPushButton('Add Pose')
        removePose_btn = qg.QPushButton('Remove Pose')
        self.pose_lv = qg.QListView()
        
        addPose_btn.setMaximumWidth(100)
        removePose_btn.setMaximumWidth(100)
        self.pose_lv.setMaximumWidth(250)
        self.pose_lv.setMaximumHeight(20)
        
        pose_lo = qg.QHBoxLayout()
        pose_lo.addWidget(removePose_btn)
        pose_lo.addWidget(addPose_btn)
        pose_lo.addWidget(self.pose_lv)
        
        ### Target add
        addTarget_btn = qg.QPushButton('Add Target')
        removeTarget_btn = qg.QPushButton('Remove Target')
        self.targets_lv = qg.QListView()
        
        addTarget_btn.setMaximumWidth(100)
        removeTarget_btn.setMaximumWidth(100)
        self.targets_lv.setMaximumWidth(250)
                
        target_btn_lo = qg.QHBoxLayout()
        targets_lv_lo = qg.QHBoxLayout()
        target_lo = qg.QHBoxLayout()
        target_btn_lo.addWidget(removeTarget_btn)
        target_btn_lo.addWidget(addTarget_btn)
        target_btn_lo.setAlignment(qc.Qt.AlignTop)
        targets_lv_lo.addWidget(self.targets_lv)
        target_lo.addLayout(target_btn_lo)
        target_lo.addLayout(targets_lv_lo)
        
        ### Checkbox and radio button add
        self.matrix_chk = qg.QCheckBox('Connect Matrix')
        self.world_rad = qg.QRadioButton('World matrix')
        self.local_world = qg.QRadioButton('Local matrix')
        self.matrix_chk.setChecked(True)
        self.world_rad.setChecked(True)
        rad_grp = qg.QButtonGroup()
        rad_grp.addButton(self.world_rad)
        rad_grp.addButton(self.local_world)
        
        matrix_lo = qg.QHBoxLayout()
        matrix_lo.addWidget(self.matrix_chk)
        matrix_lo.addWidget(self.world_rad)
        matrix_lo.addWidget(self.local_world)
        self.matrix_chk.setMinimumWidth(200)

        self.rgb_chk = qg.QCheckBox('Connect RGB Values from Material')
        self.rgb_chk.setChecked(True)
        rgb_lo = qg.QHBoxLayout()
        rgb_lo.addWidget(self.rgb_chk)
        
        self.alpha_chk = qg.QCheckBox('Connect Alpha Values from Material')
        self.alpha_chk.setChecked(True)
        alpha_lo = qg.QHBoxLayout()
        alpha_lo.addWidget(self.alpha_chk)
        
        self.alias_chk = qg.QCheckBox('Use Aliases for Targets on RBF Node')
        self.alias_chk.setChecked(True)
        alias_lo = qg.QHBoxLayout()
        alias_lo.addWidget(self.alias_chk)
        
        self.sigma_chk = qg.QCheckBox('Create Spheres to Visualize Falloff  (most accurate for Gaussian)')
        self.sigma_chk.setChecked(True)
        sigma_lo = qg.QHBoxLayout()
        sigma_lo.addWidget(self.sigma_chk)
        
        checks_lo = qg.QVBoxLayout()
        checks_lo.addLayout(matrix_lo)
        checks_lo.addLayout(rgb_lo)
        checks_lo.addLayout(alpha_lo)
        checks_lo.addLayout(alias_lo)
        checks_lo.addLayout(sigma_lo)
        
        ### Bottom buttons
        close_btn = qg.QPushButton('Close')
        close_btn.setMaximumWidth(200)
        buildRBF_btn = qg.QPushButton('Build RBF Network')
        buildRBF_btn.setMaximumWidth(250)
        
        button_lo = qg.QHBoxLayout()
        button_lo.addWidget(close_btn)
        button_lo.addWidget(buildRBF_btn)
        
        ### Add layouts to tab pages
        create_lo.addLayout(pose_lo)
        create_lo.addLayout(target_lo)
        create_lo.addLayout(checks_lo)
        create_lo.addLayout(button_lo)
        
        pose_lo.setContentsMargins(0,0,0,10)
        target_lo.setContentsMargins(0,0,0,10)
        
        ### Add tabs to main layout
        main_lo.addLayout(header_lo)
        main_lo.addWidget(tab_widget)
        self.setLayout(main_lo)
        main_lo.setContentsMargins(10,10,10,10)
        
        ### Create connections
        addPose_btn.clicked.connect(self.add_pose)
        removePose_btn.clicked.connect(self.remove_pose)
        
        addTarget_btn.clicked.connect(self.add_targets)
        removeTarget_btn.clicked.connect(self.remove_targets)
        
        close_btn.clicked.connect(self.close_window)
        buildRBF_btn.clicked.connect(self.build_rbf)
       
    ### SLOT FUNCTIONS
    #@Slot(qc.QModelIndex)
    def add_pose(self):
        sel = pm.ls(sl=1)
        if len(sel) == 0:
            print 'Error: must select pose to add'
            return
        elif len(sel) > 1:
            print 'Error: only one pose can connect to the RBF node'
            return
        model = qg.QStandardItemModel(self.pose_lv)
        item = qg.QStandardItem(sel[0].name())
        model.appendRow(item)
        self.pose_lv.setModel(model)
        self.pose = [sel]

    def remove_pose(self):
        model = qg.QStandardItemModel(self.pose_lv)
        model.removeRow(0)
        self.pose_lv.setModel(model)
        self.pose = []
        
    def add_targets(self):
        list = pm.ls(sl=1)
        list += self.targets
        self.targets_mod = qg.QStandardItemModel(self.targets_lv)
        
        for i in range(0, len(list)):
            item = qg.QStandardItem(list[i].name())
            self.targets_mod.appendRow(item)
        self.targets_lv.setModel(self.targets_mod)
        
        self.targets = list
            
    def remove_targets(self):
        idxs = self.targets_lv.selectedIndexes()
        for idx in idxs:
            self.targets_mod.removeRow(idx.row())
        
    def close_window(self):
        self.close()
        
    def build_rbf(self):
        try:
            pm.loadPlugin('jsRadial.mll')
        except:
            pm.error('ERROR: jsRadial.mll not loaded.')
        
        pm.undoInfo(openChunk=True)
        rbf = pm.createNode('jsRadPose')
        
        # Notify if RBF is made with no connections
        if len(self.pose) == 0 and len(self.targets) == 0:
            print "RBF node made with no connections"
        
        # Only create one sigma blinn material
        if self.sigma_chk.isChecked():
            self.sigma_shader = pm.shadingNode("lambert", asShader=True, name="sigmaLmbt")
            pmc.setAttr(self.sigma_shader.color.colorR, 0.0)
            pmc.setAttr(self.sigma_shader.color.colorG, 0.5)
            pmc.setAttr(self.sigma_shader.color.colorB, 1.0)
            pmc.setAttr(self.sigma_shader.transparency, (0.5, 0.5, 0.5))
            self.sigma_sg = pm.sets(renderable=1, noSurfaceShader=1, empty=1, n="sigmaLmbt_SG")
            pm.connectAttr(self.sigma_shader.outColor, self.sigma_sg.surfaceShader)
            self.falloff_group = pm.group(empty=1, n="GRP_rbf_falloff")
            
        # Connect pose matrix
        if len(self.pose) == 1:
            if self.matrix_chk.isChecked() == True:
                if self.world_rad.isChecked() == True:
                    pm.connectAttr(self.pose[0][0].worldMatrix[0], rbf.poseMatrix)
                else:
                    pm.connectAttr(self.pose[0][0].parentMatrix[0], rbf.poseMatrix)
        
            # Connect pose color
            if self.rgb_chk.isChecked() == True or self.alpha_chk.isChecked() == True:
                shape = pm.listRelatives(self.pose[0], shapes=1)[0]
                shader_grp = pm.listConnections(shape, type='shadingEngine')
                shader = pm.listConnections(shader_grp[0], d=0, s=1)[0]
                if self.rgb_chk.isChecked():
                    try:
                        pm.connectAttr(shader.color, rbf.poseColor)
                    except:
                        try:
                            pm.connectAttr(shader.outColor, rbf.poseColor)
                        except:
                            pass
                if self.alpha_chk.isChecked():
                    try:
                        pm.connectAttr(shader.transparency, rbf.poseTransparency)
                    except:
                        try:
                            pm.connectAttr(shader.outTransparency, rbf.poseTransparency)
                        except:
                            pass

        
        # Check target list for pose and duplicates
        if len(self.pose) == 1:
            single_set = set(self.targets)
            self.targets = list(single_set)
            if self.pose[0][0] in self.targets:
                print "Pose " + self.pose[0][0] + " was removed from target list but kept as input pose."
                self.targets.remove(self.pose[0][0])

        # Connect pose matrices
        for i in range(0, len(self.targets)):
            if self.matrix_chk.isChecked() == True:
                if self.world_rad.isChecked() == True:
                    pm.connectAttr(self.targets[i].worldMatrix[0], rbf.target[i].targetMatrix)
                else:
                    pm.connectAttr(self.targets[i].parentMatrix[0], rbf.target[i].targetMatrix)
            
            if self.rgb_chk.isChecked() == True or self.alpha_chk.isChecked() == True:
                shape = pm.listRelatives(self.targets[i], shapes=1)[0]
                shader_grp = pm.listConnections(shape, type='shadingEngine')
                shader = pm.listConnections(shader_grp[0], d=0, s=1)[0]
                if self.rgb_chk.isChecked() == True:
                    try:
                        pm.connectAttr(shader.color, rbf.target[i].targetColor)
                    except:
                        try:
                            pm.connectAttr(shader.outColor, rbf.target[i].targetColor)
                        except:
                            pass
                if self.alpha_chk.isChecked() == True:
                    try:
                        pm.connectAttr(shader.transparency, rbf.target[i].targetTransparency)
                    except:
                        try:
                            pm.connectAttr(shader.outTransparency, rbf.target[i].targetTransparency)
                        except:
                            pass
            
            # Build alias attrs on RBF node
            if self.alias_chk.isChecked():
                alias = self.targets[i]
                if '|' in alias:
                    alias = alias.replace('|','_')
                pm.aliasAttr(alias, rbf.target[i])
                outAlias = alias + 'Out'
                sigmaAlias = alias + 'Falloff'
                pm.aliasAttr(outAlias, rbf.outputInterpolate[i])
                pm.aliasAttr(sigmaAlias, rbf.outputSigma[i])
                pm.getAttr(rbf.outputInterpolate[i])
                pm.getAttr(rbf.outputSigma[i])
        
            # Build falloff spheres
            if self.sigma_chk.isChecked():
                bb = pm.xform(self.targets[i], q=1, bb=1)
                scale = bb[3] - bb[0]
                print "radius =="
                print scale/2
                ### hard coding sphere radius from trial and error
                sphere = pm.polySphere(r=5.0, sx=20, sy=20, ax=(0,1,0), ch=0, n=str(self.targets[i]) + '_falloff')
                sphereShp = pm.listRelatives(sphere, s=1)
                pcon = pm.parentConstraint(self.targets[i], sphere, mo=0)
                pm.sets(self.sigma_sg, e=1, forceElement = sphereShp)
                
                mult = pm.createNode('multiplyDivide', n='MULT_' + self.targets[i] + '_falloff')
                pm.setAttr(mult.input1X, 0.5)
                pm.setAttr(mult.operation, 2)
                pm.connectAttr(rbf.outputSigma[i], mult.input2X)
                
                pm.connectAttr(mult.outputX, sphere[0].scaleX)
                pm.connectAttr(mult.outputX, sphere[0].scaleY)
                pm.connectAttr(mult.outputX, sphere[0].scaleZ)
                
                pm.parent(sphere, self.falloff_group)
                    
        pm.undoInfo(closeChunk=True)
        self.close()
        

### LAUNCH UI
if __name__ == '__main__':
    try:
        TestUI.deleteLater()
    except:
        pass
    
    test_ui = TestUI()
    
    try:
        test_ui.create_ui()
        test_ui.show()
    except:
        test_ui.deleteLater()