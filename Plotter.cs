using System.Linq;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityPlotter;

namespace UnityPlotter
{
    [RequireComponent(typeof(RectTransform))]
    [ExecuteInEditMode]
    public class Plotter : MonoBehaviour
    {
                                          public  GameObject                         backgroundObject;
                                          public  GameObject                         plotMeshObject;
        [SerializeField, HideInInspector] private PlotterAddon.PlotMeshBuildManager _plotMeshHandler;

        [SerializeField, HideInInspector] private Color _backgroundColor = new Color(1f, 1f, 1f, 0.5f); public Color backgroundColor { set { _backgroundColor = value; OnValidate(); } get { return _backgroundColor; } }
        [SerializeField, HideInInspector] private bool    _xAxisAutoScaling   = false; public bool xAxisAutoScaling { set { _xAxisAutoScaling = value; OnValidate(); } get { return _xAxisAutoScaling; } }
        [SerializeField, HideInInspector] private bool    _yAxisAutoScaling   = false; public bool yAxisAutoScaling { set { _yAxisAutoScaling = value; OnValidate(); } get { return _yAxisAutoScaling; } }
        [SerializeField, HideInInspector] private Color[] _plotColorList = new Color[] {
            new Color(0f,      0.4470f, 0.7410f, 1f),
            new Color(0.8500f, 0.3250f, 0.0980f, 1f),
            new Color(0.9290f, 0.6940f, 0.1250f, 1f),
            new Color(0.4940f, 0.1840f, 0.5560f, 1f),
            new Color(0.4660f, 0.6740f, 0.1880f, 1f),
            new Color(0.3010f, 0.7450f, 0.9330f, 1f),
            new Color(0.6350f, 0.0780f, 0.1840f, 1f)
        };
        private int _colorCount = -1;

        ///////////////////////////////////////////////////////////////////////////////////* Render options *//////////////////////////////////////////////////////////////////////////////////////////////////////
        [SerializeField, HideInInspector] private int _orderInLayer = 0; public int orderInLayer { set { _orderInLayer = value; OnValidate(); } get { return _orderInLayer; } }
        public enum RenderMode
        {
            Render3D,
            Render2D
        }
        [SerializeField, HideInInspector] private RenderMode _renderMode = RenderMode.Render3D; public RenderMode renderMode     { set { _renderMode = value; OnValidate(); } get { return _renderMode; } }
        [SerializeField, HideInInspector] private GameObject _rotationHandle;                   public GameObject rotationHandle { set { _rotationHandle = value; _plotMeshHandler.rotationHandle = _rotationHandle; }  get { return _rotationHandle; } }
        ///////////////////////////////////////////////////////////////////////////////////* Render options *//////////////////////////////////////////////////////////////////////////////////////////////////////

