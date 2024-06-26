#pragma once
#include "pch.h"
#include "CustomVertex.h"
#include <vector>
#include <algorithm>

CustomVertex::CustomVertex(Vector3 p1, Vector3 p2, Vector3 p3, Color color)
{
	centerZ = (p1.z + p2.z + p3.z) / 3.0f;

	vertices[0] = p1; vertices[0].z = 0.0f;
	vertices[1] = p2; vertices[1].z = 0.0f;
	vertices[2] = p3; vertices[2].z = 0.0f;

	colors[0] = color;
	colors[1] = color;
	colors[2] = color;
}

CustomVertex::CustomVertex(Vector3 p1, Vector3 p2, Vector3 p3, Color c1, Color c2, Color c3)
{
	centerZ = (p1.z + p2.z + p3.z) / 3.0f;

	vertices[0] = p1; vertices[0].z = 0.0f;
	vertices[1] = p2; vertices[1].z = 0.0f;
	vertices[2] = p3; vertices[2].z = 0.0f;

	colors[0] = c1;
	colors[1] = c2;
	colors[2] = c3;
}


VertexGenerator::VertexGenerator(PlotShapeConfig* plotShapeConfig)
{
	_range.xLimit[0] = -plotShapeConfig->rectSize.x / 2.0f;
	_range.xLimit[1] = +plotShapeConfig->rectSize.x / 2.0f;
	_range.yLimit[0] = -plotShapeConfig->rectSize.y / 2.0f;
	_range.yLimit[1] = +plotShapeConfig->rectSize.y / 2.0f;
}

void VertexGenerator::Clear()
{
	_points.clear();
	_dataInfo.clear();
}

void VertexGenerator::AddData(Vector3 point, DataInfo dataInfo)
{
	_points.push_back(point);
	_dataInfo.push_back(dataInfo);
}

void VertexGenerator::CheckRange(Vector3* point)
{
	if (point->x < _range.xLimit[0]) point->x = _range.xLimit[0];
	if (point->x > _range.xLimit[1]) point->x = _range.xLimit[1];
	if (point->y < _range.yLimit[0]) point->y = _range.yLimit[0];
	if (point->y > _range.yLimit[1]) point->y = _range.yLimit[1];
}

void VertexGenerator::BuildMesh(int segments, PlotShapeConfig* plotShapeConfig)
{
	if (segments < 3) segments = 3;

	if (_points.size() == 1) {
		// Plot point가 하나일 때 : 점 메쉬 생성
		BuildScatterMesh(segments, plotShapeConfig);
	}
	// Plot point가 둘 이상일 때 : 라인/필 메쉬 생성
	else {
		if (_dataInfo[0].isFillMesh) BuildFillMesh(segments, plotShapeConfig);
		else                         BuildLineMesh(segments, plotShapeConfig);
	}
}

void VertexGenerator::BuildScatterMesh(int segments, PlotShapeConfig* plotShapeConfig)
{
	for (int i = 0; i < segments - 2; i++) {
		Vector3 p1 = Vector3(
			_points[0].x + _dataInfo[0].radius * _points[0].z * cos(2.0f * (float)M_PI * 0.0f / segments),
			_points[0].y + _dataInfo[0].radius * _points[0].z * sin(2.0f * (float)M_PI * 0.0f / segments),
			_points[0].z
		);
		Vector3 p2 = Vector3(
			_points[0].x + _dataInfo[0].radius * _points[0].z * cos(2.0f * (float)M_PI * (i + 1) / segments),
			_points[0].y + _dataInfo[0].radius * _points[0].z * sin(2.0f * (float)M_PI * (i + 1) / segments),
			_points[0].z
		);
		Vector3 p3 = Vector3(
			_points[0].x + _dataInfo[0].radius * _points[0].z * cos(2.0f * (float)M_PI * (i + 2) / segments),
			_points[0].y + _dataInfo[0].radius * _points[0].z * sin(2.0f * (float)M_PI * (i + 2) / segments),
			_points[0].z
		);
		CheckRange(&p1); CheckRange(&p2); CheckRange(&p3);
		customVerticies.push_back(CustomVertex(p1, p2, p3, _dataInfo[0].color));
	}
}

