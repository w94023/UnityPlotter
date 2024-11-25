#pragma once
#include "pch.h"
#include "PlotHelper.h"

PlotHelper::PlotHelper(int pointLimit, int vertexLimit, int tickLimit)
{
	_plotDataHolder = new PlotDataHolder(&_globalPlotShapeConfig, &_plotDataConfig, pointLimit, tickLimit);
	_meshGenerator = new MeshGenerator(_plotDataHolder, &_globalPlotShapeConfig, &_plotDataConfig, pointLimit, vertexLimit);
	//_lck = std::unique_lock<std::mutex>(_mtx);

	//std::thread _thread([this]() {
	//	while(!_stopBackgroundThread) {
	//		//_cv.wait(_lck, [this]{ return _isDataSetDone; });

	//		//_isMeshBuilderBusy = true;

	//		//bool isDataRefreshed = _isDataRefreshed;
	//		//bool isConfigChanged = !_localPlotShapeConfig.IsShapeNotChanged(_globalPlotShapeConfig);
	//		//if (!isDataRefreshed && !isConfigChanged) continue;

	//		//_localPlotShapeConfig = _globalPlotShapeConfig;
	//		//_meshGenerator->BuildMesh();
	//		//_meshGenerator->GetMeshData(_vertices, _colors, _triangles, _nCount);
	//		//_plotDataHolder->GetTextInfo(_textPositions, _textContents, _textCount);

	//		//_isDataTransferred = false;
	//		//if (_onDataReady) _onDataReady();
	//		//_cv.wait(_lck, [this]{ return _isDataTransferred; });

	//		//_isMeshBuilderBusy = false;

	//		//// BuildMesh 중간에 DataRefresh 요청이 들어온 경우
	//		//if (!isDataRefreshed && _isDataRefreshed) _isDataRefreshed = true;
	//		//else                                      _isDataRefreshed = false;
	//	}

	//	_isThreadStopped = true;
	//	_cv.notify_all();
	//});
	//_thread.detach();
}

PlotHelper::~PlotHelper()
{
	delete _plotDataHolder;
	delete _meshGenerator;
	if (_isThreadStopped) return;
	StopThread();
}

void PlotHelper::StopThread()
{
	std::thread _thread([this]() {
		while (!_isMeshBuildDone) {
			Sleep(1);
			if (_isMeshBuildDone) break;
		}
	});
	_thread.join();
}


void PlotHelper::RegisterEventCallback(EventCallback callback)
{
	_onDataReady = callback;
}

void PlotHelper::SetPlotShapeConfig(PlotShapeConfig config)
{
	_globalPlotShapeConfig = config;
	if (!_isPlotShapeConfigInitialized) {
		_localPlotShapeConfig = _globalPlotShapeConfig;
		_isPlotShapeConfigInitialized = true;

		std::cout << "Rectsize : " << _localPlotShapeConfig.rectSize.x << "," << _localPlotShapeConfig.rectSize.y << std::endl;
		std::cout << "rotEuler : " << _localPlotShapeConfig.rotEuler.x << "," << _localPlotShapeConfig.rotEuler.y << "," << _localPlotShapeConfig.rotEuler.z << std::endl;
	}

	bool isConfigChanged = !_localPlotShapeConfig.IsShapeNotChanged(_globalPlotShapeConfig);

	if (isConfigChanged) {
		_localPlotShapeConfig = _globalPlotShapeConfig;
		std::cout << "Rectsize : " << _localPlotShapeConfig.rectSize.x << "," << _localPlotShapeConfig.rectSize.y << std::endl;
		std::cout << "rotEuler : " << _localPlotShapeConfig.rotEuler.x << "," << _localPlotShapeConfig.rotEuler.y << "," << _localPlotShapeConfig.rotEuler.z << std::endl;
		MeshBuildRequest();
	}
}

void PlotHelper::SetPlotDataConfig(PlotDataConfig config)
{
	_plotDataConfig = config;
}

void PlotHelper::SetGradient(Gradient gradient)
{
	_plotDataHolder->gradient = gradient;
}

void PlotHelper::PlotDataSetStart()
{
	_plotDataHolder->ClearData();
}

void PlotHelper::SetPlotData(float* x, float* y, int axis, int nPoints)
{
	_plotDataHolder->SetPlotData(x, y, axis, nPoints);
}

void PlotHelper::SetPlotData(float* x, float* y, int nPoints)
{
	_plotDataHolder->SetPlotData(x, y, nPoints);
}

void PlotHelper::SetPlotData(Vector3* points, int nPoints)
{
	_plotDataHolder->SetPlotData(points, nPoints);
}

void PlotHelper::SetPlotData(Vector3* startPoints, Vector3* endPoints, int nPoints)
{
	_plotDataHolder->SetPlotData(startPoints, endPoints, nPoints);
}

void PlotHelper::SetPlotData(float* xMap, float* yMap, float* zMap, int nRow, int nCol)
{
	_plotDataHolder->SetPlotData(xMap, yMap, zMap, nRow, nCol);
}

void PlotHelper::SetFillData(float* xMap, float* yMap, float* zMap, int nRow, int nCol)
{
	_plotDataHolder->SetFillData(xMap, yMap, zMap, nRow, nCol);
}

void PlotHelper::PlotDataSetDone()
{
	_plotDataHolder->DataSetDone();
	MeshBuildRequest();	
}

void PlotHelper::MeshBuildRequest()
{
	_isDrawRequested = true;
	BuildMesh();
}

void PlotHelper::BuildMesh()
{
	if (!_isDataTransferred) return;
	_isDataTransferred = false;

	_plotDataHolder->GetPointsData();
	_meshGenerator->BuildMesh();
	_meshGenerator->GetMeshData(_vertices, _colors, _triangles, _nCount);
	_plotDataHolder->GetTextInfo(_textPositions, _textAxis, _textContents, _textCount);

	if (_onDataReady) _onDataReady();
	_isDrawRequested = false;
}

void PlotHelper::SetMeshDataMemorySync(Vector3* vertices, Color* colors, int* triangles, int* nCount)
{
	_vertices  = vertices;
	_colors    = colors;
	_triangles = triangles;
	_nCount    = nCount;
}

void PlotHelper::SetTextInfoMemorySync(Vector3* positions, int* textAxis, float* contents, int* nCount)
{
	_textPositions = positions;
	_textAxis      = textAxis;
	_textContents  = contents;
	_textCount     = nCount;
}

void PlotHelper::DataTransferred()
{
	_isDataTransferred = true;
	if (_isDrawRequested) BuildMesh();
}