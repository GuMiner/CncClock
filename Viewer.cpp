#include <glm\gtc\matrix_transform.hpp>
#include "logging\Logger.h"
#include "Input.h"
#include "Viewer.h"

// Default our view to -64 x to +64 x, -36 y to +36 y, with Z+ into the screen and y+ up.
Viewer::Viewer()
    : fovY(2 * 74.47589f), aspectRatio(1.77778f), nearPlane(0.10f), farPlane(1000.0f), 
      ScreenWidth(1280), ScreenHeight(720), MaxFramerate(120),
      useCache(false), viewerCache()
{
    position = glm::vec3(0, 0, -6.0f); // LH coordinate system, Z+ is into the screen.
    target = glm::vec3(0, 0, 4.0f);
    up = glm::vec3(0, 1, 0);
    UpdateMatrices();

    RecomputeCache();
}

void Viewer::Update(float frameTime)
{
    bool movedLeftRight = CheckMoveAxis(GLFW_KEY_W, GLFW_KEY_Q, frameTime, &position.x, &target.x);
    bool movedUpDown = CheckMoveAxis(GLFW_KEY_S, GLFW_KEY_X, frameTime, &position.y, &target.y);

    bool movedInOut = CheckMoveAxis(GLFW_KEY_A, GLFW_KEY_Z, frameTime, &position.z, &target.z);
    if (movedInOut && position.z > -0.1f)
    {
        position.z = -0.1f;
        target.z = 9.9f;
    }

    if (movedLeftRight || movedUpDown || movedInOut)
    {
        UpdateMatrices();
        RecomputeCache();
    }
}

void Viewer::SetScreenSize(int width, int height)
{
    ScreenWidth = width;
    ScreenHeight = height;
    RecomputeCache();
}

void Viewer::RecomputeCache()
{
    useCache = false;
    viewerCache.unitsPerPixel = GetUnitsPerPixel();
    viewerCache.xSize = GetXSize();
    viewerCache.ySize = GetYSize();
    viewerCache.letterboxingScreenOffset = GetLetterboxingScreenOffset();
    useCache = true;
}

float Viewer::GetUnitsPerPixel()
{
    if (useCache)
    {
        return viewerCache.unitsPerPixel;
    }

    float visibleScreenDist;
    int pixels;

    // We letterbox based on whatever will give us the maximum size, so check
    float necessaryWidth = (float)ScreenHeight * aspectRatio;
    if (necessaryWidth > ScreenWidth)
    {
        visibleScreenDist = GetXSize();
        pixels = ScreenWidth;
    }
    else
    {
        visibleScreenDist = GetYSize();
        pixels = ScreenHeight;
    }

    return visibleScreenDist / (float)pixels;
}

glm::vec2 Viewer::GetLetterboxingScreenOffset()
{
    if (useCache)
    {
        return viewerCache.letterboxingScreenOffset;
    }

    glm::vec2 offset(0.0f, 0.0f);

    // Remove an x or y offset based on whether we are letterboxing or not.
    float necessaryWidth = (float)ScreenHeight * aspectRatio;
    if (necessaryWidth > ScreenWidth)
    {
        float effectiveHeight = (float)ScreenWidth / aspectRatio;
        offset.y -= (ScreenHeight - effectiveHeight) / 2.0f;
    }
    else
    {
        offset.x -= (ScreenWidth - necessaryWidth) / 2.0f;
    }

    return offset;
}

glm::vec2 Viewer::GetGridPos(glm::ivec2 screenPos)
{
    return GetGridPos(glm::vec2((float)screenPos.x, (float)screenPos.y));
}

glm::vec2 Viewer::GetGridPos(glm::vec2 screenPos)
{
    screenPos += GetLetterboxingScreenOffset();

    float unitsPerPixel = GetUnitsPerPixel();
    glm::vec2 gridPos = screenPos * unitsPerPixel;

    // Offset so that 0, 0 is centered in the screen and invert the y-axis.
    return (gridPos - glm::vec2(GetXSize() / 2.0f, GetYSize() / 2.0f)) * glm::vec2(1.0f, -1.0f) + glm::vec2(position.x, position.y);
}

float Viewer::GetScreenScaleFactor()
{
    return std::tan(glm::radians(fovY / 2.0f)) * (position.z / -10.0f);
}

float Viewer::GetXSize()
{
    if (useCache)
    {
        return viewerCache.xSize;
    }

    // NOTE: We're assuming we're always on the Z axis.
    float distToXYPlane = target.z - position.z;
    return (2.0f * aspectRatio * distToXYPlane) * GetScreenScaleFactor();
}

float Viewer::GetYSize()
{
    if (useCache)
    {
        return viewerCache.ySize;
    }

    // NOTE: We're assuming we're always on the Z axis.
    float distToXYPlane = target.z - position.z;
    return (2.0f * distToXYPlane) * GetScreenScaleFactor();
}

float Viewer::GetAspectRatio() const
{
    return aspectRatio;
}

void Viewer::UpdateMatrices()
{
    viewMatrix = glm::lookAtLH(position, target, up);
    perspectiveMatrix = glm::perspectiveLH(glm::radians(fovY), aspectRatio, nearPlane, farPlane);
    Logger::LogDebug("View Position ", position, ". Target ", target, ". Up ", up, ".");
    Logger::LogDebug("View Projection: Aspect ", aspectRatio, ". FOV-Y: ", fovY, ". Near Plane: ", nearPlane, ". Far Plane: ", farPlane, ".");
}

bool Viewer::CheckMoveAxis(int posKeyId, int negKeyId, float frameTime, float* eye, float* target) const
{
    const float motionSpeed = 2.0f;
    DialVariable(posKeyId, negKeyId, motionSpeed * frameTime, eye);
    return DialVariable(posKeyId, negKeyId, motionSpeed * frameTime, target);
}

// Dials a variable positively or negatively by an amount based on the key pressed.
bool Viewer::DialVariable(int posKeyId, int negKeyId, float dialAmount, float* value) const
{
    if (Input::IsKeyPressed(posKeyId))
    {
        (*value) += dialAmount;
        return true;
    }

    if (Input::IsKeyPressed(negKeyId))
    {
        (*value) -= dialAmount;
        return true;
    }

    return false;
}
