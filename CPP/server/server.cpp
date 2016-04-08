
// : astyle --style=google --indent=spaces --pad-header  --lineend=linux server.cpp
//
#include <iostream>
#include <fstream>
#include <string>

#include <ace/Arg_Shifter.h>

#include "LeakS.h"

namespace {
class StorageImpl : public virtual POA_Storage {
    ::BtData _data;

  public:
    StorageImpl() {}
    virtual ~StorageImpl() {}

    virtual void put(const ::BtData & data);
    virtual void get(::BtData_out data);

    virtual void put_any(const ::CORBA::Any & adata);
    virtual void get_any(::CORBA::Any_out adata);
};

void StorageImpl::put(const ::BtData & data) {
    std::cout << "StorageImpl::put() requested: " << data.requested << " - min_permitted: " << data.min_permitted << " - max_permitted: " << data.max_permitted << " - error_msg: " << data.error_msg << std::endl;
    _data = data;
}

void StorageImpl::get(::BtData_out data) {
    std::cout << "StorageImpl::get() requested: " << _data.requested << " - min_permitted: " << _data.min_permitted << " - max_permitted: " << _data.max_permitted << " - error_msg: " << _data.error_msg << std::endl;
    data = new ::BtData(_data);
}

void StorageImpl::put_any(const ::CORBA::Any & adata) {
    std::cout << "StorageImpl::put_any() requested: " << std::endl;
    const ::BtData *data;
    adata >>= data; // extract read-only pointer
    this->put(*data);
}

void StorageImpl::get_any(::CORBA::Any_out adata) {
    std::cout << "StorageImpl::get_any() requested: " << std::endl;

    ::BtData *data;
    this->get(data);
    adata = new ::CORBA::Any;
    *adata <<= data;
}

void GetArgs(int argc, char *argv[], std::string &ior_file) {
    ACE_Arg_Shifter arg_shifter(argc, argv);
    while (arg_shifter.is_anything_left()) {
        const ACE_TCHAR *currentArg = 0;
        if ((currentArg = arg_shifter.get_the_parameter(ACE_TEXT("-ior"))) != 0) {
            ior_file = currentArg;
            arg_shifter.consume_arg();
        } else
            arg_shifter.ignore_arg();
    }
}
}

int main(int argc, char *argv[]) {
    try {
        // obtain arguments
        std::string ior_file;
        GetArgs(argc, argv, ior_file);

        // initialize the ORB
        CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);

        // obtain a reference to the RootPOA
        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var poa = PortableServer::POA::_narrow(obj.in());

        // activate the POAManager
        PortableServer::POAManager_var mgr = poa->the_POAManager();
        mgr->activate();

        // create the Storage servant
        StorageImpl servant;
        PortableServer::ObjectId_var oid = poa->activate_object(&servant);

        CORBA::Object_var messenger_obj = poa->id_to_reference(oid.in());

        // write the object reference to a file
        CORBA::String_var ior = orb->object_to_string(messenger_obj.in());
        std::ofstream out(ior_file.c_str());
        out << ior;
        out.close();

        // accept requests from clients
        orb->run();

        // cleanup
        orb->destroy();
    } catch (CORBA::Exception& ex) {
        std::cerr << "CORBA exception: " << ex << std::endl;
        return 1;
    }

    return 0;
}

