#pragma once
#include "pch.h"
#include "StructManager.h"
#include "PlotDataHolder.h"
#include "CustomVertex.h"
#include "VertexHelper.h"

class MeshGenerator
{
public:
	// 생성자
	MeshGenerator(PlotDataHolder* plotDataHolder, PlotShapeConfig* plotShapeConfig, PlotDataConfig* plotDataConfig, int pointLimit, int vertexLimit);
	// 주어진 plot data를 기반으로 메쉬 생성
	void BuildMesh();
	// 주어진 매개변수에 생성된 메쉬 정보 복사
	void GetMeshData(Vector3* vertices, Color* colors, int* triangles, int* nCount);

private:
	// CustomVertex의 정보 복사
	void AddVertexInfo(VertexGenerator vertexGenerator);
	// CustomVertex의 정보 복사
	void AddVertexInfo(CustomVertex customVertex);

	PlotDataHolder*  _plotDataHolder;

	// Configs
	PlotShapeConfig* _plotShapeConfig;
	PlotDataConfig*  _plotDataConfig;

	// Plotting points
	std::vector<DataInfo>* _dataInfo;
	std::vector<Vector3>*  _localPoints;
	std::vector<Vector3>*  _normalizedPoints;
	std::vector<Vector3>*  _projectedPoints;

	// Data
	int _vertexLimit;
	std::vector<Vector3> _vertices;
	std::vector<Color>   _colors;
};