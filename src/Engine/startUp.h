//
// Created by Kelvin Macartney on 04/03/2020.
//

#ifndef VENUS_STARTUP_H
#define VENUS_STARTUP_H

namespace Engine {
    class StartUp {
    public:
        //Ignites the engine and prepares it for operation
        void Ignition();

        // Engine is starts main loop
        int go();
    };
}


#endif //VENUS_STARTUP_H
