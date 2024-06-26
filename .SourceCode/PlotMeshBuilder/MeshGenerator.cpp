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
	// PlotDataHolder와 데이터 메모리 주소 공유
	_plotDataHolder->SetPointArraySync(&_dataInfo, &_normalizedPoints, &_localPoints, &_projectedPoints);

	// Config 저장
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
			// 링크 시작점
			// 다음 링크가 없거나 현재 데이터가 마지막일 경우 버텍스 생성
			if ((*_dataInfo)[i + 1].linkCount == 0 || i == _projectedPoints->size() - 1) {
				vertexGenerator.BuildMesh(_plotDataConfig->segments, _plotShapeConfig);
				vertexGenerator.Clear();
			}
		}
		else if ((*_dataInfo)[i].linkCount == 2) {
			// 링크 종점
			vertexGenerator.BuildMesh(_plotDataConfig->segments, _plotShapeConfig);
			vertexGenerator.Clear();
		}
    }

	AddVertexInfo(vertexGenerator);
}

void MeshGenerator::GetMeshData(Vector3* vertices, Color* colors, int* triangles, int* nCount)
{
	// _vertexCount 만큼 매개 변수 vertices, colors, triangles에 데이터 복사
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
		[](CustomVertex a, CustomVertex b) { return a.centerZ < b.centerZ; } // center z 크기의 오름차순으로 정렬
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