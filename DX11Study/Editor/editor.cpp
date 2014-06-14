#include "stdafx.h"
#include "editor.h"
#include "D3DUtils.h"
#include "SharedParameters.h"
#include <iostream>

using namespace std;

Editor::Editor(QWidget *parent)
	: QMainWindow(parent),
	mRenderModel(false),
	mWireframe(false),
	mShowTexture(false),
	mMouseRightButtonDown(false),
	mScreenWidth(0.0f),
	mScreenHeight(0.0f),
	mMenuBarHeight(0.0f),
	mToolBarHeight(0.0f),
	mStatusBarHeight(0.0f),
	mRenderWidgetTopOffset(0.0f),
	mRenderWidgetBottomOffset(0.0f),
	mLastMousePositionX(0),
	mLastMousePositionY(0),
	mRotateAxisX(XMMatrixIdentity()),
	mRotateAxisY(XMMatrixIdentity()),
	mRotate(XMMatrixIdentity())
{
	AllocConsole();
	FILE* file;
	freopen_s(&file, "CONOUT$", "w+t", stdout);
	freopen_s(&file, "CONIN$", "r+t", stdin);

	ui.setupUi(this);
	resize(QSize(900, 600));
	// 设置焦点，接受输入事件。
	setFocusPolicy(Qt::StrongFocus);

	mLocationLabel = new QLabel(" W999 ");
	mLocationLabel->setAlignment(Qt::AlignHCenter);
	mLocationLabel->setMinimumSize(mLocationLabel->sizeHint());

	// 即便在Qt Designer里移除了statusBar，在调用statusBar()的时候就会重新创建。
	statusBar()->addWidget(mLocationLabel);

	mMenuBarHeight = menuBar()->height();
	mScreenWidth = width() - 200.0f;
	mStatusBarHeight = statusBar()->height();
	mRenderWidgetTopOffset = mMenuBarHeight + mToolBarHeight;
	mRenderWidgetBottomOffset = mStatusBarHeight;
	mScreenHeight = height();

	d3dWidget = new D3DRenderingWidget(mScreenWidth, mScreenHeight, this);
	d3dWidget->setGeometry(QRect(0, mRenderWidgetTopOffset, mScreenWidth, mScreenHeight));

	// setCentralWidget可以使QWidget根据其他的QDockWidget来自适应缩放。
	setCentralWidget(d3dWidget);

	setWindowTitle(tr("Qt D3D Demo"));

	//QDockWidget* dock = new QDockWidget(this);
	//addDockWidget(Qt::LeftDockWidgetArea, dock);
	//dock->setWidget(d3dWidget);

	QTimer* timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->start(20);

	mRenderer = new Direct3DRenderer(mScreenWidth, mScreenHeight, L"Qt D3D Demo");
	mRenderer->initD3D(getHWND());
	mRenderer->setViewport(mScreenWidth, mScreenHeight, 0.0f, 1.0f, 0.0f, 0.0f);

	mFBXImporter = new FBXImporter();
	mFBXImporter->Init();
	mGeometry = new Geometry();

	mCamera = new Camera();
	mCamera->setAspectRatio(mScreenWidth / mScreenHeight);

	mTimer.Reset();
	QMenu* menu = menuBar()->actions().at(0)->menu();
	QAction* loadModel = menu->actions().at(0);

	connect(loadModel, SIGNAL(triggered()), this, SLOT(loadModel()));

	setMouseTracking(true);

	createPropertyBrowser();
}

Editor::~Editor()
{
	FreeConsole();
	SafeDelete(mRenderer);
}

