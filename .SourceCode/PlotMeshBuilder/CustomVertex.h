#pragma once
#include "pch.h"
#include "StructManager.h"
#include "VertexModifier.h"

struct CustomVertex
{
	Vector3 vertices[3];
	Color   colors[3];
	float   centerZ;

	CustomVertex(Vector3 p1, Vector3 p2, Vector3 p3, Color color);
	CustomVertex(Vector3 p1, Vector3 p2, Vector3 p3, Color c1, Color c2, Color c3);
};

class VertexGenerator
{
public:
	// 생성자
	VertexGenerator(PlotShapeConfig* plotShapeConfig);
	// Vertices 등에 추가한 데이터 초기화
	void Clear();
	// Plot data 추가
	void AddData(Vector3 point, DataInfo dataInfo);
	// Plot data를 기반으로 메쉬 생성
	void BuildMesh(int segments, PlotShapeConfig* plotShapeConfig);

	std::vector<CustomVertex> customVerticies;

private:
	void BuildScatterMesh(int segments, PlotShapeConfig* plotShapeConfig);
	void BuildLineMesh   (int segments, PlotShapeConfig* plotShapeConfig);
	void BuildFillMesh   (int segments, PlotShapeConfig* plotShapeConfig);

	void AddCustomVertex(Vector3 v1, Vector3 v2, Vector3 v3, Color color);
	void AddCustomVertex(Vector3 v1, Vector3 v2, Vector3 v3, Color c1, Color c2, Color c3);
	void CheckRange(Vector3* point);
	// Vertex가 화면 영역을 벗어날 경우 수정
	void AdjustPlotPoints(std::vector<Vector3> verticesInput, std::vector<bool> isVerticiesInRange, RangeInfo range, std::vector<Color> colors);
	void AdjustPlotPoints(std::vector<Vector3> verticesInput, std::vector<bool> isVerticiesInRange, RangeInfo range, Color color);

	std::vector<Vector3>  _points;
	std::vector<DataInfo> _dataInfo;
	RangeInfo             _range;
};