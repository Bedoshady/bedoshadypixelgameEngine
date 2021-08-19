#include "bedoPIxelGameEngine.h"
#include <fstream>
#include<strstream>
#include <vector>
#include<algorithm>
#include<list>

class Graphics3D : public bedoPixelGameEngine {
private:
	float fov = 90;
	float a = (float)ScreenHeight() / (float)ScreenWidth();
	float fRad = 1.0f / tanf((float)fov * 0.5f / 180.0f * 3.14159f);
	float Zfar = 1000;
	float Znear = 0.1f;


	float yaw = 0;
	struct Matrix {
		float i[4] = { 0,0,0,0 };
		float j[4] = { 0,0,0,0 };
		float k[4] = { 0,0,0,0 };
		float w[4] = { 0,0,0,0 };

	};


	struct vec3d {
		float x = 0, y = 0, z = 0;


	};
	struct vec2d {
		float u = 0.0, v = 0.0, w = 1;

	};
	struct Triangle {
		vec3d p[3];
		//vec2d tex[3];

	};
	struct mesh {
		std::vector<Triangle> tris;
	};



	bool LoadFromObjectFile(std::string sFilename, bool bHasTexture = false)
	{
		std::ifstream f(sFilename);
		if (!f.is_open())
			return false;


		// Local cache of verts
		std::vector<vec3d> verts;
		std::vector<vec2d> texs;

		while (!f.eof())
		{
			char line[128];
			f.getline(line, 128);

			std::strstream s;
			s << line;

			char junk;

			if (line[0] == 'v')
			{
				if (line[1] == 't')
				{
					vec2d v;
					s >> junk >> junk >> v.u >> v.v;
					// A little hack for the spyro texture
					//v.u = 1.0f - v.u;
					//v.v = 1.0f - v.v;
					texs.push_back(v);
				}
				else
				{
					vec3d v;
					s >> junk >> v.x >> v.y >> v.z;
					verts.push_back(v);
				}
			}

			if (!bHasTexture)
			{
				if (line[0] == 'f')
				{
					int f[3];
					s >> junk >> f[0] >> f[1] >> f[2];
					meshCube.tris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
				}
			}
			else
			{
				if (line[0] == 'f')
				{
					s >> junk;

					std::string tokens[6];
					int nTokenCount = -1;


					while (!s.eof())
					{
						char c = s.get();
						if (c == ' ' || c == '/')
							nTokenCount++;
						else
							tokens[nTokenCount].append(1, c);
					}

					tokens[nTokenCount].pop_back();


					//meshCube.tris.push_back({ verts[stoi(tokens[0]) - 1], verts[stoi(tokens[2]) - 1], verts[stoi(tokens[4]) - 1],
						//texs[stoi(tokens[1]) - 1], texs[stoi(tokens[3]) - 1], texs[stoi(tokens[5]) - 1] });

				}

			}
		}
		return true;

	}
	vec3d CrossProduct(vec3d t1, vec3d t2) {
		vec3d ans;
		ans.x = t1.y * t2.z - t1.z * t2.y;
		ans.y = -(t1.x * t2.z - t1.z * t2.x);
		ans.z = t1.x * t2.y - t1.y * t2.x;
		return ans;
	}
	float dotProduct(const vec3d& t1, const vec3d& t2) {

		return t1.x * t2.x + t1.y * t2.y + t1.z * t2.z;

	}
	vec3d normalize(vec3d t) {
		float length = sqrt(t.x * t.x + t.y * t.y + t.z * t.z);

		return { t.x / length, t.y / length, t.z / length };
	}


	vec3d IntersectPointWithPlane(vec3d Plane, vec3d PlaneNor, vec3d start, vec3d end, float& t) {
		float d = dotProduct(Plane, PlaneNor);
		vec3d endsubStart = vec3d{ end.x - start.x, end.y - start.y, end.z - start.z };
		t = (float)(d - dotProduct(start, PlaneNor)) / dotProduct(PlaneNor, endsubStart);

		return { t * (end.x - start.x) + start.x, t * (end.y - start.y) + start.y, t * (end.z - start.z) + start.z };
	}
	vec3d Vector_Add(vec3d t1, vec3d t2) {
		return{ t1.x + t2.x, t1.y + t2.y, t1.z + t2.z };

	}
	vec3d Vector_Sub(vec3d t1, vec3d t2) {
		return{ t1.x - t2.x, t1.y - t2.y, t1.z - t2.z };

	}
	vec3d Vector_Mul(vec3d t1, float t) {
		return{ t1.x * t, t1.y * t, t1.z * t };

	}


