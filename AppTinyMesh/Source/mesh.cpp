#include "mesh.h"

/*!
\class Mesh mesh.h

\brief Core triangle mesh class.
*/



/*!
\brief Initialize the mesh to empty.
*/
Mesh::Mesh()
{
}

/*!
\brief Initialize the mesh from a list of vertices and a list of triangles.

Indices must have a size multiple of three (three for triangle vertices and three for triangle normals).

\param vertices List of geometry vertices.
\param indices List of indices wich represent the geometry triangles.
*/
Mesh::Mesh(const std::vector<Vector>& vertices, const std::vector<int>& indices) :vertices(vertices), varray(indices)
{
	normals.resize(vertices.size(), Vector::Z);
}

/*!
\brief Create the mesh.

\param vertices Array of vertices.
\param normals Array of normals.
\param va, na Array of vertex and normal indexes.
*/
Mesh::Mesh(const std::vector<Vector>& vertices, const std::vector<Vector>& normals, const std::vector<int>& va, const std::vector<int>& na) :vertices(vertices), normals(normals), varray(va), narray(na)
{
}

/*!
\brief Creates an axis aligned box.

The object has 8 vertices, 6 normals and 12 triangles.
\param box The box.
*/
Mesh::Mesh(const Box& box)
{
	// Vertices
	vertices.resize(8);

	for (int i = 0; i < 8; i++)
	{
		vertices[i] = box.Vertex(i);
	}

	// Normals
	normals.push_back(Vector(-1, 0, 0));
	normals.push_back(Vector(1, 0, 0));
	normals.push_back(Vector(0, -1, 0));
	normals.push_back(Vector(0, 1, 0));
	normals.push_back(Vector(0, 0, -1));
	normals.push_back(Vector(0, 0, 1));

	// Reserve space for the triangle array
	varray.reserve(12 * 3);
	narray.reserve(12 * 3);

	AddTriangle(0, 2, 1, 4);
	AddTriangle(1, 2, 3, 4);

	AddTriangle(4, 5, 6, 5);
	AddTriangle(5, 7, 6, 5);

	AddTriangle(0, 4, 2, 0);
	AddTriangle(4, 6, 2, 0);

	AddTriangle(1, 3, 5, 1);
	AddTriangle(3, 7, 5, 1);

	AddTriangle(0, 1, 5, 2);
	AddTriangle(0, 5, 4, 2);

	AddTriangle(3, 2, 7, 3);
	AddTriangle(6, 7, 2, 3);
}

/*!
\brief
*/
Mesh::Mesh(const Sphere& sphere, int n)
{
	const Vector c = sphere.Center();
	const double r = sphere.Radius();
	const int stackCount = n;
	const int sectorCount = 2 * n;

	const double sectorStep = 2.0 * Math::Pi / sectorCount;
	const double stackStep = Math::Pi / stackCount;

	for (int i = 0; i <= stackCount; i++)
	{
		double stackAngle = Math::Pi / 2 - i * stackStep;		// starting from pi/2 to -pi/2
		for (int j = 0; j <= sectorCount; ++j)
		{
			double sectorAngle = j * sectorStep;				// starting from 0 to 2pi

			double x = r * cos(stackAngle) * cos(sectorAngle);	// r * cos(u) * cos(v)
			double y = r * cos(stackAngle) * sin(sectorAngle);	// r * cos(u) * sin(v)
			double z = r * sin(stackAngle);						// r * sin(u)
			vertices.push_back(Vector(x, y, z));

			normals.push_back(Normalized(vertices.back() - c));
		}
	}

	for (int i = 0; i < stackCount; i++)
	{
		int k1 = i * (sectorCount + 1);     // beginning of current stack
		int k2 = k1 + sectorCount + 1;      // beginning of next stack
		for (int j = 0; j < sectorCount; j++, k1++, k2++)
		{
			// 2 triangles per sector excluding first and last stacks
			// k1 => k2 => k1+1
			if (i != 0)
				AddSmoothTriangle(k1, k1, k2, k2, k1 + 1, k1 + 1);

			// k1+1 => k2 => k2+1
			if (i != (stackCount - 1))
				AddSmoothTriangle(k1 + 1, k1 + 1, k2, k2, k2 + 1, k2 + 1);
		}
	}
}

