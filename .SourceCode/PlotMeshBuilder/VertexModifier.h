#pragma once
#include "pch.h"
#include "StructManager.h"

// 주어진 점이 범위 내에 있는지 확인
bool IsPointInRange(Vector3 pt, RangeInfo range);

// 주어진 점이 삼각형 내에 있는지 확인
bool IsPointInTriangle(std::vector<Vector3> trianglePoints, Vector3 pt);

// 주어진 점을 범위 내로 조정하되 기울기를 유지
Vector3 AdjustPointToBoxKeepingSlope(const Vector3& pt, const Vector3& prevPt, RangeInfo range);

// 세 점이 일직선 상에 있는지 확인
bool AreCollinear(const Vector3& p1, const Vector3& p2, const Vector3& p3);

// 점 P가 선분 AB 위에 있는지 확인
bool IsPointOnLineSegment(const Vector3& A, const Vector3& B, const Vector3& P);

// 범위에 포함되지 않는 주어진 두 점 사이의 점 탐색
std::vector<Vector3> AddPointsToBoxKeepingSlope(const Vector3& pt, const Vector3& prevPt, RangeInfo range);