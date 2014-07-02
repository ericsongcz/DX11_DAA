#ifndef EDITOR_H
#define EDITOR_H

#include <QtWidgets/QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include "qtpropertybrowser/qtpropertymanager.h"
#include "qtpropertybrowser/qteditorfactory.h"
#include "qtpropertybrowser/qttreepropertybrowser.h"
#include <QMap>
#include "ui_editor.h"
#include "d3drenderingwidget.h"
#include "Direct3DRenderer.h"
#include "Geometry.h"
#include "Camera.h"
#include "GameTimer.h"
#include "FBXImporter.h"

class Editor : public QMainWindow
{
	Q_OBJECT

	enum ERenderingPath
	{
		RP_FOWARD,
		RP_DEFERRED
	};

	enum EFogType
	{
		FT_LINEAR,
		FT_EXP,
		FT_EXP2
	};

	const QString SAMPLER_FILTER = tr("Sampler Filter");
	const QString SAMPLER_FILTER_LINEAR = tr("Linear");
	const QString SAMPLER_FILTER_ANISOTROPIC = tr("Anisotropic");
	const QString RENDERING_PATH = tr("Rendering Path");
	const QString FORWARD_RENDERING = tr("Forward Rendering");
	const QString DEFFERED_RENDERING = tr("Deferred Rendering");
	const QString WIREFRAME_MODE = tr("Wireframe Mode");
	const QString SHOW_TEXTURE = tr("Show Texture");
	const QString CLEAR_COLOR = tr("Clear Color");
	const QString AMBIENT_COLOR = tr("Ambient Color");
	const QString AMBIENT_INTENSITY = tr("Ambient Intensity");
	const QString AMBIENT_INTENSITY_SLIDER = tr("Ambient Intensity Slider");
	const QString DIFFUSE_COLOR = tr("Diffuse Color");
	const QString DIFFUSE_INTENSITY = tr("Diffuse Intensity");
	const QString DIFFUSE_INTENSITY_SLIDER = tr("Diffuse Intensity Slider");
	const QString FOG = tr("Fog");
	const QString SHOW_FOG = tr("Show Fog");
	const QString FOG_TYPE = tr("Fog Type");
	const QString FOG_DENSITY = tr("Fog Density");
	const QString FOG_DENSITY_SLIDER = tr("Fog Density Slider");
	const QString FOG_TYPE_LINEAR = tr("Linear");
	const QString FOG_TYPE_EXP = tr("Fog Exp");
	const QString FOG_TYPE_EXP2 = tr("Fog Exp2");
	const QString FOG_COLOR = tr("Fog Color");
	const QString FOG_START = tr("Fog Start");
	const QString FOG_RANGE = tr("Fog Range");
public:
	Editor(QWidget *parent = 0);
	~Editor();

	HWND getHWND();
	void drawScene();
	void createPropertyBrowser();
protected:
	virtual void keyPressEvent(QKeyEvent *event);
	virtual void resizeEvent(QResizeEvent* event);
	virtual void paintEvent(QPaintEvent* event);
	virtual void mouseMoveEvent(QMouseEvent* event);
	virtual void mousePressEvent(QMouseEvent* event);
	virtual void mouseReleaseEvent(QMouseEvent* event);
	virtual QPaintEngine* paintEngine() const { return NULL; }
	virtual void wheelEvent(QWheelEvent* event);
private slots:
	void loadModel();
	void fillModeChanged(QtProperty* property, bool value);
	void showTextureChanged(QtProperty* property, bool value);
	void renderingPathChanged(QtProperty* property, int value);
	void samplerFilterChanged(QtProperty* property, int value);
	void clearColorChanged(QtProperty* property, const QColor& value);
	void ambientColorChanged(QtProperty* property, const QColor& value);
	void ambientIntensityChanged(QtProperty *property, int value);
	void diffuseColorChanged(QtProperty* property, const QColor& value);
	void diffuseIntensityChanged(QtProperty* property, int value);
	void fogColorChanged(QtProperty* property, const QColor& value);
	void fogTypeChanged(QtProperty* property, int value);
	void fogStartChanged(QtProperty* property, double value);
	void fogRangeChanged(QtProperty* property, double value);
	void fogDensityChanged(QtProperty* property, int value);
	void enableFogChanged(QtProperty* property, bool value);
private:
	Ui::EditorClass ui;
	D3DRenderingWidget* d3dWidget;
	Direct3DRenderer* mRenderer;
	Camera* mCamera;
	Geometry* mGeometry;
	float mScreenWidth;
	float mScreenHeight;
    int mMenuBarHeight;
	int mToolBarHeight;
	int mStatusBarHeight;
	int mRenderWidgetTopOffset;
	int mRenderWidgetBottomOffset;
	GameTimer mTimer;
	FBXImporter* mFBXImporter;
	bool mRenderModel;
	bool mWireframe;
	bool mShowTexture;
	bool mMouseRightButtonDown;
	bool mDeferredRendering;
	QHBoxLayout* mLeftLayout;
	QHBoxLayout* mRightLayout;
	QMap<QString, QtProperty*> mPropertys;
	QMap<QString, bool> mPropertyValues;
	QMap<QString, int> mPropertyIndices;
	QtEnumPropertyManager* mSamplerFilterTypePropertyManager;
	QtEnumPropertyManager* mRenderingPathPropertyManager;
	QtBoolPropertyManager* mFillModePropertyManager;
	QtBoolPropertyManager* mShowTexturePropertyManager;
	QtColorPropertyManager* mClearColorPropertyManager;
	QtColorPropertyManager* mAmbientColorPropertyManager;
	QtColorPropertyManager* mDiffuseColorPropertyManager;
	QtIntPropertyManager* mAmbientIntensitySpinBoxPropertyManager;
	QtIntPropertyManager* mAmbientIntensitySliderPropertyManager;
	QtIntPropertyManager* mDiffuseIntensitySpinBoxPropertyManager;
	QtIntPropertyManager* mDiffuseIntensitySliderPropertyManager;
	QtDoublePropertyManager* mFogStartPropertyManager;
	QtDoublePropertyManager* mFogRangePropertyManager;
	QtColorPropertyManager* mFogColorPropertyManager;
	QtEnumPropertyManager* mFogTypePropertyManager;
	QtIntPropertyManager* mFogDensitySpinBoxPropertyManager;
	QtIntPropertyManager* mFogDensitySliderPropertyManager;
	QtBoolPropertyManager* mEnableFogPropertyManager;
	QLabel* mLocationLabel;
	int mLastMousePositionX;
	int mLastMousePositionY;
	XMMATRIX mRotateAxisX;
	XMMATRIX mRotateAxisY;
	XMMATRIX mRotate;
	XMFLOAT4 mAmbientColor;
	float mAmbientIntensity;
	XMFLOAT4 mDiffuseColor;
	float mDiffuseIntensity;
	XMFLOAT4 mFogColor;
	float mFogStart;
	float mFogRange;
	float mFogDensity;
	EFogType mFogType;
	bool mShowFog;
	QString mFBXFileName;
};

#endif // EDITOR_H
