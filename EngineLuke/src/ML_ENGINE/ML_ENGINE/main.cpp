#include <fstream>
#include "Engine\Engine.h"
//int main()
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