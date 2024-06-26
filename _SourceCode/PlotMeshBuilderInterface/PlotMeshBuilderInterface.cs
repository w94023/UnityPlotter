using System;
using System.Security.Cryptography;
using System.Runtime.InteropServices;
using System.Threading;
using UnityEngine;

namespace UnityUI
{ 
    [StructLayout(LayoutKind.Sequential)]
    public struct PlotShapeConfig
    {
        public int     autoScaling;
        public Color   axisColor;
        public float   axisRadius;

        public float   xTickSpan;
        public float   yTickSpan;
        public float   zTickSpan;
        public float   tickLength;
        public float   tickRadius;

        public Vector3 origin;
        public Vector2 xLimit;
        public Vector2 yLimit;
        public Vector2 zLimit;
        public Vector2 rectSize;
        public Vector3 rotEuler;
        public float   zProjectionScale;

        public int isRender2D;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct PlotDataConfig
    {
        public float radius;
        public Color color;
        public int   segments;
    }

    public class PlotMeshBuilder
    {
        private delegate void EventCallback();

        [DllImport("PlotMeshBuilder.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void OpenConsoleAndPrint();

        [DllImport("PlotMeshBuilder.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void GenerateXData(float[] x, int startIndex, int nCount);

        [DllImport("PlotMeshBuilder.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool CreateHandle(int handle, int pointLimit, int vertexLimit, int tickLimit);

        [DllImport("PlotMeshBuilder.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void StopThread(int handle);

        [DllImport("PlotMeshBuilder.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void DeleteHandle(int handle);

        [DllImport("PlotMeshBuilder.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void RegisterEventCallback(int handle, EventCallback callback);

        [DllImport("PlotMeshBuilder.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void SetPlotShapeConfig(int handle, PlotShapeConfig config);

        [DllImport("PlotMeshBuilder.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void SetPlotDataConfig(int handle, PlotDataConfig config);

        [DllImport("PlotMeshBuilder.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void SetGradientConfig(int handle, GradientColorKey[] gradientColorKeys, int gradientColorKeyCount, GradientAlphaKey[] gradientAlphaKeys, int gradientAlphaKeyCount);

        [DllImport("PlotMeshBuilder.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void FloatArrayToVectorArray(Vector3[] vectorArr, float[] x, float[] y, int nPoints);

        [DllImport("PlotMeshBuilder.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void PlotDataSetStart(int handle);

        [DllImport("PlotMeshBuilder.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void SetFloatArrayDataForBarPlot(int handle, float[] x, float[] y, int axis, int nPoints);

        [DllImport("PlotMeshBuilder.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void SetFloatArrayDataForLinePlot(int handle, float[] x, float[] y, int nPoints);

        [DllImport("PlotMeshBuilder.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void SetSingleVectorData(int handle, Vector3[] points, int nPoints);

        [DllImport("PlotMeshBuilder.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void SetDoubleVectorData(int handle, Vector3[] x, Vector3[] y, int nPoints);

        [DllImport("PlotMeshBuilder.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void SetMapDataForPlot(int handle, float[,] xMap, float[,] yMap, float[,] zMap, int nRow, int nCol);

        [DllImport("PlotMeshBuilder.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void SetMapDataForFill(int handle, float[,] xMap, float[,] yMap, float[,] zMap, int nRow, int nCol);

        [DllImport("PlotMeshBuilder.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void PlotDataSetDone(int handle);

        [DllImport("PlotMeshBuilder.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void SetMeshDataMemorySync(int handle, IntPtr vertices, IntPtr colors, int[] triangles, ref int nCount);

        [DllImport("PlotMeshBuilder.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void SetTextInfoMemorySync(int handle, IntPtr positions, int[] axis, float[] contents, ref int nCount);

        [DllImport("PlotMeshBuilder.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void DataTransferred(int handle);

        [DllImport("PlotMeshBuilder.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void StackArray(float[] arr, int nCount, float value);

        private int _CPPHandle;
        private int _pointLimit = 1000000;

        private EventCallback _onDataReady;
        public  Action         onDataReady;
        private SynchronizationContext _mainThreadContext;

        private bool _isAlive = true;

        // 암호학적으로 안전한 난수 생성기 인스턴스화
        private RNGCryptoServiceProvider rngCsp = new RNGCryptoServiceProvider();

        public PlotMeshBuilder(int vertexLimit, int tickLimit)
        {
            OpenConsoleAndPrint();
            _CPPHandle = InitializeCPPHandle(vertexLimit, tickLimit);

            _mainThreadContext = SynchronizationContext.Current;
            _onDataReady = new EventCallback(OnDataReady);
            RegisterEventCallback(_CPPHandle, _onDataReady);
        }

        ~PlotMeshBuilder()
        {
            if (_isAlive) {
                OnDestroy();
            }
        }

        public void OnDestroy()
        {
            PlotDataSetDone();
            GetMeshDataDone();

            StopThread();
            DeleteHandle(_CPPHandle);
            rngCsp.Dispose();

            _isAlive = false;
        }

        // 암호학적으로 안전한 난수 생성 함수
        private int GenerateRandomNumber(int minValue, int maxValue)
        {
            if (minValue >= maxValue) {
                throw new ArgumentException("minValue must be lower than maxValue");
            }

            // int 범위의 난수 생성
            byte[] randomNumber = new byte[4];
            rngCsp.GetBytes(randomNumber);
            int value = BitConverter.ToInt32(randomNumber, 0);

            // minValue와 maxValue 사이의 난수를 반환
            return Math.Abs(value % (maxValue - minValue)) + minValue;
        }

        private int InitializeCPPHandle(int vertexLimit, int tickLimit)
        {
            //System.Random rnd = new System.Random();
            //int handle = rnd.Next(1000, 10000); // 1000부터 9999까지의 랜덤 정수 생성
            int handle = GenerateRandomNumber(1000, 10000); // 1000부터 9999까지의 랜덤 정수 생성

            if (CreateHandle(handle, _pointLimit, vertexLimit, tickLimit)) {
                return handle;
            }
            else {
                return InitializeCPPHandle(vertexLimit, tickLimit);
            }
        }

        public void SetMeshDataMemorySync(ref Vector3[] vertices, ref Color[] colors, ref int[] triangles, ref int vertexCount)
        {
            GCHandle verticesGCH = GCHandle.Alloc(vertices, GCHandleType.Pinned);
            IntPtr   verticesPtr = verticesGCH.AddrOfPinnedObject();
            GCHandle colorsGCH = GCHandle.Alloc(colors, GCHandleType.Pinned);
            IntPtr   colorsPtr = colorsGCH.AddrOfPinnedObject();

            SetMeshDataMemorySync(_CPPHandle, verticesPtr, colorsPtr, triangles, ref vertexCount);

            verticesGCH.Free();
            colorsGCH.Free();
        }

        public void SetTextInfoMemorySync(ref Vector3[] positions, ref int[] axis, ref float[] contents, ref int count)
        {
            GCHandle positionsGCH = GCHandle.Alloc(positions, GCHandleType.Pinned);
            IntPtr   positionsPtr = positionsGCH.AddrOfPinnedObject();

            SetTextInfoMemorySync(_CPPHandle, positionsPtr, axis, contents, ref count);

            positionsGCH.Free();
        }

        public void StopThread()
        {
            StopThread(_CPPHandle);
        }

        public void SetPlotShapeConfig(PlotShapeConfig config)
        {
            SetPlotShapeConfig(_CPPHandle, config);
        }

        public void SetPlotDataConfig(PlotDataConfig config)
        { 
            SetPlotDataConfig(_CPPHandle, config);
        }

        public void SetGradientConfig(Gradient gradient)
        {
            SetGradientConfig(_CPPHandle, gradient.colorKeys, gradient.colorKeys.Length, gradient.alphaKeys, gradient.alphaKeys.Length);
        }

        public void PlotDataSetStart()
        {
            PlotDataSetStart(_CPPHandle);
        }

        public float[] CreateXData(float[] y)
        {
            float[] x = new float[y.Length];
            GenerateXData(x, 0, x.Length);
            return x;
        }

        public Vector3[] FloatArrayToVectorArray(float[] x, float[] y)
        {
            if (x == null) return null;
            if (y == null) return null;
            if (x.Length != y.Length) return null;

            Vector3[] vectorArr = new Vector3[x.Length];
            FloatArrayToVectorArray(vectorArr, x, y, vectorArr.Length);

            return vectorArr;
        }

        public void SetPlotData(float[] x, float[] y, int axis)
        {
            // Bar plot 용도
            // axis = 0 : 가로 bar
            // axis = 1 : 세로 bar
            if (x.Length != y.Length) return;
            SetFloatArrayDataForBarPlot(_CPPHandle, x, y, axis, x.Length);
        }

        public void SetPlotData(float[] x, float[] y)
        {
            // Line plot 용도
            if (x.Length != y.Length) return;
            SetFloatArrayDataForLinePlot(_CPPHandle, x, y, x.Length);
        }

        public void SetPlotData(Vector3[] points)
		{
            SetSingleVectorData(_CPPHandle, points, points.Length);
        }

        public void SetPlotData(Vector3[] startPoints, Vector3[] endPoints)
        {
            if (startPoints.Length != endPoints.Length) return;
            SetDoubleVectorData(_CPPHandle, startPoints, endPoints, startPoints.Length);
        }

        public void SetPlotData(float[,] xMap, float[,] yMap, float[,] zMap)
        {
            if (xMap == null) return;
            if (yMap == null) return;
            if (zMap == null) return;
            if (xMap.GetLength(0) != yMap.GetLength(0) || yMap.GetLength(0) != zMap.GetLength(0)) return;
            if (xMap.GetLength(1) != yMap.GetLength(1) || yMap.GetLength(1) != zMap.GetLength(1)) return;

            SetMapDataForPlot(_CPPHandle, xMap, yMap, zMap, xMap.GetLength(0), xMap.GetLength(1));
        }

        public void SetFillData(float[,] xMap, float[,] yMap, float[,] zMap)
        {
            if (xMap == null) return;
            if (yMap == null) return;
            if (zMap == null) return;
            if (xMap.GetLength(0) != yMap.GetLength(0) || yMap.GetLength(0) != zMap.GetLength(0)) return;
            if (xMap.GetLength(1) != yMap.GetLength(1) || yMap.GetLength(1) != zMap.GetLength(1)) return;

            SetMapDataForFill(_CPPHandle, xMap, yMap, zMap, xMap.GetLength(0), xMap.GetLength(1));
        }

        public void PlotDataSetDone()
        {
            PlotDataSetDone(_CPPHandle);
        }

        public void GetMeshDataDone()
        {
            DataTransferred(_CPPHandle);
        }

        private void OnDataReady()
		{
            if (_mainThreadContext == null) onDataReady?.Invoke();
            else _mainThreadContext.Post(state => { onDataReady?.Invoke(); }, null);
        }

        public static void StackPlotData(ref float[] data, float value)
        {
            StackArray(data, data.Length, value);
        }
    }
}