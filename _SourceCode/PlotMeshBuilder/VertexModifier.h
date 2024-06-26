#pragma once
#include "pch.h"
#include "StructManager.h"

// �־��� ���� ���� ���� �ִ��� Ȯ��
bool IsPointInRange(Vector3 pt, RangeInfo range);

// �־��� ���� �ﰢ�� ���� �ִ��� Ȯ��
bool IsPointInTriangle(std::vector<Vector3> trianglePoints, Vector3 pt);

// �־��� ���� ���� ���� �����ϵ� ���⸦ ����
Vector3 AdjustPointToBoxKeepingSlope(const Vector3& pt, const Vector3& prevPt, RangeInfo range);

// �� ���� ������ �� �ִ��� Ȯ��
bool AreCollinear(const Vector3& p1, const Vector3& p2, const Vector3& p3);

// �� P�� ���� AB ���� �ִ��� Ȯ��
bool IsPointOnLineSegment(const Vector3& A, const Vector3& B, const Vector3& P);

// ������ ���Ե��� �ʴ� �־��� �� �� ������ �� Ž��
std::vector<Vector3> AddPointsToBoxKeepingSlope(const Vector3& pt, const Vector3& prevPt, RangeInfo range);