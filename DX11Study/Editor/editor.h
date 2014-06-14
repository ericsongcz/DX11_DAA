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

private slots:
	void loadModel();
	void fillModeChanged(QtProperty* property, bool value);
	void showTextureChanged(QtProperty* property, bool value);
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
	QtBoolPropertyManager* mFillModePropertyManager;
	QtBoolPropertyManager* mShowTexturePropertyManager;
	QLabel* mLocationLabel;
	int mLastMousePositionX;
	int mLastMousePositionY;
	XMMATRIX mRotateAxisX;
	XMMATRIX mRotateAxisY;
	XMMATRIX mRotate;
};

#endif // EDITOR_H
