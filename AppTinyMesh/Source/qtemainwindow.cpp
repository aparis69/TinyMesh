#include "qte.h"
#include "implicits.h"

MainWindow::MainWindow()
{
	// Chargement de l'interface
	uiw.setupUi(this);

	// Chargement du GLWidget
	meshWidget = new MeshWidget;
	QGridLayout* GLlayout = new QGridLayout;
	GLlayout->addWidget(meshWidget, 0, 0);
	GLlayout->setContentsMargins(0, 0, 0, 0);
	uiw.widget_GL->setLayout(GLlayout);

	// Creation des connect
	CreateActions();

	meshWidget->SetCamera(Camera(Vector(10, 0, 0), Vector(0.0, 0.0, 0.0)));
}

MainWindow::~MainWindow()
{
	delete meshWidget;
}

void MainWindow::CreateActions()
{
	// Buttons
	connect(uiw.boxMesh, SIGNAL(clicked()), this, SLOT(BoxMeshExample()));
	connect(uiw.sphereImplicit, SIGNAL(clicked()), this, SLOT(SphereImplicitExample()));
	connect(uiw.resetcameraButton, SIGNAL(clicked()), this, SLOT(ResetCamera()));
	connect(uiw.wireframe, SIGNAL(clicked()), this, SLOT(UpdateMaterial()));
	connect(uiw.radioShadingButton_1, SIGNAL(clicked()), this, SLOT(UpdateMaterial()));
	connect(uiw.radioShadingButton_2, SIGNAL(clicked()), this, SLOT(UpdateMaterial()));

	// Widget edition
	connect(meshWidget, SIGNAL(_signalEditSceneLeft(const Ray&)), this, SLOT(editingSceneLeft(const Ray&)));
	connect(meshWidget, SIGNAL(_signalEditSceneRight(const Ray&)), this, SLOT(editingSceneRight(const Ray&)));
}

void MainWindow::editingSceneLeft(const Ray&)
{
}

void MainWindow::editingSceneRight(const Ray&)
{
}

void MainWindow::BoxMeshExample()
{
	Mesh boxMesh = Mesh(Box(1.0));

	std::vector<Color> cols;
	cols.resize(boxMesh.Vertexes());
	for (int i = 0; i < cols.size(); i++)
		cols[i] = Color(double(i) / 6.0, fmod(double(i) * 39.478378, 1.0), 0.0);

	meshColor = MeshColor(boxMesh, cols, boxMesh.VertexIndexes());
	UpdateGeometry();
}

void MainWindow::SphereImplicitExample()
{
	AnalyticScalarField implicit;

	//Mesh mesh = Mesh(Torus(Vector::Null, Normalized(Vector(1, 0.2, 0.5)), 2.0, 0.5), 16, 16);
	//Mesh mesh = Mesh(Cylinder(Vector(0), Vector(2, 2, 1), 1.0), 16);
	//Mesh mesh = Mesh(Disc(Vector::Null, Normalized(Vector(1, 0.2, 0.5)), 2.0), 32);
	Mesh mesh = Mesh(Sphere(Vector::Null, 1.0), 16);
	//mesh.Scale(Matrix3::Scaling(Vector(1.0, 1.0, 2.0)));

	//mesh.SphereWarp(Vector(1, 0, 0), 0.5, -Normalized(Vector(1, 0, 1)));
	mesh.Scale(Matrix3::Scaling(Vector(1, 2, 3)));

	std::vector<Color> cols;
	cols.resize(mesh.Vertexes());
	for (int i = 0; i < cols.size(); i++)
	  cols[i] = Color(0.8, 0.8, 0.8);

	meshColor = MeshColor(mesh, cols, mesh.VertexIndexes());
	UpdateGeometry();
}

void MainWindow::UpdateGeometry()
{
	meshWidget->ClearAll();
	meshWidget->AddMesh("BoxMesh", meshColor);

	uiw.lineEdit->setText(QString::number(meshColor.Vertexes()));
	uiw.lineEdit_2->setText(QString::number(meshColor.Triangles()));

	UpdateMaterial();
}

void MainWindow::UpdateMaterial()
{
	meshWidget->UseWireframeGlobal(uiw.wireframe->isChecked());

	if (uiw.radioShadingButton_1->isChecked())
		meshWidget->SetMaterialGlobal(MeshMaterial::Normal);
	else
		meshWidget->SetMaterialGlobal(MeshMaterial::Color);
}

void MainWindow::ResetCamera()
{
	meshWidget->SetCamera(Camera(Vector(-10.0), Vector(0.0)));
}
