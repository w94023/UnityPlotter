#pragma once
#include "pch.h"
//#include "VertexHelper.h"
//
//Vector3 NormalizedPosToLocalPos(Vector3 normalizedPos, Vector2 rectSizeDelta, Vector2 rectPivot, bool applyPivot)
//{
//    // ���� ������Ʈ�� RectTransform ���� Ȯ��
//    // AnchorMin.x�� AnchorMax.x, AnchorMin.y�� AnchorMin.y�� ���� �ٸ� ���
//    // Strecth��尡 �Ǳ� ������ ��ũ��Ʈ���� ���̿� ���̸� Ȯ���� �� ����
//    // > �̷� ��� �θ� ������Ʈ�� ���̿� ���̷κ��� �����
//
//    Vector3 localPos = Vector3();
//    //Vector2 pivot    = Vector2(0.5f, 0.5f);
//    //if (applyPivot) pivot = rectPivot;
//
//    //localPos.x = Lerp(- rectSizeDelta.x * pivot.x, - rectSizeDelta.x * pivot.x + rectSizeDelta.x, normalizedPos.x);
//    //localPos.y = Lerp(- rectSizeDelta.y * pivot.y, - rectSizeDelta.y * pivot.y + rectSizeDelta.y, normalizedPos.y);
//    //localPos.z = normalizedPos.z;
//    return localPos;
//}
//
//Vector3 LocalPosToNormalizedPos(Vector3 localPos, Vector2 rectSizeDelta, Vector2 rectPivot, bool applyPivot)
//{
//    Vector3 normalizedPos = Vector3();
//    //Vector2 pivot    = Vector2(0.5f, 0.5f);
//    //if (applyPivot) pivot = rectPivot;
//
//    //normalizedPos.x = InverseLerp(- rectSizeDelta.x * pivot.x, - rectSizeDelta.x * pivot.x + rectSizeDelta.x, localPos.x);
//    //normalizedPos.y = InverseLerp(- rectSizeDelta.y * pivot.y, - rectSizeDelta.y * pivot.y + rectSizeDelta.y, localPos.y);
//    //normalizedPos.z = localPos.z;
//    return normalizedPos;
//}
//
//Vector3 GetProjectedPosition(Vector3 position, Vector3 planeRotation)
//{
//    Quaternion quaternionRotation = Quaternion::Euler(planeRotation.x, planeRotation.y, planeRotation.z);
//    Vector3 projectedLocalPosition = quaternionRotation * Vector3(position.x, position.y, position.z);
//
//    return Vector3(projectedLocalPosition.x, projectedLocalPosition.y, projectedLocalPosition.z);
//}
//
//Vector3 ApplyCameraRotation(Vector3 position, Vector3 cameraRotation, bool useZProjection)
//{
//    Vector3 newPos = Vector3(position.x, position.y, 0.0f);
//    if (useZProjection) return GetProjectedPosition(newPos, cameraRotation);
//    else                return GetProjectedPosition(newPos, cameraRotation) + Vector3(0.0f, 0.0f, position.z);
//}
//
//bool CheckRangeOut(Vector2 rectSizeDelta, Vector2 rectLocalPoisition, Vector3 position)
//{
//    Vector2 rectSize = rectSizeDelta;
//    bool isContained = true;
//    float offset = 5.0f;
//    if (std::abs(position.x + rectLocalPoisition.x) > (rectSize.x / 2.0f + offset)) isContained = false;
//    if (std::abs(position.y + rectLocalPoisition.y) > (rectSize.y / 2.0f + offset)) isContained = false;
//
//    return isContained;
//}
//
