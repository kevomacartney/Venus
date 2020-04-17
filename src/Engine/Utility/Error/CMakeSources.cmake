set(UTILITY_DIR "Utility")

set(ENGINE_TIME_INC
        ${UTILITY_DIR}/Time/vTime.h
        ${UTILITY_DIR}/Time/vTimer.h
        )

set(ENGINE_TIME_SRC
        ${UTILITY_DIR}/Time/vTime.cpp
        ${UTILITY_DIR}/Time/vTimer.cpp
        )

message("time stuff:  ${ENGINE_TIME_INC}")