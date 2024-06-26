#pragma once
#include "pch.h"
#include "StructManager.h"
#include "VertexModifier.h"
#include "AxisDataHandler.h"
#include "PlotDataHandler.h"

//#define POINT_LIMIT 10000

class PlotDataHolder
{
public:
	// 생성자
	PlotDataHolder(PlotShapeConfig* plotShapeConfig, PlotDataConfig* plotDataConfig, int pointLimit, int tickLimit);
	// 소멸자
	~PlotDataHolder();

	// 매개변수와 데이터 메모리 주소 공유
	void SetPointArraySync(std::vector<DataInfo>** dataInfo, std::vector<Vector3>** normalizedPoints, std::vector<Vector3>** localPoints, std::vector<Vector3>** projectedPoints);

	// 저장된 플롯 데이터 초기화
	void ClearData();
	// 데이터 저장
	void SetPlotData(float* x, float* y, int axis, int nPoints);
	void SetPlotData(float* x, float* y, int nPoints);
	void SetPlotData(Vector3* points, int nPoints);
	void SetPlotData(Vector3* startPoints, Vector3* endPoints, int nPoints);
	void SetPlotData(float* xMap, float* yMap, float* zMap, int nRow, int nCol);
	void SetFillData(float* xMap, float* yMap, float* zMap, int nRow, int nCol);
	void DataSetDone();
	void GetPointData(DataInfo* dataInfo, Vector3* localPoints, Vector3* normalizedPoints, Vector3* projectedPoints, int* nPoints);

	// _normalizedPoints, _localPoints, _projectedPoints 계산 및 axis, grid 데이터 추가
	void GetPointsData();
	void GetTextInfo(Vector3* positions, int* textAxis, float* textContents, int* nCount);

	Gradient gradient;
	
private:
	Vector2 GetMinMaxValueFromData(std::vector<Vector3> data, int axis);
	void    NormalizedPosToLocalPos(Vector2 rectSize, RangeInfo rangeInfo);
	void    ApplyRotationToData(Vector3 rotEuler);
	void    GetProjectedPos(float zProjectionScale);

	AxisDataHandler* _axisDataHandler;
	PlotDataHandler* _plotDataHandler;

	// Configs
	PlotShapeConfig* _plotShapeConfig;
	PlotDataConfig*  _plotDataConfig;
	int              _pointLimit;

	std::vector<DataInfo> _dataInfo;
	std::vector<Vector3>  _plotData;
	std::vector<Vector3>  _normalizedPoints;
	std::vector<Vector3>  _localPoints;
	std::vector<Vector3>  _projectedPoints;

	// Grid Info
	Vector3*  _textPositions;
	int*      _textAxis;
	float*    _textContents;
	int       _textCount;

	bool      _getDataDone = true;
	bool      _isDataSetDone = true;
	bool      _isDataCopyDone = true;
};