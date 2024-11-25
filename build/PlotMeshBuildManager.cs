using System;
using System.Linq;
using System.Reflection;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityPlotter;

namespace UnityPlotter
{
    namespace PlotterAddon
    {
        internal class MeshHandler
        {
            public bool isAlive = true;

            // 너무 많은 vertex를 하나의 메쉬로 그리게 되면 스택 오버플로우 발생
            // 해당 길이마다 메쉬를 쪼개서 렌더링
            private int _vertexLimit = 15000;

            private GameObject     _root;
            private Material       _canvasMaterial;
            private CanvasRenderer _renderer;
            private Mesh           _mesh;

            private List<CanvasRenderer> _renderers = new List<CanvasRenderer>();
            private List<Mesh>           _meshes    = new List<Mesh>();

            internal MeshHandler(MonoBehaviour mono, GameObject root)
            {
                _root = root;
                _canvasMaterial = new Material(Shader.Find("Sprites/Default"));
                // mono.StartCoroutine(DrawingThread());
            }

            internal void Clear()
            {
                for (int i = 0; i < _renderers.Count; i++) {
                    _renderers[i].Clear();
                    _renderers[i].SetMaterial(_canvasMaterial, null);
                }

                if (_meshes.Count == 0) return;

                for (int i = 0; i < _meshes.Count; i++) {
                    _meshes[i].Clear();
                }
            }

            private void AdjustRendererCount(int targetCount)
            {
                if (_renderers.Count == targetCount) return;
                int currentCount = _renderers.Count;

                if (currentCount > targetCount) {
                    for (int i = 0; i < currentCount - targetCount; i++) {
                        GameObject.DestroyImmediate(_renderers[0].gameObject, true);
                        _renderers.RemoveAt(0);
                        _meshes.RemoveAt(0);
                    }
                }
                else if (currentCount < targetCount) {
                    for (int i = 0; i < targetCount - currentCount; i++) {
                        var renderObject = ComponentHelper.CreateEmptyObject(_root, "Renderer");
                        var renderer = renderObject.InitializeComponent<CanvasRenderer>();
                        renderer.SetMaterial(_canvasMaterial, null);
                        _renderers.Add(renderer);
                        _meshes.Add(new Mesh());
                    }
                }
            }

            internal void SetMesh(Vector3[] vertices, Color[] colors, int[] triangles)
            {
                int renderCount = (int)vertices.Length / _vertexLimit + 1;
                AdjustRendererCount(renderCount);
                for (int i = 0; i < renderCount; i++) {
                    int vertexCount = (vertices.Length > _vertexLimit * (i + 1)) ? _vertexLimit : vertices.Length - _vertexLimit * i;

                    _meshes[i].Clear();
                    _meshes[i].vertices  =  vertices. Skip(_vertexLimit * i).Take(vertexCount).ToArray();
                    _meshes[i].colors    =  colors.   Skip(_vertexLimit * i).Take(vertexCount).ToArray();
                    _meshes[i].triangles = (triangles.Skip(_vertexLimit * i).Take(vertexCount).ToArray()).Select(n => n - _vertexLimit * i).ToArray();
                }

                if (_renderers.Count == 0) return;
                if (_renderers.Count != _meshes.Count) return;
                for (int i = 0; i < _renderers.Count; i++) {
                    // _renderers[i].Clear();
                    // _renderers[i].SetMaterial(_canvasMaterial, null);
                    _renderers[i].SetMesh(_meshes[i]);
                }
            }
        }
        
        [RequireComponent(typeof(RectTransform))]
        [RequireComponent(typeof(CanvasRenderer))]
        [ExecuteInEditMode]
        public class PlotMeshBuildManager : MonoBehaviour
        {
            public struct Config
            {
                public Vector3    origin;
                public Vector2    xLimit;
                public Vector2    yLimit;
                public Vector2    zLimit;
                public float      zProjectionScale;
            }

            private RectTransform   _rect;
            private MeshHandler     _meshHandler;
            private PlotMeshBuilder _plotHelper;

            public GameObject          _textRoot;
            public List<Text>          _textComps;
            public List<RectTransform> _textRects;

            // _vertices, _colors, _traingles 길이는 항상 같아야하고, 3으로 나누어 져야 함
            [SerializeField, HideInInspector] private int       _vertexLimit = 666666;
            [SerializeField, HideInInspector] private Vector3[] _vertices;
            [SerializeField, HideInInspector] private Color[]   _colors;
            [SerializeField, HideInInspector] private int[]     _triangles;
            [SerializeField, HideInInspector] private int       _vertexCount = 0;

