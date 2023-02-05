#ifndef CRYPTO_H
#define CRYPTO_H
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1

#include "cryptlib.h"
#include "channels.h"
#include "filters.h"
#include "files.h"
#include "crc.h"
#include "hex.h"
#include "md5.h"

#include <string>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <msg.h>

namespace crypto
{

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
    bool Safe(std::string fp, std::string hash);
}

#endif //CRYPTO_H