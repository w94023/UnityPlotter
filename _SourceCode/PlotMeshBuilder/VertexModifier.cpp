#pragma once
#include "pch.h"
#include "VertexModifier.h"
#include <vector>
#include <algorithm>

bool IsPointInRange(Vector3 pt, RangeInfo range)
{
	return (pt.x >= range.xLimit[0] && pt.x <= range.xLimit[1] &&
			pt.y >= range.yLimit[0] && pt.y <= range.yLimit[1]);
}

bool IsPointInTriangle(std::vector<Vector3> trianglePoints, Vector3 pt)
{
	// 삼각형의 각 변에 대한 벡터 생성
	Vector3 v1v2 = trianglePoints[1] - trianglePoints[0];
	Vector3 v2v3 = trianglePoints[2] - trianglePoints[1];
	Vector3 v3v1 = trianglePoints[0] - trianglePoints[2];
	v1v2.z = 0.0f;
	v2v3.z = 0.0f;
	v3v1.z = 0.0f;

	// 점과 꼭짓점에 대한 벡터
	Vector3 pv1 = pt - trianglePoints[0];
	Vector3 pv2 = pt - trianglePoints[1];
	Vector3 pv3 = pt - trianglePoints[2];
	pv1.z = 0.0f;
	pv2.z = 0.0f;
	pv3.z = 0.0f;

	// 삼각형 평면의 법선 벡터
	Vector3 normal = Vector3::Cross(v1v2, v2v3);

	// 벡터 외적을 이용하여 점이 삼각형 내부에 있는지 검사
	float d1 = Vector3::Dot(Vector3::Cross(pv1, v1v2), normal);
	float d2 = Vector3::Dot(Vector3::Cross(pv2, v2v3), normal);
	float d3 = Vector3::Dot(Vector3::Cross(pv3, v3v1), normal);

	// 모든 d1, d2, d3가 동일한 부호를 가지면 점은 삼각형 내부에 있음
	// 0 포함할 시 경계도 포함됨
	return (d1 > 0 && d2 > 0 && d3 > 0) || (d1 < 0 && d2 < 0 && d3 < 0);
}

Vector3 AdjustPointToBoxKeepingSlope(const Vector3& pt, const Vector3& prevPt, RangeInfo range)
{
	// 결과값
	Vector3 clamped = pt;

	// 기울기 계산
	float dx = pt.x - prevPt.x;
	float dy = pt.y - prevPt.y;

	if (dx == 0) { // 수직 선
		clamped.y = (dy > 0) ? range.yLimit[1] : range.yLimit[0];
		clamped.x = pt.x;
		return clamped;
	}
	else if (dy == 0) { // 수평 선
		clamped.x = (dx > 0) ? range.xLimit[1] : range.xLimit[0];
		clamped.y = pt.y;
		return clamped;
	}
	else {
		float slope = dy / dx;

		if (pt.x > range.xLimit[1]) {
			clamped.x = range.xLimit[1];
			clamped.y = slope * (clamped.x - prevPt.x) + prevPt.y;
			if (IsPointInRange(clamped, range)) return clamped;
		}
		if (pt.x < range.xLimit[0]) {
			clamped.x = range.xLimit[0];
			clamped.y = slope * (clamped.x - prevPt.x) + prevPt.y;
			if (IsPointInRange(clamped, range)) return clamped;
		}
		if (pt.y > range.yLimit[1]) {
			clamped.y = range.yLimit[1];
			clamped.x = 1 / slope * (clamped.y - prevPt.y) + prevPt.x;
			if (IsPointInRange(clamped, range)) return clamped;
		}
		if (pt.y < range.yLimit[0]) {
			clamped.y = range.yLimit[0];
			clamped.x = 1 / slope * (clamped.y - prevPt.y) + prevPt.x;
			if (IsPointInRange(clamped, range)) return clamped;
		}
	}
}

bool AreCollinear(const Vector3& p1, const Vector3& p2, const Vector3& p3)
{
	float area = (p1.x * (p2.y - p3.y) + p2.x * (p3.y - p1.y) + p3.x * (p1.y - p2.y));
	return area == 0;
}

