#include "stdafx.h"
#include "editor.h"
#include <QMessageBox>
#include <QFileDialog>
#include "qtpropertybrower/qtpropertymanager.h"
#include "qtpropertybrower/qtvariantproperty.h"
#include "qtpropertybrower/qttreepropertybrowser.h"
#include "D3DUtils.h"
#include "SharedParameters.h"
#include <iostream>

using namespace std;

Editor::Editor(QWidget *parent)
	: QMainWindow(parent),
	mRenderModel(false),
	mScreenWidth(0.0f),
	mScreenHeight(0.0f),
	mMenuBarHeight(0.0f),
	mToolBarHeight(0.0f),
	mStatusBarHeight(0.0f),
	mRenderWidgetTopOffset(0.0f),
	mRenderWidgetBottomOffset(0.0f)
{
	AllocConsole();
	FILE* file;
	freopen_s(&file, "CONOUT$", "w+t", stdout);
	freopen_s(&file, "CONIN$", "r+t", stdin);

	ui.setupUi(this);
	resize(QSize(900, 600));
	// 设置焦点，接受输入事件。
	setFocusPolicy(Qt::StrongFocus);

	QLabel* locationLabel = new QLabel(" W999 ");
	locationLabel->setAlignment(Qt::AlignHCenter);
	locationLabel->setMinimumSize(locationLabel->sizeHint());

	// 即便在Qt Designer里移除了statusBar，在调用statusBar()的时候就会重新创建。
	statusBar()->addWidget(locationLabel);

	mMenuBarHeight = menuBar()->height();
	mScreenWidth = width() - 200.0f;
	mStatusBarHeight = statusBar()->height();
	mRenderWidgetTopOffset = mMenuBarHeight + mToolBarHeight;
	mRenderWidgetBottomOffset = mStatusBarHeight;
	mScreenHeight = height();

	d3dWidget = new D3DRenderingWidget(mScreenWidth, mScreenHeight, this);
	d3dWidget->setGeometry(QRect(0, mRenderWidgetTopOffset, mScreenWidth, mScreenHeight));

	//QDockWidget* dock = new QDockWidget(this);
	//addDockWidget(Qt::LeftDockWidgetArea, dock);
	//dock->setWidget(d3dWidget);

	setWindowTitle(tr("Qt D3D Demo"));

	QHBoxLayout* mainLayout = new QHBoxLayout();
	mLeftLayout = new QHBoxLayout();
	mRightLayout = new QHBoxLayout();

	mLeftLayout->addWidget(d3dWidget);
	mainLayout->addLayout(mLeftLayout);
	mainLayout->addLayout(mRightLayout);

	setLayout(mainLayout);

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

			if (renderPackages[i].hasDiffuseTexture)
			{
				renderParameters.hasDiffuseTexture = true;
			}

			if (renderPackages[i].hasNormalMapTexture)
			{
				renderParameters.hasNormalMapTexture = true;
			}

			mRenderer->render(renderParameters, worldMatrix, mCamera->getViewMatrix(), mCamera->getProjectionMatrix());

			if (renderPackages[i].textures.size() > 0)
			{
				mRenderer->setShaderResource(&renderPackages[i].textures[0], renderPackages[i].textures.size());
			}

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
	QtVariantPropertyManager* variantManager = new QtVariantPropertyManager();
	int i = 0;
	QtProperty* topItem = variantManager->addProperty(QtVariantPropertyManager::groupTypeId(), QString::number(i++) + QLatin1String(" Group Property"));

	QtVariantProperty* item = variantManager->addProperty(QVariant::Bool, QString::number(i++) + QLatin1String(" Bool Property"));
	item->setValue(true);
	topItem->addSubProperty(item);

	QtVariantEditorFactory* variantFactory = new QtVariantEditorFactory();
	QtTreePropertyBrowser* variantEditor = new QtTreePropertyBrowser();

	variantEditor->setFactoryForManager(variantManager, variantFactory);
	variantEditor->addProperty(topItem);
	variantEditor->setPropertiesWithoutValueMarked(true);
	variantEditor->setRootIsDecorated(false);

	QDockWidget *dock = new QDockWidget(this);
	addDockWidget(Qt::RightDockWidgetArea, dock);
	dock->setWidget(variantEditor);

	variantEditor->show();

	// 可以获得QDockWidget添加QWidget之后的实际尺寸。
	int width = dock->sizeHint().width();
}