void VertexGenerator::BuildLineMesh(int segments, PlotShapeConfig* plotShapeConfig)
{
	for (int i = 0; i < _points.size() - 1; i++) {
		Vector3 directionVector = _points[i + 1] - _points[i];
		directionVector.z = 0.0f;
		directionVector.Normalize();

		Vector3 offsetVector = Vector3::Cross(directionVector, Vector3(0.0f, 0.0f, 1.0f));
		offsetVector.z = 0.0f;
		offsetVector.Normalize();

		float zScaleFactor;
		if (_dataInfo[i].dataType != PlotData) zScaleFactor = 1.0f;
		else						           zScaleFactor = _points[i].z;

		std::vector<Vector3> verticiesTemp;
		std::vector<bool>    isVerticiesInRange;
		std::vector<Color>   colors;

		//Color color = Lerp(_dataInfo[i].color, _dataInfo[i + 1].color, 0.5f);
		colors.push_back(_dataInfo[i].color);
		colors.push_back(_dataInfo[i + 1].color);
		colors.push_back(_dataInfo[i + 1].color);
		verticiesTemp.push_back(_points[i    ] - _dataInfo[i].radius * zScaleFactor * offsetVector);
		verticiesTemp.push_back(_points[i + 1] - _dataInfo[i].radius * zScaleFactor * offsetVector);
		verticiesTemp.push_back(_points[i + 1] + _dataInfo[i].radius * zScaleFactor * offsetVector);
		isVerticiesInRange.push_back(IsPointInRange(verticiesTemp[0], _range));
		isVerticiesInRange.push_back(IsPointInRange(verticiesTemp[1], _range));
		isVerticiesInRange.push_back(IsPointInRange(verticiesTemp[2], _range));
		AdjustPlotPoints(verticiesTemp, isVerticiesInRange, _range, colors);

		colors.clear();
		verticiesTemp.clear();
		isVerticiesInRange.clear();
		colors.push_back(_dataInfo[i].color);
		colors.push_back(_dataInfo[i].color);
		colors.push_back(_dataInfo[i + 1].color);
		verticiesTemp.push_back(_points[i    ] - _dataInfo[i].radius * zScaleFactor * offsetVector);
		verticiesTemp.push_back(_points[i    ] + _dataInfo[i].radius * zScaleFactor * offsetVector);
		verticiesTemp.push_back(_points[i + 1] + _dataInfo[i].radius * zScaleFactor * offsetVector);
		isVerticiesInRange.push_back(IsPointInRange(verticiesTemp[0], _range));
		isVerticiesInRange.push_back(IsPointInRange(verticiesTemp[1], _range));
		isVerticiesInRange.push_back(IsPointInRange(verticiesTemp[2], _range));
		AdjustPlotPoints(verticiesTemp, isVerticiesInRange, _range, colors);
	}
}

void VertexGenerator::BuildFillMesh(int segments, PlotShapeConfig* plotShapeConfig)
{
	std::vector<Vector3> verticiesTemp;
	std::vector<bool>    isVerticiesInRange;
	std::vector<Color>   colors;

	//Color color = Color(
	//	(_dataInfo[0].color.r + _dataInfo[1].color.r + _dataInfo[2].color.r + _dataInfo[3].color.r / 4.0f),
	//	(_dataInfo[0].color.g + _dataInfo[1].color.g + _dataInfo[2].color.g + _dataInfo[3].color.g / 4.0f),
	//	(_dataInfo[0].color.b + _dataInfo[1].color.b + _dataInfo[2].color.b + _dataInfo[3].color.b / 4.0f),
	//	(_dataInfo[0].color.a + _dataInfo[1].color.a + _dataInfo[2].color.a + _dataInfo[3].color.a / 4.0f)
	//);

	colors.push_back(_dataInfo[0].color);
	colors.push_back(_dataInfo[1].color);
	colors.push_back(_dataInfo[2].color);
	verticiesTemp.push_back(_points[0]);
	verticiesTemp.push_back(_points[1]);
	verticiesTemp.push_back(_points[2]);
	isVerticiesInRange.push_back(IsPointInRange(verticiesTemp[0], _range));
	isVerticiesInRange.push_back(IsPointInRange(verticiesTemp[1], _range));
	isVerticiesInRange.push_back(IsPointInRange(verticiesTemp[2], _range));
	AdjustPlotPoints(verticiesTemp, isVerticiesInRange, _range, colors);

	colors.clear();
	verticiesTemp.clear();
	isVerticiesInRange.clear();
	colors.push_back(_dataInfo[0].color);
	colors.push_back(_dataInfo[2].color);
	colors.push_back(_dataInfo[3].color);
	verticiesTemp.push_back(_points[0]);
	verticiesTemp.push_back(_points[2]);
	verticiesTemp.push_back(_points[3]);
	isVerticiesInRange.push_back(IsPointInRange(verticiesTemp[0], _range));
	isVerticiesInRange.push_back(IsPointInRange(verticiesTemp[1], _range));
	isVerticiesInRange.push_back(IsPointInRange(verticiesTemp[2], _range));
	AdjustPlotPoints(verticiesTemp, isVerticiesInRange, _range, colors);

	//std::vector<Vector3> verticiesTemp;
	//std::vector<bool>    isVerticiesInRange;

	//for (int i = 0; i < _points.size(); i++) {
	//	float zScaleFactor;
	//	if (_dataInfo[i].dataType != PlotData) zScaleFactor = 1.0f;
	//	else						           zScaleFactor = _points[i].z;

	//	verticiesTemp.push_back(_points[i]);
	//	isVerticiesInRange.push_back(IsPointInRange(_points[i], _range));
	//}

	//Color color = Color(
	//	(_dataInfo[0].color.r + _dataInfo[1].color.r + _dataInfo[2].color.r / 3.0f),
	//	(_dataInfo[0].color.g + _dataInfo[1].color.g + _dataInfo[2].color.g / 3.0f),
	//	(_dataInfo[0].color.b + _dataInfo[1].color.b + _dataInfo[2].color.b / 3.0f),
	//	(_dataInfo[0].color.a + _dataInfo[1].color.a + _dataInfo[2].color.a / 3.0f)
	//);

	//AdjustPlotPoints(verticiesTemp, isVerticiesInRange, _range, color);
}