/*!
\brief
*/
Mesh::Mesh(const Disc& d, int n)
{
	const Vector c = d.Center();
	const double r = d.Radius();

	// Orthonormal basis
	const Vector z = Normalized(d.Normal());
	Vector x, y;
	z.Orthonormal(x, y);

	// Vertices
	const double dtPhi = Math::TwoPi / n;
	vertices.reserve(n + 1); // +1 for the center
	for (int i = 0; i < n; i++)
	{
		const double phi = i * dtPhi;
		const Vector v = x * cos(phi) + y * sin(phi);
		vertices.push_back(c + r * v);
	}
	vertices.push_back(c);

	// Single normal
	normals.push_back(z);

	// Triangles
	for (int i = 0; i < n; i++)
		AddTriangle(n, i, (i + 1) % n, 0);
}

/*!
\brief
*/
Mesh::Mesh(const Cylinder& c, int n)
{
	const Vector a = c.Vertex(0);
	const Vector b = c.Vertex(1);
	const double r = c.Radius();

	// Orthonormal basis
	const Vector z = Normalized(b - a);
	Vector x, y;
	z.Orthonormal(x, y);

	vertices.reserve((n * 2) + 2); // +2 for the cap centers

	// First cap
	const double dtPhi = Math::TwoPi / n;
	for (int i = 0; i < n; i++)
	{
		const double phi = i * dtPhi;
		const Vector v = x * cos(phi) + y * sin(phi);
		vertices.push_back(a + r * v);
	}
	vertices.push_back(a);
	normals.push_back(-z);
	for (int i = 0; i < n; i++)
		AddTriangle(n, i, (i + 1) % n, 0);

	// Second cap
	const int offset = int(vertices.size());
	for (int i = 0; i < n; i++)
	{
		const double phi = i * dtPhi;
		const Vector v = x * cos(phi) + y * sin(phi);
		vertices.push_back(b + r * v);
	}
	vertices.push_back(b);
	normals.push_back(z);
	for (int i = 0; i < n; i++)
		AddTriangle(offset + n, offset + i, offset + ((i + 1) % n), 0);

	// Interior triangles (with normals)
	for (int i = 0; i < n; i++)
	{
		const Vector nn = Normalized(a - vertices[i]);
		normals.push_back(nn);

		AddTriangle(i, offset + i, (i + 1) % n, int(normals.size()) - 1);
		AddTriangle((i + 1) % n, offset + i, offset + ((i + 1) % n), int(normals.size()) - 1);
	}
}

/*!
\brief
*/
Mesh::Mesh(const Torus& torus, int n, int slice)
{
	const Vector c = torus.Center();
	const double r = torus.Radius();
	const double r1 = torus.InnerRadius();

	// Orthonormal basis
	const Vector z = Normalized(torus.Normal());
	Vector x, y;
	z.Orthonormal(x, y);

	// Vertices
	const double dtPhi = Math::TwoPi / double(slice);
	const double dtTheta = Math::TwoPi / double(n);
	for (int i = 0; i < slice; i++)
	{
		const double theta = i * dtPhi;
		for (int j = 0; j < n; j++)
		{
			const double phi = j * dtTheta;
			
			const Vector u = cos(theta) * x + sin(theta) * y;
			const Vector v = cos(phi) * u + sin(phi) * z;

			vertices.push_back(c + r * u + r1 * v);
			normals.push_back(v);
		}
	}

	const int size = slice;
	const int p = n;

	// Triangles
	for (int j = 0; j < slice; j++)
	{
		for (int i = 0; i < p; i++)
			AddSmoothQuadrangle(j * p + i, j * p + i, ((j + 1) % n) * p + i, ((j + 1) % n) * p + i, ((j + 1) % n) * p + (i + 1) % p, ((j + 1) % n) * p + (i + 1) % p, j * p + (i + 1) % p, j * p + (i + 1) % p);
	}
}

