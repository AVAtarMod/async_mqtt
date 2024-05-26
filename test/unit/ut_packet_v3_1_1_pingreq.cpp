// Copyright Takatoshi Kondo 2022
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "../common/test_main.hpp"
#include "../common/global_fixture.hpp"

#include <boost/lexical_cast.hpp>

#define ASYNC_MQTT_UNIT_TEST_FOR_PACKET

BOOST_AUTO_TEST_SUITE(ut_packet)
struct v311_pingreq;
BOOST_AUTO_TEST_SUITE_END()

#include <async_mqtt/packet/v3_1_1_pingreq.hpp>
#include <async_mqtt/packet/packet_iterator.hpp>
#include <async_mqtt/packet/packet_traits.hpp>

#define ASYNC_MQTT_UNIT_TEST_FOR_PACKET

BOOST_AUTO_TEST_SUITE(ut_packet)

namespace am = async_mqtt;

BOOST_AUTO_TEST_CASE(v311_pingreq) {
    BOOST_TEST(am::is_pingreq<am::v3_1_1::pingreq_packet>());
    BOOST_TEST(am::is_v3_1_1<am::v3_1_1::pingreq_packet>());
    BOOST_TEST(!am::is_v5<am::v3_1_1::pingreq_packet>());
    BOOST_TEST(am::is_client_sendable<am::v3_1_1::pingreq_packet>());
    BOOST_TEST(!am::is_server_sendable<am::v3_1_1::pingreq_packet>());

    auto p = am::v3_1_1::pingreq_packet{};

    {
        auto cbs = p.const_buffer_sequence();
        BOOST_TEST(cbs.size() == p.num_of_const_buffer_sequence());
        char expected[] {
            char(0xc0),                         // fixed_header
            0x00,                               // remaining_length
        };
        auto [b, e] = am::make_packet_range(cbs);
        BOOST_TEST(std::equal(b, e, std::begin(expected)));

        am::buffer buf{std::begin(expected), std::end(expected)};
        am::error_code ec;
        auto p = am::v3_1_1::pingreq_packet{buf, ec};
        BOOST_TEST(!ec);

        auto cbs2 = p.const_buffer_sequence();
        BOOST_TEST(cbs2.size() == p.num_of_const_buffer_sequence());
        auto [b2, e2] = am::make_packet_range(cbs2);
        BOOST_TEST(std::equal(b2, e2, std::begin(expected)));
    }
    BOOST_TEST(
        boost::lexical_cast<std::string>(p) ==
        "v3_1_1::pingreq{}"
    );
}

BOOST_AUTO_TEST_SUITE_END()
