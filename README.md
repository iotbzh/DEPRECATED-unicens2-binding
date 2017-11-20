------------------------------------------------------------------------
AGL-UNICENS binding expose UNICENS-V2 APIs through AGL framework
------------------------------------------------------------------------

Cloning Audio-Binding from Git
-------------------------------------------------------

```
git clone --recurse-submodules https://github.com/iotbzh/unicens2-binding
cd  unicens2-binding
```

AFB_daemon dependency on Standard Linux Distributions
-------------------------------------------------------
    # handle dependencies > (OpenSuse-42.2, Fedora-25, Ubuntu 16.04.2LTS)
    gcc > 4.8
    systemd-devel (libsystemd-dev>=222)
    libuuid-devel
    file-devel(OpenSuSe) or libmagic-dev(Ubuntu)
    libjson-c-devel
    alsa-devel
    ElectricFence (BUG should not be mandatory)
    libopenssl-devel libgcrypt-devel libgnutls-devel (optional but requested by libmicrohttpd for https)
    libxml2-devel

    OpenSuse >=42.2
      zypper in gcc5 gdb gcc5-c++ git cmake make ElectricFence systemd-devel libopenssl-devel  libuuid-devel alsa-devel libgcrypt-devel libgnutls-devel libjson-c-devel file-devel libxml2-devel

    Ubuntu >= 16.4 libuuid-devel
      apt-get install cmake git electric-fence libsystemd-dev libssl-dev uuid-dev libasound2-dev libgcrypt20-dev libgnutls-dev libgnutls-dev libjson-c-dev libmagic-dev libxml2-dev

    libmicrohttpd>=0.9.55 (as today OpenSuse-42.2 or Ubuntu-.16.4 ship older versions)
    afb-daemon from AGL Gerrit git clone https://gerrit.automotivelinux.org/gerrit/src/app-framework-binder

```
    # Might want to add following variables into ~/.bashrc
    echo "#----------  AGL options Start ---------" >>~/.bashrc
    echo "# Object: AGL cmake option for  binder/bindings" >>~/.bashrc
    echo "# Date: `date`" >>~/.bashrc
    echo 'export CC=gcc-5; export CXX=g++-5' >>~/.bashrc   # if using gcc5
    echo 'export INSTALL_PREFIX=$HOME/opt' >>~/.bashrc
    echo 'export LD_LIBRARY_PATH=$INSTALL_PREFIX/lib64:$INSTALL_PREFIX/lib' >>~/.bashrc
    echo 'export LIBRARY_PATH=$INSTALL_PREFIX/lib64:$INSTALL_PREFIX/lib' >>~/.bashrc
    echo 'export PKG_CONFIG_PATH=$INSTALL_PREFIX/lib64/pkgconfig:$INSTALL_PREFIX/lib/pkgconfig' >>~/.bashrc
    echo 'export PATH=$INSTALL_PREFIX/bin:$PATH' >>~/.bashrc
    echo 'export RSYNC_TARGET=MY_TARGET_HOSTNAME' >>~/.bashrc
    echo 'export RSYNC_PREFIX=./opt' >>~/.bashrc

    echo "#----------  AGL options End ---------" >>~/.bashrc
    source ~/.bashrc

    # install LibMicroHttpd
    LIB_MH_VERSION=0.9.55
    wget https://ftp.gnu.org/gnu/libmicrohttpd/libmicrohttpd-${LIB_MH_VERSION}.tar.gz
    tar -xzf libmicrohttpd-${LIB_MH_VERSION}.tar.gz
    cd libmicrohttpd-${LIB_MH_VERSION}
    ./configure --prefix=${INSTALL_PREFIX}
    make
    sudo make install-strip

    # retrieve last AFB_daemon from AGL
    git clone https://gerrit.automotivelinux.org/gerrit/src/app-framework-binder

    # Warning: previous GCC options should be set before initial cmake (clean Build/*)
    cd app-framework-binder; mkdir -p build; cd build
    cmake -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX ..
    make
    make install
```


# Compile binding

