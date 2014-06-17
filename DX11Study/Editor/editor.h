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
	
	const QString WIREFRAME_MODE = tr("Wireframe Mode");
	const QString SHOW_TEXTURE = tr("Show Texture");
	const QString CLEAR_COLOR = tr("Clear Color");
	const QString AMBIENT_COLOR = tr("Ambient Color");
	const QString AMBIENT_INTENSITY = tr("Ambient Intensity");
	const QString AMBIENT_INTENSITY_SLIDER = tr("Ambient Intensity Slider");
	const QString DIFFUSE_COLOR = tr("Diffuse Color");
	const QString DIFFUSE_INTENSITY = tr("Diffuse Intensity");
	const QString DIFFUSE_INTENSITY_SLIDER = tr("Diffuse Intensity Slider");
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
	void clearColorChanged(QtProperty* property, const QColor& value);
	void ambientColorChanged(QtProperty* property, const QColor& value);
	void ambientIntensityChanged(QtProperty *property, int value);
	void diffuseColorChanged(QtProperty* property, const QColor& value);
	void diffuseIntensityChanged(QtProperty* property, const int& value);
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
	QHBoxLayout* mLeftLayout;
	QHBoxLayout* mRightLayout;
	QMap<QString, QtProperty*> mPropertys;
	QMap<QString, bool> mPropertyValues;
	QMap<QString, int> mPropertyIndices;
	QtBoolPropertyManager* mFillModePropertyManager;
	QtBoolPropertyManager* mShowTexturePropertyManager;
	QtColorPropertyManager* mClearColorPropertyManager;
	QtColorPropertyManager* mAmbientColorPropertyManager;
	QtColorPropertyManager* mDiffuseColorPropertyManager;
	QtIntPropertyManager* mAmbientIntensitySpinBoxPropertManager;
	QtIntPropertyManager* mAmbientIntensitySliderPropertyManager;
	QtIntPropertyManager* mDiffuseIntensitySpinBoxPropertManager;
	QtIntPropertyManager* mDiffuseIntensitySliderPropertyManager;
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
};

#endif // EDITOR_H
