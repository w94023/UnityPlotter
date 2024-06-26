#pragma once
#include "pch.h"
#include "StructManager.h"
#include "DataHandler.h"

class AxisDataHandler : public DataHandler
{
public:
	AxisDataHandler(int pointLimit, int tickLimit, std::vector<Vector3>* plotData, std::vector<DataInfo>* dataInfo, std::vector<Vector3>* localPoints, std::vector<Vector3>* projectedPoints):
		DataHandler(pointLimit, plotData, dataInfo)
	{
		_tickLimit       = tickLimit;
		_localPoints     = localPoints;
		_projectedPoints = projectedPoints;
	}

	void Clear()
	{
		_dataCountWithoutAxisData = -1;
	}

	void CreateData(RangeInfo rangeInfo, PlotShapeConfig* config)
	{
		if (_dataCountWithoutAxisData == -1) {
			_dataCountWithoutAxisData = _plotData->size();
		}
		else {
			if (_dataCountWithoutAxisData < _plotData->size()) {
				_plotData->erase(_plotData->begin() + _dataCountWithoutAxisData, _plotData->end());
				_dataInfo->erase(_dataInfo->begin() + _dataCountWithoutAxisData, _dataInfo->end());
			}
		}

		_projectedPointRangeInfo.xLimit = Vector2(-config->rectSize.x / 2.0f - _textOffset, config->rectSize.x / 2.0f + _textOffset);
		_projectedPointRangeInfo.yLimit = Vector2(-config->rectSize.y / 2.0f - _textOffset, config->rectSize.y / 2.0f + _textOffset);
		
		SetAxisData(rangeInfo, config);
		SetTickData(rangeInfo.xLimit, config->xTickSpan, 0, config->axisColor, config->tickRadius, config->isRender2D, rangeInfo.yLimit.x);
		SetTickData(rangeInfo.yLimit, config->yTickSpan, 1, config->axisColor, config->tickRadius, config->isRender2D, rangeInfo.xLimit.x);
	}

	void SetAxisData(RangeInfo rangeInfo, PlotShapeConfig* config)
	{
		std::vector<std::vector<Vector3>> plotData;
		std::vector<Vector3> plotDataTemp;

		float xClampPoint = 0.0f;
		if (config->isRender2D == 1) xClampPoint = rangeInfo.xLimit.x;

		float yClampPoint = 0.0f;
		if (config->isRender2D == 1) yClampPoint = rangeInfo.yLimit.x;
		
		// X axis
		plotDataTemp.clear();
		plotDataTemp.push_back(Vector3(-std::numeric_limits<float>::infinity(), yClampPoint, 0.0f));
		plotDataTemp.push_back(Vector3(									  0.0f, yClampPoint, 0.0f));
		plotData.push_back(plotDataTemp);
		plotDataTemp.clear();
		plotDataTemp.push_back(Vector3(									  0.0f, yClampPoint, 0.0f));
		plotDataTemp.push_back(Vector3(+std::numeric_limits<float>::infinity(), yClampPoint, 0.0f));
		plotData.push_back(plotDataTemp);
		// Y axis
		plotDataTemp.clear();
		plotDataTemp.push_back(Vector3(xClampPoint, -std::numeric_limits<float>::infinity(), 0.0f));
		plotDataTemp.push_back(Vector3(xClampPoint, 								   0.0f, 0.0f));
		plotData.push_back(plotDataTemp);
		plotDataTemp.clear();
		plotDataTemp.push_back(Vector3(xClampPoint, 							       0.0f, 0.0f));
		plotDataTemp.push_back(Vector3(xClampPoint, +std::numeric_limits<float>::infinity(), 0.0f));
		plotData.push_back(plotDataTemp);
		// Z axis
		plotDataTemp.clear();
		plotDataTemp.push_back(Vector3(0.0f, 0.0f, -std::numeric_limits<float>::infinity()));
		plotDataTemp.push_back(Vector3(0.0f, 0.0f, 								      0.0f));
		plotData.push_back(plotDataTemp);
		plotDataTemp.clear();
		plotDataTemp.push_back(Vector3(0.0f, 0.0f, 									  0.0f));
		plotDataTemp.push_back(Vector3(0.0f, 0.0f, +std::numeric_limits<float>::infinity()));
		plotData.push_back(plotDataTemp);

		FlattenLinkedData(plotData, 1, 6, AxisData, config->axisColor, config->axisRadius, false);
	}

