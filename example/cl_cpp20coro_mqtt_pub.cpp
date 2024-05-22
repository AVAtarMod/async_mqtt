// Copyright Takatoshi Kondo 2023
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <string>

#include <boost/asio.hpp>

#include <async_mqtt/all.hpp>

namespace as = boost::asio;
namespace am = async_mqtt;

// Use as::use_awaitable as the default completion token for am::client
using awaitable_client =
    boost::asio::use_awaitable_t<>::as_default_on_t<
        am::client<am::protocol_version::v3_1_1, am::protocol::mqtt>
    >;

as::awaitable<void>
proc(
    awaitable_client& amcl,
    std::string_view host,
    std::string_view port) {

    std::cout << "start" << std::endl;

    try {
        // all underlying layer handshaking
        // (Resolve hostname, TCP handshake)
        co_await am::async_underlying_handshake(amcl.next_layer(), host, port);
        std::cout << "mqtt undlerlying handshaked" << std::endl;

        // MQTT connect and receive loop start
        auto connack_opt = co_await amcl.async_start(
            true,                // clean_start
            std::uint16_t(0),    // keep_alive
            "ClientIdentifierPub1",
            std::nullopt,        // will
            "UserName1",
            "Password1"
        );
        if (connack_opt) {
            std::cout << *connack_opt << std::endl;
        }

        auto print_pubres =
            [](auto const& pubres) {
                if (pubres.puback_opt) {
                    std::cout << *pubres.puback_opt << std::endl;
                }
                if (pubres.pubrec_opt) {
                    std::cout << *pubres.pubrec_opt << std::endl;
                }
                if (pubres.pubcomp_opt) {
                    std::cout << *pubres.pubcomp_opt << std::endl;
                }
            };

        // publish
        // MQTT publish QoS0 and wait response (socket write complete)
        auto pubres0 = co_await amcl.async_publish(
            "topic1",
            "payload1",
            am::qos::at_most_once
        );
        print_pubres(pubres0);

        // MQTT publish QoS1 and wait response (puback receive)
        auto pid_pub1_opt = co_await amcl.async_acquire_unique_packet_id(); // async version
        auto pubres1 = co_await amcl.async_publish(
            // you can pass v5::publish_packet constructor's args directly
            *pid_pub1_opt,
            "topic2",
            "payload2",
            am::qos::at_least_once
        );
        print_pubres(pubres1);

        // MQTT publish QoS2 and wait response (pubrec, pubcomp receive)
        auto pid_pub2 = co_await amcl.async_acquire_unique_packet_id_wait_until(); // async version
        auto pubres2 = co_await amcl.async_publish(
            pid_pub2,
            "topic3",
            "payload3",
            am::qos::exactly_once
        );
        print_pubres(pubres2);

        // disconnect
        co_await amcl.async_disconnect();
        std::cout << "finished" << std::endl;
    }
    catch (boost::system::system_error const& se) {
        std::cout << se.code().message() << std::endl;
    }
}

int main(int argc, char* argv[]) {
    am::setup_log(am::severity_level::warning);
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " host port" << std::endl;
        return -1;
    }
    as::io_context ioc;
    awaitable_client amcl{ioc.get_executor()};
    as::co_spawn(amcl.get_executor(), proc(amcl, argv[1], argv[2]), as::detached);
    ioc.run();
}
