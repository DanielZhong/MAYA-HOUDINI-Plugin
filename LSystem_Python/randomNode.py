import sys
import random
import maya.OpenMaya as OpenMaya
import maya.OpenMayaMPx as OpenMayaMPx
import LSystem
import maya.cmds as cmds
import maya.mel as mel
import os

kPluginNodeTypeName = "randomNode"
randomNodeId = OpenMaya.MTypeId(0x8704)

def MAKE_INPUT(attr):
    attr.setKeyable(True)
    attr.setStorable(True)
    attr.setReadable(True)
    attr.setWritable(True)

def MAKE_OUTPUT(attr):
    attr.setKeyable(False)
    attr.setStorable(False)
    attr.setReadable(True)
    attr.setWritable(False)

class randomNode(OpenMayaMPx.MPxNode):
    inNumPoints = OpenMaya.MObject()
    minVector = OpenMaya.MObject()
    maxVector = OpenMaya.MObject()
    outPoints = OpenMaya.MObject()

    def __init__(self):
        OpenMayaMPx.MPxNode.__init__(self)

    def compute(self, plug, data):
        print("Compute random Node\n")
        if plug != randomNode.outPoints:
            return OpenMaya.kUnknownParameter

        numPoints = data.inputValue(randomNode.inNumPoints).asInt()
        minVector = data.inputValue(randomNode.minVector).asFloat3()
        maxVector = data.inputValue(randomNode.maxVector).asFloat3()

        pointsData = data.outputValue(randomNode.outPoints)
        pointsAAD = OpenMaya.MFnArrayAttrsData()
        pointsObject = pointsAAD.create()
        positionArray = pointsAAD.vectorArray("position")
        idArray = pointsAAD.doubleArray("id")

        for i in range(numPoints):
            positionArray.append(OpenMaya.MVector(random.uniform(minVector[0], maxVector[0]), random.uniform(minVector[1], maxVector[1]), random.uniform(minVector[2], maxVector[2])))
            idArray.append(i)

        pointsData.setMObject(pointsObject)
        data.setClean(plug)

def nodeInitializer():
    nAttr = OpenMaya.MFnNumericAttribute()
    tAttr = OpenMaya.MFnTypedAttribute()
    cAttr = OpenMaya.MFnCompoundAttribute()

    randomNode.inNumPoints = nAttr.create("numPoints", "np", OpenMaya.MFnNumericData.kInt, 10)
    MAKE_INPUT(nAttr)

    randomNode.minVector = cAttr.create("minVector", "minV")
    minX = nAttr.create("minX", "minX", OpenMaya.MFnNumericData.kFloat, -1.0)
    minY = nAttr.create("minY", "minY", OpenMaya.MFnNumericData.kFloat, -1.0)
    minZ = nAttr.create("minZ", "minZ", OpenMaya.MFnNumericData.kFloat, -1.0)
    cAttr.addChild(minX)
    cAttr.addChild(minY)
    cAttr.addChild(minZ)
    MAKE_INPUT(cAttr)

    randomNode.maxVector = cAttr.create("maxVector", "maxV")
    maxX = nAttr.create("maxX", "maxX", OpenMaya.MFnNumericData.kFloat, 1.0)
    maxY = nAttr.create("maxY", "maxY", OpenMaya.MFnNumericData.kFloat, 1.0)
    maxZ = nAttr.create("maxZ", "maxZ", OpenMaya.MFnNumericData.kFloat, 1.0)
    cAttr.addChild(maxX)
    cAttr.addChild(maxY)
    cAttr.addChild(maxZ)
    MAKE_INPUT(cAttr)

    randomNode.outPoints = tAttr.create("outPoints", "op", OpenMaya.MFnArrayAttrsData.kDynArrayAttrs)
    MAKE_OUTPUT(tAttr)

    randomNode.addAttribute(randomNode.inNumPoints)
    randomNode.addAttribute(randomNode.minVector)
    randomNode.addAttribute(randomNode.maxVector)
    randomNode.addAttribute(randomNode.outPoints)

    randomNode.attributeAffects(randomNode.inNumPoints, randomNode.outPoints)
    randomNode.attributeAffects(randomNode.minVector, randomNode.outPoints)
    randomNode.attributeAffects(randomNode.maxVector, randomNode.outPoints)

