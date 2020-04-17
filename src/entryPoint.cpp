#include <Engine/startUp.h>
#include "Engine/Core/Threading/engineThread.h"

int main() {
    using namespace Engine::Core::Threading;

    Engine::StartUp startUp;
    startUp.Ignition();

}
