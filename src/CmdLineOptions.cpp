//
// Created by mwo on 6/11/15.
//

#include "CmdLineOptions.h"


namespace xmreg
{
    /**
     * Take the acc and *avv[] from the main() and check and parse
     * all the options given
     */
    CmdLineOptions::CmdLineOptions(int acc, const char *avv[]) {


        options_description desc(
                "xmreg03, recover private mnemonic seed, private view key "
                " and monero address using private spendkey");

        desc.add_options()
                ("help,h", //value<bool>()->default_value(false)->implicit_value(true),
                 "produce help message")
                ("spendkey,s", value<string>(),
                 "private spend key string")
                ("mnemonic,m", value<string>(),
                 "25 word representation of the private view key");

        store(command_line_parser(acc, avv)
                          .options(desc)
                          .run(), vm);

        conflicting_options("spendkey", "mnemonic");

        notify(vm);

        if (vm.count("help"))
        {
            cout << desc << endl;
        }
    }

    void
    CmdLineOptions::conflicting_options(const char* opt1, const char* opt2)
    {
        if (vm.count(opt1) && !vm[opt1].defaulted()
                && vm.count(opt2) && !vm[opt2].defaulted())
            {
                throw logic_error(string("Specifly only  '")
                                    + opt1 + "' or '" + opt2 + "'. Not both.");
            }
    }


/**
     * Return the value of the argument passed to the program
     * in wrapped around boost::optional
     */
    template<typename T>
    boost::optional<T>
    CmdLineOptions::get_option(const string & opt_name) const
    {

        if (!vm.count(opt_name))
        {
            return boost::none;
        }

        return vm[opt_name].as<T>();
    }


    // explicit instantiations of get_option template function
    template  boost::optional<string>
    CmdLineOptions::get_option<string>(const string & opt_name) const;

    template  boost::optional<bool>
    CmdLineOptions::get_option<bool>(const string & opt_name) const;


    bool
    CmdLineOptions::help_requested() const
    {
        return vm.count("help");
    }

}
