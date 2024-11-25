#pragma once
#include "pch.h"
#include "PlotDataHolder.h"
#include <cmath>
#include <limits>
#include <algorithm>
#include <iostream>
#include <vector>
#include <functional>

PlotDataHolder::PlotDataHolder(PlotShapeConfig* plotShapeConfig, PlotDataConfig* plotDataConfig, int pointLimit, int tickLimit)
{
	_plotShapeConfig = plotShapeConfig;
	_plotDataConfig  = plotDataConfig;
	_pointLimit      = pointLimit;

	_textPositions = new Vector3[tickLimit];
	_textAxis      = new int[tickLimit];
	_textContents  = new float[tickLimit];
	_textCount     = 0;

	_axisDataHandler = new AxisDataHandler(_pointLimit, tickLimit, &_plotData, &_dataInfo, &_localPoints, &_projectedPoints);
	_plotDataHandler = new PlotDataHandler(_pointLimit, &_plotData, &_dataInfo);
}

PlotDataHolder::~PlotDataHolder()
{
	delete _axisDataHandler;
	delete _plotDataHandler;
}

void PlotDataHolder::SetPointArraySync(std::vector<DataInfo>** dataInfo, std::vector<Vector3>** normalizedPoints, std::vector<Vector3>** localPoints, std::vector<Vector3>** projectedPoints)
{
	*dataInfo         = &_dataInfo;
	*normalizedPoints = &_normalizedPoints;
	*localPoints      = &_localPoints;
	*projectedPoints  = &_projectedPoints;
}


void PlotDataHolder::ClearData()
{
	_plotData.clear();
	_dataInfo.clear();
}

void PlotDataHolder::SetPlotData(float* x, float* y, int axis, int nPoints)
{
	_plotDataHandler->SetPlotData(x, y, axis, nPoints, _plotDataConfig);
}

void PlotDataHolder::SetPlotData(float* x, float* y, int nPoints)
{
	_plotDataHandler->SetPlotData(x, y, nPoints, _plotDataConfig);
}

void PlotDataHolder::SetPlotData(Vector3* points, int nPoints)
{
	_plotDataHandler->SetPlotData(points, nPoints, _plotDataConfig);
}

void PlotDataHolder::SetPlotData(Vector3* startPoints, Vector3* endPoints, int nPoints)
{
	_plotDataHandler->SetPlotData(startPoints, endPoints, nPoints, _plotDataConfig);
}

void PlotDataHolder::SetPlotData(float* xMap, float* yMap, float* zMap, int nRow, int nCol)
{
	_plotDataHandler->SetPlotData(xMap, yMap, zMap, nRow, nCol, _plotDataConfig);
}

void PlotDataHolder::SetFillData(float* xMap, float* yMap, float* zMap, int nRow, int nCol)
{
	_plotDataHandler->SetFillData(xMap, yMap, zMap, nRow, nCol, _plotDataConfig);
}

void PlotDataHolder::DataSetDone()
{
	_axisDataHandler->Clear();
}

Vector2 PlotDataHolder::GetMinMaxValueFromData(std::vector<Vector3> data, int axis)
{
	Vector2 range;
	if (data.size() == 0) return range;

	range[0] = +std::numeric_limits<float>::infinity(); // 양의 무한대
	range[1] = -std::numeric_limits<float>::infinity(); // 음의 무한대
	for (int i = 0; i < data.size(); i++) {
		if (std::isinf(data[i][axis])) continue;
		if (range[0] > data[i][axis]) range[0] = data[i][axis];
		if (range[1] < data[i][axis]) range[1] = data[i][axis];
	}

	if (std::isinf(range[0])) range[0] = 0.0f;
	if (std::isinf(range[1])) range[1] = 0.0f;

	return range;
}

void PlotDataHolder::GetPointData(DataInfo* dataInfo, Vector3* localPoints, Vector3* normalizedPoints, Vector3* projectedPoints, int* nPoints)
{
	*nPoints = _plotData.size();
	for (int i = 0; i < _plotData.size(); i++) {
		dataInfo[i]         = _dataInfo[i];
		localPoints[i]      = _localPoints[i];
		normalizedPoints[i] = _normalizedPoints[i];
		projectedPoints[i]  = _projectedPoints[i];
	}
}