	int clip(vec3d planePoint, vec3d planeNor, Triangle tri, Triangle& tri2, Triangle& tri3)
	{
		//normalize(planeNor);
		vec3d* outsidePoints[3], * insidePoints[3];
		vec2d* outsideTexPoints[3], * insideTexPoints[3];
		int numberOfinsidePoints = 0, numberOfOutsidePoints = 0;
		for (int i = 0; i < 3; i++) {
			if (dotProduct(Vector_Sub(tri.p[i], planePoint), planeNor) >= 0) {
				insidePoints[numberOfinsidePoints++] = &tri.p[i];
				//insideTexPoints[numberOfinsidePoints - 1] = &tri.tex[i];
			}
			else {
				outsidePoints[numberOfOutsidePoints++] = &tri.p[i];
				//outsideTexPoints[numberOfOutsidePoints - 1] = &tri.tex[i];
			}
		}
		if (numberOfinsidePoints == 0) {
			return 0;
		}
		if (numberOfinsidePoints == 3)
		{
			tri2 = tri;
			return 1;
		}
		if (numberOfinsidePoints == 1) {
			float t;
			tri2.p[0] = *insidePoints[0];
			//tri2.tex[0] = *insideTexPoints[0];

			tri2.p[1] = IntersectPointWithPlane(planePoint, planeNor, *insidePoints[0], *outsidePoints[0], t);

//			tri2.tex[1].u = (*insideTexPoints[0]).u + ((*outsideTexPoints[0]).u - (*insideTexPoints[0]).u) * t;
	//		tri2.tex[1].v = (*insideTexPoints[0]).v + ((*outsideTexPoints[0]).v - (*insideTexPoints[0]).v) * t;
		//	tri2.tex[1].w = (*insideTexPoints[0]).w + ((*outsideTexPoints[0]).w - (*insideTexPoints[0]).w) * t;

			tri2.p[2] = IntersectPointWithPlane(planePoint, planeNor, *insidePoints[0], *outsidePoints[1], t);

			//tri2.tex[2].u = (*insideTexPoints[0]).u + ((*outsideTexPoints[1]).u - (*insideTexPoints[0]).u) * t;
			//tri2.tex[2].v = (*insideTexPoints[0]).v + ((*outsideTexPoints[1]).v - (*insideTexPoints[0]).v) * t;
			//tri2.tex[2].w = (*insideTexPoints[0]).w + ((*outsideTexPoints[1]).w - (*insideTexPoints[0]).w) * t;
			return 1;

		}
		if (numberOfinsidePoints == 2) {
			float t;
			tri2.p[0] = *insidePoints[0];
			//tri2.tex[0] = *insideTexPoints[0];

			tri2.p[1] = *insidePoints[1];
			//tri2.tex[1] = *insideTexPoints[1];

			tri2.p[2] = IntersectPointWithPlane(planePoint, planeNor, *insidePoints[1], *outsidePoints[0], t);
		//	tri2.tex[2].u = (*insideTexPoints[0]).u + ((*outsideTexPoints[0]).u - (*insideTexPoints[1]).u) * t;
		//	tri2.tex[2].v = (*insideTexPoints[0]).v + ((*outsideTexPoints[0]).v - (*insideTexPoints[1]).v) * t;
		//	tri2.tex[2].w = (*insideTexPoints[0]).w + ((*outsideTexPoints[0]).w - (*insideTexPoints[1]).w) * t;


			tri3.p[0] = *insidePoints[0];
	//		tri3.tex[0] = *insideTexPoints[0];

			tri3.p[1] = tri2.p[2];
//			tri3.tex[1] = tri2.tex[2];

			tri3.p[2] = IntersectPointWithPlane(planePoint, planeNor, *insidePoints[0], *outsidePoints[0], t);

			//tri3.tex[2].u = (*insideTexPoints[1]).u + ((*outsideTexPoints[0]).u - (*insideTexPoints[0]).u) * t;
			//tri3.tex[2].v = (*insideTexPoints[1]).v + ((*outsideTexPoints[0]).v - (*insideTexPoints[0]).v) * t;
			//tri3.tex[2].w = (*insideTexPoints[1]).w + ((*outsideTexPoints[0]).w - (*insideTexPoints[0]).w) * t;

			return 2;


		}

	}

public:
	Graphics3D() {
		appname = L"3D Graphics";
	}


