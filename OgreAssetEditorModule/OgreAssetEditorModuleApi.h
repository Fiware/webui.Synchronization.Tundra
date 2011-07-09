// For conditions of distribution and use, see copyright notice in license.txt

#pragma once

#if defined (_WINDOWS)
#if defined(ASSET_EDITOR_MODULE_EXPORTS) 
#define ASSET_EDITOR_MODULE_API __declspec(dllexport)
#else
#define ASSET_EDITOR_MODULE_API __declspec(dllimport) 
#endif
#else
#define ASSET_EDITOR_MODULE_API
#endif