void PlotDataHolder::GetPointsData()
{
	if (!_isDataSetDone) return;
	_getDataDone = false;

	_normalizedPoints.clear();
	_localPoints.clear();
	_projectedPoints.clear();

	RangeInfo rangeInfo;
	if (_plotShapeConfig->autoScaling == 1) {
		rangeInfo.xLimit = GetMinMaxValueFromData(_plotData, 0);
		rangeInfo.yLimit = GetMinMaxValueFromData(_plotData, 1);
		rangeInfo.zLimit = GetMinMaxValueFromData(_plotData, 2);
	}
	else {
		rangeInfo.xLimit = _plotShapeConfig->xLimit;
		rangeInfo.yLimit = _plotShapeConfig->yLimit;
		rangeInfo.zLimit = _plotShapeConfig->zLimit;
	}

	_axisDataHandler->CreateData(rangeInfo, _plotShapeConfig);
	
	for (int i = 0; i < _plotData.size(); i++) {
		if (_dataInfo[i].dataType == PlotData || _dataInfo[i].dataType == MapData) {
			_plotData[i].Clamp(rangeInfo);
		}

		_normalizedPoints.push_back(Vector3(
			InverseLerp(rangeInfo.xLimit[0], rangeInfo.xLimit[1], _plotData[i].x - _plotShapeConfig->origin.x),
			InverseLerp(rangeInfo.yLimit[0], rangeInfo.yLimit[1], _plotData[i].y - _plotShapeConfig->origin.y),
			InverseLerp(rangeInfo.zLimit[0], rangeInfo.zLimit[1], _plotData[i].z - _plotShapeConfig->origin.z)
		));

		if (_dataInfo[i].dataType == MapData) {
			_dataInfo[i].color = gradient.Evaluate(InverseLerp(rangeInfo.zLimit[0], rangeInfo.zLimit[1], _plotData[i].z));
		}
	}

	NormalizedPosToLocalPos(_plotShapeConfig->rectSize, rangeInfo);
	_axisDataHandler->AdjustAxisDataLocalPoints(_plotShapeConfig);
	ApplyRotationToData(_plotShapeConfig->rotEuler);
	GetProjectedPos(_plotShapeConfig->zProjectionScale);

	_axisDataHandler->GetTextInfo(_textPositions, _textAxis, _textContents, &_textCount);

	_getDataDone = true;
}

void PlotDataHolder::GetTextInfo(Vector3* positions, int* textAxis, float* textContents, int* nCount)
{
	*nCount = _textCount;
	for (int i = 0; i < _textCount; i++) {
		positions[i] = _textPositions[i];
		textAxis[i] = _textAxis[i];
		textContents[i] = _textContents[i];
	}
}

void PlotDataHolder::NormalizedPosToLocalPos(Vector2 rectSize, RangeInfo rangeInfo)
{
	float xMinPoint = - rectSize.x * 0.5f;
	float xMaxPoint = - rectSize.x * 0.5f + rectSize.x;
	float yMinPoint = - rectSize.y * 0.5f;
	float yMaxPoint = - rectSize.y * 0.5f + rectSize.y;
	//float zMinPoint = (xMinPoint + yMinPoint) / 2.0f * (rangeInfo.zLimit[1] / rangeInfo.xLimit[1] + rangeInfo.zLimit[1] / rangeInfo.yLimit[1]) / 2.0f;
	//float zMaxPoint = (xMaxPoint + yMaxPoint) / 2.0f * (rangeInfo.zLimit[0] / rangeInfo.xLimit[0] + rangeInfo.zLimit[0] / rangeInfo.yLimit[0]) / 2.0f;
	float zMinPoint = (xMinPoint + yMinPoint) / 2.0f;
	float zMaxPoint = (xMaxPoint + yMaxPoint) / 2.0f;

	// 유니티 Z 축은 뒤집혀있어서 반대로 설정해야 함
	for (int i = 0; i < _normalizedPoints.size(); i++) {
		_localPoints.push_back(Vector3(
			Lerp(xMinPoint, xMaxPoint, +_normalizedPoints[i].x),
			Lerp(yMinPoint, yMaxPoint, +_normalizedPoints[i].y),
			Lerp(zMaxPoint, zMinPoint, +_normalizedPoints[i].z)
		));
	}
}

void PlotDataHolder::ApplyRotationToData(Vector3 rotEuler)
{
    Quaternion quaternionRotation = Quaternion::Euler(rotEuler.x, rotEuler.y, rotEuler.z);

	for(int i = 0; i < _plotData.size(); i++) {
		_localPoints[i] = quaternionRotation * _localPoints[i];
	}
}

void PlotDataHolder::GetProjectedPos(float zProjectionScale)
{
	for (int i = 0; i < _localPoints.size(); i++) {
		_projectedPoints.push_back(_localPoints[i]);
		float zBasedScaler = 1.0f - _localPoints[i].z * zProjectionScale;
		_projectedPoints[i].z = (zBasedScaler < 0) ? 0.0f : zBasedScaler;
	}
}