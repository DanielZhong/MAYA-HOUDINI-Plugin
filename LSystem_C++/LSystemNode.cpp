#include "LSystemNode.h"
#include "LSystem.h"
#include "cylinder.h"
#include <maya/MFnMesh.h>
#include <maya/MGlobal.h>

#include <maya/MFnUnitAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnMeshData.h>

MStatus returnStatus;

MTypeId LSystemNode::id(0x00000123);
MObject LSystemNode::time;
MObject LSystemNode::angle;
MObject LSystemNode::stepSize;
MObject LSystemNode::grammar;
MObject LSystemNode::outputMesh;



// The creator() method simply returns new instances of this node. The return type is a void* so
// Maya can create node instances internally in a general fashion without having to know the return type.
void* LSystemNode::creator()
{
	return new LSystemNode;
}

// The initialize method is called only once when the node is first registered with Maya. In this method 
// you define the attributes of the node, what data comes in and goes out of the node that other nodes may want to connect to. 
MStatus LSystemNode::initialize() {
	MGlobal::displayInfo("test1");
	MFnNumericAttribute nAttr;
	MFnTypedAttribute tAttr;
	MFnUnitAttribute uAttr;

	MStatus returnStatus;
	// Angle
	angle = nAttr.create("angle", "angle", MFnNumericData::kDouble, 90.0);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	uAttr.setWritable(true);
	uAttr.setReadable(true);

	// StepSize
	stepSize = nAttr.create("stepSize", "stepSize", MFnNumericData::kDouble, 1.0);
	nAttr.setKeyable(true);
	nAttr.setStorable(true);
	uAttr.setWritable(true);
	uAttr.setReadable(true);

	// Grammar
	grammar = tAttr.create("grammar", "grammar", MFnData::kString);
	tAttr.setKeyable(true);
	tAttr.setStorable(true);
	uAttr.setWritable(true);
	uAttr.setReadable(true);
	tAttr.setUsedAsFilename(true);

	// Time
	time = uAttr.create("time", "time", MFnUnitAttribute::kTime, 0.0);
	uAttr.setKeyable(true);
	uAttr.setStorable(true);
	uAttr.setWritable(true);
	uAttr.setReadable(true);

	// Output
	outputMesh = tAttr.create("outputMesh", "outputMesh", MFnData::kMesh);
	tAttr.setWritable(false);
	tAttr.setStorable(false);
	tAttr.setKeyable(false);
	tAttr.setReadable(true);

	// Add attributes to node
	addAttribute(angle);
	addAttribute(stepSize);
	addAttribute(grammar);
	addAttribute(time);
	addAttribute(outputMesh);

	// Attribute affects
	attributeAffects(time, outputMesh);
	attributeAffects(angle, outputMesh);
	attributeAffects(stepSize, outputMesh);
	attributeAffects(grammar, outputMesh);

	return MS::kSuccess;
}

MStatus LSystemNode::compute(const MPlug& plug, MDataBlock& data)
{
	if (plug != outputMesh) {
		return MS::kUnknownParameter;
	}

	MTime timeValue = data.inputValue(LSystemNode::time).asTime();
	double angleValue = data.inputValue(LSystemNode::angle).asDouble();
	double stepSizeValue = data.inputValue(LSystemNode::stepSize).asDouble();
	MString grammarValue = data.inputValue(LSystemNode::grammar).asString();

	// Get output object
	MDataHandle outputHandle = data.outputValue(outputMesh);

	MFnMeshData dataCreator;
	MObject newOutputData = dataCreator.create();

	MFnMesh meshFS;
	LSystem lSystem;
	lSystem.loadProgram(grammarValue.asChar());
	lSystem.setDefaultAngle(angleValue);
	lSystem.setDefaultStep(stepSizeValue);
	std::vector<LSystem::Branch> branches;
	lSystem.process(static_cast<unsigned int>(timeValue.value()), branches);

	// Draw the branches from the final iteration
	MIntArray faceCounts;
	MIntArray faceConnects;
	MPointArray points;

	for (int j = 0; j < branches.size(); j++) {
		vec3 start = branches.at(j).first;
		vec3 end = branches.at(j).second;
		MPoint mStart(start[0], start[2], start[1]);
		MPoint mEnd(end[0], end[2], end[1]);
		CylinderMesh cylinder(mStart, mEnd);
		cylinder.appendToMesh(points, faceCounts, faceConnects);
	}

	// Create the output mesh
	MObject newMesh = meshFS.create(points.length(), faceCounts.length(), points, faceCounts, faceConnects, newOutputData);

	outputHandle.set(newOutputData);
	data.setClean(plug);

	return MS::kSuccess;
}