            [SerializeField, HideInInspector] private int       _tickLimit     = 1000;
            [SerializeField, HideInInspector] private Vector3[] _textPositions;
            [SerializeField, HideInInspector] private int[]     _textAxis;
            [SerializeField, HideInInspector] private float[]   _textContents;
            [SerializeField, HideInInspector] private int       _textCount     = 0;

            [SerializeField, HideInInspector] private bool  _autoScaling; public bool  autoScaling { set { _autoScaling = value; _plotShapeConfig.autoScaling = _autoScaling ? 1 : 0; _plotHelper?.SetPlotShapeConfig(_plotShapeConfig); } get { return _autoScaling; } }
            [SerializeField, HideInInspector] private Color _axisColor;   public Color axisColor   { set { _axisColor   = value; _plotShapeConfig.axisColor   = _axisColor;           _plotHelper?.SetPlotShapeConfig(_plotShapeConfig); } get { return _axisColor;   } }
            [SerializeField, HideInInspector] private float _axisRadius;  public float axisRadius  { set { _axisRadius  = value; _plotShapeConfig.axisRadius  = _axisRadius;          _plotHelper?.SetPlotShapeConfig(_plotShapeConfig); } get { return _axisRadius;  } }

            [SerializeField, HideInInspector] private float _xTickSpan;  public float xTickSpan  { set { _xTickSpan  = value; _plotShapeConfig.xTickSpan  = _xTickSpan;  _plotHelper?.SetPlotShapeConfig(_plotShapeConfig); } get { return _xTickSpan;  } }
            [SerializeField, HideInInspector] private float _yTickSpan;  public float yTickSpan  { set { _yTickSpan  = value; _plotShapeConfig.yTickSpan  = _yTickSpan;  _plotHelper?.SetPlotShapeConfig(_plotShapeConfig); } get { return _yTickSpan;  } }
            [SerializeField, HideInInspector] private float _zTickSpan;  public float zTickSpan  { set { _zTickSpan  = value; _plotShapeConfig.zTickSpan  = _zTickSpan;  _plotHelper?.SetPlotShapeConfig(_plotShapeConfig); } get { return _zTickSpan;  } }
            [SerializeField, HideInInspector] private float _tickLength; public float tickLength { set { _tickLength = value; _plotShapeConfig.tickLength = _tickLength; _plotHelper?.SetPlotShapeConfig(_plotShapeConfig); } get { return _tickLength; } }
            [SerializeField, HideInInspector] private float _tickRadius; public float tickRadius { set { _tickRadius = value; _plotShapeConfig.tickRadius = _tickRadius; _plotHelper?.SetPlotShapeConfig(_plotShapeConfig); } get { return _tickRadius; } }
            [SerializeField, HideInInspector] private Font  _textFont;   public Font  textFont   { set { _textFont = value; } get { return _textFont; } }
            [SerializeField, HideInInspector] private int   _fontSize;   public int   fontSize   { set { _fontSize = value; } get { return _fontSize; } }

            [SerializeField, HideInInspector] private int _isRender2D; public int isRender2D { set { _isRender2D = value; _plotShapeConfig.isRender2D = _isRender2D; _plotHelper?.SetPlotShapeConfig(_plotShapeConfig); } get { return _isRender2D; } }
            [SerializeField, HideInInspector] private Vector3 _origin; public Vector3 origin { set { _origin = value; _plotShapeConfig.origin = _origin; _plotHelper?.SetPlotShapeConfig(_plotShapeConfig); } get { return _origin; } }
            [SerializeField, HideInInspector] private Vector2 _xLimit; public Vector2 xLimit { set { _xLimit = value; _plotShapeConfig.xLimit = _xLimit; _plotHelper?.SetPlotShapeConfig(_plotShapeConfig); } get { return _xLimit; } }
            [SerializeField, HideInInspector] private Vector2 _yLimit; public Vector2 yLimit { set { _yLimit = value; _plotShapeConfig.yLimit = _yLimit; _plotHelper?.SetPlotShapeConfig(_plotShapeConfig); } get { return _yLimit; } }
            [SerializeField, HideInInspector] private Vector2 _zLimit; public Vector2 zLimit { set { _zLimit = value; _plotShapeConfig.zLimit = _zLimit; _plotHelper?.SetPlotShapeConfig(_plotShapeConfig); } get { return _zLimit; } }
            public  GameObject rotationHandle;
            [SerializeField, HideInInspector] private float _zProjectionScale = 0.001f; 
                                              public  float  zProjectionScale { set { _zProjectionScale = value; _plotShapeConfig.zProjectionScale = _zProjectionScale; _plotHelper?.SetPlotShapeConfig(_plotShapeConfig); } get { return _zProjectionScale; } }

