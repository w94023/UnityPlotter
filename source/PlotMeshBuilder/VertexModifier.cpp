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
	// �ﰢ���� �� ���� ���� ���� ����
	Vector3 v1v2 = trianglePoints[1] - trianglePoints[0];
	Vector3 v2v3 = trianglePoints[2] - trianglePoints[1];
	Vector3 v3v1 = trianglePoints[0] - trianglePoints[2];
	v1v2.z = 0.0f;
	v2v3.z = 0.0f;
	v3v1.z = 0.0f;

	// ���� �������� ���� ����
	Vector3 pv1 = pt - trianglePoints[0];
	Vector3 pv2 = pt - trianglePoints[1];
	Vector3 pv3 = pt - trianglePoints[2];
	pv1.z = 0.0f;
	pv2.z = 0.0f;
	pv3.z = 0.0f;

	// �ﰢ�� ����� ���� ����
	Vector3 normal = Vector3::Cross(v1v2, v2v3);

	// ���� ������ �̿��Ͽ� ���� �ﰢ�� ���ο� �ִ��� �˻�
	float d1 = Vector3::Dot(Vector3::Cross(pv1, v1v2), normal);
	float d2 = Vector3::Dot(Vector3::Cross(pv2, v2v3), normal);
	float d3 = Vector3::Dot(Vector3::Cross(pv3, v3v1), normal);

	// ��� d1, d2, d3�� ������ ��ȣ�� ������ ���� �ﰢ�� ���ο� ����
	// 0 ������ �� ��赵 ���Ե�
	return (d1 > 0 && d2 > 0 && d3 > 0) || (d1 < 0 && d2 < 0 && d3 < 0);
}

Vector3 AdjustPointToBoxKeepingSlope(const Vector3& pt, const Vector3& prevPt, RangeInfo range)
{
	// �����
	Vector3 clamped = pt;

	// ���� ���
	float dx = pt.x - prevPt.x;
	float dy = pt.y - prevPt.y;

	if (dx == 0) { // ���� ��
		clamped.y = (dy > 0) ? range.yLimit[1] : range.yLimit[0];
		clamped.x = pt.x;
		return clamped;
	}
	else if (dy == 0) { // ���� ��
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
	// �� P�� ���� AB ���� ��������, ���� �� ������ ��� �����Ǿ�� �մϴ�:
	// 1. P, A, B�� ������ �� �־�� �մϴ� (������ �̿��� �ݸ��Ͼ� �˻�).
	// 2. P�� x��ǥ�� A�� B�� x��ǥ ���̿� �־�� �մϴ�.
	// 3. P�� y��ǥ�� A�� B�� y��ǥ ���̿� �־�� �մϴ�.
    
	// 1�� �˻� ���� (�ش� �ڵ尡 ����� �������� ���� > ���� �ʿ�)
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
	// ��� ��
	std::vector<Vector3> result;

	// ���� ���
	float dx = pt.x - prevPt.x;
	float dy = pt.y - prevPt.y;

	if (dx == 0) { // ���� ��
		if (pt.x <= range.xLimit[0] || pt.x >= range.xLimit[1]) return result;
		if (pt.y > range.yLimit[1] && prevPt.y < range.yLimit[0] || pt.y < range.yLimit[0] && prevPt.y > range.yLimit[1]) {
			result.push_back(Vector3(pt.x, range.yLimit[0], 0.0f));
			result.push_back(Vector3(pt.x, range.yLimit[1], 0.0f));
			return result;
		}
	}
	else if (dy == 0) { // ���� ��
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
			// x ����� �������� �������� ����
			std::sort(result.begin(), result.end(), [](const Vector3& a, const Vector3& b) {
				return a.x < b.x;
			});
		}
		else {
			// x ����� �������� �������� ����
			std::sort(result.begin(), result.end(), [](const Vector3& a, const Vector3& b) {
				return a.x > b.x;
			});
		}
			
		return result;
	}
}
