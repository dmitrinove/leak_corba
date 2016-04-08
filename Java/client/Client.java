
// : java -cp .:./Java/client/:./Java/IDL/ Client -ior my.ior
// astyle --style=google --indent=spaces --pad-header  --lineend=linux Client.java
import java.io.*;

public class Client {
    private static class Args {
        final public String ior;

        Args(String ior) {
            this.ior = ior;
        }
    }

    public static Args GetArgs(String[] args) throws FileNotFoundException, IOException {
        String ior = "";

        int i=0;
        while (i < args.length) {
            if (args[i].equals("-ior")) {
                String ior_file = args[++i];
                BufferedReader in = new BufferedReader(new FileReader(ior_file));
                ior = in.readLine();
                in.close();
            }
            ++i;
        }

        return new Args(ior);
    }


    public static void main(String[] args) {
        try {
            // obtain arguments
            final Args a = GetArgs(args);
            String in_val = new String("hello");
            org.omg.CORBA.StringHolder inout_val = new org.omg.CORBA.StringHolder("bye");
            org.omg.CORBA.StringHolder out_val = new org.omg.CORBA.StringHolder();

            // initialize the ORB
            org.omg.CORBA.ORB orb = org.omg.CORBA.ORB.init(args, null);

            // obtain an object reference
            Storage storage = StorageHelper.narrow(orb.string_to_object(a.ior));

            for (int i = 0; i < 100000; i++) {
                for (short h = 0; h < 10000; h++) {
                    {
                        org.omg.CORBA.Any adata = orb.create_any();
                        BtData data = new BtData((short)(6+h), (short)2, (short)8, "error message");
                        BtDataHelper.insert(adata, data);
                        // invoke the method
                        storage.put_any(adata);
                    }

                    {
                        org.omg.CORBA.AnyHolder adata = new org.omg.CORBA.AnyHolder();
                        storage.get_any(adata);
                        BtData data = BtDataHelper.extract(adata.value);
                    }
                }
            }

            // cleanup
            orb.destroy();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}