/*!
\brief Empty
*/
Mesh::~Mesh()
{
}

/*!
\brief Reserve memory for arrays.
\param nv,nn,nvi,nvn Number of vertices, normals, vertex indexes and vertex normals.
*/
void Mesh::Reserve(int nv, int nn, int nvi, int nvn)
{
	vertices.reserve(nv);
	normals.reserve(nn);
	varray.reserve(nvi);
	narray.reserve(nvn);
}

/*!
\brief Smooth the normals of the mesh.

This function weights the normals of the faces by their corresponding area.
\sa Triangle::AreaNormal()
*/
void Mesh::SmoothNormals()
{
	// Initialize 
	normals.resize(vertices.size(), Vector::Null);

	narray = varray;

	// Accumulate normals
	for (int i = 0; i < varray.size(); i += 3)
	{
		Vector tn = Triangle(vertices[varray.at(i)], vertices[varray.at(i + 1)], vertices[varray.at(i + 2)]).AreaNormal();
		normals[narray[i + 0]] += tn;
		normals[narray[i + 1]] += tn;
		normals[narray[i + 2]] += tn;
	}

	// Normalize 
	for (int i = 0; i < normals.size(); i++)
	{
		Normalize(normals[i]);
	}
}

/*!
\brief Add a smooth triangle to the geometry.
\param a, b, c Index of the vertices.
\param na, nb, nc Index of the normals.
*/
void Mesh::AddSmoothTriangle(int a, int na, int b, int nb, int c, int nc)
{
	varray.push_back(a);
	narray.push_back(na);
	varray.push_back(b);
	narray.push_back(nb);
	varray.push_back(c);
	narray.push_back(nc);
}

/*!
\brief Add a triangle to the geometry.
\param a, b, c Index of the vertices.
\param n Index of the normal.
*/
void Mesh::AddTriangle(int a, int b, int c, int n)
{
	varray.push_back(a);
	narray.push_back(n);
	varray.push_back(b);
	narray.push_back(n);
	varray.push_back(c);
	narray.push_back(n);
}

/*!
\brief Add a smmoth quadrangle to the geometry.

Creates two smooth triangles abc and acd.

\param a, b, c, d  Index of the vertices.
\param na, nb, nc, nd Index of the normal for all vertices.
*/
void Mesh::AddSmoothQuadrangle(int a, int na, int b, int nb, int c, int nc, int d, int nd)
{
	// First triangle
	AddSmoothTriangle(a, na, b, nb, c, nc);

	// Second triangle
	AddSmoothTriangle(a, na, c, nc, d, nd);
}

/*!
\brief Add a quadrangle to the geometry.

\param a, b, c, d  Index of the vertices and normals.
*/
void Mesh::AddQuadrangle(int a, int b, int c, int d)
{
	AddSmoothQuadrangle(a, a, b, b, c, c, d, d);
}

/*!
\brief Compute the bounding box of the object.
*/
Box Mesh::GetBox() const
{
	if (vertices.size() == 0)
	{
		return Box::Null;
	}
	return Box(vertices);
}

/*!
\brief Scale the mesh.
\param s Scaling factor.
*/
void Mesh::Scale(double s)
{
	// Vertexes
	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i] *= s;
	}

	if (s < 0.0)
	{
		// Normals
		for (int i = 0; i < normals.size(); i++)
		{
			normals[i] = -normals[i];
		}
	}
}

/*!
\brief
*/
void Mesh::Scale(const Matrix3& m)
{
	const Matrix3 m_inv_t = m.Inverse().Transpose();
	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i] = m * vertices[i];
		normals[i] = Normalized(m_inv_t * normals[i]);
	}
}

/*!
\brief
*/
void Mesh::Rotate(const Matrix3& m)
{
	for (int i = 0; i < vertices.size(); i++)
		vertices[i] = m * vertices[i];
	for (int i = 0; i < normals.size(); i++)
		normals[i] = m * normals[i];
}


