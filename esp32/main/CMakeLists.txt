set(COMPONENT_ADD_INCLUDEDIRS  include ../../cbor11 ../../spine/src /usr/local/include )

idf_component_register(SRCS main.cpp LedBlinker.cpp comm.cpp ppp_frame.cpp ${WORKSPACE}/cbor11/cbor11.cpp
                    INCLUDE_DIRS ${WORKSPACE}/limero/inc ${WORKSPACE}/cbor11 /usr/local/include  )
