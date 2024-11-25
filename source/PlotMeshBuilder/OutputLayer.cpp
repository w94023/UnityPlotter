#pragma once
#include "pch.h"
#include "PlotHelper.h"
#include <map>

#include <iostream>
#include <windows.h>

std::map<int, PlotHelper*> _plotHelper;

extern "C" {
    __declspec(dllexport) void OpenConsoleAndPrint() 
    {
        // 콘솔 창 생성
        AllocConsole();

        // 표준 출력을 콘솔 창으로 리다이렉션 (freopen_s 사용)
        FILE* stream;
        errno_t err = freopen_s(&stream, "CONOUT$", "w", stdout);

        if (err != 0) {
            std::cerr << "Error reopening stdout." << std::endl;
        }

        // 사용자 입력 대기 (옵션)
        std::cin.get();
    }

    __declspec(dllexport) void StackArray(float* arr, int nCount, float value)
    {
        for (int i = 0; i < nCount - 1; i++) {
            arr[i] = arr[i + 1];
        }
        arr[nCount - 1] = value;
    }

    __declspec(dllexport) void GenerateXData(float* x, int startIndex, int nCount)
    {
        if (nCount == 0) return;
        int count = 0;
        for (int i = startIndex; i < startIndex + nCount; i++) {
            x[count] = (float)i;
            count++;
        }
    }

    __declspec(dllexport) bool CreateHandle(int handle, int pointLimit, int vertexLimit, int tickLimit)
    {
        if (_plotHelper.find(handle) != _plotHelper.end()) {
			return false;
		}
        else {
            std::cout << "New thread started : " << handle << std::endl;
            _plotHelper[handle] = new PlotHelper(pointLimit, vertexLimit, tickLimit);
            return true;
        }
    }

    __declspec(dllexport) void StopThread(int handle)
    {
        if (_plotHelper.find(handle) != _plotHelper.end()) { 
            _plotHelper[handle]->StopThread();
		}
    }

    __declspec(dllexport) void DeleteHandle(int handle)
    {
        if (_plotHelper.find(handle) != _plotHelper.end()) { 
			delete _plotHelper[handle];
			_plotHelper.erase(handle);
		}
    }

    __declspec(dllexport) void RegisterEventCallback(int handle, EventCallback callback)
    {
        if (_plotHelper.find(handle) != _plotHelper.end()) { 
			_plotHelper[handle]->RegisterEventCallback(callback);
		}
    }

    __declspec(dllexport) void SetPlotShapeConfig(int handle, PlotShapeConfig config)
    {
        if (_plotHelper.find(handle) != _plotHelper.end()) { 
			_plotHelper[handle]->SetPlotShapeConfig(config);
		}
    }

    __declspec(dllexport) void SetPlotDataConfig(int handle, PlotDataConfig config)
    {
        if (_plotHelper.find(handle) != _plotHelper.end()) { 
			_plotHelper[handle]->SetPlotDataConfig(config);
		}
    }

    __declspec(dllexport) void SetGradientConfig(int handle, GradientColorKey* gradientColorKeys, int colorCount, GradientAlphaKey* gradientAlphaKeys, int alphaCount)
    {
        Gradient gradient = Gradient();
        gradient.SetColorKeys(gradientColorKeys, colorCount);
        gradient.SetAlphaKeys(gradientAlphaKeys, alphaCount);

        if (_plotHelper.find(handle) != _plotHelper.end()) { 
			_plotHelper[handle]->SetGradient(gradient);
		}
    }

    __declspec(dllexport) void FloatArrayToVectorArray(Vector3* vectorArr, float* x, float* y, int nCount)
    {
        for (int i = 0; i < nCount; i++) {
            vectorArr[i].x = x[i];
            vectorArr[i].y = y[i];
        }
    }

    __declspec(dllexport) void PlotDataSetStart(int handle)
    {
        if (_plotHelper.find(handle) != _plotHelper.end()) { 
			_plotHelper[handle]->PlotDataSetStart();
		}  
    }

    __declspec(dllexport) void SetFloatArrayDataForBarPlot(int handle, float* x, float *y, int axis, int nPoints)
    {
        if (_plotHelper.find(handle) != _plotHelper.end()) { 
			_plotHelper[handle]->SetPlotData(x, y, axis, nPoints);
		}  
    }
    
    __declspec(dllexport) void SetFloatArrayDataForLinePlot(int handle, float* x, float *y, int nPoints)
    {
        if (_plotHelper.find(handle) != _plotHelper.end()) { 
			_plotHelper[handle]->SetPlotData(x, y, nPoints);
		}  
    }

    __declspec(dllexport) void SetSingleVectorData(int handle, Vector3* points, int nPoints)
    {
        if (_plotHelper.find(handle) != _plotHelper.end()) { 
			_plotHelper[handle]->SetPlotData(points, nPoints);
		}  
    }

    __declspec(dllexport) void SetDoubleVectorData(int handle, Vector3* startPoints, Vector3* endPoints, int nPoints)
    {
        if (_plotHelper.find(handle) != _plotHelper.end()) { 
			_plotHelper[handle]->SetPlotData(startPoints, endPoints, nPoints);
		}  
    }

    __declspec(dllexport) void SetMapDataForPlot(int handle, float* xMap, float* yMap, float* zMap, int nRow, int nCol)
    {
        if (_plotHelper.find(handle) != _plotHelper.end()) { 
			_plotHelper[handle]->SetPlotData(xMap, yMap, zMap, nRow, nCol);
		} 
    }

    __declspec(dllexport) void SetMapDataForFill(int handle, float* xMap, float* yMap, float* zMap, int nRow, int nCol)
    {
        if (_plotHelper.find(handle) != _plotHelper.end()) { 
			_plotHelper[handle]->SetFillData(xMap, yMap, zMap, nRow, nCol);
		} 
    }

    __declspec(dllexport) void PlotDataSetDone(int handle)
    {
        if (_plotHelper.find(handle) != _plotHelper.end()) { 
			_plotHelper[handle]->PlotDataSetDone();
		}  
    }

    __declspec(dllexport) void SetMeshDataMemorySync(int handle, Vector3* vertices, Color* colors, int* triangles, int* nCount)
    {
        if (_plotHelper.find(handle) != _plotHelper.end()) { 
		    _plotHelper[handle]->SetMeshDataMemorySync(vertices, colors, triangles, nCount);
	    }
    }

    __declspec(dllexport) void SetTextInfoMemorySync(int handle, Vector3* positions, int* axis, float* contents, int* nCount)
    {
        if (_plotHelper.find(handle) != _plotHelper.end()) { 
		    _plotHelper[handle]->SetTextInfoMemorySync(positions, axis, contents, nCount);
	    }
    }

    __declspec(dllexport) void DataTransferred(int handle)
    {
        if (_plotHelper.find(handle) != _plotHelper.end()) { 
		    _plotHelper[handle]->DataTransferred();
	    }
    }
}