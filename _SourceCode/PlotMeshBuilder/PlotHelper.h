#pragma once
#include "pch.h"
#include "StructManager.h"
#include "VertexHelper.h"
#include "PlotDataHolder.h"
#include "MeshGenerator.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

class PlotHelper
{
public:
    PlotHelper(int pointLimit, int vertexLimit, int tickLimit);

	~PlotHelper();

	void StopThread();

	void RegisterEventCallback(EventCallback callback);

	void SetPlotShapeConfig(PlotShapeConfig config);

	void SetPlotDataConfig(PlotDataConfig config);

	void SetGradient(Gradient gradient);

	void PlotDataSetStart();

	void SetPlotData(float* x, float* y, int axis, int nPoints);

	void SetPlotData(float* x, float* y, int nPoints);

	void SetPlotData(Vector3* points, int nPoints);

	void SetPlotData(Vector3* startPoints, Vector3* endPoints, int nPoints);

	void SetPlotData(float* xMap, float* yMap, float* zMap, int nRow, int nCol);

	void SetFillData(float* xMap, float* yMap, float* zMap, int nRow, int nCol);

	void PlotDataSetDone();

	void SetMeshDataMemorySync(Vector3* vertices, Color* colors, int* triangles, int* nCount);

	void SetTextInfoMemorySync(Vector3* positions, int* textAxis, float* contents, int* nCount);

	void DataTransferred();

private:
	void MeshBuildRequest();
	void BuildMesh();

	// ������ ������ ���� Ŭ����
	PlotDataHolder*         _plotDataHolder;
	MeshGenerator*          _meshGenerator;

	// Config ����ü
	bool                    _isPlotShapeConfigInitialized = false;
	PlotShapeConfig         _globalPlotShapeConfig;
	PlotShapeConfig         _localPlotShapeConfig;
	PlotDataConfig          _plotDataConfig;

	// �񵿱� ������ ����
	std::mutex                   _mtx;
	std::condition_variable      _cv;
	std::unique_lock<std::mutex> _lck;
	bool                         _stopBackgroundThread = false;
	bool                         _isThreadStopped = true;
	
	// �̺�Ʈ ����
	EventCallback _onDataReady;
	bool _isMeshBuildDone   = true;
	bool _isDrawRequested   = false;
	bool _isDataTransferred = true;

	// ���ؽ� ������
	Vector3*                _vertices;
	Color*                  _colors;
	int*                    _triangles;
	int*                    _nCount;

	// �׸��� ������ ���� �ؽ�Ʈ ������
	Vector3*                _textPositions;
	int*                    _textAxis;
	float*                  _textContents;
	int*                    _textCount;
};