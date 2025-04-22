#include "screen.h"

void MultiplyMatrixVector(vec3d &i, vec3d &o, mat4x4 &m)
{
    o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
    o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
    o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
    float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

    if (w != 0.0f)
    {
        o.x /= w;
        o.y /= w;
        o.z /= w;
    }
}

mat4x4 CreateRotationMatrixZ(float angle)
{
    mat4x4 matRotZ;
    matRotZ.m[0][0] = cosf(angle);
    matRotZ.m[0][1] = sinf(angle);
    matRotZ.m[1][0] = -sinf(angle);
    matRotZ.m[1][1] = cosf(angle);
    matRotZ.m[2][2] = 1.0f;
    matRotZ.m[3][3] = 1.0f;
    return matRotZ;
}

// Fonction pour créer une matrice de rotation autour de l'axe X
mat4x4 CreateRotationMatrixX(float angle)
{
    mat4x4 matRotX;
    matRotX.m[0][0] = 1.0f;
    matRotX.m[1][1] = cosf(angle);
    matRotX.m[1][2] = sinf(angle);
    matRotX.m[2][1] = -sinf(angle);
    matRotX.m[2][2] = cosf(angle);
    matRotX.m[3][3] = 1.0f;
    return matRotX;
}

void DrawFilledTriangle(SDL_Renderer *renderer, int x1, int y1, int x2, int y2, int x3, int y3)
{
    // Sort vertices by y-coordinate ascending (y1 <= y2 <= y3)
    if (y1 > y2)
    {
        std::swap(y1, y2);
        std::swap(x1, x2);
    }
    if (y1 > y3)
    {
        std::swap(y1, y3);
        std::swap(x1, x3);
    }
    if (y2 > y3)
    {
        std::swap(y2, y3);
        std::swap(x2, x3);
    }

    auto drawLine = [&](int sx, int ex, int y)
    {
        for (int x = sx; x <= ex; x++)
        {
            SDL_RenderDrawPoint(renderer, x, y);
        }
    };

    int total_height = y3 - y1;

    for (int i = 0; i < total_height; i++)
    {
        bool second_half = i > y2 - y1 || y2 == y1;
        int segment_height = second_half ? y3 - y2 : y2 - y1;
        float alpha = (float)i / total_height;
        float beta = (float)(i - (second_half ? y2 - y1 : 0)) / segment_height; // be careful: with above conditions no division by zero here

        int A = x1 + (x3 - x1) * alpha;
        int B = second_half ? x2 + (x3 - x2) * beta : x1 + (x2 - x1) * beta;

        if (A > B)
            std::swap(A, B);
        drawLine(A, B, y1 + i);
    }
}

void DrawTriangle(SDL_Renderer *renderer, int x1, int y1, int x2, int y2, int x3, int y3)
{
    // Dessiner les 3 côtés du triangle
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2); // Ligne entre (x1, y1) et (x2, y2)
    SDL_RenderDrawLine(renderer, x2, y2, x3, y3); // Ligne entre (x2, y2) et (x3, y3)
    SDL_RenderDrawLine(renderer, x3, y3, x1, y1); // Ligne entre (x3, y3) et (x1, y1)
}

Screen::Screen(mesh &meshCube) : meshCube(meshCube) /*, cam(cam)*/
{
    SDL_Init(SDL_INIT_VIDEO);
    width = 640;
    height = 480;
    vCamera = {0.0f, 0.0f, 0.0f};
    SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer);
    float fNear = 0.1f;
    float fFar = 1000.0f;
    float fFov = 90.0f;
    float fAspectRatio = (float)height / (float)width;
    float fFovRad = 1.0f / tanf(fFov * 0.5 / 180.0f * 3.14159f);
    matProj.m[0][0] = fAspectRatio * fFovRad;
    matProj.m[1][1] = fFovRad;
    matProj.m[2][2] = fFar / (fFar - fNear);
    matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
    matProj.m[2][3] = 1.0f;
    matProj.m[3][3] = 0.0f;
    return;
}

