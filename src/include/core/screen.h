/* Include files*/
#include <SDL2/SDL.h>
#include <vector>
#include <cmath>
#include <iostream>
#include <chrono>
#include <fstream>
#include <sstream>
#include <algorithm>
using namespace std;

struct vec3d
{
    float x;
    float y;
    float z;
};

struct triangle
{
    vec3d p[3];
    int red_color = 255;
    int green_color = 255;
    int blue_color = 255;
};

struct mesh
{
    vector<triangle> tris;
    bool LoadFromObjectFile(string sFilename)
    {
        ifstream f(sFilename);
        if (!f.is_open())
            return false;
        vector<vec3d> verts;
        while (!f.eof())
        {
            char line[128];
            f.getline(line, 128);
            stringstream s;
            s << line;
            char junk;
            if (line[0] == 'v')
            {
                vec3d v;
                s >> junk >> v.x >> v.y >> v.z;
                verts.push_back(v);
            }
            if (line[0] == 'f')
            {
                int f[3];
                s >> junk >> f[0] >> f[1] >> f[2];
                tris.push_back({verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1]});
            }
        }
        return true;
    }
};

struct mat4x4
{
    float m[4][4] = {0};
};

class Screen
{
public:
    Screen(mesh &meshCube);

    void show(float elapsedTime);

    void input();

private:
    SDL_Event e;
    SDL_Window *window;
    SDL_Renderer *renderer;
    mesh meshCube;
    mat4x4 matProj;
    int width, height;
    vec3d vCamera;
    float rotation_x = 0.0f;
    float rotation_y = 0.0f;
    float rotation_z = 0.0f;
    float FOV = 10.0;
    float DeltaTime = 0.0f;
};