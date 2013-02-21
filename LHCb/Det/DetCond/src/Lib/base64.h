/*
 * base64.h
 *
 *  Created on: Feb 11, 2013
 *      Author: Liang Sun
 *      Basic implementation of base64
 *      retrieved from http://stackoverflow.com/questions/342409/how-do-i-base64-encode-decode-in-c
 */

#ifndef BASE64_H_
#define BASE64_H_


//#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string>

static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};
static char *decoding_table = NULL;
static int mod_table[] = {0, 2, 1};


bool base64_encode(const std::string& data,
                    std::string& encoded_data) {

	const unsigned char* data2 = (const unsigned char*)data.c_str();
	size_t input_length = data.length();
	size_t output_length = ((input_length - 1) / 3) * 4 + 4;
//    *output_length = (size_t) (4.0 * ceil((double) input_length / 3.0));
//    *output_length = ((input_length - 1) / 3) * 4 + 4;
	encoded_data.resize(output_length);
//    encoded_data = new char[*output_length];
//    if (encoded_data == NULL) return false;

    for (size_t i = 0, j = 0; i < input_length;) {

        uint32_t octet_a = i < input_length ? data2[i++] : 0;
        uint32_t octet_b = i < input_length ? data2[i++] : 0;
        uint32_t octet_c = i < input_length ? data2[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
    }

    for (int i = 0; i < mod_table[input_length % 3]; i++)
        encoded_data[output_length - 1 - i] = '=';

//    encoded_data[*output_length] = '\0';
    return true;
}

void build_decoding_table() {

    decoding_table = new char[256];

    for (int i = 0; i < 0x40; i++)
        decoding_table[encoding_table[i]] = i;
}

bool base64_decode(const std::string& data,
		std::string& decoded_data) {

	size_t input_length = data.length();
    size_t output_length;

    if (decoding_table == NULL) build_decoding_table();

    if (input_length % 4 != 0) return false;

    output_length = input_length / 4 * 3;
    if (data[input_length - 1] == '=') output_length--;
    if (data[input_length - 2] == '=') output_length--;

    decoded_data.resize(output_length);
//    decoded_data = new unsigned char[*output_length];
//    if (decoded_data == NULL) return false;

    for (int i = 0, j = 0; i < input_length;) {

        uint32_t sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];

        uint32_t triple = (sextet_a << 3 * 6)
                        + (sextet_b << 2 * 6)
                        + (sextet_c << 1 * 6)
                        + (sextet_d << 0 * 6);

        if (j < output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
        if (j < output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;

    }
    return true;
}




void base64_cleanup() {
    delete decoding_table;
}



#endif /* BASE64_H_ */