void Screen::show(float elapsedTime)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    float fTheta = 0.0f;
    fTheta += elapsedTime * 1.0f;
    // Créer les matrices de rotation
    mat4x4 matRotZ = CreateRotationMatrixZ(fTheta);
    mat4x4 matRotX = CreateRotationMatrixX(fTheta * 0.5f);
    vector<triangle> vecTrianglesToRaster;
    for (auto tri : meshCube.tris)
    {
        triangle triProjected, triTranslated, triRotatedZ, triRotatedZX;
        // Rotation Z
        MultiplyMatrixVector(tri.p[0], triRotatedZ.p[0], matRotZ);
        MultiplyMatrixVector(tri.p[1], triRotatedZ.p[1], matRotZ);
        MultiplyMatrixVector(tri.p[2], triRotatedZ.p[2], matRotZ);
        // Rotation X
        MultiplyMatrixVector(triRotatedZ.p[0], triRotatedZX.p[0], matRotX);
        MultiplyMatrixVector(triRotatedZ.p[1], triRotatedZX.p[1], matRotX);
        MultiplyMatrixVector(triRotatedZ.p[2], triRotatedZX.p[2], matRotX);
        // Translation
        triTranslated = triRotatedZX;
        triTranslated.p[0].z = triRotatedZX.p[0].z + 8.0f;
        triTranslated.p[1].z = triRotatedZX.p[1].z + 8.0f;
        triTranslated.p[2].z = triRotatedZX.p[2].z + 8.0f;

        vec3d normal, line1, line2;
        line1.x = triTranslated.p[1].x - triTranslated.p[0].x;
        line1.y = triTranslated.p[1].y - triTranslated.p[0].y;
        line1.z = triTranslated.p[1].z - triTranslated.p[0].z;
        line2.x = triTranslated.p[2].x - triTranslated.p[0].x;
        line2.y = triTranslated.p[2].y - triTranslated.p[0].y;
        line2.z = triTranslated.p[2].z - triTranslated.p[0].z;
        normal.x = line1.y * line2.z - line1.z * line2.y;
        normal.y = line1.z * line2.x - line1.x * line2.z;
        normal.z = line1.x * line2.y - line1.y * line2.x;
        float l = sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);

        if (l != 0.0f)
        {
            normal.x /= l;
            normal.y /= l;
            normal.z /= l;
        }
        if (normal.x * (triTranslated.p[0].x - vCamera.x) + normal.y * (triTranslated.p[0].y - vCamera.y) + normal.z * (triTranslated.p[0].z - vCamera.z) < 0.0f)
        {
            // Lumière
            vec3d light_direction = {0.0f, 0.0f, -1.0f};
            float l = sqrtf(light_direction.x * light_direction.x + light_direction.y * light_direction.y + light_direction.z * light_direction.z);
            if (l != 0.0f)
            {
                light_direction.x /= l;
                light_direction.y /= l;
                light_direction.z /= l;
            }
            float dp = normal.x * light_direction.x + normal.y * light_direction.y + normal.z * light_direction.z;
            triProjected.red_color = 255 * dp;
            triProjected.green_color = 255 * dp;
            triProjected.blue_color = 255 * dp;
            // Projection
            MultiplyMatrixVector(triTranslated.p[0], triProjected.p[0], matProj);
            MultiplyMatrixVector(triTranslated.p[1], triProjected.p[1], matProj);
            MultiplyMatrixVector(triTranslated.p[2], triProjected.p[2], matProj);

            // Scaling
            triProjected.p[0].x += 1.0f;
            triProjected.p[0].y += 1.0f;
            triProjected.p[1].x += 1.0f;
            triProjected.p[1].y += 1.0f;
            triProjected.p[2].x += 1.0f;
            triProjected.p[2].y += 1.0f;

            triProjected.p[0].x *= 0.5f * (float)width;
            triProjected.p[0].y *= 0.5f * (float)height;
            triProjected.p[1].x *= 0.5f * (float)width;
            triProjected.p[1].y *= 0.5f * (float)height;
            triProjected.p[2].x *= 0.5f * (float)width;
            triProjected.p[2].y *= 0.5f * (float)height;
            // store triangle for z sorting
            vecTrianglesToRaster.push_back(triProjected);
        }

        // Sort triangles from back to front
        sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](triangle &t1, triangle &t2)
             { return (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f > (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f; });

        for (auto &triProjected : vecTrianglesToRaster)
        {
            SDL_SetRenderDrawColor(renderer, triProjected.red_color, triProjected.green_color, triProjected.blue_color, SDL_ALPHA_OPAQUE);
            DrawFilledTriangle(renderer, (int)triProjected.p[0].x, (int)triProjected.p[0].y, (int)triProjected.p[1].x, (int)triProjected.p[1].y, (int)triProjected.p[2].x, (int)triProjected.p[2].y);
            /*SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
            DrawTriangle(renderer, (int)triProjected.p[0].x, (int)triProjected.p[0].y, (int)triProjected.p[1].x, (int)triProjected.p[1].y, (int)triProjected.p[2].x, (int)triProjected.p[2].y);*/
        }
    }
    SDL_RenderPresent(renderer);
}

void Screen::input()
{
    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_QUIT)
        {
            SDL_Quit();
            exit(0);
        }
    }
}