void Editor::keyPressEvent(QKeyEvent *event)
{
	float time = mTimer.DeltaTime();
	float speed = 5.0f;
	float rotateRate = 1.0f;

	switch (event->key())
	{
	case Qt::Key_W:
		mCamera->fly(speed * time);

		break;
	case Qt::Key_S:
		mCamera->fly(-speed * time);

		break;
	case Qt::Key_A:
		mCamera->strafe(-speed * time);

		break;
	case Qt::Key_D:
		mCamera->strafe(speed * time);

		break;
	case Qt::Key_Q:
		mCamera->walk(speed * time);

		break;
	case Qt::Key_E:
		mCamera->walk(-speed * time);

		break;
	case Qt::Key_F:
		mRenderer->switchFillMode();
		
		mWireframe = !mWireframe;

		mFillModePropertyManager->setValue(mPropertys[WIREFRAME_MODE], mWireframe);

		break;

	case Qt::Key_T:
		mShowTexture = !mShowTexture;

		mShowTexturePropertyManager->setValue(mPropertys[SHOW_TEXTURE], mShowTexture);

		break;
	case Qt::Key_Up:
	{
		mCamera->pitch(-rotateRate * time);
	}

		break;
	case Qt::Key_Down:
	{
		mCamera->pitch(rotateRate * time);
	}

		break;
	case Qt::Key_Left:
		mCamera->yaw(rotateRate * time);

		break;
	case Qt::Key_Right:
		mCamera->yaw(-rotateRate * time);

		break;
	case Qt::Key_Escape:
		close();

		break;
	}
}

void Editor::resizeEvent(QResizeEvent* event)
{
	QMainWindow::resizeEvent(event);

	int border = geometry().x() - x();

	mScreenWidth = width() - 200.0f;
	mScreenHeight = height() - (mRenderWidgetTopOffset + mRenderWidgetBottomOffset) + border;

	d3dWidget->resize(mScreenWidth, mScreenHeight);

	mRenderer->resizeBackBuffer(mScreenWidth, mScreenHeight);
	mCamera->setAspectRatio(mScreenWidth / mScreenHeight);
}

void Editor::paintEvent(QPaintEvent* event)
{
	if (updatesEnabled())
	{
		mTimer.Tick();
		drawScene();
	}
}

HWND Editor::getHWND()
{
	return (HWND)d3dWidget->winId();
}

void Editor::drawScene()
{
	mRenderer->beginScene();

	XMMATRIX worldMatrix = XMMatrixIdentity();

#if USE_RIGHT_HAND
	XMVECTOR eye = XMLoadFloat3(&XMFLOAT3(0.0f, 0.0f, 10.0f));
#else
	XMVECTOR eye = XMLoadFloat3(&XMFLOAT3(0.0f, 0.0f, -10.0f));
#endif
	XMVECTOR lookAt = XMLoadFloat3(&XMFLOAT3(0.0f, 0.0f, 0.0f));
	XMVECTOR up = XMLoadFloat3(&XMFLOAT3(0.0f, 1.0f, 0.0f));

#if USE_RIGHT_HAND
	XMMATRIX viewMatrix = XMMatrixLookAtRH(eye, lookAt, up);

	XMMATRIX projectionMatrix = XMMatrixPerspectiveFovRH(XM_PI / 4, mScreenWidth / mScreenHeight, 1.0f, 1000.0f);
#else
	XMMATRIX viewMatrix = XMMatrixLookAtLH(eye, lookAt, up);

	XMMATRIX projectionMatrix = XMMatrixPerspectiveFovLH(XM_PI / 4, mScreenWidth / mScreenHeight, 1.0f, 1000.0f);
#endif

	if (mRenderModel)
	{
		MeshData* meshData = mGeometry->GetMeshData();
		vector<RenderPackage> renderPackages = meshData->renderPackages;
		int renderPackageSize = renderPackages.size();

		for (int i = 0; i < renderPackageSize; i++)
		{
			RenderParameters renderParameters;

			worldMatrix = renderPackages[i].globalTransform;

			worldMatrix = XMMatrixMultiply(worldMatrix, mRotate);

			if (mShowTexture)
			{
				if (renderPackages[i].hasDiffuseTexture)
				{
					renderParameters.hasDiffuseTexture = true;
				}
			}

			if (renderPackages[i].hasNormalMapTexture)
			{
				renderParameters.hasNormalMapTexture = true;
			}

			if (renderPackages[i].textures.size() > 0)
			{
				mRenderer->setShaderResource(&renderPackages[i].textures[0], renderPackages[i].textures.size());
			}

			mRenderer->render(renderParameters, worldMatrix, mCamera->getViewMatrix(), mCamera->getProjectionMatrix());



			mGeometry->renderBuffer(renderPackages[i].indicesCount, renderPackages[i].indicesOffset, 0);
		}
	}

	mRenderer->endScene();
}

