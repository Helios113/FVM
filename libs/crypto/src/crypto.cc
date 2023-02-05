#include <crypto.h>

namespace crypto {
bool Safe(std::string fp, std::string hash) {
    try {
        std::string cur_hash;
        CryptoPP::Weak::MD5 md5;

        CryptoPP::HashFilter f1(
            md5, new CryptoPP::HexEncoder(new CryptoPP::StringSink(cur_hash)));
        CryptoPP::ChannelSwitch cs;
        cs.AddDefaultRoute(f1);
        CryptoPP::FileSource(fp.c_str(), true /*pumpAll*/,
                             new CryptoPP::Redirector(cs));
        return boost::iequals(cur_hash, hash);
    } catch (std::exception& e) {
        msg::Msg(e.what(), msg::MsgType::Error, true);
    }
    return false;
}
}  // namespace crypto