```
    source ~/.bashrc  # or any other file where your have place your compilation preferences
    mkdir -p build
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX ..
    make

    afb-daemon --workdir=.. --ldpaths=build --port=1234  --roothttp=./htdocs --token="" --verbose

    speaker-test -twav -D hw:ep01 -c 2
    firefox http://localhost:1234
```

# Archive

```
VERSION=0.1
GIT_TAG=master
PKG_NAME=UNICENS-agent
git archive --format=tar.gz --prefix=${PKG_NAME}-${VERSION}/ ${GIT_TAG} -o ${PKG_NAME}_${VERSION}.orig.tar.gz
```

# Local Source Debug with GDB

Warning: technically AGL bindings are shared libraries loaded thought 'dlopen'. GDB supports source debug of dynamically
loaded libraries, but user should be warn that the actual path to sharelib symbols is directly inherited from DLOPEN.
As a result if you change your directory after binder start with --workdir=xxx then GDB will not find symbols anymore

```
    Examples:

    # WORK when running in direct
    afb-daemon --workdir=.. --ldpaths=build --port=1234 --roothttp=./htdocs

    # FAIL when using GDB with warning: Could not load shared library ....
    gdb -args afb-daemon --workdir=.. --ldpaths=build --port=1234 --roothttp=./htdocs
    ...
    warning: Could not load shared library symbols for ./build/ucs2-afb/afb-ucs2.so.
    Do you need "set solib-search-path" or "set sysroot"?
    ...
```

To debug sharelib symbol path: start your binder under GDB. Then break your session after the binder has
loaded its bindings. Finally use "info sharedlibrary" and check 'SymsRead'. If equal to 'No' then either you start GDB
from the wrong relative directory, either you have to use 'set solib-search-path' to force the path.

```
    gdb -args afb-daemon --workdir=.. --ldpaths=build --port=1234 --roothttp=./htdocs
    run
        ...
        NOTICE: API UNICENS added
        NOTICE: Waiting port=1234 rootdir=.
        NOTICE: Browser URL= http://localhost:1234
    (hit Ctrl-C to break the execution)
    info sharedlibrary afb-*
```

# Running an debugging on a target

```
export RSYNC_TARGET=root@xx.xx.xx.xx
export RSYNC_PREFIX=/opt    # WARNING: installation directory should exist

mkdir $RSYNC_TARGET
cd    $RSYNC_TARGET

cmake -DRSYNC_TARGET=$RSYNC_TARGET -DRSYNC_PREFIX=$RSYNC_PREFIX ..
make remote-target-populate

    ./build/target/start-${RSYNC_TARGET}.sh
    firefox http://localhost:1234    # WARNING: do not forget firewall if any

    ssh -tt ${RSYNC_TARGET} speaker-test -twav -D hw:ep01 -c 2
```

Note: remote-target-populate will
 - create a script to remotely start the binder on the target in ./build/target/start-on-target-name.sh
 - create a gdbinit file to transparently debug remotely in source code with gdb -x ./build/target/gdb-on-target-name.ini
 - to run and debug directly from your IDE just configure the run and debug properties with the corresponding filename

Note that Netbeans impose to set debug directory to ./build/pkgout or it won't find binding symbols for source debugging

# Default Volume of Amplifiers
The binding currently supports two use cases for amplifiers.
1. Amplifiers are initialized with a default volume. The head unit uses software volume 
   to change the volume of streaming data.
2. Amplifiers are initialized muted. The head unit uses hardware volume (e.g. HAL-MOST-UNICENS)
   to change the volume of amplifiers remotely.

Use case 2 is the default use case. If you like to use this binding without hardware volume support
please adopt the ```config_multichannel_audio_kit.xml``` as explained below.

```
<!-- Register 7: Master Volume (Max Volume=07 00 00 and Min Volume=07 03 FF) -->
<!--   - together with HAL-MOST-UNICENS binding use "07 03 FF" = muted after start -->
<!--   - otherwise use "07 01 50" = default volume -->
<I2CPortWrite Address="0x2A" Payload="07 03 FF"/>
```
