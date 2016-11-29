/******************************************************************************/
/*!
\file main.cpp
\project CS399_TeamLuke
\author Jiyun Kang

Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#include <fstream>
#include "Engine\Engine.h"
#include "../ML_ENGINE/Test/RecommenderSystemTest.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
  Engine* engine = Engine::GetEngine();

  if (!engine)
    return 0;

  bool result = engine->Initialize(hInstance);
  
  if (result)
    engine->Run();

  engine->Shutdown();

  delete engine;

  return 0;
}