void Editor::loadModel()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open 3D Model"), tr("."), tr("Model Files(*.fbx)"));

	if (fileName.length() == 0)
	{
		QMessageBox::information(this, tr("Path"), tr("You didn's select any files!"));
	}
	else
	{
		mFBXImporter->LoadScene(fileName.toStdString().c_str());
		mFBXImporter->WalkHierarchy();

		mGeometry->FillMeshData(mFBXImporter->GetMeshInfo());

		if (!mGeometry->Initialize(SharedParameters::device, SharedParameters::deviceContext))
		{
			return;
		}

		mRenderModel = true;
	}
}

void Editor::createPropertyBrowser()
{
	QDockWidget *dock = new QDockWidget(this);
	addDockWidget(Qt::RightDockWidgetArea, dock);

	QtTreePropertyBrowser* variantEditor = new QtTreePropertyBrowser(dock);
	dock->setWidget(variantEditor);

	QtGroupPropertyManager* commonGroupPropertyManager = new QtGroupPropertyManager(this);
	QtProperty* group = commonGroupPropertyManager->addProperty("Common");

	mFillModePropertyManager = new QtBoolPropertyManager(this);
	connect(mFillModePropertyManager, SIGNAL(valueChanged(QtProperty*, bool)), this, SLOT(fillModeChanged(QtProperty*, bool)));

	QtCheckBoxFactory* checkBoxFactory = new QtCheckBoxFactory(this);
	variantEditor->setFactoryForManager(mFillModePropertyManager, checkBoxFactory);

	QtProperty* property = mFillModePropertyManager->addProperty(WIREFRAME_MODE);
	mFillModePropertyManager->setValue(property, false);
	mPropertys[WIREFRAME_MODE] = property;

	group->addSubProperty(property);

	mShowTexturePropertyManager = new QtBoolPropertyManager(this);
	connect(mShowTexturePropertyManager, SIGNAL(valueChanged(QtProperty*, bool)), this, SLOT(showTextureChanged(QtProperty*, bool)));
	variantEditor->setFactoryForManager(mShowTexturePropertyManager, checkBoxFactory);

	property = mShowTexturePropertyManager->addProperty(SHOW_TEXTURE);
	mShowTexturePropertyManager->setValue(property, true);
	mPropertys[SHOW_TEXTURE] = property;

	group->addSubProperty(property);

	mClearColorPropertyManager = new QtColorPropertyManager(this);
	QtSpinBoxFactory* spinBoxFactory = new QtSpinBoxFactory(this);
	QtColorEditorFactory* colorEditorFactory = new QtColorEditorFactory(this);
	connect(mClearColorPropertyManager, SIGNAL(valueChanged(QtProperty*, const QColor&)), this, SLOT(clearColorChanged(QtProperty*, const QColor&)));
	variantEditor->setFactoryForManager(mClearColorPropertyManager->subIntPropertyManager(), spinBoxFactory);
	variantEditor->setFactoryForManager(mClearColorPropertyManager, colorEditorFactory);

	property = mClearColorPropertyManager->addProperty(CLEAR_COLOR);
	mClearColorPropertyManager->setValue(property, QColor(100, 149, 237));
	mPropertys[CLEAR_COLOR] = property;

	group->addSubProperty(property);
	variantEditor->addProperty(group);

	QtGroupPropertyManager* lightingGroupProperyManager = new QtGroupPropertyManager(this);
	group = lightingGroupProperyManager->addProperty("Lighting");

	mAmbientColorPropertyManager = new QtColorPropertyManager(this);
	property = mAmbientColorPropertyManager->addProperty(AMBIENT_COLOR);
	mAmbientColorPropertyManager->setValue(property, QColor(255, 255, 255));
	mPropertys[AMBIENT_COLOR] = property;

	group->addSubProperty(property);

	connect(mAmbientColorPropertyManager, SIGNAL(valueChanged(QtProperty*, const QColor&)), this, SLOT(ambientColorChanged(QtProperty*, const QColor&)));
	variantEditor->setFactoryForManager(mAmbientColorPropertyManager->subIntPropertyManager(), spinBoxFactory);
	variantEditor->setFactoryForManager(mAmbientColorPropertyManager, colorEditorFactory);

	mAmbientIntensitySpinBoxPropertManager = new QtIntPropertyManager(this);
	variantEditor->setFactoryForManager(mAmbientIntensitySpinBoxPropertManager, spinBoxFactory);

	property = mAmbientIntensitySpinBoxPropertManager->addProperty(AMBIENT_INTENSITY);
	mAmbientIntensitySpinBoxPropertManager->setValue(property, 50);
	mAmbientIntensitySpinBoxPropertManager->setMinimum(property, 0);
	mAmbientIntensitySpinBoxPropertManager->setMaximum(property, 100);
	mPropertys[AMBIENT_INTENSITY] = property;

	group->addSubProperty(property);

	mAmbientIntensitySliderPropertyManager = new QtIntPropertyManager(this);
	QtSliderFactory* sliderFactory = new QtSliderFactory(this);
	variantEditor->setFactoryForManager(mAmbientIntensitySliderPropertyManager, sliderFactory);

	property = mAmbientIntensitySliderPropertyManager->addProperty(AMBIENT_INTENSITY);
	mAmbientIntensitySliderPropertyManager->setValue(property, 50);
	mAmbientIntensitySliderPropertyManager->setMinimum(property, 0);
	mAmbientIntensitySliderPropertyManager->setMaximum(property, 100);
	mPropertys[AMBIENT_INTENSITY] = property;

	group->addSubProperty(property);

	variantEditor->addProperty(group);

	// 设置是否显示属性前面的折叠小箭头。
	variantEditor->setPropertiesWithoutValueMarked(true);
	//variantEditor->setRootIsDecorated(false);

	variantEditor->show();

	// 可以获得QDockWidget添加QWidget之后的实际尺寸。
	int width = dock->sizeHint().width();
}

