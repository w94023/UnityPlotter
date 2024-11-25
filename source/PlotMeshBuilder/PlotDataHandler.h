#pragma once
#include "pch.h"
#include "StructManager.h"
#include "DataHandler.h"

#include <algorithm>

class PlotDataHandler : public DataHandler
{
public:
	PlotDataHandler(int pointLimit, std::vector<Vector3>* plotData, std::vector<DataInfo>* dataInfo):
		DataHandler(pointLimit, plotData, dataInfo) {}

	void SetPlotData(float* x, float* y, int axis, int nPoints, PlotDataConfig* config)
	{
		// Bar data 생성
		if (nPoints == 0) return;
		if (axis != 0 && axis != 1) return;

		int dataCount = nPoints;

		std::vector<std::vector<Vector3>> plotData;

		for (int i = 0; i < dataCount; i++) {
			std::vector<Vector3> plotDataTemp;
			plotDataTemp.push_back(Vector3());
			plotDataTemp.push_back(Vector3());
			plotDataTemp[0][axis] = x[i];
			plotDataTemp[0][(axis + 1) - (axis + 1) / 2 * 2] = -std::numeric_limits<float>::infinity();
			plotDataTemp[1][axis] = x[i];
			plotDataTemp[1][(axis + 1) - (axis + 1) / 2 * 2] = y[i];
			plotData.push_back(plotDataTemp);
			
			//plotData[i][0] = Vector3();
			//plotData[i][0][axis] = x[i];
			//plotData[i][0][(axis + 1) - (axis + 1) / 2 * 2] = -std::numeric_limits<float>::infinity();
			//plotData[i][1] = Vector3();
			//plotData[i][1][axis] = x[i];
			//plotData[i][1][(axis + 1) - (axis + 1) / 2 * 2] = y[i];
		}

		FlattenLinkedData(plotData, 1, dataCount, PlotData, config->color, config->radius, false);
	}

	void SetPlotData(float* x, float* y, int nPoints, PlotDataConfig* config)
	{
		// Line data 생성
		if (nPoints < 2) return;

		int linkCount = nPoints-1;
		int dataCount = 1;

		std::vector<std::vector<Vector3>> plotData;
		std::vector<Vector3> plotDataTemp;

		int targetDataCount = _pointLimit - plotData.size();

		// 데이터 압축 off
		if (nPoints <= targetDataCount) {
			for (int i = 0; i < nPoints; i++) {
				plotDataTemp.push_back(Vector3(x[i], y[i], 0.0f));
			}
			 
			//plotData[0] = new Vector3[nPoints];

			//for (int i = 0; i < nPoints; i++) {
			//	plotData[0][i].x = x[i];
			//	plotData[0][i].y = y[i];
			//}
		}
		// 데이터 압축 on
		else {
			float* xTemp = new float[targetDataCount];
			float* yTemp = new float[targetDataCount];
			int dataCount = DataCompress(xTemp, yTemp, x, y, nPoints, targetDataCount);

			for (int i = 0; i < dataCount; i++) {
				plotDataTemp.push_back(Vector3(xTemp[i], xTemp[i], 0.0f));
			}

			//plotData[0] = new Vector3[dataCount];
			//for (int i = 0; i < dataCount; i++) {
			//	plotData[0][i].x = xTemp[i];
			//	plotData[0][i].y = yTemp[i];
			//}

			delete[] xTemp;
			delete[] yTemp;
		}

		plotData.push_back(plotDataTemp);
		FlattenLinkedData(plotData, linkCount, dataCount, PlotData, config->color, config->radius, false);
	}

	void SetPlotData(Vector3* points, int nPoints, PlotDataConfig* config)
	{
		if (nPoints == 0) return;

		int linkCount = 0;
		int dataCount = nPoints;

		std::vector<std::vector<Vector3>> plotData;

		for (int i = 0; i < nPoints; i++) {
			std::vector<Vector3> plotDataTemp;
			plotDataTemp.push_back(points[i]);
			plotData.push_back(plotDataTemp);
		}

		FlattenLinkedData(plotData, linkCount, dataCount, PlotData, config->color, config->radius, false);
	}

