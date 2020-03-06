#include "Engine/Core/Threading/engineThread.h"

int main() {
    using namespace Engine::Core::Threading;

    EngineThread thread;
    thread.run();
}