void VertexGenerator::AdjustPlotPoints(std::vector<Vector3> verticesInput, std::vector<bool> isVerticiesInRange, RangeInfo range, std::vector<Color> colors)
{
	std::vector<Vector3> verticesTemp;
	std::vector<Color> colorTemp;
	bool previousPointResult = isVerticiesInRange[0];

	for (int i = 1; i <= 3; i++) {
		int currentIndex = i;
		int previousIndex = i - 1;
		if (i == 3) currentIndex = 0;

		bool currentPointResult = isVerticiesInRange[currentIndex];
		if (!currentPointResult) {
			// 현재 포인트는 범위 밖이지만, 이전 포인트는 범위 내일 경우
			if (previousPointResult) {
				verticesTemp.push_back(AdjustPointToBoxKeepingSlope(verticesInput[currentIndex], verticesInput[previousIndex], range));
				colorTemp.push_back(colors[currentIndex]);
			}
			// 현재 포인트와 이전 포인트 모두 범위 밖일 경우
			else {
				std::vector<Vector3> addPoints = AddPointsToBoxKeepingSlope(verticesInput[currentIndex], verticesInput[previousIndex], range);
				if (addPoints.size() > 0) {
					for (int j = 0; j < addPoints.size(); j++) {
						verticesTemp.push_back(addPoints[j]);
						colorTemp.push_back(colors[currentIndex]);
					}
				}
			}
		}
		else {
			// 현재 포인트는 범위 안이지만, 이전 포인트는 범위 밖일 경우
			if (!previousPointResult) {
				verticesTemp.push_back(AdjustPointToBoxKeepingSlope(verticesInput[previousIndex], verticesInput[currentIndex], range));
				colorTemp.push_back(colors[currentIndex]);
				verticesTemp.push_back(verticesInput[currentIndex]);
				colorTemp.push_back(colors[previousIndex]);
			}
			// 현재 포인트와 이전 포인트 모두 범위 내일 경우
			else {
				verticesTemp.push_back(verticesInput[currentIndex]);
				colorTemp.push_back(colors[currentIndex]);
			}
		}

		previousPointResult = currentPointResult;
	}

	if (!isVerticiesInRange[0] || !isVerticiesInRange[1] || !isVerticiesInRange[2]) {
		float zValue = (verticesInput[0].z + verticesInput[1].z + verticesInput[2].z) / 3.0f;
		if (IsPointInTriangle(verticesInput, Vector3(range.xLimit[0], range.yLimit[0], 0.0f))) { verticesTemp.push_back(Vector3(range.xLimit[0], range.yLimit[0], zValue)); colorTemp.push_back(colors[0]); }
		if (IsPointInTriangle(verticesInput, Vector3(range.xLimit[0], range.yLimit[1], 0.0f))) { verticesTemp.push_back(Vector3(range.xLimit[0], range.yLimit[1], zValue)); colorTemp.push_back(colors[0]); }
		if (IsPointInTriangle(verticesInput, Vector3(range.xLimit[1], range.yLimit[0], 0.0f))) { verticesTemp.push_back(Vector3(range.xLimit[1], range.yLimit[0], zValue)); colorTemp.push_back(colors[0]); }
		if (IsPointInTriangle(verticesInput, Vector3(range.xLimit[1], range.yLimit[1], 0.0f))) { verticesTemp.push_back(Vector3(range.xLimit[1], range.yLimit[1], zValue)); colorTemp.push_back(colors[0]); }
	}
	

	if (verticesTemp.size() < 3) {
		return;
	}

	for (int i = 0; i < verticesTemp.size() - 2; i++) {
		AddCustomVertex(verticesTemp[0], verticesTemp[i + 1], verticesTemp[i + 2], colorTemp[i], colorTemp[i + 1], colorTemp[i + 2]);
	}
	AddCustomVertex(verticesTemp[verticesTemp.size() - 1], verticesTemp[verticesTemp.size() - 2], verticesTemp[verticesTemp.size() - 3], 
		colorTemp[verticesTemp.size() - 1], colorTemp[verticesTemp.size() - 2], colorTemp[verticesTemp.size() - 3]);
}