bool IsPointOnLineSegment(const Vector3& A, const Vector3& B, const Vector3& P)
{
	// 점 P가 선분 AB 위에 있으려면, 다음 세 조건이 모두 만족되어야 합니다:
	// 1. P, A, B가 일직선 상에 있어야 합니다 (면적을 이용한 콜리니어 검사).
	// 2. P의 x좌표가 A와 B의 x좌표 사이에 있어야 합니다.
	// 3. P의 y좌표가 A와 B의 y좌표 사이에 있어야 합니다.
    
	// 1번 검사 생략 (해당 코드가 제대로 동작하지 않음 > 수정 필요)
	//if (!AreCollinear(A, B, P)) {
	//	return false;
	//}
    
	if (P.x >= Min(A.x, B.x) && P.x <= Max(A.x, B.x) &&
		P.y >= Min(A.y, B.y) && P.y <= Max(A.y, B.y)) {
		return true;
	}
	else {
		return false;
	}
}

std::vector<Vector3> AddPointsToBoxKeepingSlope(const Vector3& pt, const Vector3& prevPt, RangeInfo range)
{
	// 결과 값
	std::vector<Vector3> result;

	// 기울기 계산
	float dx = pt.x - prevPt.x;
	float dy = pt.y - prevPt.y;

	if (dx == 0) { // 수직 선
		if (pt.x <= range.xLimit[0] || pt.x >= range.xLimit[1]) return result;
		if (pt.y > range.yLimit[1] && prevPt.y < range.yLimit[0] || pt.y < range.yLimit[0] && prevPt.y > range.yLimit[1]) {
			result.push_back(Vector3(pt.x, range.yLimit[0], 0.0f));
			result.push_back(Vector3(pt.x, range.yLimit[1], 0.0f));
			return result;
		}
	}
	else if (dy == 0) { // 수평 선
		if (pt.y <= range.yLimit[0] || pt.y >= range.yLimit[1]) return result;
		if (pt.x > range.xLimit[1] && prevPt.x < range.xLimit[0] || pt.x < range.xLimit[0] && prevPt.x > range.xLimit[1]) {
			result.push_back(Vector3(range.xLimit[0], pt.y, 0.0f));
			result.push_back(Vector3(range.xLimit[1], pt.y, 0.0f));
			return result;
		}
	}
	else {
		float slope = dy / dx;
		Vector3 testPoint;

		testPoint.x = range.xLimit[0];
		testPoint.y = slope * (testPoint.x - prevPt.x) + prevPt.y;
		if (IsPointOnLineSegment(pt, prevPt, testPoint) && IsPointInRange(testPoint, range)) result.push_back(testPoint);
			
		testPoint.x = range.xLimit[1];
		testPoint.y = slope * (testPoint.x - prevPt.x) + prevPt.y;
		if (IsPointOnLineSegment(pt, prevPt, testPoint) && IsPointInRange(testPoint, range)) result.push_back(testPoint);

		testPoint.y = range.yLimit[0];
		testPoint.x = 1 / slope * (testPoint.y - prevPt.y) + prevPt.x;
		if (IsPointOnLineSegment(pt, prevPt, testPoint) && IsPointInRange(testPoint, range)) result.push_back(testPoint);

		testPoint.y = range.yLimit[1];
		testPoint.x = 1 / slope * (testPoint.y - prevPt.y) + prevPt.x;
		if (IsPointOnLineSegment(pt, prevPt, testPoint) && IsPointInRange(testPoint, range)) result.push_back(testPoint);

		if (dx > 0) {
			// x 멤버를 기준으로 오름차순 정렬
			std::sort(result.begin(), result.end(), [](const Vector3& a, const Vector3& b) {
				return a.x < b.x;
			});
		}
		else {
			// x 멤버를 기준으로 내림차순 정렬
			std::sort(result.begin(), result.end(), [](const Vector3& a, const Vector3& b) {
				return a.x > b.x;
			});
		}
			
		return result;
	}
}
