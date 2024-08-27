using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityPlotter;

public static class UnityPlotterAddon
{
    public static (int, int) CountDigit(float num)
    {
        if (num >= 1) {
            // 1보다 큰 경우: 자릿수 계산
            int digitCount = (num.ToString().Split('.')[0]).Length;
            return (digitCount, -1);
        }
        else if (num > 0 && num < 1) {
            // 0보다 크고 1보다 작은 경우: 소수점 이하 자릿수 계산
            string numberStr = num.ToString();
            int decimalPlaces = numberStr.Length - numberStr.IndexOf('.') - 1;
            return (-1, decimalPlaces);
        }
        else {
            return (-1, -1);
        }
    }
}

namespace UnityPlotter
{
    public static class ComponentHelper
    {
        public static List<GameObject> GetAllChildren(this GameObject parent)
        {
            List<GameObject> children = new List<GameObject>();
            GetChildrenRecursive(parent, children);
            return children;
        }

        public static void GetChildrenRecursive(GameObject parent, List<GameObject> children)
        {
            foreach (Transform child in parent.transform) {
                children.Add(child.gameObject);
                GetChildrenRecursive(child.gameObject, children);
            }
        }

        public static GameObject Find(this List<GameObject> list, string filename)
        {
            foreach (GameObject gameObject in list) {
                if (gameObject.name == filename) return gameObject;
            }

            return null;
        }

        public static GameObject InitializeObject(this GameObject obj, GameObject parentObject, string name)
        {
            if (obj == null) {
                obj = FindObject(parentObject, name);
                if (obj == null) {
                    obj = CreateEmptyObject(parentObject, name);
                }
            }
            obj.transform.SetAsLastSibling();
            return obj;
        }

        public static GameObject InitializeObject(GameObject parentObject, string name)
        {
            GameObject obj = FindObject(parentObject, name);
            if (obj == null) {
                obj = CreateEmptyObject(parentObject, name);
            }

            obj.transform.SetAsLastSibling();
            return obj;
        }

        public static GameObject CreateEmptyObject(GameObject parentObject, string name)
        {
            GameObject childPrefab = new GameObject(name);
            childPrefab.transform.parent = parentObject.transform;
            childPrefab.transform.localScale = new Vector3(1f, 1f, 1f);
            childPrefab.transform.localPosition = new Vector3(0f, 0f, 0f);
            childPrefab.AddComponent<RectTransform>();

            return childPrefab;
        }

        // T 타입의 컴포넌트가 게임 오브젝트에 있는지 확인하고, 없으면 추가합니다.
        public static T InitializeComponent<T>(this GameObject obj) where T : Component
        {
            T component = obj.GetComponent<T>();
            if (component == null) {
                component = obj.AddComponent<T>();
            }
            return component;
        }

        public static GameObject AddObjectAsChild(this GameObject obj, string name)
        {
            GameObject childPrefab = new GameObject(name);
            childPrefab.transform.parent = obj.transform;
            childPrefab.transform.localScale = new Vector3(1f, 1f, 1f);
            childPrefab.transform.localPosition = new Vector3(0f, 0f, 0f);

            return childPrefab;
        }

        public static GameObject FindObject(GameObject parentObject, string name)
        {
            // Transform thisTransform = transform;
            Transform childObjectTransform = parentObject.transform.Find(name);

            if (childObjectTransform == null) return null;
            else return childObjectTransform.gameObject;
        }

        public static void SetStretchMode(RectTransform rect)
        {
            rect.anchorMin = new Vector2(0f, 0f);
            rect.anchorMax = new Vector2(1f, 1f);
            rect.offsetMin = new Vector2(0f, 0f);
            rect.offsetMax = new Vector2(0f, 0f);
        }

        public static Vector2 GetRectSize(this RectTransform rect)
        {
            Vector2 rectSize = new Vector2(0f, 0f);
            RectTransform parentRect = rect.transform.parent.gameObject.GetComponent<RectTransform>();

            if (rect.anchorMin.x != rect.anchorMax.x && parentRect != null) rectSize.x = GetRectSize(parentRect).x * (rect.anchorMax.x - rect.anchorMin.x) + rect.sizeDelta.x;
            else                                                            rectSize.x = rect.sizeDelta.x;
            if (rect.anchorMin.y != rect.anchorMax.y && parentRect != null) rectSize.y = GetRectSize(parentRect).y * (rect.anchorMax.y - rect.anchorMin.y) + rect.sizeDelta.y;
            else                                                            rectSize.y = rect.sizeDelta.y;

            return rectSize;
        }

        public static List<T> GetListFromString<T>(this string text, char separator)
        {
            List<T> output = new List<T>();
            string[] tokens = text.Split(separator);

            for (int i = 0; i < tokens.Length; i++) {
                if      (typeof(T) == typeof(float)) { if (float.TryParse(tokens[i], out float value)) output.Add((T)Convert.ChangeType(value, typeof(T))); }
                else if (typeof(T) == typeof(int)  ) { if (int.  TryParse(tokens[i], out int   value)) output.Add((T)Convert.ChangeType(value, typeof(T))); }
            }

            return output;
        }

        public static string GetStringFromList<T>(this List<T> list, string separator)
        {
            string str = "";
            for (int i = 0; i < list.Count; i++) {
                str += list[i].ToString();
                if (i != list.Count - 1) {
                    str += separator;
                }
            }

            return str;
        }

        public static void SetTextWithInvokeEndEdit(this InputField inputField, string text)
        {
            inputField.text = text;
            inputField.onEndEdit.Invoke(text);
        }
    }
}

