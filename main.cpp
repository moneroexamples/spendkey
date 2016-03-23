#include "src/CmdLineOptions.h"
#include "src/tools.h"

#include "mnemonics/electrum-words.h"


using crypto::ElectrumWords::words_to_bytes;
using crypto::ElectrumWords::bytes_to_words;

using xmreg::operator<<;

using namespace std;

namespace epee
{
    unsigned int g_test_dbg_lock_sleep = 0;
}

int main(int ac, const char* av[]) {

    // get command line options
    xmreg::CmdLineOptions opts {ac, av};

    // if help was chosen, display help text and finish
    if (opts.help_requested())
    {
        return 0;
    }

    // default language for the mnemonic
    // representation of the private spend key
    string language {"English"};

    // get private spend key options
    auto spendkey_opt = opts.get_option<string>("spendkey");
    auto mnemonic_opt = opts.get_option<string>("mnemonic");

    // get the program command line options, or
    // some default values for quick check
    string spendkey_str = spendkey_opt ? *spendkey_opt : "af6082af29108abda69cc385dfed2102b892a871695367cb22a4b9b6df8b3206";
    string mnemonic_str = mnemonic_opt ? *mnemonic_opt : "spout midst duckling tepid odds glass enhanced avatar ocean rarest eavesdrop egotistic oxygen trying future airport session nanny tedious guru asylum superior cement cunning eavesdrop";


    crypto::secret_key private_spend_key;


    if ((spendkey_opt && !mnemonic_opt) || (!spendkey_opt && !mnemonic_opt))
    {
        // if spend key is given or nothing is provided by the user,
        // then derive the mnemonic using either key provided or the default one

        if (spendkey_opt)
        {
            // use provided spend key by the user
            spendkey_str = *spendkey_opt;
        }

        // parse string representing given private spend key
        if (!xmreg::parse_str_secret_key(spendkey_str, private_spend_key))
        {
            cerr << "Cant parse the spend key: " << spendkey_str << endl;
            return 1;
        }

        // derive the mnemonic version of the spend key.
        // 25 word mnemonic that is provided by the simplewallet
        // is just a word representation of the private spend key
        if (!bytes_to_words(private_spend_key, mnemonic_str, language))
        {
            cerr << "\nCant create the mnemonic for the private spend key: "
                 << private_spend_key << endl;
            return 1;
        }
    }
    else if (!spendkey_opt && mnemonic_opt)
    {
        // if mnemonic is given, then derive the spend key
        if (!words_to_bytes(*mnemonic_opt, private_spend_key, language))
        {
            cerr << "\nCant create private spend key for the mnemonic: "
                 << *mnemonic_opt << endl;
            return 1;
        }
    }


    cout << "\n"
         << "Mnemonic: " << mnemonic_str << endl;

    // the seed for generation of private view key later on
    // is the private spend key.
    cout << "\n"
         << "Hexadecimal seed : " << private_spend_key << endl;


    cout << "\n"
         << "Private spend key: " << private_spend_key << endl;


    // we have private_spend_key, so now
    // we need to get the corresponding
    // public_spend_key
    crypto::public_key public_spend_key;


    // generate public key based on the private key
    crypto::secret_key_to_public_key(private_spend_key, public_spend_key);


    cout << "Public spend key : "  << public_spend_key  << endl;


    // now its time to get the view keys.
    // in monero's simplewallet, the private view key
    // is generated using keccak hash of the private spend
    // key. in other words, the hash of the private spend key
    // is used for generating the private view key,
    // and subsequently, its corresponding public view key.


    crypto::hash hash_of_private_spend_key;

    // calculate keccak hash of the private spend key
    xmreg::keccak_hash(private_spend_key, hash_of_private_spend_key);


    crypto::secret_key private_view_key;
    crypto::public_key public_view_key;

    // get the view keys using the hash_of_private_spend_key
    // as the seed.
    crypto::generate_keys(public_view_key, private_view_key,
                          xmreg::get_key_from_hash<crypto::secret_key>(hash_of_private_spend_key),
                          true /* recover keys*/);


    cout << "\n"
         << "Private view key : "  << private_view_key << "\n"
         << "Public view key  : "  << public_view_key  << endl;


    // once we have both private view and spend keys, we can get our
    // address. Monero's address is a combination of the two
    // public keys.
    cryptonote::account_public_address address {public_spend_key, public_view_key};


    cout << "\n"
         << "Monero address   : "  << address << endl;

    cout << "\n"
         << "End of program." << endl;

    return 0;
}