            private PlotShapeConfig _plotShapeConfig = new PlotShapeConfig();
            private PlotDataConfig  _plotDataConfig  = new PlotDataConfig();
            // 이벤트 동기화 관리
            private bool _isPlotDataSetDone = true;
            // OnDataReady 콜백의 경우, C++에서 비동기적으로 호출하기 때문에,
            // PlotMeshBuildManager의 생명주기와 무관하게 호출될 수 있어 bool을 통해 호출을 조절
            private bool _enterPlayMode = false;
            private bool _isAlive = true;
            private bool _clearRequested = false;

            private void OnEnable()
            {
                if (_vertices == null || _vertices.Length != _vertexLimit) {
                    _vertices  = new Vector3[_vertexLimit];
                    _colors    = new Color[_vertexLimit];
                    _triangles = new int[_vertexLimit];
                }
                if (_textPositions == null || _textPositions.Length != _tickLimit) {
                    _textPositions = new Vector3[_tickLimit];
                    _textAxis      = new int[_tickLimit];
                    _textContents  = new float[_tickLimit];
                }
                _isAlive = true;
                // Rect 정보 획득
                _rect = GetComponent<RectTransform>();

                // 초기값 설정
                _plotShapeConfig.isRender2D       = _isRender2D;
                _plotShapeConfig.autoScaling      = 0; // 사용하지 않음
                _plotShapeConfig.rectSize         = _rect.GetRectSize();
                _plotShapeConfig.zProjectionScale = _zProjectionScale;
                _plotShapeConfig.rotEuler         = new Vector3(0f, 0f, 0f);
                _plotShapeConfig.origin           = _origin;
            }

            private void Start()
            {
                if (!Application.isPlaying) return;

                _enterPlayMode = true;

                // Text 관리 오브젝트 생성
                if (_textRoot == null) {
                    _textRoot = ComponentHelper.CreateEmptyObject(this.gameObject, "Text Root");
                    _textComps = new List<Text>();
                    _textRects = new List<RectTransform>();
                }

                // config 적용
                _plotShapeConfig.xLimit = _xLimit;
                _plotShapeConfig.yLimit = _yLimit;
                _plotShapeConfig.zLimit = _zLimit;

                // Plot helper 설정
                _plotHelper = new PlotMeshBuilder(_vertexLimit, _tickLimit);
                _plotHelper.onDataReady += OnDataReady;
                _plotHelper.SetPlotShapeConfig(_plotShapeConfig);

                _plotHelper.SetMeshDataMemorySync(ref _vertices, ref _colors, ref _triangles, ref _vertexCount);
                _plotHelper.SetTextInfoMemorySync(ref _textPositions, ref _textAxis, ref _textContents, ref _textCount);
                _meshHandler = new MeshHandler(this, this.gameObject);
            }
            
            private void Update()
            {
                if (rotationHandle == null) return;
                if (_isRender2D == 1) return;

                Vector3 rotEuler = rotationHandle.transform.localRotation.eulerAngles;
                if (rotEuler != _plotShapeConfig.rotEuler) {
                    _plotShapeConfig.rotEuler = rotEuler;
                    _plotHelper?.SetPlotShapeConfig(_plotShapeConfig);
                }
            }

            private void OnRectTransformDimensionsChange()
            {
                if (_rect == null) return;
                _plotShapeConfig.rectSize = _rect.GetRectSize();
                _plotHelper?.SetPlotShapeConfig(_plotShapeConfig);
            }

            public void Clear()
            {
                _clearRequested = true;
                if (_meshHandler != null) _meshHandler.Clear();

                int count = _textComps.Count;
                for (int i = 0; i < count; i++) {
                    DestroyImmediate(_textComps[0].gameObject, true);
                    _textComps.RemoveAt(0);
                    _textRects.RemoveAt(0);
                }
            }

