using UnityEngine;
using UnityEditor;
using UnityPlotter;

[CustomEditor(typeof(Plotter))]
[CanEditMultipleObjects]
public class PlotterEditor : Editor
{
    public override void OnInspectorGUI()
    {
        base.OnInspectorGUI();
        serializedObject.Update();

        Plotter _target = (Plotter)target;

        EditorGUILayout.Space();
        EditorGUILayout.LabelField("Render options", EditorStyles.boldLabel);
        EditorGUI.indentLevel = 1;
        EditorGUILayout.PropertyField(serializedObject.FindProperty("_orderInLayer"), new GUIContent("orderInLayer"), true);
        EditorGUILayout.PropertyField(serializedObject.FindProperty("_renderMode"), new GUIContent("renderMode"), true);
        if (_target.renderMode == Plotter.RenderMode.Render3D) {
            EditorGUILayout.PropertyField(serializedObject.FindProperty("_rotationHandle"), new GUIContent("rotationHandle"), true);
        }
        EditorGUI.indentLevel = 0;

        EditorGUILayout.Space();
        EditorGUILayout.LabelField("Axis options", EditorStyles.boldLabel);
        EditorGUI.indentLevel = 1;
        EditorGUILayout.LabelField("X axis", EditorStyles.boldLabel);
        EditorGUI.indentLevel = 2;
        EditorGUILayout.LabelField("Limit");
        EditorGUI.indentLevel = 3;
        EditorGUILayout.PropertyField(serializedObject.FindProperty("_xLimitMin"),  new GUIContent("Min"),  true);
        EditorGUILayout.PropertyField(serializedObject.FindProperty("_xLimitMax"),  new GUIContent("Max"),  true);
        if (_target.useAxisLine) {
            if (_target.useAxisTick) {
                EditorGUI.indentLevel = 2;
                EditorGUILayout.PropertyField(serializedObject.FindProperty("_xTickSpan"),  new GUIContent("TickSpan"),  true);
            }
        }

        EditorGUILayout.Space();
        EditorGUI.indentLevel = 1;
        EditorGUILayout.LabelField("Y axis", EditorStyles.boldLabel);
        EditorGUI.indentLevel = 2;
        EditorGUILayout.LabelField("Limit");
        EditorGUI.indentLevel = 3;
        EditorGUILayout.PropertyField(serializedObject.FindProperty("_yLimitMin"),  new GUIContent("Min"),  true);
        EditorGUILayout.PropertyField(serializedObject.FindProperty("_yLimitMax"),  new GUIContent("Max"),  true);
        if (_target.useAxisLine) {
            if (_target.useAxisTick) {
                EditorGUI.indentLevel = 2;
                EditorGUILayout.PropertyField(serializedObject.FindProperty("_yTickSpan"),  new GUIContent("TickSpan"),  true);
            }
        }

        EditorGUILayout.Space();
        EditorGUI.indentLevel = 1;
        EditorGUILayout.LabelField("Z axis", EditorStyles.boldLabel);
        EditorGUI.indentLevel = 2;
        EditorGUILayout.LabelField("Limit");
        EditorGUI.indentLevel = 3;
        EditorGUILayout.PropertyField(serializedObject.FindProperty("_zLimitMin"),  new GUIContent("Min"),  true);
        EditorGUILayout.PropertyField(serializedObject.FindProperty("_zLimitMax"),  new GUIContent("Max"),  true);
        EditorGUI.indentLevel = 0;

        EditorGUILayout.Space();
        GUILayout.Label("Graphic options", EditorStyles.boldLabel);
        EditorGUI.indentLevel = 1;
        EditorGUILayout.LabelField("Axis", EditorStyles.boldLabel);
        EditorGUI.indentLevel = 2;
        EditorGUILayout.PropertyField(serializedObject.FindProperty("_origin"), new GUIContent("origin"), true);
        EditorGUILayout.PropertyField(serializedObject.FindProperty("_textFont"), new GUIContent("textFont"), true);
        EditorGUILayout.PropertyField(serializedObject.FindProperty("_fontSize"), new GUIContent("fontSize"), true);
        EditorGUILayout.PropertyField(serializedObject.FindProperty("_useAxisLine"), new GUIContent("useAxisLine"), true);
        if (_target.useAxisLine) {
            EditorGUI.indentLevel = 3;
            EditorGUILayout.PropertyField(serializedObject.FindProperty("_axisLineThickness"), new GUIContent("axisLineThickness"), true);
            EditorGUILayout.PropertyField(serializedObject.FindProperty("_axisLineColor"),     new GUIContent("axisLineColor"),     true);
        }
        EditorGUI.indentLevel = 2;
        EditorGUILayout.PropertyField(serializedObject.FindProperty("_useAxisTick"), new GUIContent("useAxisTick"), true);
        if (_target.useAxisTick && _target.useAxisLine) {
            EditorGUI.indentLevel = 3;
            EditorGUILayout.PropertyField(serializedObject.FindProperty("_tickLength"), new GUIContent("tickLength"), true);
            EditorGUILayout.PropertyField(serializedObject.FindProperty("_tickRadius"), new GUIContent("tickThickness"), true);
        }

        EditorGUI.indentLevel = 1;
        EditorGUILayout.LabelField("Background", EditorStyles.boldLabel);
        EditorGUI.indentLevel = 2;
        EditorGUILayout.PropertyField(serializedObject.FindProperty("_backgroundColor"), new GUIContent("backgroundColor"), true);

        EditorGUI.indentLevel = 1;
        EditorGUILayout.LabelField("Plot color", EditorStyles.boldLabel);
        EditorGUI.indentLevel = 2;
        EditorGUILayout.PropertyField(serializedObject.FindProperty("_plotColorList"), new GUIContent("plotColorList"), true);
        EditorGUI.indentLevel = 0;

        serializedObject.ApplyModifiedProperties();
    }
}
