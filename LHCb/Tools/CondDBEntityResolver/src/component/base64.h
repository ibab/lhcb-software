#ifndef BASE64_H_
#define BASE64_H_

/*
 * base64.h
 *
 *  Created on: Feb 11, 2013
 *      Author: Liang Sun
 *      Basic implementation of base64
 *      retrieved from http://stackoverflow.com/questions/342409/how-do-i-base64-encode-decode-in-c
 */


#include <cstdint>
#include <cstdlib>
#include <string>

static std::array<char,64> encoding_table =
                               {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};

std::array<char,256> build_decoding_table() {
    std::array<char,256> tbl;
    for (int i = 0; i < 0x40; i++) tbl[(size_t)encoding_table[i]] = i;
    return tbl;
}
static const auto decoding_table = build_decoding_table();

std::string base64_encode(const std::string& data, const std::string& preamble) {

	auto data2 = reinterpret_cast<const unsigned char*>(data.data());
	auto input_length = data.length();
	auto output_length = (input_length!=0 ? ((input_length - 1) / 3) * 4 + 4 : 0);
    std::string encoded_data(output_length+preamble.size(),char{});
    std::copy(preamble.begin(),preamble.end(),encoded_data.begin());

    for (size_t i = 0, j = preamble.size(); i < input_length;) {

        uint32_t octet_a = (i < input_length ? data2[i++] : 0);
        uint32_t octet_b = (i < input_length ? data2[i++] : 0);
        uint32_t octet_c = (i < input_length ? data2[i++] : 0);

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
    }
    std::fill( std::prev(encoded_data.end(), (3-input_length%3)%3),
               encoded_data.end(), '=' );
    return encoded_data;
}


std::string base64_decode(const char* data, size_t input_length) {

    if (input_length % 4 != 0) return {};

    std::string::size_type output_length = input_length / 4 * 3;
    if (data[input_length - 1] == '=') --output_length;
    if (data[input_length - 2] == '=') --output_length;

    std::string decoded_data(output_length,char{});

    for (size_t i = 0, j = 0; i < input_length;) {
        uint32_t sextet_a = data[i] == '=' ? 0: decoding_table[(size_t)data[i]]; i++;
        uint32_t sextet_b = data[i] == '=' ? 0: decoding_table[(size_t)data[i]]; i++;
        uint32_t sextet_c = data[i] == '=' ? 0: decoding_table[(size_t)data[i]]; i++;
        uint32_t sextet_d = data[i] == '=' ? 0: decoding_table[(size_t)data[i]]; i++;

        uint32_t triple = (sextet_a << 3 * 6)
                        + (sextet_b << 2 * 6)
                        + (sextet_c << 1 * 6)
                        + (sextet_d << 0 * 6);

        if (j < output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
        if (j < output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
    }
    return decoded_data;
}

#endif /* BASE64_H_ */
