set(XRCE ${WORKSPACE}/Micro-XRCE-DDS-Client)
set(XRCE_CDR ${WORKSPACE}/Micro-CDR)

set(CONFIG_PROFILE_COMMON_CREATE_ENTITIES CONFIG_PROFILE_CREATE_ENTITIES_REF OR CONFIG_PROFILE_CREATE_ENTITIES_XML)

set(COMPONENT_ADD_INCLUDEDIRS ${XRCE}/include ${XRCE_CDR}/include include ${WORKSPACE}/limero/inc)

#file(GLOB_RECURSE COMPONENT_SRCS ${XRCE}/src/c/*.c)

set(TRANSPORT_TYPES "SERIAL")

# Transport sources.
foreach(TRANS_TYPE ${TRANSPORT_TYPES})
#    if(CONFIG_PROFILE_${TRANS_TYPE}_TRANSPORT)
        string(TOLOWER ${TRANS_TYPE} TT)
        set(${TRANS_TYPE}_SRCS
            ${XRCE}/src/c/profile/transport/${TT}/${TT}_transport.c
#            ${XRCE}/src/c/profile/transport/${TT}/${TT}_transport_posix.c
            )
#    else()
#        set(${TRANS_TYPE}_SRCS )
#    endif()
endforeach()

# Transport discovery source.
if(CONFIG_PROFILE_DISCOVERY AND CONFIG_PROFILE_UDP_TRANSPORT)
    set(UDP_DISCOVERY_SRCS ${XRCE}/src/c/profile/discovery/transport/udp_transport_datagram_linux.c)
endif()


set(COMPONENT_SRCS
    ${XRCE}/src/c/core/session/stream/input_best_effort_stream.c
    ${XRCE}/src/c/core/session/stream/input_reliable_stream.c
    ${XRCE}/src/c/core/session/stream/output_best_effort_stream.c
    ${XRCE}/src/c/core/session/stream/output_reliable_stream.c
    ${XRCE}/src/c/core/session/stream/stream_storage.c
    ${XRCE}/src/c/core/session/stream/stream_id.c
    ${XRCE}/src/c/core/session/stream/seq_num.c
    ${XRCE}/src/c/core/session/session.c
    ${XRCE}/src/c/core/session/session_info.c
    ${XRCE}/src/c/core/session/submessage.c
    ${XRCE}/src/c/core/session/object_id.c
#    ${XRCE}/src/c/core/serialization/xrce_protocol.c
    ${XRCE}/src/c/core/serialization/xrce_header.c
    ${XRCE}/src/c/core/serialization/xrce_subheader.c
    ${XRCE}/src/c/util/time.c
    ${XRCE}/src/c/util/ping.c
    ${XRCE}/src/c/profile/transport/stream_framing/stream_framing_protocol.c
    ${XRCE}/src/c/core/serialization/xrce_types.c
    $<$<OR:$<BOOL:${CONFIG_VERBOSE_MESSAGE}>,$<BOOL:${CONFIG_VERBOSE_SERIALIZATION}>>:src/core/log/log.c>
#    $<$<BOOL:${CONFIG_PROFILE_COMMON_CREATE_ENTITIES}>:${XRCE}/src/c/core/session/common_create_entities.c>
    $<$<BOOL:${CONFIG_PROFILE_CREATE_ENTITIES_REF}>:${XRCE}/src/c/core/session/create_entities_ref.c>
    $<$<BOOL:${CONFIG_PROFILE_CREATE_ENTITIES_XML}>:${XRCE}/src/c/core/session/create_entities_xml.c>
    $<$<BOOL:${CONFIG_PROFILE_READ_ACCESS}>:${XRCE}/src/c/core/session/read_access.c>
    $<$<BOOL:${CONFIG_PROFILE_WRITE_ACCESS}>:${XRCE}/src/c/core/session/write_access.c>
    $<$<BOOL:${CONFIG_PROFILE_DISCOVERY}>:${XRCE}/src/c/profile/discovery/discovery.c>
    ${UDP_DISCOVERY_SRCS}
    ${UDP_SRCS}
    ${TCP_SRCS}
    ${SERIAL_SRCS}
    ${XRCE_CDR}/src/c/common.c
    ${XRCE_CDR}/src/c/types/array.c
    ${XRCE_CDR}/src/c/types/basic.c
    ${XRCE_CDR}/src/c/types/sequence.c
    ${XRCE_CDR}/src/c/types/string.c
    serial.cpp
    )

set(COMPONENT_PRIV_REQUIRES lwip  newlib)

register_component()