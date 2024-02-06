#ifndef LSystemNode_H_
#define LSystemNode_H_

#include <maya/MPxNode.h>

class LSystemNode : public MPxNode
{
public:
	LSystemNode() {};
	virtual ~LSystemNode() {};
	virtual MStatus compute(const MPlug& plug, MDataBlock& data);
	static  void* creator();
	static  MStatus initialize();

public:
	static MTypeId id;
	static MObject time;
	static MObject angle;
	static MObject stepSize;
	static MObject grammar;
	static MObject outputMesh;
};


#endif