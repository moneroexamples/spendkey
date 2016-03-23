# Recover Monero address using the private spend key

[Monero](https://getmonero.org/) `simplewallet` has a command called `spendkey` which prints
out your private spend key. However, its not clear what to do with this private key.

It turns out, that this private key is the hexadecimal representation of the 25 word
mnemonic seed, which `simplewallet` gives you when you create a new wallet. For example, the following mnemonic seed:

```
vinegar talent sorry hybrid ultimate template nimbly jukebox axes inactive veered toenail pride plotting chrome victim agnostic science bailed paddles wounded peaches king laptop king
```

corresponds to this private spend key:

```
6ee02ef8647856f4080882a1ec4fabee19ec047ca24d3abb13c0ce589a46f702
```

Since mnemonic seed allows you to restore deterministic `simplewallet` wallet address. The same thing can be done
with just the private spend key. This includes recovery of your private view key.

More on Monero's keys and addresses can be found on [Address Generation Tests](http://xmrtests.llcoins.net/addresstests.html) website.



# Monero address

Monero address is as a combination of public spend and view keys. The public keys
are obtained directly from the corresponding private spend and view keys, respectively.
The private view key is obtained from the private spend key (in `simplewallet`),
which in turn can be obtained from the mnemonic seed. This means that
spend key is much more important than the view key, since it allows
 to recover your private view key (revers is not true). So what it means, is that you should not be disclosing your private spend key to strangers!



 Please note that this is not the only way of deriving private keys. For example, [MyMonero](https://mymonero.com/) is using a different procedure, which is not compatible with that in `simplewallet`. The procedure presented here is same as the one used in `simplewallet`. More on this is
[here](http://xmrtests.llcoins.net/addresstests.html)
and [here](https://en.reddit.com/r/Monero/comments/3s80l2/why_mymonero_key_derivation_is_different_than_for/).


# Pre-requisites

The example was tested on Ubuntu 16.04 x64 and Monero 0.9.

Instruction for Monero 0.9 compilation and setup of Monero's header files and libraries are at:
 - [Compile Monero 0.9 on Ubuntu 16.04](https://github.com/moneroexamples/compile-monero-09-on-ubuntu-16-04)

# C++ code
The most interesting C++ file is `main.cpp`.
Therefore, I will present only that file here. Full source code is
at [github](https://github.com/moneroexamples/spendkey.git). The github's version can
also be slightly different than the code presented here, as it might be updated
more frequently.
So for the latest version
of this example, please check the github repository directly.

## main.cpp
This is the main file of the example. The program takes
two input arguments:

 - `spendkey,s` - the private spend key.
 - `mnemonic,m` - 25 word mnemonic representation of the spend key.


```c++
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
```

# Output example 1
Executing the program as follows:

```bash
./spendkey
```

Results in the following output:

```bash
Mnemonic: spout midst duckling tepid odds glass enhanced avatar ocean rarest eavesdrop egotistic oxygen trying future airport session nanny tedious guru asylum superior cement cunning eavesdropspout midst duckling tepid odds glass enhanced avatar ocean rarest eavesdrop egotistic oxygen trying future airport session nanny tedious guru asylum superior cement cunning eavesdrop

Hexadecimal seed : <af6082af29108abda69cc385dfed2102b892a871695367cb22a4b9b6df8b3206>

Private spend key: <af6082af29108abda69cc385dfed2102b892a871695367cb22a4b9b6df8b3206>
Public spend key : <7aff30fbdc005ecb03f57a11e250e0d665621ffde1d44c6aa84a8212cc0d1236>

Private view key : <157874dc4e2961c872f87aaf4346146d0f596e2f116a51fbac01b693a8e3020a>
Public view key  : <25c1b6920540fbcfcb0e36bd2c88f5c1e62e5ef1d621279e7230b47648e64a63>

Monero address   : <46HSxE7KoiDaxWFWR1wmJfcrunNj4TLiPJqiCJkQn345A4JJzgBNhUvbkrYWJX4EVJZS4kJGfGj7CTW8GEUHsbEZCEupMt6>
```
These results agree with those obtained using
 [Address Generation Tests](http://xmrtests.llcoins.net/addresstests.html) website.

# Output example 2

Executing the program as follows:

```bash
./spendkey -s 11830f8a232b7b0152a709340bb653357d2aa92985f83b6e0dcf7cb2bf11330d
```
Results in the following output:

```bash
Mnemonic: spout midst duckling tepid odds glass enhanced avatar ocean rarest eavesdrop egotistic oxygen trying future airport session nanny tedious guru asylum superior cement cunning eavesdropsiblings together icing idols sovereign sprig suture aloof egotistic speedy diplomat impel wise glass long cafe perfect awakened ought enforce voted mime fight goggles egotistic

Hexadecimal seed : <11830f8a232b7b0152a709340bb653357d2aa92985f83b6e0dcf7cb2bf11330d>

Private spend key: <11830f8a232b7b0152a709340bb653357d2aa92985f83b6e0dcf7cb2bf11330d>
Public spend key : <269e626955fe97f3853d768e0ad02bf92d0a95a8c03704fd8a96d5ad6a38c42a>

Private view key : <21e63fb10d4adad609a327395e42d737bbd772dce53d64d2733c4aacc69eb300>
Public view key  : <b4e0cf1d4c7c60b590081db50e877e7576a36fb2c257f5f94932222d23d34b36>

Monero address   : <435zP7PmQwYhjTJu7AmU8SigKVMsY9j3yjQfnwnfjRZM89Jp2ZxEpsyXNPKkr5hfwBLeYTiZeqfaLihPNWWEzbgN77gqFHp>
```

These results also agree with those obtained using
 [Address Generation Tests](http://xmrtests.llcoins.net/addresstests.html) website.

# Output example 3
Executing the program as follows:
```bash
./spendkey -m "essential future brunt cajun upper ammo incur smelting usual tyrant tattoo virtual long hectare idols guarded blender usage ghost sample eagle shelter does dozen usage"

```
Results in the following output:

```bash
Mnemonic: essential future brunt cajun upper ammo incur smelting usual tyrant tattoo virtual long hectare idols guarded blender usage ghost sample eagle shelter does dozen usage

Hexadecimal seed : <0a0214cf7716292246d277214830411b20d3cd08cd119dcd9e149d7bd1151e02>

Private spend key: <0a0214cf7716292246d277214830411b20d3cd08cd119dcd9e149d7bd1151e02>
Public spend key : <7e9f73449c1b3a9a83c25a641a95c8f74f7ccd309dd3fba5222ce349052af567>

Private view key : <bd613fbad795df25b8218a7e4f80f4e3158db0646d1cb7d22d84f0b603a0f60b>
Public view key  : <fad7d278ee9fedf0034da1812115c5a369d6bd67e1b076afbc4aee579a609ce3>

Monero address   : <46RRCV68frZSqzBsh9TWf9iNDrYhPkABUUd16zFLoPECJPjd3AtuTyeh9RhSWAqiCLULKGf9SC1UHWPra64ykRiTSg3RPmW>
```

These results also agree with those obtained using
 [Address Generation Tests](http://xmrtests.llcoins.net/addresstests.html) website.


## Compile this example
The dependencies are same as those for Monero, so I assume Monero compiles
correctly. If so then to download and compile this example, the following
steps can be executed:

```bash
# download the source code
git clone https://github.com/moneroexamples/spendkey.git

# enter the downloaded sourced code folder
cd spendkey

# create the makefile
cmake .

# compile
make
```

After this, `spendkey` executable file should be present in spendkey
folder.

## How can you help?

Constructive criticism, code and website edits are always welcome. They can be made through github.

Some Monero are also welcome:
```
48daf1rG3hE1Txapcsxh6WXNe9MLNKtu7W7tKTivtSoVLHErYzvdcpea2nSTgGkz66RFP4GKVAsTV14v6G3oddBTHfxP6tU
```