	mesh meshCube;
	Matrix Projection;
	//Matrix RotationX;
	//Matrix RotationZ;
	Matrix matY;
	Matrix translation;
	vec3d Camera = { 0,0,0 };
	vec3d lookPos = { 0,0,1 };
	Matrix cam;
protected:
	bool OnUserCreate() {
		a = (float)ScreenHeight() / (float)ScreenWidth();
	/*	std::string filename = "C:\\dev\\test\\utah tea pot.obj";
		if (!LoadFromObjectFile(filename, false))return false;*/

		Projection.i[0] = 1 * fRad * a;
		Projection.i[1] = 0;
		Projection.i[2] = 0;
		Projection.i[3] = 0;

		//
		Projection.j[0] = 0;
		Projection.j[1] = 1 * fRad;
		Projection.j[2] = 0;
		Projection.j[3] = 0;
		//

		Projection.k[0] = 0;
		Projection.k[1] = 0;
		Projection.k[2] = (float)Zfar / (float)(Zfar - Znear);
		Projection.k[3] = 1.0f;


		Projection.w[0] = 0;
		Projection.w[1] = 0;
		Projection.w[2] = -(float)Znear * Zfar / (Zfar - Znear);
		Projection.w[3] = 0;


		meshCube.tris = {

		// SOUTH
		{ 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

		// EAST                                                      
		{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

		// NORTH                                                     
		{ 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

		// WEST                                                      
		{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

		// TOP                                                       
		{ 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
		{ 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

		// BOTTOM                                                    
		{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },

		};
		float pi = 3.141592653;
		yaw = pi * 1;
		Camera.z = 4;
		//bool x = spr.LoadImage("C:\\dev\\test\\test\\spyro.png");
	//	if (x == false)
		//	std::cout << "error";
		//spr.Resize(20, 32)
		return true;
	}
	void MultiplyMatrix(const Matrix& matrix, const vec3d& point, vec3d& Output) {
		Output.x = point.x * matrix.i[0] + point.y * matrix.j[0] + point.z * matrix.k[0] + matrix.w[0];
		Output.y = point.x * matrix.i[1] + point.y * matrix.j[1] + point.z * matrix.k[1] + matrix.w[1];
		Output.z = point.x * matrix.i[2] + point.y * matrix.j[2] + point.z * matrix.k[2] + matrix.w[2];
		float w =  point.x * matrix.i[3] + point.y * matrix.j[3] + point.z * matrix.k[3] + matrix.w[3];
		if (w != 0)
		{
			Output.x /= (w);
			Output.y /= (w);
		}
	}
	float theata;
	vec3d light = { 0,0,-1 };
	bool OnUserUpdate(float fElapseTime)override {
		pixel rgb;
		rgb.SetRGB(0, 0, 0);
		Fill(0, 0, ScreenWidth(), ScreenHeight(), rgb);

		//theata += fElapseTime;


		/*RotationZ.i[0] = cosf(theata);
		RotationZ.i[1] = -sinf(theata);

		RotationZ.j[0] = sinf(theata);
		RotationZ.j[1] = cosf(theata);

		RotationZ.k[2] = 1;

		RotationX.j[1] = cosf(0.5 * theata);
		RotationX.j[2] = -sinf(0.5 * theata);

		RotationX.k[1] = sinf(0.5 * theata);
		RotationX.k[2] = cosf(0.5 * theata);

		RotationX.i[0] = 1;
		*/
		if (GetAsyncKeyState('D') & 0x8000)
			yaw += fElapseTime * 5;
		if (GetAsyncKeyState('A') & 0x8000)
			yaw -= fElapseTime * 5;

		matY.i[0] = cos(yaw);
		matY.i[2] = sin(yaw);
		matY.j[1] = 1;
		matY.k[0] = -sin(yaw);
		matY.k[2] = cos(yaw);

		cam.i[0] = cos(-yaw);
		cam.i[2] = sin(-yaw);
		cam.j[1] = 1;
		cam.k[0] = -sin(-yaw);
		cam.k[2] = cos(-yaw);

		translation.i[0] = 1;
		translation.j[1] = 1;
		translation.k[2] = 1;
		//translation.w[3] = 1;
		translation.w[0] = -Camera.x;
		translation.w[1] = -Camera.y;
		translation.w[2] = -Camera.z;
		lookPos = { 0,0,1 };
		MultiplyMatrix(cam, lookPos, lookPos);
		//	lookPos = { 0,0,1 };
		float speed = 2.0f;
		if (GetAsyncKeyState('X') & 0x8000) {
			//	Camera.z -= fElapseTime * 2;

			Camera.x -= speed * lookPos.x * fElapseTime * 2;
			Camera.y -= speed * lookPos.y * fElapseTime * 2;
			Camera.z -= speed * lookPos.z * fElapseTime * 2;
		}
		if (GetAsyncKeyState('W') & 0x8000)
		{
			Camera.x += speed * lookPos.x * fElapseTime * 2;
			Camera.y += speed * lookPos.y * fElapseTime * 2;
			Camera.z += speed * lookPos.z * fElapseTime * 2;
		}

		if (GetAsyncKeyState('P') & 0x8000) {
			Camera.y -= speed * fElapseTime * 2;
		}
		if (GetAsyncKeyState('L') & 0x8000) {
			Camera.y += speed * fElapseTime * 2;
		}
		std::vector<std::pair<Triangle, double>> trise;
		for (const auto& tri : meshCube.tris) {
			Triangle triProjected, triTranslated;//, triRotatedZX, triRotatedZ;

		
			MultiplyMatrix(translation, tri.p[0], triTranslated.p[0]);
			MultiplyMatrix(translation, tri.p[1], triTranslated.p[1]);
			MultiplyMatrix(translation, tri.p[2], triTranslated.p[2]);

			//triTranslated.tex[0] = tri.tex[0];
			//triTranslated.tex[1] = tri.tex[1];
			//triTranslated.tex[2] = tri.tex[2];

			MultiplyMatrix(matY, triTranslated.p[0], triTranslated.p[0]);
			MultiplyMatrix(matY, triTranslated.p[1], triTranslated.p[1]);
			MultiplyMatrix(matY, triTranslated.p[2], triTranslated.p[2]);





			// translation finished..............................................................







			vec3d toPoint, vec1, vec2, crossProduct;


			vec1.x = triTranslated.p[1].x - triTranslated.p[0].x;
			vec1.y = triTranslated.p[1].y - triTranslated.p[0].y;
			vec1.z = triTranslated.p[1].z - triTranslated.p[0].z;

			vec2.x = triTranslated.p[2].x - triTranslated.p[0].x;
			vec2.y = triTranslated.p[2].y - triTranslated.p[0].y;
			vec2.z = triTranslated.p[2].z - triTranslated.p[0].z;

			crossProduct = CrossProduct(vec1, vec2);

			toPoint.x = triTranslated.p[0].x;
			toPoint.y = triTranslated.p[0].y;
			toPoint.z = triTranslated.p[0].z;

			if (dotProduct(toPoint, crossProduct) < 0) {


				crossProduct = normalize(crossProduct);

				float dot = dotProduct(light, crossProduct);
				int nClippedTriangles = 0;
				Triangle clipped[2];
				nClippedTriangles = clip({ 0,0,0.1 }, { 0,0,1 }, triTranslated, clipped[0], clipped[1]);

				for (int i = 0; i < nClippedTriangles; i++) {
					/*clipped[i].tex[0].w = 1.0f / clipped[i].p[0].z;
					clipped[i].tex[0].u = clipped[i].tex[0].u * clipped[i].tex[0].w;
					clipped[i].tex[0].v = clipped[i].tex[0].v * clipped[i].tex[0].w;

					clipped[i].tex[1].w = 1.0f / clipped[i].p[1].z;
					clipped[i].tex[1].u = clipped[i].tex[1].u * clipped[i].tex[1].w;
					clipped[i].tex[1].v = clipped[i].tex[1].v * clipped[i].tex[1].w;

					clipped[i].tex[2].w = 1.0f / clipped[i].p[2].z;
					clipped[i].tex[2].u = clipped[i].tex[2].u * clipped[i].tex[2].w;
					clipped[i].tex[2].v = clipped[i].tex[2].v * clipped[i].tex[2].w;


					triProjected.tex[0] = clipped[i].tex[0];
					triProjected.tex[1] = clipped[i].tex[1];
					triProjected.tex[2] = clipped[i].tex[2];*/


					MultiplyMatrix(Projection, clipped[i].p[0], triProjected.p[0]);
					MultiplyMatrix(Projection, clipped[i].p[1], triProjected.p[1]);
					MultiplyMatrix(Projection, clipped[i].p[2], triProjected.p[2]);

					//triProjected = clipped[i];

					float sizeX = 0.5f * ScreenWidth();
					float sizeY = 0.5f * ScreenHeight();

					float offsetX = ScreenWidth() * 0.5f;
					float offsetY = ScreenHeight() * 0.5f;

					triProjected.p[0].x *= sizeX; triProjected.p[0].y *= sizeY;
					triProjected.p[1].x *= sizeX; triProjected.p[1].y *= sizeY;
					triProjected.p[2].x *= sizeX; triProjected.p[2].y *= sizeY;

					triProjected.p[0].x += offsetX; triProjected.p[0].y += offsetY;
					triProjected.p[1].x += offsetX; triProjected.p[1].y += offsetY;
					triProjected.p[2].x += offsetX; triProjected.p[2].y += offsetY;

					trise.push_back({ triProjected, dot });

				}
			}
		}
		std::sort(trise.begin(), trise.end(), [](std::pair<Triangle, double> t1, std::pair<Triangle, double> t2) {return (float)(t2.first.p[0].z + t2.first.p[1].z + t2.first.p[2].z) / 3.0f <
			(float)(t1.first.p[0].z + t1.first.p[1].z + t1.first.p[2].z) / 3.0f; });
		for (auto triToRaster : trise) {
			Triangle clipped[2];
			std::list<std::pair<Triangle, float>> listTriangles;
			std::pair<Triangle, float> f = { triToRaster.first, triToRaster.second };
			listTriangles.push_back(f);
			int nNewTriangles = 1;

			for (int p = 0; p < 4; p++)
			{
				int nTrisToAdd = 0;
				while (nNewTriangles > 0)
				{
					// Take triangle from front of queue
					std::pair<Triangle, float> test = listTriangles.front();
					listTriangles.pop_front();
					nNewTriangles--;

					switch (p)
					{
					case 0:	nTrisToAdd = clip({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, test.first, clipped[0], clipped[1]); break;
					case 1:	nTrisToAdd = clip({ 0.0f, (float)ScreenHeight() - 1, 0.0f }, { 0.0f, -1.0f, 0.0f }, test.first, clipped[0], clipped[1]); break;
					case 2:	nTrisToAdd = clip({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, test.first, clipped[0], clipped[1]); break;
					case 3:	nTrisToAdd = clip({ (float)ScreenWidth() - 1, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, test.first, clipped[0], clipped[1]); break;
					}


					for (int w = 0; w < nTrisToAdd; w++)
						listTriangles.push_back(std::pair<Triangle, float>{clipped[w], test.second});
				}
				nNewTriangles = listTriangles.size();
			}
			if (!listTriangles.empty()) {
				//		CHAR_INFO c = GetColor(triToRaster.second);

				for (auto& h : listTriangles) {
					//	auto v = triToRaster.first;
					Triangle v = h.first;
					pixel rgb;
					//float dx = triToRaster.
					rgb.SetRGB(h.second * 255, h.second * 255, h.second * 255);
					rgb.SetRGB(255, 255, 255);
					FillTriangle(v.p[0].x, v.p[0].y, v.p[1].x, v.p[1].y, v.p[2].x, v.p[2].y, rgb);

					//DrawTexturedTriangle(spr,v.p[0].x, v.p[0].y, v.tex[0].u, v.tex[0].v, v.tex[0].w,
						//					  v.p[1].x, v.p[1].y, v.tex[1].u, v.tex[1].v, v.tex[1].w,
							//				  v.p[2].x, v.p[2].y, v.tex[2].u, v.tex[2].v, v.tex[2].w);

					rgb.SetRGB(0, 0, 0);
					DrawTriangle(v.p[0].x, v.p[0].y, v.p[1].x, v.p[1].y, v.p[2].x, v.p[2].y, rgb);


				}
			}



		}
		return true;
	}


};



int main() {
	Graphics3D d;
	d.ConstructPixel(250, 150, 0, 0, 4, 4, L"hello");
	//d.ConstructConsole(250, 150, 4, 4);
	d.run();
}