void Editor::fillModeChanged(QtProperty* property, bool value)
{
	mWireframe = value;

	if (mWireframe)
	{
		mRenderer->changeFillMode(D3D11_FILL_WIREFRAME);
	}
	else
	{
		mRenderer->changeFillMode(D3D11_FILL_SOLID);
	}
}

void Editor::showTextureChanged(QtProperty* property, bool value)
{
	mShowTexture = value;
}

void Editor::mouseMoveEvent(QMouseEvent* event)
{
	int deltaX = event->x() - mLastMousePositionX;
	int deltaY = event->y() - mLastMousePositionY;

	if (event->buttons() & Qt::LeftButton)
	{
		mRotateAxisX = XMMatrixRotationAxis(XMLoadFloat3(&XMFLOAT3(0.0f, 1.0f, 0.0f)), (float)deltaX / 100.0f);
		mRotateAxisY = XMMatrixRotationAxis(XMLoadFloat3(&XMFLOAT3(1.0f, 0.0f, 0.0f)), (float)deltaY / 100.0f);

		mRotate *= mRotateAxisX * mRotateAxisY;

		Log("Mouse left button drag.\n");
	}

	if (event->buttons() & Qt::RightButton)
	{
		mCamera->yaw(-(float)deltaX / 1000.0f);
		mCamera->pitch(-(float)deltaY / 1000.0f);
	}

	mLastMousePositionX = event->x();
	mLastMousePositionY = event->y();

	// 这里需要减去QMenuBar的高度才能得到正确的y坐标位置，因为我们是在QMainWindow而不是QWidget里追踪鼠标位置。
	mLocationLabel->setText(tr("x = ") + QString::number(event->x()) + ", y = " + QString::number(event->y() - mMenuBarHeight));
}

void Editor::mousePressEvent(QMouseEvent* event)
{
	mLastMousePositionX = event->x();
	mLastMousePositionY = event->y();

	if (event->button() == Qt::RightButton)
	{
		mMouseRightButtonDown = true;

		Log("Mouse right button down.\n");
	}
}

void Editor::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::RightButton)
	{
		mMouseRightButtonDown = false;

		Log("Mouse right button up.\n");
	}
}

void Editor::clearColorChanged(QtProperty* property, const QColor& value)
{
	mRenderer->setClearColor(value.red(), value.green(), value.blue());
}

void Editor::ambientColorChanged(QtProperty* property, const QColor& value)
{

}
