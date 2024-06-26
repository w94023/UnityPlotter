#pragma once
#include "pch.h"
#include "StructManager.h"
#include "PlotDataHolder.h"
#include "CustomVertex.h"
#include "VertexHelper.h"

class MeshGenerator
{
public:
	// ������
	MeshGenerator(PlotDataHolder* plotDataHolder, PlotShapeConfig* plotShapeConfig, PlotDataConfig* plotDataConfig, int pointLimit, int vertexLimit);
	// �־��� plot data�� ������� �޽� ����
	void BuildMesh();
	// �־��� �Ű������� ������ �޽� ���� ����
	void GetMeshData(Vector3* vertices, Color* colors, int* triangles, int* nCount);

private:
	// CustomVertex�� ���� ����
	void AddVertexInfo(VertexGenerator vertexGenerator);
	// CustomVertex�� ���� ����
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