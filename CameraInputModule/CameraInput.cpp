// For conditions of distribution and use, see copyright notice in license.txt

#include "StableHeaders.h"
#include "CameraInputModule.h"
#include "CameraInput.h"

CameraInput::CameraInput(QObject *parent, Foundation::Framework *framework) :
    QObject(parent),
    framework_(framework),
    hasDevice_(false),
    capturing_(false)
{
}

CameraInput::~CameraInput()
{
}

// Public

bool CameraInput::HasDevice() 
{ 
    return hasDevice_; 
}

bool CameraInput::IsCapturing()
{
    return capturing_;
}

void CameraInput::StartCapturing()
{
    if (capturing_)
        return;

    capturing_ = true;
    emit Capturing(capturing_);
}

void CameraInput::StopCapturing()
{
    if (!capturing_)
        return;

    capturing_ = false;
    emit Capturing(capturing_);

    // Check if device was found
    if (!HasDevice())
    {
        capturing_ = false;
        emit Capturing(capturing_);
    }
}

const QImage &CameraInput::CurrentFrame()
{
    return currentFrame_;
}

// Protected

void CameraInput::SetEnabled(bool enabled)
{
    hasDevice_ = enabled;
}

void CameraInput::SetFrame(const QImage &frame)
{
    currentFrame_ = frame;
    emit frameUpdate(currentFrame_);
}