def nodeCreator():
    return OpenMayaMPx.asMPxPtr(randomNode())


# ----------------------------------------------------------
kPluginInstanceNodeTypeName = "LSystemInstanceNode"

instanceNodeId = OpenMaya.MTypeId(0x8705)

class LSystemInstanceNode(OpenMayaMPx.MPxNode):
    angle = OpenMaya.MObject()
    stepSize = OpenMaya.MObject()
    grammar = OpenMaya.MObject()
    iterations = OpenMaya.MObject()
    outBranch = OpenMaya.MObject()
    outFlower = OpenMaya.MObject()
    
    def __init__(self):
        OpenMayaMPx.MPxNode.__init__(self)
        
        
    def compute(self, plug, data):
        if plug not in [LSystemInstanceNode.outBranch, LSystemInstanceNode.outFlower]:
            return OpenMaya.kUnknownParameter

        angleValue = data.inputValue(LSystemInstanceNode.angle).asDouble()
        stepSizeValue = data.inputValue(LSystemInstanceNode.stepSize).asDouble()
        grammarValue = data.inputValue(LSystemInstanceNode.grammar).asString()
        iterationsValue = data.inputValue(LSystemInstanceNode.iterations).asInt()

        if not os.path.exists(grammarValue):
            sys.stderr.write("Grammar file does not exist\n")

        outBranchesAAD = OpenMaya.MFnArrayAttrsData()
        outFlowersAAD = OpenMaya.MFnArrayAttrsData()
        outBranches = outBranchesAAD.create()
        outFlowers = outFlowersAAD.create()

        system = LSystem.LSystem()
        system.loadProgram(str(grammarValue))
        system.setDefaultAngle(angleValue)
        system.setDefaultStep(stepSizeValue)
        branches = LSystem.VectorPyBranch()
        flowers = LSystem.VectorPyBranch()
        for i in range(iterationsValue):
            system.processPy(i, branches, flowers)

        positionArrayBranch = outBranchesAAD.vectorArray("position")
        idArrayBranch = outBranchesAAD.doubleArray("id")
        scaleArrayBranch = outBranchesAAD.vectorArray("scale")
        aimDirArrayBranch = outBranchesAAD.vectorArray("aimDirection")
        for i, branch in enumerate(branches):
            endPos = OpenMaya.MVector(branch[3], branch[5], branch[4])
            dir = endPos - OpenMaya.MVector(branch[0], branch[2], branch[1])
            positionArrayBranch.append(endPos)
            idArrayBranch.append(i)
            scaleArrayBranch.append(OpenMaya.MVector(1, 1, 1))
            aimDirArrayBranch.append(dir)

        positionArrayFlower = outFlowersAAD.vectorArray("position")
        idArrayFlower = outFlowersAAD.doubleArray("id")
        scaleArrayFlower = outFlowersAAD.vectorArray("scale")
        aimDirArrayFlower = outFlowersAAD.vectorArray("aimDirection")
        for i, flower in enumerate(flowers):
            pos = OpenMaya.MVector(flower[0], flower[2], flower[1])
            positionArrayFlower.append(pos)
            idArrayFlower.append(i)
            scaleArrayFlower.append(OpenMaya.MVector(1, 1, 1))
            aimDirArrayFlower.append(OpenMaya.MVector(1, 1, 1))

        data.outputValue(LSystemInstanceNode.outBranch).setMObject(outBranches)
        data.outputValue(LSystemInstanceNode.outFlower).setMObject(outFlowers)
        data.setClean(plug)


