set(ENGINE_LOGGING_FACTORY_INC # include directories
        "loggingFactory.h"
        )

set(ENGINE_LOGGING_FACTORY_SRC # source directories
        "loggingFactory.cpp"
        )

set(ENGINE_CORE_FACTORY_SRC
        ${ENGINE_LOGGING_FACTORY_INC}
        ${ENGINE_LOGGING_FACTORY_SRC}
        )