void Mesh::SphereWarp(const Vector& c, double r, const Vector& d)
{
	for (int i = 0; i < vertices.size(); i++)
	{
		const double dd = Norm(vertices[i] - c);
		double t = dd / r;
		t = Math::Clamp(t);
		vertices[i] += d * t;
	}

	SmoothNormals();
}


#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QRegularExpression>
#include <QtCore/qstring.h>

/*!
\brief Import a mesh from an .obj file.
\param filename File name.
*/
void Mesh::Load(const QString& filename)
{
	vertices.clear();
	normals.clear();
	varray.clear();
	narray.clear();

	QFile data(filename);

	if (!data.open(QFile::ReadOnly))
		return;
	QTextStream in(&data);

	// Set of regular expressions : Vertex, Normal, Triangle
	QRegularExpression rexv("v\\s*([-|+|\\s]\\d*\\.\\d+)\\s*([-|+|\\s]\\d*\\.\\d+)\\s*([-|+|\\s]\\d*\\.\\d+)");
	QRegularExpression rexn("vn\\s*([-|+|\\s]\\d*\\.\\d+)\\s*([-|+|\\s]\\d*\\.\\d+)\\s*([-|+|\\s]\\d*\\.\\d+)");
	QRegularExpression rext("f\\s*(\\d*)/\\d*/(\\d*)\\s*(\\d*)/\\d*/(\\d*)\\s*(\\d*)/\\d*/(\\d*)");
	while (!in.atEnd())
	{
		QString line = in.readLine();
		QRegularExpressionMatch match = rexv.match(line);
		QRegularExpressionMatch matchN = rexn.match(line);
		QRegularExpressionMatch matchT = rext.match(line);
		if (match.hasMatch())//rexv.indexIn(line, 0) > -1)
		{
			Vector q = Vector(match.captured(1).toDouble(), match.captured(2).toDouble(), match.captured(3).toDouble()); vertices.push_back(q);
		}
		else if (matchN.hasMatch())//rexn.indexIn(line, 0) > -1)
		{
			Vector q = Vector(matchN.captured(1).toDouble(), matchN.captured(2).toDouble(), matchN.captured(3).toDouble());  normals.push_back(q);
		}
		else if (matchT.hasMatch())//rext.indexIn(line, 0) > -1)
		{
			varray.push_back(matchT.captured(1).toInt() - 1);
			varray.push_back(matchT.captured(3).toInt() - 1);
			varray.push_back(matchT.captured(5).toInt() - 1);
			narray.push_back(matchT.captured(2).toInt() - 1);
			narray.push_back(matchT.captured(4).toInt() - 1);
			narray.push_back(matchT.captured(6).toInt() - 1);
		}
	}
	data.close();
}

/*!
\brief Save the mesh in .obj format, with vertices and normals.
\param url Filename.
\param meshName %Mesh name in .obj file.
*/
void Mesh::SaveObj(const QString& url, const QString& meshName) const
{
	QFile data(url);
	if (!data.open(QFile::WriteOnly))
		return;
	QTextStream out(&data);
	out << "g " << meshName << Qt::endl;
	for (int i = 0; i < vertices.size(); i++)
		out << "v " << vertices.at(i)[0] << " " << vertices.at(i)[1] << " " << vertices.at(i)[2] << QString('\n');
	for (int i = 0; i < normals.size(); i++)
		out << "vn " << normals.at(i)[0] << " " << normals.at(i)[1] << " " << normals.at(i)[2] << QString('\n');
	for (int i = 0; i < varray.size(); i += 3)
	{
		out << "f " << varray.at(i) + 1 << "//" << narray.at(i) + 1 << " "
			<< varray.at(i + 1) + 1 << "//" << narray.at(i + 1) + 1 << " "
			<< varray.at(i + 2) + 1 << "//" << narray.at(i + 2) + 1 << " "
			<< "\n";
	}
	out.flush();
	data.close();
}