def instanceNodeInitializer():
    nAttr = OpenMaya.MFnNumericAttribute()
    tAttr = OpenMaya.MFnTypedAttribute()
    
    LSystemInstanceNode.angle = nAttr.create("angle", "a", OpenMaya.MFnNumericData.kDouble, 90.0)
    MAKE_INPUT(nAttr)
    
    LSystemInstanceNode.stepSize = nAttr.create("stepSize", "ss", OpenMaya.MFnNumericData.kDouble, 1.0)
    MAKE_INPUT(nAttr)
    
    LSystemInstanceNode.grammar = tAttr.create("grammar", "gf", OpenMaya.MFnData.kString)
    tAttr.setUsedAsFilename(True) 
    MAKE_INPUT(tAttr)
    
    LSystemInstanceNode.iterations = nAttr.create("iterations", "it", OpenMaya.MFnNumericData.kInt, 1)
    MAKE_INPUT(nAttr)
    
    LSystemInstanceNode.outBranch = tAttr.create("outBranch", "ob", OpenMaya.MFnArrayAttrsData.kDynArrayAttrs)
    MAKE_OUTPUT(tAttr)
    
    LSystemInstanceNode.outFlower = tAttr.create("outFlower", "of", OpenMaya.MFnArrayAttrsData.kDynArrayAttrs)
    MAKE_OUTPUT(tAttr)
    
    LSystemInstanceNode.addAttribute(LSystemInstanceNode.angle)
    LSystemInstanceNode.addAttribute(LSystemInstanceNode.stepSize)
    LSystemInstanceNode.addAttribute(LSystemInstanceNode.grammar)
    LSystemInstanceNode.addAttribute(LSystemInstanceNode.iterations)
    LSystemInstanceNode.addAttribute(LSystemInstanceNode.outBranch)
    LSystemInstanceNode.addAttribute(LSystemInstanceNode.outFlower)
    
    LSystemInstanceNode.attributeAffects(LSystemInstanceNode.angle, LSystemInstanceNode.outBranch)
    LSystemInstanceNode.attributeAffects(LSystemInstanceNode.angle, LSystemInstanceNode.outFlower)
    LSystemInstanceNode.attributeAffects(LSystemInstanceNode.stepSize, LSystemInstanceNode.outBranch)
    LSystemInstanceNode.attributeAffects(LSystemInstanceNode.stepSize, LSystemInstanceNode.outFlower)
    LSystemInstanceNode.attributeAffects(LSystemInstanceNode.grammar, LSystemInstanceNode.outBranch)
    LSystemInstanceNode.attributeAffects(LSystemInstanceNode.grammar, LSystemInstanceNode.outFlower)
    LSystemInstanceNode.attributeAffects(LSystemInstanceNode.iterations, LSystemInstanceNode.outBranch)
    LSystemInstanceNode.attributeAffects(LSystemInstanceNode.iterations, LSystemInstanceNode.outFlower)

def instanceNodeCreator():
    return OpenMayaMPx.asMPxPtr(LSystemInstanceNode())

def initializePlugin(mobject):
    mplugin = OpenMayaMPx.MFnPlugin(mobject)

    try:
        mplugin.registerNode(kPluginNodeTypeName, randomNodeId, nodeCreator, nodeInitializer)
    except:
        sys.stderr.write(f"Failed to register node: {kPluginNodeTypeName}\n")
        raise

    try:
        mplugin.registerNode(kPluginInstanceNodeTypeName, instanceNodeId, instanceNodeCreator, instanceNodeInitializer)
    except:
        sys.stderr.write(f"Failed to register node: {kPluginInstanceNodeTypeName}\n")
        raise

    OpenMaya.MGlobal.executeCommand("source \"" + mplugin.loadPath() + "/menu.mel\"")

def uninitializePlugin(mobject):
    mplugin = OpenMayaMPx.MFnPlugin(mobject)
    try:
        mplugin.deregisterNode(randomNodeId)
    except:
        sys.stderr.write(f"Failed to unregister node: {kPluginNodeTypeName}\n")

    try:
        mplugin.deregisterNode(instanceNodeId)
    except:
        sys.stderr.write(f"Failed to unregister node: {kPluginInstanceNodeTypeName}\n")

    