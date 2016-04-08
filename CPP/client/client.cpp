#include <stdexcept>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <ace/Arg_Shifter.h>

#include "LeakC.h"

namespace {
void GetArgs(int argc, char *argv[], std::string &ior) {
    ACE_Arg_Shifter arg_shifter(argc, argv);
    while (arg_shifter.is_anything_left()) {
        const ACE_TCHAR *currentArg = 0;

        if ((currentArg = arg_shifter.get_the_parameter(ACE_TEXT("-ior"))) != 0) {
            std::ifstream in(currentArg);
            if (!in)
                throw std::runtime_error((std::string("Cannot open: ") + currentArg).c_str());
            std::ostringstream ss;
            ss << in.rdbuf();
            if (!in && !in.eof())
                throw std::runtime_error((std::string("Cannot read IOR from: ") + currentArg).c_str());
            ior = ss.str();
            arg_shifter.consume_arg();

        } else
            arg_shifter.ignore_arg();
    }
}
}

int main(int argc, char *argv[]) {
    try {
        // obtain arguments
        std::string ior;
        GetArgs(argc, argv, ior);

        // initialize the ORB
        CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);

        // obtain an object reference
        CORBA::Object_var obj = orb->string_to_object(ior.c_str());
        Storage_var storage = Storage::_narrow(obj.in());
        if (CORBA::is_nil(storage.in()))
            throw std::runtime_error("IOR was not a Storage object reference");

        {
            // invoke the method
            ::BtData data = { 6, 2, 8, "error message" };
            storage->put(data);
        }

        {
            ::BtData_var data;
            storage->get(data.out());
        }

        // cleanup
        orb->destroy();
    } catch (CORBA::Exception& ex) {
        std::cerr << "CORBA exception: " << ex << std::endl;
        return 1;
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