	void SetTickData(Vector2 range, float span, int axis, Color color, float radius, int isRender2D, float clampPoint)
	{
		if (span < 0.0f) return;

		int   negativeCount = 0;
		int   positiveCount = 0;
		int   zeroCount     = 0;
		float startPoint    = 0.0f;
		if (range[0] < 0.0f && range[1] < 0.0f) {
			startPoint = static_cast<int>(range[0]);
			int endPoint = static_cast<int>(range[1]) - 1;
			negativeCount = static_cast<int>((endPoint - startPoint) / span) + 1;
		
		}
		else if (range[0] > 0.0f && range[1] > 0.0f) {
			startPoint = static_cast<int>(range[0]) + 1;
			int endPoint = static_cast<int>(range[1]);
			positiveCount = static_cast<int>((endPoint - startPoint) / span) + 1;
		}
		else {
			negativeCount = static_cast<int>(-range[0] / span);
			positiveCount = static_cast<int>(range[1] / span);
			zeroCount     = 1;
			startPoint = -span * negativeCount;
		}

		int entireCount = negativeCount + positiveCount + zeroCount;
		if (entireCount <= 0) return;

		int dataSize = (entireCount > _tickLimit / 2) ? _tickLimit / 2 : entireCount;

		std::vector<std::vector<Vector3>> plotData;
		std::vector<std::vector<Vector3>> textData;

		float clampingPoint = 0.0f;
		if (isRender2D == 1) clampingPoint = clampPoint;

		for (int i = 0; i < dataSize; i++) {
			std::vector<Vector3> plotDataTemp;
			plotDataTemp.push_back(Vector3());
			plotDataTemp.push_back(Vector3());
			plotDataTemp[0][(axis + 1) - (axis + 1) / 2 * 2] = clampingPoint;
			plotDataTemp[1][(axis + 1) - (axis + 1) / 2 * 2] = clampingPoint;
			plotDataTemp[0][axis] = startPoint + i * span;
			plotDataTemp[1][axis] = startPoint + i * span;

			std::vector<Vector3> textDataTemp;
			textDataTemp.push_back(plotDataTemp[0]);

			plotData.push_back(plotDataTemp);
			textData.push_back(textDataTemp);
		}

		FlattenLinkedData(plotData, 1, dataSize, (DataType)(axis + 3), color, radius, false);
		FlattenLinkedData(textData, 0, dataSize, (DataType)(axis + 5), color, 0.0f, false);
	}

	void AdjustAxisDataLocalPoints(PlotShapeConfig* config)
	{
		for (int i = _dataCountWithoutAxisData; i < _plotData->size(); i++) {
			if ((*_dataInfo)[i].linkCount == 2) {
				if ((*_dataInfo)[i].dataType == XAxisTickData) {
					(*_localPoints)[i].y += config->tickLength;
				}
				else if ((*_dataInfo)[i].dataType == YAxisTickData) {
					(*_localPoints)[i].x += config->tickLength;
				}
			}

			if ((*_dataInfo)[i].dataType == YAxisTextData) {
				(*_localPoints)[i].x -= 10.0f;
			}
			else if ((*_dataInfo)[i].dataType == XAxisTextData) {
				(*_localPoints)[i].y -= 10.0f;
			}
		}
	}

	void GetTextInfo(Vector3* positions, int* textAxis, float* contents, int* nCount)
	{
		int count = 0;
		for (int i = _dataCountWithoutAxisData; i < _plotData->size(); i++) {
			if ((*_dataInfo)[i].dataType == XAxisTextData) {
				if (IsPointInRange((*_projectedPoints)[i], _projectedPointRangeInfo)) {
					positions[count] = Vector3((*_projectedPoints)[i].x, (*_projectedPoints)[i].y, 0.0f);
					textAxis[count] = 0;
					contents[count] = (*_plotData)[i].x;
					count++;
				}
			}
			else if ((*_dataInfo)[i].dataType == YAxisTextData) {
				if (IsPointInRange((*_projectedPoints)[i], _projectedPointRangeInfo)) {
					positions[count] = Vector3((*_projectedPoints)[i].x, (*_projectedPoints)[i].y, 0.0f);
					textAxis[count] = 1;
					contents[count] = (*_plotData)[i].y;
					count++;
				}
			}
		}

		*nCount = count;
	}
	
private:
	int                   _tickLimit;
	int                   _dataCountWithoutAxisData = -1;
	std::vector<Vector3>* _localPoints;
	std::vector<Vector3>* _projectedPoints;

	RangeInfo _projectedPointRangeInfo;
	float     _textOffset = 10.0f;
	Vector3*  _textPositions;
	float*    _textContents;
	int       _textCount;
};