void VertexGenerator::AdjustPlotPoints(std::vector<Vector3> verticesInput, std::vector<bool> isVerticiesInRange, RangeInfo range, Color color)
{
	std::vector<Vector3> verticesTemp;
	bool previousPointResult = isVerticiesInRange[0];

	for (int i = 1; i <= 3; i++) {
		int currentIndex = i;
		int previousIndex = i - 1;
		if (i == 3) currentIndex = 0;

		bool currentPointResult = isVerticiesInRange[currentIndex];
		if (!currentPointResult) {
			// 현재 포인트는 범위 밖이지만, 이전 포인트는 범위 내일 경우
			if (previousPointResult) {
				verticesTemp.push_back(AdjustPointToBoxKeepingSlope(verticesInput[currentIndex], verticesInput[previousIndex], range));
			}
			// 현재 포인트와 이전 포인트 모두 범위 밖일 경우
			else {
				std::vector<Vector3> addPoints = AddPointsToBoxKeepingSlope(verticesInput[currentIndex], verticesInput[previousIndex], range);
				if (addPoints.size() > 0) {
					for (int j = 0; j < addPoints.size(); j++) {
						verticesTemp.push_back(addPoints[j]);
					}
				}
			}
		}
		else {
			// 현재 포인트는 범위 안이지만, 이전 포인트는 범위 밖일 경우
			if (!previousPointResult) {
				verticesTemp.push_back(AdjustPointToBoxKeepingSlope(verticesInput[previousIndex], verticesInput[currentIndex], range));
				verticesTemp.push_back(verticesInput[currentIndex]);
			}
			// 현재 포인트와 이전 포인트 모두 범위 내일 경우
			else {
				verticesTemp.push_back(verticesInput[currentIndex]);
			}
		}

		previousPointResult = currentPointResult;
	}

	if (!isVerticiesInRange[0] || !isVerticiesInRange[1] || !isVerticiesInRange[2]) {
		Vector3  includedCorner;
		if      (IsPointInTriangle(verticesInput, Vector3(range.xLimit[0], range.yLimit[0], 0.0f))) includedCorner = Vector3(range.xLimit[0], range.yLimit[0], 0.0f);
		else if (IsPointInTriangle(verticesInput, Vector3(range.xLimit[0], range.yLimit[1], 0.0f))) includedCorner = Vector3(range.xLimit[0], range.yLimit[1], 0.0f);
		else if (IsPointInTriangle(verticesInput, Vector3(range.xLimit[1], range.yLimit[0], 0.0f))) includedCorner = Vector3(range.xLimit[1], range.yLimit[0], 0.0f);
		else if (IsPointInTriangle(verticesInput, Vector3(range.xLimit[1], range.yLimit[1], 0.0f))) includedCorner = Vector3(range.xLimit[1], range.yLimit[1], 0.0f);

		if (includedCorner != Vector3()) {
			includedCorner.z = (verticesInput[0].z + verticesInput[1].z + verticesInput[2].z) / 3.0f;
			verticesTemp.push_back(includedCorner);
		}
	}
	

	if (verticesTemp.size() < 3) {
		return;
	}

	for (int i = 0; i < verticesTemp.size() - 2; i++) {
		AddCustomVertex(verticesTemp[0], verticesTemp[i + 1], verticesTemp[i + 2], color);
	}
	AddCustomVertex(verticesTemp[verticesTemp.size() - 1], verticesTemp[verticesTemp.size() - 2], verticesTemp[verticesTemp.size() - 3], color);
}

void VertexGenerator::AddCustomVertex(Vector3 v1, Vector3 v2, Vector3 v3, Color color)
{
	if (v1 != v2 && v2 != v3 && v3 != v1) {
		customVerticies.push_back(CustomVertex(v1, v2, v3, color));
	}
}

void VertexGenerator::AddCustomVertex(Vector3 v1, Vector3 v2, Vector3 v3, Color c1, Color c2, Color c3)
{
	if (v1 != v2 && v2 != v3 && v3 != v1) {
		customVerticies.push_back(CustomVertex(v1, v2, v3, c1, c2, c3));
	}
}