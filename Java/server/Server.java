
// astyle --style=google --indent=spaces --pad-header  --lineend=linux Server.java
//
import java.io.*;

public class Server {
    private static class Args {
        final public String ior_file;

        Args(String ior_file) {
            this.ior_file = ior_file;
        }
    }

    private static Args GetArgs(String[] args) throws FileNotFoundException, IOException {
        String ior_file = "";

        int i=0;
        while (i < args.length) {
            if (args[i].equals("-ior")) {
                ior_file = args[++i];
            }
            ++i;
        }

        return new Args(ior_file);
    }

    // java server required an orb reference because it need orb.create_any()
    // method to build an Any object instead c++ server use a plain
    // new ::CORBA::Any
    private static class StorageImpl extends StoragePOA {
        private org.omg.CORBA.ORB _orb;
        private BtData _data = new BtData();

        public StorageImpl(org.omg.CORBA.ORB orb) {
            _orb = orb;
        }

        public void put (BtData data) {
            System.out.println("StorageImpl.put() requested: " + data.requested + " - min_permitted: " + data.min_permitted + " - max_permitted: " + data.max_permitted + " - error_msg: " + data.error_msg);
            _data = new BtData(data.requested, data.min_permitted, data.max_permitted, data.error_msg);
        }

        public void get (BtDataHolder data) {
            System.out.println("StorageImpl.get() requested: " + _data.requested + " - min_permitted: " + _data.min_permitted + " - max_permitted: " + _data.max_permitted + " - error_msg: " + _data.error_msg);
            data.value = new BtData(_data.requested, _data.min_permitted, _data.max_permitted, _data.error_msg);
        }

        public void put_any (org.omg.CORBA.Any adata) {
            System.out.println("StorageImpl.put_any() requested: ");

            BtData data = BtDataHelper.extract(adata);
            this.put(data);

        }

        public void get_any (org.omg.CORBA.AnyHolder adata) {
            System.out.println("StorageImpl.get_any() requested: ");
            BtDataHolder data = new BtDataHolder();
            this.get(data);

            adata.value = _orb.create_any();
            BtDataHelper.insert(adata.value, data.value);

        }

    }


    public static void main(String[] args) {
        try {
            // obtain arguments
            final Args a = GetArgs(args);

            // initialize the ORB
            org.omg.CORBA.ORB orb = org.omg.CORBA.ORB.init(args, null);

            // get reference to rootpoa & activate the POAManager
            org.omg.PortableServer.POA rootpoa = org.omg.PortableServer.POAHelper.narrow(orb.resolve_initial_references("RootPOA"));
            rootpoa.the_POAManager().activate();

            // create the Storage servan
            org.omg.CORBA.Object o = rootpoa.servant_to_reference(new StorageImpl(orb));

            // write the object reference to a file
            try (PrintWriter pw = new PrintWriter(new FileOutputStream(new File(a.ior_file)))) {
                pw.println(orb.object_to_string(o));
            }

            // accept requests from clients
            orb.run();

            // cleanup
            orb.destroy();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}


