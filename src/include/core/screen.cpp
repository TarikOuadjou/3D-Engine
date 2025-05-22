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

vec3d Vector_Add(const vec3d &a, const vec3d &b)
{
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

vec3d Vector_Sub(const vec3d &a, const vec3d &b)
{
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

vec3d Vector_Mul(const vec3d &v, float k)
{
    return {v.x * k, v.y * k, v.z * k};
}

vec3d Vector_Div(const vec3d &v, float k)
{
    return {v.x / k, v.y / k, v.z / k};
}

float Vector_Dot(const vec3d &a, const vec3d &b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float Vector_Length(const vec3d &v)
{
    return sqrtf(Vector_Dot(v, v));
}

vec3d Vector_Normalise(const vec3d &v)
{
    float l = Vector_Length(v);
    return Vector_Div(v, l);
}

vec3d Vector_CrossProduct(const vec3d &a, const vec3d &b)
{
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x};
}

mat4x4 Matrix_PointAt(vec3d &pos, vec3d &target, vec3d &up)
{
    // Calculer la nouvelle direction vers laquelle la caméra regarde
    vec3d newForward = Vector_Sub(target, pos);
    newForward = Vector_Normalise(newForward);

    // Un vecteur temporaire pour construire les axes
    vec3d a = Vector_Mul(newForward, Vector_Dot(up, newForward));
    vec3d newUp = Vector_Sub(up, a);
    newUp = Vector_Normalise(newUp);

    vec3d newRight = Vector_CrossProduct(newUp, newForward);

    // Construire la matrice
    mat4x4 matrix;
    matrix.m[0][0] = newRight.x;
    matrix.m[0][1] = newRight.y;
    matrix.m[0][2] = newRight.z;
    matrix.m[0][3] = 0.0f;
    matrix.m[1][0] = newUp.x;
    matrix.m[1][1] = newUp.y;
    matrix.m[1][2] = newUp.z;
    matrix.m[1][3] = 0.0f;
    matrix.m[2][0] = newForward.x;
    matrix.m[2][1] = newForward.y;
    matrix.m[2][2] = newForward.z;
    matrix.m[2][3] = 0.0f;
    matrix.m[3][0] = pos.x;
    matrix.m[3][1] = pos.y;
    matrix.m[3][2] = pos.z;
    matrix.m[3][3] = 1.0f;
    return matrix;
}

// Matrice de vue = inverse de PointAt
mat4x4 Matrix_QuickInverse(mat4x4 &m)
{
    mat4x4 matrix;
    matrix.m[0][0] = m.m[0][0];
    matrix.m[0][1] = m.m[1][0];
    matrix.m[0][2] = m.m[2][0];
    matrix.m[0][3] = 0.0f;
    matrix.m[1][0] = m.m[0][1];
    matrix.m[1][1] = m.m[1][1];
    matrix.m[1][2] = m.m[2][1];
    matrix.m[1][3] = 0.0f;
    matrix.m[2][0] = m.m[0][2];
    matrix.m[2][1] = m.m[1][2];
    matrix.m[2][2] = m.m[2][2];
    matrix.m[2][3] = 0.0f;
    matrix.m[3][0] = -(m.m[3][0] * matrix.m[0][0] + m.m[3][1] * matrix.m[1][0] + m.m[3][2] * matrix.m[2][0]);
    matrix.m[3][1] = -(m.m[3][0] * matrix.m[0][1] + m.m[3][1] * matrix.m[1][1] + m.m[3][2] * matrix.m[2][1]);
    matrix.m[3][2] = -(m.m[3][0] * matrix.m[0][2] + m.m[3][1] * matrix.m[1][2] + m.m[3][2] * matrix.m[2][2]);
    matrix.m[3][3] = 1.0f;
    return matrix;
}

Screen::Screen(mesh &meshCube) : meshCube(meshCube)
{
    SDL_Init(SDL_INIT_VIDEO);
    width = 640;
    height = 480;
    cam.position = {0.0f, 0.0f, 0.0f};
    cam.target = {0.0f, 0.0f, 1.0f};
    cam.up = {0.0f, 1.0f, 0.0f};
    float fYaw = 0.0f; // rotation autour de l'axe Y (gauche/droite)
    window = SDL_CreateWindow("Mon jeu FPS",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              1280, 720,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    // SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_GetWindowSize(window, &width, &height);
    SDL_ShowCursor(SDL_FALSE);
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

void Screen::show(float elapsedTime, float deltatime)
{
    if (rotation_active)
    {
        SDL_WarpMouseInWindow(window, width / 2, height / 2);
    }
    DeltaTime = deltatime;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    //  Créer les matrices de rotation
    mat4x4 matRotZ = CreateRotationMatrixZ(elapsedTime);
    mat4x4 matRotX = CreateRotationMatrixX(elapsedTime * 0.5f);
    vector<triangle> vecTrianglesToRaster;
    // Créer la matrice de vue
    mat4x4 matCam = Matrix_PointAt(cam.position, cam.target, cam.up);
    mat4x4 matView = Matrix_QuickInverse(matCam);
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
        triTranslated.p[0] = triRotatedZX.p[0];
        triTranslated.p[1] = triRotatedZX.p[1];
        triTranslated.p[2] = triRotatedZX.p[2];
        triTranslated.p[0].z += 8.0f;
        triTranslated.p[1].z += 8.0f;
        triTranslated.p[2].z += 8.0f;

        triangle triViewed;
        MultiplyMatrixVector(triTranslated.p[0], triViewed.p[0], matView);
        MultiplyMatrixVector(triTranslated.p[1], triViewed.p[1], matView);
        MultiplyMatrixVector(triTranslated.p[2], triViewed.p[2], matView);
        vec3d normal, line1, line2;
        line1.x = triViewed.p[1].x - triViewed.p[0].x;
        line1.y = triViewed.p[1].y - triViewed.p[0].y;
        line1.z = triViewed.p[1].z - triViewed.p[0].z;
        line2.x = triViewed.p[2].x - triViewed.p[0].x;
        line2.y = triViewed.p[2].y - triViewed.p[0].y;
        line2.z = triViewed.p[2].z - triViewed.p[0].z;
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
        if (normal.x * triViewed.p[0].x +
                normal.y * triViewed.p[0].y +
                normal.z * triViewed.p[0].z <
            0.0f)
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
            MultiplyMatrixVector(triViewed.p[0], triProjected.p[0], matProj);
            MultiplyMatrixVector(triViewed.p[1], triProjected.p[1], matProj);
            MultiplyMatrixVector(triViewed.p[2], triProjected.p[2], matProj);

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
    SDL_RenderPresent(renderer);
}

void Screen::input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            SDL_Quit();
            exit(0);
        }
        else if (event.type == SDL_KEYDOWN)
        {
            float step = 10.0f;
            switch (event.key.keysym.sym)
            {
            case SDLK_z:
                cam.position.z += step * DeltaTime;
                break;
            case SDLK_s:
                cam.position.z -= step * DeltaTime;
                break;
            case SDLK_q:
                cam.position.x -= step * DeltaTime;
                break;
            case SDLK_d:
                cam.position.x += step * DeltaTime;
                break;
            case SDLK_ESCAPE:
                rotation_active = !rotation_active;
                if (rotation_active == false)
                {
                    SDL_ShowCursor(SDL_TRUE);
                }
                else
                {
                    SDL_ShowCursor(SDL_FALSE);
                    SDL_WarpMouseInWindow(window, width / 2, height / 2);
                }
            }
        }
        else if (event.type == SDL_MOUSEMOTION)
        {
            if (rotation_active == true)
            {
                int mouseX = event.motion.x;
                int mouseY = event.motion.y;

                int deltaX = mouseX - width / 2;
                int deltaY = mouseY - height / 2;

                float sensitivity = 0.002f;
                yaw += deltaX * sensitivity;
                pitch += deltaY * sensitivity;

                // Clamp le pitch pour éviter les retournements
                if (pitch > 1.5f)
                    pitch = 1.5f;
                if (pitch < -1.5f)
                    pitch = -1.5f;

                // Recalculer le vecteur direction
                cam.target.x = cosf(pitch) * sinf(yaw);
                cam.target.y = sinf(pitch);
                cam.target.z = cosf(pitch) * cosf(yaw);

                cam.target = Vector_Add(cam.position, cam.target); // Assure la direction
            }
        }
    }
}