#include "stdafx.h"
#include "d3drenderingwidget.h"
#include "D3DUtils.h"
#include "FBXImporter.h"
#include "SharedParameters.h"

D3DRenderingWidget::D3DRenderingWidget(QWidget *parent)
	: QWidget(parent)
{

}

D3DRenderingWidget::D3DRenderingWidget(int width, int height, QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	mScreenWidth = width;
	mScreenHeight = height;
	float topLeftY = ((QMainWindow*)parent)->menuBar()->height();

	setAttribute(Qt::WA_PaintOnScreen, true);
	setAttribute(Qt::WA_NativeWindow, true);
	//setFocusPolicy(Qt::StrongFocus);

	QTimer* timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->start(20);

	//mRenderer = new Direct3DRenderer(mScreenWidth, mScreenHeight, L"Qt D3D Demo");
	//mRenderer->initD3D(getHWND());
	//mRenderer->setViewport(mScreenWidth, mScreenHeight, 0.0f, 1.0f, 0.0f, 0.0f);

	//FBXImporter* fbxImporter = new FBXImporter();
	//fbxImporter->Init();
	//fbxImporter->LoadScene("teapotTextured.fbx");
	//fbxImporter->WalkHierarchy();

	//mGeometry = new Geometry();
	//mGeometry->FillMeshData(fbxImporter->GetMeshInfo());

	//if (!mGeometry->Initialize(SharedParameters::device, SharedParameters::deviceContext))
	//{
	//	return;
	//}

	//mCamera = new Camera();
	//mCamera->setAspectRatio(mScreenWidth / mScreenHeight);

	//mTimer.Reset();
}

D3DRenderingWidget::~D3DRenderingWidget()
{
	SafeDelete(mRenderer);
}

void D3DRenderingWidget::paintEvent(QPaintEvent* event)
{
	if (updatesEnabled())
	{
		mTimer.Tick();
		//drawScene();
	}
}

HWND D3DRenderingWidget::getHWND()
{
	return (HWND)winId();
}

void D3DRenderingWidget::resizeEvent(QResizeEvent* event)
{
	//mScreenWidth = event->size().width();
	//mScreenHeight = event->size().height();

	//mRenderer->resizeBackBuffer(mScreenWidth, mScreenHeight);
	//mCamera->setAspectRatio(mScreenWidth / mScreenHeight);
}

void D3DRenderingWidget::drawScene()
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

	mRenderer->endScene();
}

void D3DRenderingWidget::keyPressEvent(QKeyEvent *event)
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
	}
}
