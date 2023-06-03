# async_mqtt

Asynchronous MQTT communication library.

Version 0.1 [![Actions Status](https://github.com/redboltz/async_mqtt/workflows/CI/badge.svg)](https://github.com/redboltz/async_mqtt/actions)[![codecov](https://codecov.io/gh/redboltz/async_mqtt/branch/main/graph/badge.svg)](https://codecov.io/gh/redboltz/async_mqtt)

# Overview

[API Reference](https://redboltz.github.io/async_mqtt)

## Boost.Asio style asynchronous APIs support

### [Completion Token](https://www.boost.org/doc/html/boost_asio/overview/model/completion_tokens.html) is supported
- Callbacks
  - [example](example/ep_cb_mqtt_client.cpp)
- [`boost::asio::use_future`](https://www.boost.org/doc/html/boost_asio/overview/composition/futures.html)
  - [example](example/ep_future_mqtt_client.cpp)
- [Stackless Coroutine (`boost::asio::coroutine`)](https://www.boost.org/doc/html/boost_asio/overview/composition/coroutine.html)
  - [example](example/ep_slcoro_mqtt_client.cpp)
- [and more](https://www.boost.org/doc/html/boost_asio/overview/composition.html)

I recommend using [Stackless Coroutine (`boost::asio::coroutine`)](https://www.boost.org/doc/html/boost_asio/overview/composition/coroutine.html) because it can avoid deep nested callbacks and higher performance than [`boost::asio::use_future`](https://www.boost.org/doc/html/boost_asio/overview/composition/futures.html).

### recv() funtion
You need to call recv() function when you want to receive MQTT packet. It is similar to Boost.Asio read() function.
You can control packet receiving timing. async_mqtt doesn't use handler registering style APIs such as `set_publish_handler()`. If you need handler registering APIs, you can create them using recv().
recv() function is more flexible than handler registering APIs. In addition, it works well with [Completion Token](https://www.boost.org/doc/html/boost_asio/overview/model/completion_tokens.html).

#### packet_variant
recv()'s CompletionToken parameter is [packet_variant](/include/async_mqtt/packet/packet_variant.hpp). It is a variant type of all MQTT packets and error.

NOTE: async_mqtt has basic_foobar type and foobar type if the type contains MQTT's Packet Identifier. basic_foobar takes PacketIdBytes parameter. basic_foobar<2> is the same as foobar. MQTT spec defines the size of Packet Identifier to 2. But some of clustering brokers use expanded Packet Identifer for inter brokers communication. General users doesn't need to care basic_foobar types, simply use foobar.

You can access packet_variant as follows:

```cpp
namespace am = async_mqtt; // always use this namespace alias in this document
```

```cpp
am::packet_variant pv = ...; // from CompletionToken
if (pv) { // pv is packet_variant
     pv.visit(
         am::overload {
              [&](am::v3_1_1::connack_packet const& p) {
                  std::cout
                      << "MQTT CONNACK recv "
                      << "sp:" << p.session_present()
                      << std::endl;
              },
              // other packets handling code here
              [](auto const&) {}
         }
    );
}
else {
    std::cout
         << "MQTT CONNACK recv error:"
         << pv.get<am::system_error>().what()
         << std::endl;
}
```

`pv` can evaluated as bool. If `pv` has valid packet then it is convert to true, otherwise false. Typically, when underlying socket is disconnected, `pv` contains error and it is evaluated as false.

#### Control Packet Type filter
You might interested in the specific packets. Your application doesn't want to care non important packet like pingresp, puback, pubrec, pubrel, and pubcomp packets.

You can filter packets as follows:

```cpp
// ep is endpoint
ep.recv(am::filter::match, {am::control_packet_type::publish}, completion_token);
```

When you set `filter::match` as the first argument, the second parameter is a list of matching MQTT Control Packet types. If unmatched packets are received, completion_token isn't invoked but received packets are apropriately proccessed.
If error is happened, completion_token is invoked with packet_variant that contains error.

```cpp
// ep is endpoint
ep.recv(am::filter::except, {am::control_packet_type::pingresp, am::control_packet_type::puback}, completion_token);
```

When you set `filter::except` as the first argument, the second parameter is a list of ignoring MQTT Control Packet types. If the packets int the list are received, completion_token isn't invoked but received packets are apropriately proccessed.
If error is happened, completion_token is invoked with packet_variant that contains error.


### send() function

### Customizable underlying layer

You can use TCP(mqtt), TLS(mqtts), WebSocket(ws), TLS+WebSocket(wss). They are [predefined](TBD).
In addition, you can use any underlying layer that is compatible to `boost::asio::ip::tcp::socket`.


TLS, WS configuration and handshaking are separated from async_mqtt core.

### Packet Based APIs

async_mqtt automatically update endpoint's interenal state when packet sending and receiving. For example, When you send CONNECT packet with maximum_packet_size property, endpoint set maximum packet size for receiving.


## Requirement

- Compiler: C++17 or later
- Boost Libraries:  1.81.0 or later