	void SetPlotData(Vector3* startPoints, Vector3* endPoints, int nPoints, PlotDataConfig* config)
	{
		if (nPoints == 0) return;

		int linkCount = 1;
		int dataCount = nPoints;

		std::vector<std::vector<Vector3>> plotData;

		for (int i = 0; i < nPoints; i++) {
			std::vector<Vector3> plotDataTemp;
			plotDataTemp.push_back(startPoints[i]);
			plotDataTemp.push_back(endPoints[i]);
			plotData.push_back(plotDataTemp);
			//plotData[i] = new Vector3[2];
			//plotData[i][0] = startPoints[i];
			//plotData[i][1] = endPoints[i];
		}

		FlattenLinkedData(plotData, linkCount, dataCount, PlotData, config->color, config->radius, false);
	}

	void SetPlotData(float* xMap, float* yMap, float* zMap, int nRow, int nCol, PlotDataConfig* config)
	{
		if (nRow < 2) return;
		if (nCol < 2) return;

		int linkCount = 2;
		int dataCount = (nRow - 1) * (nCol - 1);

		std::vector<std::vector<Vector3>> plotData;

		//int count = 0;
		for (int i = 0; i < nRow - 1; i++) {
			for (int j = 0; j < nCol - 1; j++) {
				std::vector<Vector3> plotDataTemp;

				// 원점 + (col + 1)
				plotDataTemp.push_back(Vector3(
					xMap[nCol * i + j + 1],
					yMap[nCol * i + j + 1],
					zMap[nCol * i + j + 1]
				));

				// 원점
				plotDataTemp.push_back(Vector3(
					xMap[nCol * i + j],
					yMap[nCol * i + j],
					zMap[nCol * i + j]
				));

				// 원점 + (row + 1)
				plotDataTemp.push_back(Vector3(
					xMap[nCol * (i + 1) + j],
					yMap[nCol * (i + 1) + j],
					zMap[nCol * (i + 1) + j]
				));

				plotData.push_back(plotDataTemp);

				//plotData[count] = new Vector3[linkCount + 1];
				
				//// 원점 + (col + 1)
				//plotData[count][0].x = xMap[nCol * i + j + 1];
				//plotData[count][0].y = yMap[nCol * i + j + 1];
				//plotData[count][0].z = zMap[nCol * i + j + 1];

				//// 원점
				//plotData[count][1].x = xMap[nCol * i + j];
				//plotData[count][1].y = yMap[nCol * i + j];
				//plotData[count][1].z = zMap[nCol * i + j];

				//// 원점 + (row + 1)
				//plotData[count][2].x = xMap[nCol * (i + 1) + j];
				//plotData[count][2].y = yMap[nCol * (i + 1) + j];
				//plotData[count][2].z = zMap[nCol * (i + 1) + j];

				//count++;
			}
		}

		FlattenLinkedData(plotData, linkCount, dataCount, MapData, config->color, config->radius, false);

		linkCount = 1;
		dataCount = nRow - 1;
		std::vector<std::vector<Vector3>> verticalLineData;
		
		//count = 0;
		for (int i = 0; i < nRow - 1; i++) {
			std::vector<Vector3> plotDataTemp;

			// 원점
			plotDataTemp.push_back(Vector3(
				xMap[nCol * i + nCol - 1],
				yMap[nCol * i + nCol - 1],
				zMap[nCol * i + nCol - 1]
			));

			// 원점 + (row + 1)
			plotDataTemp.push_back(Vector3(
				xMap[nCol * (i + 1) + nCol - 1],
				yMap[nCol * (i + 1) + nCol - 1],
				zMap[nCol * (i + 1) + nCol - 1]
			));

			verticalLineData.push_back(plotDataTemp);

			//verticalLineData[count] = new Vector3[linkCount + 1];

			//// 원점
			//verticalLineData[count][0].x = xMap[nCol * i + nCol - 1];
			//verticalLineData[count][0].y = yMap[nCol * i + nCol - 1];
			//verticalLineData[count][0].z = zMap[nCol * i + nCol - 1];

			//// 원점 + (row + 1)
			//verticalLineData[count][1].x = xMap[nCol * (i + 1) + nCol - 1];
			//verticalLineData[count][1].y = yMap[nCol * (i + 1) + nCol - 1];
			//verticalLineData[count][1].z = zMap[nCol * (i + 1) + nCol - 1];

			//count++;
		}

		FlattenLinkedData(verticalLineData, linkCount, dataCount, MapData, config->color, config->radius, false);

		linkCount = 1;
		dataCount = nCol - 1;
		std::vector<std::vector<Vector3>> horizontalLineData;
		
		//count = 0;
		for (int i = 0; i < nCol - 1; i++) {
			std::vector<Vector3> plotDataTemp;

			// 원점
			plotDataTemp.push_back(Vector3(
				xMap[nCol * (nRow - 1) + i],
				yMap[nCol * (nRow - 1) + i],
				zMap[nCol * (nRow - 1) + i]
			));

			// 원점 + (col + 1)
			plotDataTemp.push_back(Vector3(
				xMap[nCol * (nRow - 1) + i + 1],
				yMap[nCol * (nRow - 1) + i + 1],
				zMap[nCol * (nRow - 1) + i + 1]
			));

			horizontalLineData.push_back(plotDataTemp);

			//horizontalLineData[count] = new Vector3[linkCount + 1];

			//// 원점
			//horizontalLineData[count][0].x = xMap[nCol * (nRow - 1) + i];
			//horizontalLineData[count][0].y = yMap[nCol * (nRow - 1) + i];
			//horizontalLineData[count][0].z = zMap[nCol * (nRow - 1) + i];

			//// 원점 + (col + 1)
			//horizontalLineData[count][1].x = xMap[nCol * (nRow - 1) + i + 1];
			//horizontalLineData[count][1].y = yMap[nCol * (nRow - 1) + i + 1];
			//horizontalLineData[count][1].z = zMap[nCol * (nRow - 1) + i + 1];

			//count++;
		}

		FlattenLinkedData(horizontalLineData, linkCount, dataCount, MapData, config->color, config->radius, false);
	}

