// Copyright Takatoshi Kondo 2022
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#if !defined(ASYNC_MQTT_PREDEFINED_LAYER_WS_HPP)
#define ASYNC_MQTT_PREDEFINED_LAYER_WS_HPP

#include <async_mqtt/predefined_layer/mqtt.hpp>
#include <async_mqtt/predefined_layer/customized_websocket_stream.hpp>

/**
 * @defgroup predefined_layer_ws predefined underlying layer (Websocket)
 * @ingroup predefined_layer
 */

namespace async_mqtt {

namespace as = boost::asio;
namespace bs = boost::beast;

namespace protocol {

/**
 * @ingroup predefined_layer_ws
 * @brief Type alias of boost::beast::websocket::stream of mqtt
 */
using ws = bs::websocket::stream<mqtt>;

} // namespace protocol


/**
 * @ingroup predefined_layer_ws
 * @brief Websocket handshake
 * This function does underlying layers handshaking prior to Websocket handshake
 * @param layer  Websocket layer
 * @param host   host name or IP address to connect
 * @param port   port number to connect
 * @param path   websocket path
 * @param token  completion token. signature is void(error_code)
 * @return deduced by token
 *
 * #### Requirements
 * @li Header: async_mqtt/predefined_layer/ws.hpp
 *
 */
template <
    typename NextLayer,
    typename CompletionToken = as::default_completion_token_t<
        typename bs::websocket::stream<NextLayer>::executor_type
    >
>
auto
async_underlying_handshake(
    bs::websocket::stream<NextLayer>& layer,
    std::string_view host,
    std::string_view port,
    std::string_view path,
    CompletionToken&& token = as::default_completion_token_t<
        typename bs::websocket::stream<NextLayer>::executor_type
    >{}
);

/**
 * @ingroup predefined_layer_ws
 * @brief Websocket handshake
 * This function does underlying layers handshaking prior to Websocket handshake
 * Websocket path is set as "/".
 * @param layer  Websocket layer
 * @param host   host name or IP address to connect
 * @param port   port number to connect
 * @param token  completion token. signature is void(error_code)
 * @return deduced by token
 *
 * #### Requirements
 * @li Header: async_mqtt/predefined_layer/ws.hpp
 *
 */
template <
    typename NextLayer,
    typename CompletionToken = as::default_completion_token_t<
        typename bs::websocket::stream<NextLayer>::executor_type
    >
>
auto
async_underlying_handshake(
    bs::websocket::stream<NextLayer>& layer,
    std::string_view host,
    std::string_view port,
    CompletionToken&& token = as::default_completion_token_t<
        typename bs::websocket::stream<NextLayer>::executor_type
    >{}
);

} // namespace async_mqtt

#include <async_mqtt/predefined_layer/impl/ws_handshake.hpp>

#endif // ASYNC_MQTT_PREDEFINED_LAYER_WS_HPP
