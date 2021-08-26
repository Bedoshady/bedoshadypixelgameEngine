//#include "bedoConsoleGameEingine.h"
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
	pixel *screenBuffer;
	float *depth;
	
	float yaw =  0;
	struct Matrix {
		float i[4] = { 0,0,0,0 };
		float j[4] = { 0,0,0,0 };
		float k[4] = { 0,0,0,0 };
		float w[4] = { 0,0,0,0 };

	};


	struct vec3d {
		float x = 0, y = 0, z = 0;
		float w = 1;

	};
	struct vec2d {
		float u = 0.0, v = 0.0, w = 1;
		
	};
	struct Triangle {
		vec3d p[3];
		vec2d tex[3];

	};
	struct mesh {
		std::vector<Triangle> tris;
	};

//	bool LoadFromObjectFile(std::string fileName, bool hasTexture = false) {
		/*std::ifstream f(fileName);
		if (!f.is_open())
			return false;

		std::vector<vec3d> vec;
		std::vector<vec2d> tex;
		while (!f.eof()) {
			char line[128];
			f.getline(line, 128);

			std::strstream s;
			s << line;
			char junk;

			if (line[0] == 'v')
			{
				vec3d v; 
				vec2d te;
				if (hasTexture) {
					if (line[1] == 't')
						s >> junk >> junk >> te.u >> te.v;
					else
						s >> junk >> v.x >> v.y >> v.z;
				}else
				s >> junk >> v.x >> v.y >> v.z;
				vec.push_back(v);
				tex.push_back(te);
			}
			if (line[0] == 'f') {
				if (!hasTexture) {
					int f[3];
					s >> junk >> f[0] >> f[1] >> f[2];
					meshCube.tris.push_back({ vec[f[0] - 1], vec[f[1] - 1] , vec[f[2] - 1] });
				}
				else {
					std::string token[6];
					int tokenNumber = -1;
					while (!s.eof()) {
						char c = s.get();
						if (c == ' ' || c == '/') {
							tokenNumber++;
						}
						else {
							token[tokenNumber].append(1, c);
						}
					}
					token[tokenNumber].pop_back();
					meshCube.tris.push_back({ vec[stoi(token[0]) - 1], vec[stoi(token[2]) - 1], vec[stoi(token[4]) - 1],
											 tex[stoi(token[1]) - 1], tex[stoi(token[3]) - 1], tex[stoi(token[5]) - 1] });
				}


			}
		}*/

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
						s >> junk >> junk >> v.v >> v.u;
						// A little hack for the spyro texture
						v.u = 1.0f - v.u;
						v.v = 1.0f - v.v;
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


						meshCube.tris.push_back({ verts[stoi(tokens[0]) - 1], verts[stoi(tokens[2]) - 1], verts[stoi(tokens[4]) - 1],
							texs[stoi(tokens[1]) - 1], texs[stoi(tokens[3]) - 1], texs[stoi(tokens[5]) - 1] });

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
	float dotProduct(const vec3d &t1,const vec3d &t2) {

		return t1.x * t2.x + t1.y * t2.y + t1.z * t2.z;

	}
	vec3d normalize(vec3d t) {
		float length = sqrt(t.x * t.x + t.y * t.y + t.z * t.z);

		return { t.x / length, t.y / length, t.z / length };
	}


	vec3d IntersectPointWithPlane(vec3d Plane, vec3d PlaneNor, vec3d start, vec3d end,float& t) {
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
		vec3d *outsidePoints[3], *insidePoints[3];
		vec2d* outsideTexPoints[3], * insideTexPoints[3];
		int numberOfinsidePoints = 0, numberOfOutsidePoints = 0;
		for (int i = 0; i < 3; i++) {
			if (dotProduct(Vector_Sub(tri.p[i],planePoint), planeNor) >= 0) {
				insidePoints[numberOfinsidePoints++] = &tri.p[i];
				insideTexPoints[numberOfinsidePoints-1] = &tri.tex[i];
			}
			else {
				outsidePoints[numberOfOutsidePoints++] = &tri.p[i];
				outsideTexPoints[numberOfOutsidePoints - 1] = &tri.tex[i];
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
			tri2.tex[0] = *insideTexPoints[0];
		
			tri2.p[1] = IntersectPointWithPlane(planePoint, planeNor, *insidePoints[0], *outsidePoints[0], t);
			
			tri2.tex[1].u = (*insideTexPoints[0]).u + ((*outsideTexPoints[0]).u - (*insideTexPoints[0]).u) * t;
			tri2.tex[1].v = (*insideTexPoints[0]).v + ((*outsideTexPoints[0]).v - (*insideTexPoints[0]).v) * t;
			tri2.tex[1].w = (*insideTexPoints[0]).w + ((*outsideTexPoints[0]).w - (*insideTexPoints[0]).w) * t;

			tri2.p[2] = IntersectPointWithPlane(planePoint, planeNor, *insidePoints[0], *outsidePoints[1],t);
			
			tri2.tex[2].u = (*insideTexPoints[0]).u + ((*outsideTexPoints[1]).u - (*insideTexPoints[0]).u) * t;
			tri2.tex[2].v = (*insideTexPoints[0]).v + ((*outsideTexPoints[1]).v - (*insideTexPoints[0]).v) * t;
			tri2.tex[2].w = (*insideTexPoints[0]).w + ((*outsideTexPoints[1]).w - (*insideTexPoints[0]).w) * t;
			return 1;

		}
		if (numberOfinsidePoints == 2) {
			float t;
			tri2.p[0] = *insidePoints[0];
			tri2.tex[0] = *insideTexPoints[0];

			tri2.p[1] = *insidePoints[1];
			tri2.tex[1] = *insideTexPoints[1];
		
			tri2.p[2] = IntersectPointWithPlane(planePoint, planeNor, *insidePoints[1], *outsidePoints[0],t);
			tri2.tex[2].u = (*insideTexPoints[1]).u + ((*outsideTexPoints[0]).u - (*insideTexPoints[1]).u) * t;
			tri2.tex[2].v = (*insideTexPoints[1]).v + ((*outsideTexPoints[0]).v - (*insideTexPoints[1]).v) * t;
			tri2.tex[2].w = (*insideTexPoints[1]).w + ((*outsideTexPoints[0]).w - (*insideTexPoints[1]).w) * t;


			tri3.p[0] = *insidePoints[0];
			tri3.tex[0] = *insideTexPoints[0];

			tri3.p[1] = tri2.p[2];
			tri3.tex[1] = tri2.tex[2];

			tri3.p[2] = IntersectPointWithPlane(planePoint, planeNor, *insidePoints[0], *outsidePoints[0],t);

			tri3.tex[2].u = (*insideTexPoints[0]).u + ((*outsideTexPoints[0]).u - (*insideTexPoints[0]).u) * t;
			tri3.tex[2].v = (*insideTexPoints[0]).v + ((*outsideTexPoints[0]).v - (*insideTexPoints[0]).v) * t;
			tri3.tex[2].w = (*insideTexPoints[0]).w + ((*outsideTexPoints[0]).w - (*insideTexPoints[0]).w) * t;

			return 2;


		}

	}

public:
		Graphics3D() {
			appname = L"3D Graphics";
	}


		mesh meshCube;
		Matrix Projection;
		Matrix RotationX;
		Matrix RotationZ;
		Matrix matY;
		Matrix translation;
		vec3d Camera = {0,0,0};
		vec3d lookPos = { 0,0,1 };
		Matrix cam;
protected:
	void DrawTexturedTriangle(sprite spr, int x1, int y1, float u1, float v1, float z1, 
										   int x2, int y2, float u2, float v2, float z2,
										   int x3, int y3, float u3, float v3, float z3)
	{
		//sort according to y
		if (y2 < y1) {
			std::swap(y1, y2);
			std::swap(x1, x2);


			std::swap(u1, u2);
			std::swap(v1, v2);
			std::swap(z1, z2);


		}
		if (y3 < y1) {
			std::swap(y1, y3);
			std::swap(x1, x3);
	
			std::swap(u1, u3);
			std::swap(v1, v3);
			std::swap(z1, z3);
		}
		if (y3 < y2) {
			std::swap(y3, y2);
			std::swap(x3, x2);

			std::swap(u3, u2);
			std::swap(v3, v2);
			std::swap(z3, z2);
		}

		int dx1 = x2 - x1;
		int dx2 = x3 - x1;

		float du1 = u2 - u1;
		float du2 = u3 - u1;

		float dv1 = v2 - v1;
		float dv2 = v3 - v1;
		
		float dz1 = z2 - z1;
		float dz2 = z3 - z1;

		float slopePix1 = 0, slopePix2 = 0;
		
		float slopeTexu1 = 0, slopeTexu2 = 0;
		float slopeTexv1 = 0, slopeTexv2 = 0;
		float slopeTexz1 = 0, slopeTexz2 = 0;

		if (y2 != y1) {
			slopePix1  = (float)dx1 / (float)(y2 - y1);
			slopeTexu1 = (float)du1 / (float)(y2 - y1);
			slopeTexv1 = (float)dv1 / (float)(y2 - y1);
			slopeTexz1 = (float)dz1 / (float)(y2 - y1);

		}
		if (y3 != y1) {
			slopePix2  = (float)dx2 / (float)(y3 - y1);
			slopeTexu2 = (float)du2 / (float)(y3 - y1);
			slopeTexv2 = (float)dv2 / (float)(y3 - y1);
			slopeTexz2 = (float)dz2 / (float)(y3 - y1);

		}
		if (y1 < y2) {
			for (int i = y1; i < y2; i++) {
				int sx = x1 + (float)(i - y1) * slopePix1;
				int ex = x1 + (float)(i - y1) * slopePix2;
			
				float tex_u1 = u1 + (float)(i - y1) * slopeTexu1;
				float tex_u2 = u1 + (float)(i - y1) * slopeTexu2;
				
				float tex_v1 = v1 + (float)(i - y1) * slopeTexv1;
				float tex_v2 = v1 + (float)(i - y1) * slopeTexv2;

				float tex_z1 = z1 + (float)(i - y1) * slopeTexz1;
				float tex_z2 = z1 + (float)(i - y1) * slopeTexz2;

				if (ex < sx) {
					std::swap(sx, ex);
					std::swap(tex_u1, tex_u2);
					std::swap(tex_v1, tex_v2);
					std::swap(tex_z1, tex_z2);



				}
					for (int j = sx; j < ex; j++) {
						float t = (float)(j - sx) / (float)(ex - sx);
						float ju1 = tex_u1 + t * (tex_u2 - tex_u1);
						float jv1 = tex_v1 + t * (tex_v2 - tex_v1);
						float jz1 = tex_z1 + t * (tex_z2 - tex_z1);
						ju1 /= jz1;
						jv1 /= jz1;
						DrawPixel(j, i, spr.GetSamplePixel(jv1, ju1));//spr.GetSampleGlyph(ju1,jv1), spr.GetSampleColor(ju1,jv1));
						//if (depth[i * ScreenWidth() + j] > jz1) {
						//	//DrawPixel(j, i, spr.GetSamplePixel(ju1, jv1));//spr.GetSampleGlyph(ju1, jv1), spr.GetSampleColor(ju1, jv1));
						//	screenBuffer[i * ScreenWidth() + j] = spr.GetSamplePixel(ju1, jv1);
						//	depth[i * ScreenWidth() + j] = jz1;
						//}
				     }
			
			}


		}
		du1 = u3 - u2;
		dv1 = v3 - v2;
		dz1 = z3 - z2;
		if (y3 != y2) {
			slopePix1 = (float)(x3 - x2) / (float)(y3 - y2);
			slopeTexu1 = (float)du1 / (float)(y3 - y2);
			slopeTexv1 = (float)dv1 / (float)(y3 - y2);
			slopeTexz1 = (float)dz1 / (float)(y3 - y2);

		
		}
			if (y2 < y3) {
			for (int i = y2; i < y3; i++) {
				int sx = x2 + (float)(i - y2) * slopePix1;
				int ex = x1 + (float)(i - y1) * slopePix2;

				float tex_u1 = u2 + (float)(i - y2) * slopeTexu1;
				float tex_u2 = u1 + (float)(i - y1) * slopeTexu2;

				float tex_v1 = v2 + (float)(i - y2) * slopeTexv1;
				float tex_v2 = v1 + (float)(i - y1) * slopeTexv2;

				float tex_z1 = z2 + (float)(i - y2) * slopeTexz1;
				float tex_z2 = z1 + (float)(i - y1) * slopeTexz2;

				if (ex < sx) {
					std::swap(sx, ex);
					std::swap(tex_u1, tex_u2);
					std::swap(tex_v1, tex_v2);
					std::swap(tex_z1, tex_z2);



				}
				for (int j = sx; j < ex; j++) {
					float t = (float)(j - sx) / (float)(ex - sx);
					float ju1 = tex_u1 + t * (tex_u2 - tex_u1);
					float jv1 = tex_v1 + t * (tex_v2 - tex_v1);
					float jz1 = tex_z1 + t * (tex_z2 - tex_z1);
					ju1 /= jz1;
					jv1 /= jz1;
					jz1 = 1.0f / jz1;
						DrawPixel(j, i, spr.GetSamplePixel(jv1, ju1));//spr.GetSampleGlyph(ju1, jv1), spr.GetSampleColor(ju1, jv1));
					/*if (depth[i * ScreenWidth() + j] > jz1) {
						screenBuffer[i * ScreenWidth() + j] = spr.GetSamplePixel(ju1, jv1);
						depth[i * ScreenWidth() + j] = jz1;
					}*/
				}

			}


		}
	}
	sprite spr;
	bool OnUserCreate() {
		a = (float)ScreenHeight() / (float)ScreenWidth();
		std::string filename = "C:\\dev\\test\\test\\hub.obj";
		if (!LoadFromObjectFile(filename, true)) { std::cout << "error"; return false; }

		Projection.i[0] = 1 * fRad * a;
		Projection.i[1] = 0;
		Projection.i[2] = 0;
		Projection.i[3] = 0;

		//
		Projection.j[0] = 0;
		Projection.j[1] = fRad;
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


		// meshCube.tris = {

		//// SOUTH
		//{ 0.0f, 0.0f, 0.0f,1,    0.0f, 1.0f, 0.0f,1,    1.0f, 1.0f, 0.0f,1,   1,0,1, 1,1,1, 0,1,1},
		//{ 0.0f, 0.0f, 0.0f,1,    1.0f, 1.0f, 0.0f,1,    1.0f, 0.0f, 0.0f,1,   1,0,1, 0,1,1, 0,0,1},
		//				   
		//// EAST            
		//{ 1.0f, 0.0f, 0.0f,1,    1.0f, 1.0f, 0.0f,1,    1.0f, 1.0f, 1.0f,1,   1,0,1, 1,1,1, 0,1,1 },
		//{ 1.0f, 0.0f, 0.0f,1,    1.0f, 1.0f, 1.0f,1,    1.0f, 0.0f, 1.0f,1,   1,0,1, 0,1,1, 0,0,1 },
		//				  
		//// NORTH           
		//{ 1.0f, 0.0f, 1.0f,1,    1.0f, 1.0f, 1.0f,1,    0.0f, 1.0f, 1.0f,1,   1,0,1, 1,1,1, 0,1,1 },
		//{ 1.0f, 0.0f, 1.0f,1,    0.0f, 1.0f, 1.0f,1,    0.0f, 0.0f, 1.0f,1,   1,0,1, 0,1,1, 0,0,1 },
		//				  
		//// WEST            
		//{ 0.0f, 0.0f, 1.0f,1,    0.0f, 1.0f, 1.0f,1,    0.0f, 1.0f, 0.0f,1,   1,0,1, 1,1,1, 0,1,1 },
		//{ 0.0f, 0.0f, 1.0f,1,    0.0f, 1.0f, 0.0f,1,    0.0f, 0.0f, 0.0f,1,   1,0,1, 0,1,1, 0,0,1 },
		//				  
		//// TOP             
		//{ 0.0f, 1.0f, 0.0f,1,    0.0f, 1.0f, 1.0f,1,    1.0f, 1.0f, 1.0f,1,   1,0,1, 1,1,1, 0,1,1},
		//{ 0.0f, 1.0f, 0.0f,1,    1.0f, 1.0f, 1.0f,1,    1.0f, 1.0f, 0.0f,1,   1,0,1, 0,1,1, 0,0,1},

		//// BOTTOM                                                    
		//{ 1.0f, 0.0f, 1.0f,1,    0.0f, 0.0f, 1.0f,1,    0.0f, 0.0f, 0.0f,1,   1,0,1, 1,1,1, 0,1,1 },
		//{ 1.0f, 0.0f, 1.0f,1,    0.0f, 0.0f, 0.0f,1,    1.0f, 0.0f, 0.0f,1,   1,0,1, 0,1,1, 0,0,1 },

		//};
		yaw = 0;
		bool x = spr.LoadImage("C:\\dev\\test\\test\\high.png");
		if (x == false)
			std::cout << "error";
		//spr.Resize(20, 32)
		screenBuffer = new pixel[ScreenWidth() * ScreenHeight()];
		depth = new float[ScreenWidth() * ScreenHeight()];
		
		return true;
	}
	void MultiplyMatrix(const Matrix &matrix,const vec3d point, vec3d& Output) {
		Output.x = point.x * matrix.i[0] + point.y * matrix.j[0] + point.z * matrix.k[0] + matrix.w[0];
		Output.y = point.x * matrix.i[1] + point.y * matrix.j[1] + point.z * matrix.k[1] + matrix.w[1];
		Output.z = point.x * matrix.i[2] + point.y * matrix.j[2] + point.z * matrix.k[2] + matrix.w[2];
		Output.w = point.x * matrix.i[3] + point.y * matrix.j[3] + point.z * matrix.k[3] + matrix.w[3];
		
	}
	float theata;
	vec3d light = { 0,0,-1 };

	/*CHAR_INFO GetColor(double color) {
		short bg_col, fg_col;
		wchar_t sym;
		int pixel_bw = (int)(13.0f * color);
		switch (pixel_bw)
		{
		case 0: bg_col = BG_BLACK; fg_col = FG_BLACK; sym = PIXEL_SOLID; break;

		case 1: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_QUARTER; break;
		case 2: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_HALF; break;
		case 3: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_THREEQUARTERS; break;
		case 4: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_SOLID; break;

		case 5: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_QUARTER; break;
		case 6: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_HALF; break;
		case 7: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_THREEQUARTERS; break;
		case 8: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_SOLID; break;

		case 9:  bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_QUARTER; break;
		case 10: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_HALF; break;
		case 11: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_THREEQUARTERS; break;
		case 12: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_SOLID; break;
		default:
			bg_col = BG_BLACK; fg_col = FG_BLACK; sym = PIXEL_SOLID;
		}
		CHAR_INFO c;
		c.Attributes = bg_col + fg_col;
		c.Char.UnicodeChar = sym;
		return c;
	}*/
	//sprite spr;
	bool OnUserUpdate(float fElapseTime)override {
		pixel rgb;
		rgb.SetRGB(0, 0, 0);
		Fill(0, 0, ScreenWidth(), ScreenHeight(),rgb);
		
		//theata += fElapseTime;
		for (int i = 0; i < ScreenWidth() * ScreenHeight(); i++) {
			screenBuffer[i].SetRGB(0, 0, 0);
			depth[i] = INFINITY;
		}


		RotationZ.i[0] = cosf(theata);
		RotationZ.i[1] = -sinf(theata);

		RotationZ.j[0] = sinf(theata);
		RotationZ.j[1] = cosf(theata);

		RotationZ.k[2] = 1;

		RotationX.j[1] =  cosf(0.5 * theata);
		RotationX.j[2] = -sinf(0.5 * theata);
 
		RotationX.k[1] =  sinf(0.5 * theata);
		RotationX.k[2] =  cosf(0.5 * theata);

		RotationX.i[0] = 1;
		if (GetAsyncKeyState('D') & 0x8000)
			yaw += fElapseTime * 5;
		if(GetAsyncKeyState('A') & 0x8000)
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

		lookPos = { 0,0,1 };
		MultiplyMatrix(cam, lookPos, lookPos);
	//	lookPos = { 0,0,1 };
		float speed = 20.0f;
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
		std::vector<std::pair<Triangle,double>> trise;
		for (const auto& tri : meshCube.tris) {
			Triangle triProjected, triTranslated,triTemp ,triRotatedZX, triRotatedZ;
			/*MultiplyMatrix(RotationZ,tri.p[0],triRotatedZ.p[0]);
			MultiplyMatrix(RotationZ,tri.p[1],triRotatedZ.p[1]);
			MultiplyMatrix(RotationZ,tri.p[2],triRotatedZ.p[2]);
		   //triRotatedZ = tri;
		   //Rotate in X-Axis
			triRotatedZ.tex[0] = tri.tex[0];
			triRotatedZ.tex[1] = tri.tex[1];
			triRotatedZ.tex[2] = tri.tex[2];
			
			MultiplyMatrix(RotationX,triRotatedZ.p[0], triRotatedZX.p[0]);
			MultiplyMatrix(RotationX,triRotatedZ.p[1], triRotatedZX.p[1]);
			MultiplyMatrix(RotationX,triRotatedZ.p[2], triRotatedZX.p[2]);
			
			triRotatedZX.tex[0] = triRotatedZ.tex[0];
			triRotatedZX.tex[1] = triRotatedZ.tex[1];
			triRotatedZX.tex[2] = triRotatedZ.tex[2];
			
			*/
			triRotatedZX = tri;
		/*	MultiplyMatrix(translation, tri.p[0], triTranslated.p[0]);
			MultiplyMatrix(translation, tri.p[1], triTranslated.p[1]);
			MultiplyMatrix(translation, tri.p[2], triTranslated.p[2]);
		*/

			triTranslated.p[0] = Vector_Sub(tri.p[0], Camera);
			triTranslated.p[1] = Vector_Sub(tri.p[1], Camera);
			triTranslated.p[2] = Vector_Sub(tri.p[2], Camera);

			

			triTranslated.tex[0] = tri.tex[0];
			triTranslated.tex[1] = tri.tex[1];
			triTranslated.tex[2] = tri.tex[2];


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

			toPoint.x = triTranslated.p[0].x ;
			toPoint.y = triTranslated.p[0].y ;
			toPoint.z = triTranslated.p[0].z ;
			
			if (dotProduct(toPoint, crossProduct) < 0) {


				crossProduct = normalize(crossProduct);
			
				float dot = dotProduct(light, crossProduct);
				int nClippedTriangles = 0;
				Triangle clipped[2];
				nClippedTriangles = clip({ 0,0,0.1 }, { 0,0,1 }, triTranslated, clipped[0], clipped[1]);

				for (int i = 0; i < nClippedTriangles; i++) {
					clipped[i].tex[0].w = 1.0f / clipped[i].p[0].z;
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
					triProjected.tex[2] = clipped[i].tex[2];


					MultiplyMatrix(Projection, clipped[i].p[0], triProjected.p[0]);
					MultiplyMatrix(Projection, clipped[i].p[1], triProjected.p[1]);
					MultiplyMatrix(Projection, clipped[i].p[2], triProjected.p[2]);

					for (int j = 0; j < 3; j++) {
						if (triProjected.p[j].w != 0)
						{
							triProjected.p[j].x /= triProjected.p[j].w;
							triProjected.p[j].y /= triProjected.p[j].w;
							triProjected.p[j].z /= triProjected.p[j].w;
						}

					}

					float sizeX = 0.5f *  ScreenWidth();
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
			(float)(t1.first.p[0].z + t1.first.p[1].z + t1.first.p[2].z) / 3.0f;});
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
					std::pair<Triangle,float> test = listTriangles.front();
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

			//pixel rgb;
			////float dx = triToRaster.
			//rgb.SetRGB(255, 255, 255);
			//FillTriangle(triToRaster.first.p[0].x, triToRaster.first.p[0].y, triToRaster.first.p[1].x, triToRaster.first.p[1].y, triToRaster.first.p[2].x, triToRaster.first.p[2].y, rgb);


			if (!listTriangles.empty()) {
		//		CHAR_INFO c = GetColor(triToRaster.second);

				for (auto& h : listTriangles) {
				//	auto v = triToRaster.first;
					Triangle v = h.first;
					pixel rgb;
					//float dx = triToRaster.
					//rgb.SetRGB(h.second * 255, h.second * 255, h.second * 255);
				//	rgb.SetRGB(255, 255, 255);
					//FillTriangle(v.p[0].x, v.p[0].y, v.p[1].x, v.p[1].y, v.p[2].x, v.p[2].y,rgb);
					//DrawTriangle(v.p[0].x, v.p[0].y, v.p[1].x, v.p[1].y, v.p[2].x, v.p[2].y, rgb);

					DrawTexturedTriangle(spr,v.p[0].x, v.p[0].y, v.tex[0].u, v.tex[0].v, v.tex[0].w,
											  v.p[1].x, v.p[1].y, v.tex[1].u, v.tex[1].v, v.tex[1].w,
											  v.p[2].x, v.p[2].y, v.tex[2].u, v.tex[2].v, v.tex[2].w);
									
				/*	TexturedTriangle(v.p[0].x, v.p[0].y, v.tex[0].u, v.tex[0].v, v.tex[0].w,
						v.p[1].x, v.p[1].y, v.tex[1].u, v.tex[1].v, v.tex[1].w,
						v.p[2].x, v.p[2].y, v.tex[2].u, v.tex[2].v, v.tex[2].w,&spr);*/

				//	DrawTriangle(v.p[0].x, v.p[0].y, v.p[1].x, v.p[1].y, v.p[2].x, v.p[2].y, c.Char.UnicodeChar, c.Attributes);

				
				}
			}



		}
		//for (int i = 0; i < ScreenHeight(); i++) {
		//	for (int j = 0; j < ScreenWidth(); j++) {
		//		DrawPixel(j, i, spr.GetSamplePixel(j / (float)ScreenWidth(), i / (float)ScreenHeight()));
		//			//spr.GetSampleColor(j / (float)ScreenWidth(), i / (float)ScreenHeight()));
		//	}
		//}
		//DrawLine(5, 11, 13, 40);


		//for(int i = 0; i < ScreenHeight(); i++)
		//	for (int j = 0; j < ScreenWidth(); j++) {
		//		DrawPixel(j, i, screenBuffer[i * ScreenWidth() + j]);
		//	}
		return true;
	}
	void TexturedTriangle(int x1, int y1, float u1, float v1, float w1,
		int x2, int y2, float u2, float v2, float w2,
		int x3, int y3, float u3, float v3, float w3,
		sprite* tex)
	{
		if (y2 < y1)
		{
			std::swap(y1, y2);
			std::swap(x1, x2);
			std::swap(u1, u2);
			std::swap(v1, v2);
			std::swap(w1, w2);
		}

		if (y3 < y1)
		{
			std::swap(y1, y3);
			std::swap(x1, x3);
			std::swap(u1, u3);
			std::swap(v1, v3);
			std::swap(w1, w3);
		}

		if (y3 < y2)
		{
			std::swap(y2, y3);
			std::swap(x2, x3);
			std::swap(u2, u3);
			std::swap(v2, v3);
			std::swap(w2, w3);
		}

		int dy1 = y2 - y1;
		int dx1 = x2 - x1;
		float dv1 = v2 - v1;
		float du1 = u2 - u1;
		float dw1 = w2 - w1;

		int dy2 = y3 - y1;
		int dx2 = x3 - x1;
		float dv2 = v3 - v1;
		float du2 = u3 - u1;
		float dw2 = w3 - w1;

		float tex_u, tex_v, tex_w;

		float dax_step = 0, dbx_step = 0,
			du1_step = 0, dv1_step = 0,
			du2_step = 0, dv2_step = 0,
			dw1_step = 0, dw2_step = 0;

		if (dy1) dax_step = dx1 / (float)abs(dy1);
		if (dy2) dbx_step = dx2 / (float)abs(dy2);

		if (dy1) du1_step = du1 / (float)abs(dy1);
		if (dy1) dv1_step = dv1 / (float)abs(dy1);
		if (dy1) dw1_step = dw1 / (float)abs(dy1);

		if (dy2) du2_step = du2 / (float)abs(dy2);
		if (dy2) dv2_step = dv2 / (float)abs(dy2);
		if (dy2) dw2_step = dw2 / (float)abs(dy2);

		if (dy1)
		{
			for (int i = y1; i <= y2; i++)
			{
				int ax = x1 + (float)(i - y1) * dax_step;
				int bx = x1 + (float)(i - y1) * dbx_step;

				float tex_su = u1 + (float)(i - y1) * du1_step;
				float tex_sv = v1 + (float)(i - y1) * dv1_step;
				float tex_sw = w1 + (float)(i - y1) * dw1_step;

				float tex_eu = u1 + (float)(i - y1) * du2_step;
				float tex_ev = v1 + (float)(i - y1) * dv2_step;
				float tex_ew = w1 + (float)(i - y1) * dw2_step;

				if (ax > bx)
				{
					std::swap(ax, bx);
					std::swap(tex_su, tex_eu);
					std::swap(tex_sv, tex_ev);
					std::swap(tex_sw, tex_ew);
				}

				tex_u = tex_su;
				tex_v = tex_sv;
				tex_w = tex_sw;

				float tstep = 1.0f / ((float)(bx - ax));
				float t = 0.0f;

				for (int j = ax; j < bx; j++)
				{
					tex_u = (1.0f - t) * tex_su + t * tex_eu;
					tex_v = (1.0f - t) * tex_sv + t * tex_ev;
					tex_w = (1.0f - t) * tex_sw + t * tex_ew;
					DrawPixel(j, i, tex->GetSamplePixel(tex_u / tex_w, tex_v / tex_w));
					t += tstep;
				}

			}
		}

		dy1 = y3 - y2;
		dx1 = x3 - x2;
		dv1 = v3 - v2;
		du1 = u3 - u2;
		dw1 = w3 - w2;

		if (dy1) dax_step = dx1 / (float)abs(dy1);
		if (dy2) dbx_step = dx2 / (float)abs(dy2);

		du1_step = 0, dv1_step = 0;
		if (dy1) du1_step = du1 / (float)abs(dy1);
		if (dy1) dv1_step = dv1 / (float)abs(dy1);
		if (dy1) dw1_step = dw1 / (float)abs(dy1);

		if (dy1)
		{
			for (int i = y2; i <= y3; i++)
			{
				int ax = x2 + (float)(i - y2) * dax_step;
				int bx = x1 + (float)(i - y1) * dbx_step;

				float tex_su = u2 + (float)(i - y2) * du1_step;
				float tex_sv = v2 + (float)(i - y2) * dv1_step;
				float tex_sw = w2 + (float)(i - y2) * dw1_step;

				float tex_eu = u1 + (float)(i - y1) * du2_step;
				float tex_ev = v1 + (float)(i - y1) * dv2_step;
				float tex_ew = w1 + (float)(i - y1) * dw2_step;

				if (ax > bx)
				{
					std::swap(ax, bx);
					std::swap(tex_su, tex_eu);
					std::swap(tex_sv, tex_ev);
					std::swap(tex_sw, tex_ew);
				}

				tex_u = tex_su;
				tex_v = tex_sv;
				tex_w = tex_sw;

				float tstep = 1.0f / ((float)(bx - ax));
				float t = 0.0f;

				for (int j = ax; j < bx; j++)
				{
					tex_u = (1.0f - t) * tex_su + t * tex_eu;
					tex_v = (1.0f - t) * tex_sv + t * tex_ev;
					tex_w = (1.0f - t) * tex_sw + t * tex_ew;

					
						DrawPixel(j, i, tex->GetSamplePixel(tex_u / tex_w, tex_v / tex_w));
					//	pDepthBuffer[i * ScreenWidth() + j] = tex_w;
					
					t += tstep;
				}
			}
		}
	}




};



int main() {

	Graphics3D d;
	d.ConstructPixel(250 * 4, 150 * 4, 0, 0,1,1, L"hello");
	//d.ConstructConsole(250, 150, 4, 4);
	d.run();
}