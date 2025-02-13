// Copyright © 2017-2020 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "Data.h"
#include "Hash.h"
#include "rust/bindgen/WalletCoreRSBindgen.h"

#include <string>

namespace TW::Base58 {
    /// Decodes a base 58 string into `result`, returns `false` on failure.
    static inline Data decode(const std::string& string, Base58Alphabet alphabet = Base58Alphabet::Bitcoin)  {
        if (string.empty()) {
            return {};
        }
        auto decoded = decode_base58(string.c_str(), alphabet);
        if (decoded.data == nullptr || decoded.size == 0) {
            return {};
        }
        Data decoded_vec(&decoded.data[0], &decoded.data[decoded.size]);
        std::free(decoded.data);
        return decoded_vec;
    }

    static inline Data decodeCheck(const std::string& string, Base58Alphabet alphabet = Base58Alphabet::Bitcoin, Hash::Hasher hasher = Hash::HasherSha256d) {
        auto result = decode(string, alphabet);
        if (result.size() < 4) {
            return {};
        }

        // re-calculate the checksum, ensure it matches the included 4-byte checksum
        auto hash = Hash::hash(hasher, result.data(), result.size() - 4);
        if (!std::equal(hash.begin(), hash.begin() + 4, result.end() - 4)) {
            return {};
        }

        return Data(result.begin(), result.end() - 4);
    }

    template <typename T>
    static inline std::string encode(const T& data, Base58Alphabet alphabet = Base58Alphabet::Bitcoin) {
        auto encoded = encode_base58(data.data(), data.size(), alphabet);
        std::string encoded_str(encoded);
        free_string(encoded);
        return encoded_str;
    }

    template <typename T>
    static inline std::string encodeCheck(const T& data, Base58Alphabet alphabet = Base58Alphabet::Bitcoin, Hash::Hasher hasher = Hash::HasherSha256d) {
        auto hash = Hash::hash(hasher, data);
        Data toBeEncoded(std::begin(data), std::end(data));
        toBeEncoded.insert(toBeEncoded.end(), hash.begin(), hash.begin() + 4);
        return encode(toBeEncoded, alphabet);
    }
}