        ///////////////////////////////////////////////////////////////////////////////////* Axis options *//////////////////////////////////////////////////////////////////////////////////////////////////////
        // origin
        [SerializeField, HideInInspector] private Vector3 _origin; public Vector3 origin { set { _origin = value; _plotMeshHandler.origin = _origin; } get { return _origin; } }
        // Axis graphic
        [SerializeField, HideInInspector] private bool  _useAxisLine       = true;        public bool  useAxisLine       { set { _useAxisLine       = value; OnValidate(); } get { return _useAxisLine;       } }
        [SerializeField, HideInInspector] private float _axisLineThickness = 1f;          public float axisLineThickness { set { _axisLineThickness = value; OnValidate(); } get { return _axisLineThickness; } }
        [SerializeField, HideInInspector] private Color _axisLineColor     = Color.white; public Color axisLineColor     { set { _axisLineColor     = value; OnValidate(); } get { return _axisLineColor;     } }
        // Axis range
        [SerializeField, HideInInspector] private float _xLimitMin; public float xLimitMin { set { _xLimitMin = value; OnValidate(); }  get { return _xLimitMin; } }
        [SerializeField, HideInInspector] private float _xLimitMax; public float xLimitMax { set { _xLimitMax = value; OnValidate(); }  get { return _xLimitMax; } }
        [SerializeField, HideInInspector] private float _yLimitMin; public float yLimitMin { set { _yLimitMin = value; OnValidate(); }  get { return _yLimitMin; } }
        [SerializeField, HideInInspector] private float _yLimitMax; public float yLimitMax { set { _yLimitMax = value; OnValidate(); }  get { return _yLimitMax; } }
        [SerializeField, HideInInspector] private float _zLimitMin; public float zLimitMin { set { _zLimitMin = value; OnValidate(); }  get { return _zLimitMin; } }
        [SerializeField, HideInInspector] private float _zLimitMax; public float zLimitMax { set { _zLimitMax = value; OnValidate(); }  get { return _zLimitMax; } }
        public Vector2 xLimit { set { _xLimitMin = value[0]; _xLimitMax = value[1]; OnValidate(); }  get { return new Vector2(_xLimitMin, _xLimitMax); } }
        public Vector2 yLimit { set { _yLimitMin = value[0]; _yLimitMax = value[1]; OnValidate(); }  get { return new Vector2(_yLimitMin, _yLimitMax); } }
        public Vector2 zLimit { set { _zLimitMin = value[0]; _zLimitMax = value[1]; OnValidate(); }  get { return new Vector2(_zLimitMin, _zLimitMax); } }
        // Tick text graphic
        [SerializeField, HideInInspector] private Font _textFont;      public Font textFont { set { _textFont = value; OnValidate(); } get { return _textFont; } }
        [SerializeField, HideInInspector] private int  _fontSize = 10; public int  fontSize { set { _fontSize = value; OnValidate(); } get { return _fontSize; } }
        // Tick graphic
        [SerializeField, HideInInspector] private bool  _useAxisTick       = true;        public bool   useAxisTick       { set { _useAxisTick       = value; OnValidate(); } get { return _useAxisTick;       } }
        [SerializeField, HideInInspector] private float _tickLength        = 5f;          public float  tickLength        { set { _tickLength        = value; OnValidate(); } get { return _tickLength;        } }
        [SerializeField, HideInInspector] private float _tickRadius        = 1f;          public float  tickRadius        { set { _tickRadius        = value; OnValidate(); } get { return _tickRadius;        } }
        [SerializeField, HideInInspector] private float _xTickSpan         = 1f;          public float  xTickSpan         { set { _xTickSpan         = value; OnValidate(); } get { return _xTickSpan;         } }
        [SerializeField, HideInInspector] private float _yTickSpan         = 1f;          public float  yTickSpan         { set { _yTickSpan         = value; OnValidate(); } get { return _yTickSpan;         } }
        [SerializeField, HideInInspector] private float _zTickSpan         = 1f;          public float  zTickSpan         { set { _zTickSpan         = value; OnValidate(); } get { return _zTickSpan;         } }
        ///////////////////////////////////////////////////////////////////////////////////* Axis options *//////////////////////////////////////////////////////////////////////////////////////////////////////
    
        private RectTransform         _rect;
        private RectTransform         _backgroundRect;
        private Image                 _backgroundImage;

        private void OnEnable()
        {
            _rect = GetComponent<RectTransform>();

            if (backgroundObject == null) {
                backgroundObject = backgroundObject.InitializeObject(this.gameObject, "Background");
                _backgroundImage = backgroundObject.InitializeComponent<Image>();
            }
            _backgroundRect  = backgroundObject.GetComponent<RectTransform>();
            _backgroundImage = backgroundObject.GetComponent<Image>();
            SetStretchMode(_backgroundRect);

            // 플로터 관리 오브젝트 생성
            if (plotMeshObject == null || _plotMeshHandler == null) {
                plotMeshObject = ComponentHelper.CreateEmptyObject(this.gameObject, "Plot Meshs");
                var rect         = plotMeshObject.GetComponent<RectTransform>();
                _plotMeshHandler = plotMeshObject.AddComponent<PlotterAddon.PlotMeshBuildManager>();
                _plotMeshHandler.rotationHandle = _rotationHandle;
                SetStretchMode(rect);
            }

            plotMeshObject.transform.SetAsLastSibling();
        }

        private void SetStretchMode(RectTransform rect)
        {
            rect.anchorMin = new Vector2(0f, 0f);
            rect.anchorMax = new Vector2(1f, 1f);
            rect.offsetMin = new Vector2(0f, 0f);
            rect.offsetMax = new Vector2(0f, 0f);
        }