            private (int, int) CountDigit(float num)
            {
                if (num >= 1) {
                    // 1보다 큰 경우: 자릿수 계산
                    int digitCount = num.ToString().Length;
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

            private string GetFormatString(int integerDigits, int decimalPlaces)
            {
                string formatString = "";
                // if (integerDigits > 0) {
                //     for (int i = 0; i < integerDigits; i++) {
                //         formatString += "0";
                //     }
                // }
                // else {
                //     formatString += "0";
                // }
                formatString += "0";

                if (decimalPlaces > 0) {
                    formatString += ".";
                    for (int i = 0; i < decimalPlaces; i++) {
                        formatString += "0";
                    }
                }

                return formatString;
            }

            private void AjustTextComponents()
            {
                if (_textComps.Count != _textCount) {
                    int count = _textComps.Count;
                    if (count > _textCount) {
                        for (int i = 0; i < count - _textCount; i++) {
                            DestroyImmediate(_textComps[0].gameObject, true);
                            _textComps.RemoveAt(0);
                            _textRects.RemoveAt(0);
                        }
                    }

                    else if (count < _textCount) {
                        for (int i = 0; i < _textCount - count; i++) {
                            GameObject textObj = ComponentHelper.CreateEmptyObject(_textRoot, "Grid Text");
                            Text textComp = textObj.AddComponent<Text>();
                            textComp.alignment = TextAnchor.MiddleCenter;
                            if (_textFont != null) {
                                // textComp.font = Resources.GetBuiltinResource<Font>("Arial.ttf");
                                textComp.font     = _textFont;
                                textComp.fontSize = _fontSize;
                            }
                            textComp.color = Color.white;
                            textComp.text = "Test";
                            RectTransform textRect = textObj.GetComponent<RectTransform>();
                            _textComps.Add(textComp);
                            _textRects.Add(textRect);
                        }
                    }
                }

                (int xDigit, int xDecimal) = CountDigit(_xTickSpan);
                (int yDigit, int yDecimal) = CountDigit(_yTickSpan);
                string xFormatString = GetFormatString(xDigit, xDecimal);
                string yFormatString = GetFormatString(yDigit, yDecimal);

                for (int i = 0; i < _textComps.Count; i++) {
                    if (_textAxis[i] == 0) {
                        _textComps[i].text = _textContents[i].ToString(xFormatString);
                    }
                    else if (_textAxis[i] == 1) {
                        _textComps[i].text = _textContents[i].ToString(yFormatString);
                    }
                    _textRects[i].anchoredPosition = new Vector2(_textPositions[i].x, _textPositions[i].y);
                    if (rotationHandle != null) {
                        _textRects[i].rotation = rotationHandle.transform.rotation;
                    }
                }
            }

            private void OnDataReady()
            {
                if (_meshHandler == null) return;

                if (_vertexCount > 0 && _isAlive && !_clearRequested) {
                    _meshHandler.SetMesh(
                        _vertices. Take(_vertexCount).ToArray(),
                        _colors.   Take(_vertexCount).ToArray(),
                        _triangles.Take(_vertexCount).ToArray()
                        );

                    // AjustTextComponents();
                }

                if (_isAlive) _plotHelper.GetMeshDataDone();
            }

            public void Plot(float[] x, float[] y, float lineWidth, Color lineColor, bool useFillMesh)
            {
                if (y == null) return;
                if (y.Length < 1) return;
                if (x == null) x = _plotHelper.CreateXData(y);
                if (x.Length != y.Length) return;

                if (_isPlotDataSetDone) {
                    _plotHelper.PlotDataSetStart();
                    _isPlotDataSetDone = false;
                }

                _plotDataConfig.radius = lineWidth;
                _plotDataConfig.color  = lineColor;

                _plotHelper.SetPlotDataConfig(_plotDataConfig);
                _plotHelper.SetPlotData(x, y);
            }

            public void Plot3D(Vector3[] startPoints, Vector3[] endPoints, float lineWidth, Color lineColor)
            {
                if (startPoints == null) return;
                if (endPoints == null) return;
                if (startPoints.Length != endPoints.Length) return;

                if (_isPlotDataSetDone) {
                    _plotHelper.PlotDataSetStart();
                    _isPlotDataSetDone = false;
                }

                _plotDataConfig.radius = lineWidth;
                _plotDataConfig.color  = lineColor;

                _plotHelper.SetPlotDataConfig(_plotDataConfig);
                _plotHelper.SetPlotData(startPoints, endPoints);
            }

            public void Scatter(float[] x, float[] y, float scatterRadius, Color scatterColor, int scatterMeshSegment)
            {
                if (y == null) return;
                if (x == null) x = _plotHelper.CreateXData(y);
                if (x.Length != y.Length) return;

                if (_isPlotDataSetDone) {
                    _plotHelper.PlotDataSetStart();
                    _isPlotDataSetDone = false;
                }

                _plotDataConfig.segments = scatterMeshSegment;
                _plotDataConfig.radius   = scatterRadius;
                _plotDataConfig.color    = scatterColor;

                _plotHelper.SetPlotDataConfig(_plotDataConfig);
                _plotHelper.SetPlotData(_plotHelper.FloatArrayToVectorArray(x, y));
            }

            public void Scatter3D(Vector3[] scatterPoints, float scatterRadius, Color scatterColor, int scatterMeshSegment)
            {
                if (scatterPoints == null) return;

                if (_isPlotDataSetDone) {
                    _plotHelper.PlotDataSetStart();
                    _isPlotDataSetDone = false;
                }
                
                _plotDataConfig.segments = scatterMeshSegment;
                _plotDataConfig.radius   = scatterRadius;
                _plotDataConfig.color    = scatterColor;

                _plotHelper.SetPlotDataConfig(_plotDataConfig);
                _plotHelper.SetPlotData(scatterPoints);
            }

            public void Bar(float[] x, float[] y, float width, Color color)
            {
                if (y == null) return;
                if (x == null) x = _plotHelper.CreateXData(y);
                if (x.Length != y.Length) return;

                if (_isPlotDataSetDone) {
                    _plotHelper.PlotDataSetStart();
                    _isPlotDataSetDone = false;
                }

                _plotDataConfig.radius = width;
                _plotDataConfig.color  = color;

                _plotHelper.SetPlotDataConfig(_plotDataConfig);
                _plotHelper.SetPlotData(x, y, 0);
            }

            public void Barh(float[] x, float[] y, float width, Color color)
            {
                if (y == null) return;
                if (x == null) x = _plotHelper.CreateXData(y);
                if (x.Length != y.Length) return;

                if (_isPlotDataSetDone) {
                    _plotHelper.PlotDataSetStart();
                    _isPlotDataSetDone = false;
                }

                _plotDataConfig.radius = width;
                _plotDataConfig.color  = color;

                _plotHelper.SetPlotDataConfig(_plotDataConfig);
                _plotHelper.SetPlotData(x, y, 1);
            }

            public void Mesh3D(float[,] x, float[,] y, float[,] z, Gradient mapColor = null, float lineWidth = 1f)
            {
                if (x == null) return;
                if (y == null) return;
                if (z == null) return;
                if (x.Length != y.Length || y.Length != z.Length) return;

                if (_isPlotDataSetDone) {
                    _plotHelper.PlotDataSetStart();
                    _isPlotDataSetDone = false;
                }

                _plotDataConfig.radius = lineWidth;

                _plotHelper.SetGradientConfig(mapColor);
                _plotHelper.SetPlotDataConfig(_plotDataConfig);
                _plotHelper.SetPlotData(x, y, z);
            }

            public void Surface3D(float[,] x, float[,] y, float[,] z, Gradient mapColor)
            {
                if (x == null) return;
                if (y == null) return;
                if (z == null) return;
                if (x.Length != y.Length || y.Length != z.Length) return;

                if (_isPlotDataSetDone) {
                    _plotHelper.PlotDataSetStart();
                    _isPlotDataSetDone = false;
                }

                _plotHelper.SetGradientConfig(mapColor);
                _plotHelper.SetPlotDataConfig(_plotDataConfig);
                _plotHelper.SetFillData(x, y, z);
            }

            public void Draw()
            {
                _clearRequested = false;
                _isPlotDataSetDone = true;
                _plotHelper.PlotDataSetDone();
            }

            private void OnDestroy()
            {
                if (_enterPlayMode) {
                    _meshHandler.isAlive = false;
                    _enterPlayMode = false;
                }
                
                _isAlive = false;

                if (_plotHelper != null) {
                    _plotHelper.OnDestroy();
                    _plotHelper = null;
                }

                // if (_drawingThread != null) StopCoroutine(_drawingThread);
            }
        }
    }
}