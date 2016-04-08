# leak_corba

source the ACE/TAO
    export ACE_ROOT=<full path>/ACE_wrappers && export TAO_ROOT=${ACE_ROOT}/TAO && export MPC_ROOT=${ACE_ROOT}/MPC && export LD_LIBRARY_PATH=${ACE_ROOT}/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH} && export PATH=${ACE_ROOT}/bin:${PATH}

generate CPP build files and executables
    mwc.pl -type gnuace -static CPP.mwc
    gmake -f GNUmakefile

generate Java build files and executables
    mwc.pl -type make Java.mwc
    gmake -f Makefile

run CPP server and client
    ./CPP/server/server -ior <ior file name>
    ./CPP/client/client -ior <ior file name>

run Java server and client
    java -cp .:./Java/server/:./Java/IDL/ Server -ior <ior file name>
    java -cp .:./Java/client/:./Java/IDL/ Client -ior <ior file name>

