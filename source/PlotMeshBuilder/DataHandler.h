#pragma once
#include "pch.h"
#include "StructManager.h"

class DataHandler
{
protected:
	DataHandler(int pointLimit, std::vector<Vector3>* plotData, std::vector<DataInfo>* dataInfo)
	{
		_pointLimit = pointLimit;
		_plotData   = plotData;
		_dataInfo   = dataInfo;
	}

	void FlattenLinkedData(std::vector<std::vector<Vector3>> plotData, int linkCount, int dataCount, DataType dataType, Color color, float radius, bool isFillMesh)
	{
		int startIndex = _plotData->size();
		int endIndex;
		if (_pointLimit - _plotData->size() > dataCount * (linkCount+1)) {
			endIndex = _plotData->size() + dataCount * (linkCount+1);
		}
		else {
			int endIndex = (_pointLimit - _plotData->size()) / (linkCount+1);
			if (endIndex <= startIndex) return;
		}

		if (radius < 0.0f) radius = 0.0f;

		for (int i = 0; i < (endIndex - startIndex) / (linkCount + 1); i++) {
			for (int j = 0; j < linkCount + 1; j++) {
				_dataInfo->push_back(DataInfo());
				int index = _dataInfo->size() - 1;
				if      (j == 0)	     (*_dataInfo)[index].linkCount = 0; // 링크 시작점
				else if (j == linkCount) (*_dataInfo)[index].linkCount = 2; // 링크 종료점
				else                     (*_dataInfo)[index].linkCount = 1; // 링크 중간점

				(*_dataInfo)[index].color      = color;
				(*_dataInfo)[index].radius     = radius;
				(*_dataInfo)[index].dataType   = dataType;
				(*_dataInfo)[index].isFillMesh = isFillMesh;
				
				_plotData->push_back(plotData[i][j]);
			}
		}
	}

	int                    _pointLimit;
	std::vector<Vector3>*  _plotData;
	std::vector<DataInfo>* _dataInfo;
};