
#include "Precompiled.h"
using namespace CK::DD;

bool GameEngine::Init(const ScreenPoint& InViewportSize)
{
	if (!mInputManager.GetXAxis || !mInputManager.GetYAxis || !mInputManager.GetZAxis 
		|| !mInputManager.GetWAxis || !mInputManager.IsSpacePressed)
	{
		return false;
	}

	mViewportSize = InViewportSize;
	return true;
}