	void SetFillData(float* xMap, float* yMap, float* zMap, int nRow, int nCol, PlotDataConfig* config)
	{
		if (nRow < 2) return;
		if (nCol < 2) return;

		int linkCount = 3;
		int dataCount = (nRow - 1) * (nCol - 1);

		std::vector<std::vector<Vector3>> plotData;

		//int count = 0;
		for (int i = 0; i < nRow - 1; i++) {
			for (int j = 0; j < nCol - 1; j++) {
				std::vector<Vector3> plotDataTemp;

				// 원점
				plotDataTemp.push_back(Vector3(
					xMap[nCol * i + j],
					yMap[nCol * i + j],
					zMap[nCol * i + j]
				));

				// 원점 + (row + 1)
				plotDataTemp.push_back(Vector3(
					xMap[nCol * (i + 1) + j],
					yMap[nCol * (i + 1) + j],
					zMap[nCol * (i + 1) + j]
				));

				// 원점 + (row + 1) + (col + 1)
				plotDataTemp.push_back(Vector3(
					xMap[nCol * (i + 1) + j + 1],
					yMap[nCol * (i + 1) + j + 1],
					zMap[nCol * (i + 1) + j + 1]
				));

				// 원점 + (col + 1)
				plotDataTemp.push_back(Vector3(
					xMap[nCol * i + j + 1],
					yMap[nCol * i + j + 1],
					zMap[nCol * i + j + 1]
				));

				plotData.push_back(plotDataTemp);
				//plotData[count] = new Vector3[linkCount + 1];

				//// 원점
				//plotData[count][0].x = xMap[nCol * i + j];
				//plotData[count][0].y = yMap[nCol * i + j];
				//plotData[count][0].z = zMap[nCol * i + j];

				//// 원점 + (row + 1)
				//plotData[count][1].x = xMap[nCol * (i + 1) + j];
				//plotData[count][1].y = yMap[nCol * (i + 1) + j];
				//plotData[count][1].z = zMap[nCol * (i + 1) + j];

				//// 원점 + (row + 1) + (col + 1)
				//plotData[count][2].x = xMap[nCol * (i + 1) + j + 1];
				//plotData[count][2].y = yMap[nCol * (i + 1) + j + 1];
				//plotData[count][2].z = zMap[nCol * (i + 1) + j + 1];

				//// 원점 + (col + 1)
				//plotData[count][3].x = xMap[nCol * i + j + 1];
				//plotData[count][3].y = yMap[nCol * i + j + 1];
				//plotData[count][3].z = zMap[nCol * i + j + 1];

				//count++;
			}
		}

		FlattenLinkedData(plotData, linkCount, dataCount, MapData, config->color, config->radius, true);
		//if (nRow < 2) return;
		//if (nCol < 2) return;

		//int linkCount = 2;
		//int dataCount = (nRow - 1) * (nCol - 1) * 2;

		//Vector3** plotData = new Vector3*[dataCount];

		//int count = 0;
		//for (int i = 0; i < nRow - 1; i++) {
		//	for (int j = 0; j < nCol - 1; j++) {
		//		plotData[count] = new Vector3[linkCount + 1];

		//		// 원점
		//		plotData[count][0].x = xMap[nCol * i + j];
		//		plotData[count][0].y = yMap[nCol * i + j];
		//		plotData[count][0].z = zMap[nCol * i + j];

		//		// 원점 + (row + 1)
		//		plotData[count][1].x = xMap[nCol * (i + 1) + j];
		//		plotData[count][1].y = yMap[nCol * (i + 1) + j];
		//		plotData[count][1].z = zMap[nCol * (i + 1) + j];

		//		// 원점 + (row + 1) + (col + 1)
		//		plotData[count][2].x = xMap[nCol * (i + 1) + j + 1];
		//		plotData[count][2].y = yMap[nCol * (i + 1) + j + 1];
		//		plotData[count][2].z = zMap[nCol * (i + 1) + j + 1];

		//		count++;

		//		plotData[count] = new Vector3[linkCount + 1];

		//		// 원점
		//		plotData[count][0].x = xMap[nCol * i + j];
		//		plotData[count][0].y = yMap[nCol * i + j];
		//		plotData[count][0].z = zMap[nCol * i + j];

		//		// 원점 + (col + 1)
		//		plotData[count][1].x = xMap[nCol * i + j + 1];
		//		plotData[count][1].y = yMap[nCol * i + j + 1];
		//		plotData[count][1].z = zMap[nCol * i + j + 1];

		//		// 원점 + (row + 1) + (col + 1)
		//		plotData[count][2].x = xMap[nCol * (i + 1) + j + 1];
		//		plotData[count][2].y = yMap[nCol * (i + 1) + j + 1];
		//		plotData[count][2].z = zMap[nCol * (i + 1) + j + 1];

		//		count++;
		//	}
		//}

		//FlattenLinkedData(plotData, linkCount, dataCount, MapData, config->color, config->radius, true);
	}

private:
	int DataCompress(float* xOut, float* yOut, float* xIn, float* yIn, int nCount, int targetCount)
	{
		std::vector<IndexedFloat> indexedValues(nCount);

		for (int i = 0; i < targetCount; ++i) {
			float* value = new float();
			int* index = new int();

			if (i == 0) *value = -1.0f;
			else        *value = sqrt((xIn[i]-xIn[i-1])*(xIn[i]-xIn[i-1]) + (yIn[i]-yIn[i-1])*(yIn[i]-yIn[i-1]));
			indexedValues[i].value = value;
				
			*index = i;
			indexedValues[i].originalIndex = index;
		}

		// 이전 점으로부터의 변위를 내림차순으로 정렬
		std::sort(indexedValues.begin(), indexedValues.end(), 
			[](const IndexedFloat& a, const IndexedFloat& b) { return *a.value > *b.value; } // value 멤버의 내림차순 정렬 람다식
		);
		// _pointLimit - _dataCount 범위 내의 데이터를 인덱스의 오름차순으로 정렬
		std::sort(indexedValues.begin(), indexedValues.begin() + (targetCount - 2) / 2, 
			[](const IndexedFloat& a, const IndexedFloat& b) { return *a.originalIndex < *b.originalIndex;} // originalIndex 멤버의 오름차순 정렬 람다식
		);

		// 데이터 시작점 저장
		xOut[0] = xIn[0];
		yOut[0] = yIn[0];

		int count = 1;
		int previousIndex = 0;
		// 최대 targetDataCount, 최소 (targetDataCount - 2) / 2 + 2개의 데이터 개수가 반환됨
		for (int i = 0; i < (targetCount - 2) / 2; i++) {
			int index = *indexedValues[i].originalIndex;

			// 이전 데이터의 인덱스가 현재 인덱스-1 값이 아닐 경우
			// 데이터 변화의 시작점이 반영되지 않을 수 있으므로 인덱스-1 지점의 데이터도 포함시킴
			if (index - 1 != previousIndex && index > 0) {
				xOut[count] = xIn[index - 1];
				yOut[count] = yIn[index - 1];
				count++;
			}
				
			xOut[count] = xIn[index];
			yOut[count] = yIn[index];
			count++;

			previousIndex = index;
		}

		// 데이터 종료점 저장
		xOut[count] = xIn[nCount - 1];
		yOut[count] = yIn[nCount - 1];
		count++;

		// 메모리 삭제
		for (int i = 0; i < nCount; i++) {
			delete indexedValues[i].value;
			delete indexedValues[i].originalIndex;
		}

		return count;
	}
};