        private void OnValidate()
        {
            if (_backgroundImage != null) {
                _backgroundImage.color = _backgroundColor;
            }

            if (_plotMeshHandler.origin != _origin) {
                _plotMeshHandler.origin = _origin;
            }

            if (_plotMeshHandler.rotationHandle != _rotationHandle) {
                _plotMeshHandler.rotationHandle = _rotationHandle;
            }

            if (_plotMeshHandler == null) return;

            if (_xLimitMax < _xLimitMin) _xLimitMax = _xLimitMin;
            if (_xLimitMax < _xLimitMin) _yLimitMax = _yLimitMin;
            if (_xLimitMax < _xLimitMin) _zLimitMax = _zLimitMin;

            if (_useAxisLine) _plotMeshHandler.axisRadius = _axisLineThickness;
            else              _plotMeshHandler.axisRadius = 0f;

            if (!_useAxisTick) {
                _plotMeshHandler.xTickSpan = -1f;
                _plotMeshHandler.yTickSpan = -1f;
                _plotMeshHandler.zTickSpan = -1f;
            }
            else {
                _plotMeshHandler.xTickSpan = _xTickSpan;
                _plotMeshHandler.yTickSpan = _yTickSpan;
                _plotMeshHandler.zTickSpan = _zTickSpan;
            }
            
            // _plotMeshHandler.autoScaling = _autoScaling;
            _plotMeshHandler.axisColor   = _axisLineColor;
            _plotMeshHandler.tickLength  = _tickLength;
            _plotMeshHandler.tickRadius  = _tickRadius;
            _plotMeshHandler.xLimit      = new Vector2(_xLimitMin, _xLimitMax);
            _plotMeshHandler.yLimit      = new Vector2(_yLimitMin, _yLimitMax);
            _plotMeshHandler.zLimit      = new Vector2(_zLimitMin, _zLimitMax);
            _plotMeshHandler.textFont    = _textFont;
            _plotMeshHandler.fontSize    = _fontSize;
            _plotMeshHandler.isRender2D  = (int)_renderMode;
        }

        public void Plot(           float[] y, float lineWidth = 1f, Color lineColor = default(Color)) 
            => Plot(null, y, lineWidth, lineColor);

        public void Plot(float[] x, float[] y, float lineWidth = 1f, Color lineColor = default(Color))
            => _plotMeshHandler.Plot(x, y, lineWidth, SetColor(lineColor), false);

        public void Plot3D(Vector3 startPoint, Vector3 endPoint, float lineWidth = 1f, Color lineColor = default(Color))
            => _plotMeshHandler.Plot3D(new Vector3[] {startPoint}, new Vector3[] {endPoint}, lineWidth, SetColor(lineColor));

        public void Plot3D(Vector3[] startPoints, Vector3[] endPoints, float lineWidth = 1f, Color lineColor = default(Color))
            => _plotMeshHandler.Plot3D(startPoints, endPoints, lineWidth, SetColor(lineColor));

        public void Scatter(           float[] y, float scatterRadius = 1f, Color scatterColor = default(Color), int scatterMeshSegment = 12)
            => Scatter(null, y, scatterRadius, scatterColor, scatterMeshSegment);

        public void Scatter(float[] x, float[] y, float scatterRadius = 1f, Color scatterColor = default(Color), int scatterMeshSegment = 12)
            => _plotMeshHandler.Scatter(x, y, scatterRadius, SetColor(scatterColor), scatterMeshSegment);

        public void Scatter3D(Vector3[] scatterPoints, float scatterRadius = 1f, Color scatterColor = default(Color), int scatterMeshSegment = 12)
            => _plotMeshHandler.Scatter3D(scatterPoints, scatterRadius, SetColor(scatterColor), scatterMeshSegment);

        public void Bar(           float[] y, float width = 1f, Color color = default(Color))
            => Bar(null, y, width, color);

        public void Bar(float[] x, float[] y, float width = 1f, Color color = default(Color))
            => _plotMeshHandler.Bar(x, y, width, SetColor(color));

        public void Barh(           float[] y, float width = 1f, Color color = default(Color))
            => Barh(null, y, width, color);

        public void Barh(float[] x, float[] y, float width = 1f, Color color = default(Color))
            => _plotMeshHandler.Barh(x, y, width, SetColor(color));

        public void Mesh3D(float[,] x, float[,] y, float[,] z, Gradient mapColor, float lineWidth = 1f)
            => _plotMeshHandler.Mesh3D(x, y, z, mapColor, lineWidth);

        public void Surface3D(float[,] x, float[,] y, float[,] z, Gradient mapColor)
            => _plotMeshHandler.Surface3D(x, y, z, mapColor);

        private Color SetColor(Color color)
        {
            if (color != default(Color)) return color;
            if (_plotColorList.Length == 0) return Color.white;

            _colorCount++;
            if (_colorCount == _plotColorList.Length) _colorCount = 0;
            return _plotColorList[_colorCount];
        }

        public void Draw()
        {
            _plotMeshHandler.Draw();
            _colorCount = -1;
        }

        public void Clear()
        {
            _plotMeshHandler.Clear();
            _colorCount = -1;
        }

        private void OnApplicationQuit()
        {
            Clear();
        }
    }
}