#pragma once
#include "pch.h"
#include "MeshGenerator.h"
#include <cmath>
#include <algorithm>
#include <vector>
#include <functional>

MeshGenerator::MeshGenerator(PlotDataHolder* plotDataHolder, PlotShapeConfig* plotShapeConfig, PlotDataConfig* plotDataConfig, int pointLimit, int vertexLimit)
{
	_plotDataHolder = plotDataHolder;
	// PlotDataHolder�� ������ �޸� �ּ� ����
	_plotDataHolder->SetPointArraySync(&_dataInfo, &_normalizedPoints, &_localPoints, &_projectedPoints);

	// Config ����
	_plotShapeConfig = plotShapeConfig;
	_plotDataConfig  = plotDataConfig;

	_vertexLimit = vertexLimit;
}

void MeshGenerator::BuildMesh()
{
	_vertices.clear();
	_colors.clear();

	if (_projectedPoints->size() == -1) return;

	VertexGenerator vertexGenerator = VertexGenerator(_plotShapeConfig);

	for (int i = 0; i < _projectedPoints->size(); i++) {
		vertexGenerator.AddData((*_projectedPoints)[i], (*_dataInfo)[i]);
		if ((*_dataInfo)[i].linkCount == 0) {
			// ��ũ ������
			// ���� ��ũ�� ���ų� ���� �����Ͱ� �������� ��� ���ؽ� ����
			if ((*_dataInfo)[i + 1].linkCount == 0 || i == _projectedPoints->size() - 1) {
				vertexGenerator.BuildMesh(_plotDataConfig->segments, _plotShapeConfig);
				vertexGenerator.Clear();
			}
		}
		else if ((*_dataInfo)[i].linkCount == 2) {
			// ��ũ ����
			vertexGenerator.BuildMesh(_plotDataConfig->segments, _plotShapeConfig);
			vertexGenerator.Clear();
		}
    }

	AddVertexInfo(vertexGenerator);
}

void MeshGenerator::GetMeshData(Vector3* vertices, Color* colors, int* triangles, int* nCount)
{
	// _vertexCount ��ŭ �Ű� ���� vertices, colors, triangles�� ������ ����
	*nCount = _vertices.size();
	for (int i = 0; i < _vertices.size(); i++) {
		vertices[i]  = _vertices[i];
		colors[i]    = _colors[i];
		triangles[i] = i;
	}
}

void MeshGenerator::AddVertexInfo(VertexGenerator vertexGenerator)
{
	std::sort(vertexGenerator.customVerticies.begin(), vertexGenerator.customVerticies.end(), 
		[](CustomVertex a, CustomVertex b) { return a.centerZ < b.centerZ; } // center z ũ���� ������������ ����
	);

	for (int i = 0; i < vertexGenerator.customVerticies.size(); i++) {
		if (_vertices.size() > _vertexLimit - 3) break;
		AddVertexInfo(vertexGenerator.customVerticies[i]);
	}
}

void MeshGenerator::AddVertexInfo(CustomVertex customVertex)
{
	for (int j = 0; j < 3; j++) {
		_vertices.push_back(customVertex.vertices[j]);
		_colors  .push_back(customVertex.colors[j]);
	}
}