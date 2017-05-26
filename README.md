------------------------------------------------------------------------
AGL-Unicens binding expose Unicens-V2 APIs through AGL framework
------------------------------------------------------------------------


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

    OpenSuse >=42.2 
      zypper in gcc5 gdb gcc5-c++ git make ElectricFence systemd-devel libopenssl-devel  libuuid-devel alsa-devel libgcrypt-devel libgnutls-devel libjson-c-devel file-devel 

    Ubuntu >= 16.4 libuuid-devel
      apt-get install cmake git electric-fence libsystemd-dev libssl-dev uuid-dev libasound2-dev libgcrypt20-dev libgnutls-dev libgnutls-dev libjson-c-dev libmagic-dev

    libmicrohttpd>=0.9.54 (as today OpenSuse-42.2 or Ubuntu-.16.4 ship older versions)
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
    wget https://ftp.gnu.org/gnu/libmicrohttpd/libmicrohttpd-0.9.54.tar.gz
    tar -xzf libmicrohttpd-0.9.54.tar.gz
    cd libmicrohttpd-0.9.54
    ./configure --prefix=$INSTALL_PREFIX
    make
    make install-strip

    # retrieve last AFB_daemon from AGL
    git clone https://gerrit.automotivelinux.org/gerrit/src/app-framework-binder

    # Warning: previous GCC options should be set before initial cmake (clean Build/*)
    cd app-framework-binder; mkdir build; cd build 
    cmake -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX ..
    make
    make install 
```


```
# Compile binding

```
source ~/.bashrc  # or any other file where your have place your compilation preferences
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX ..
make

    afb-daemon --ldpaths=. --port=1234 --workdir=. --roothttp=./htdocs --token="" --verbose

    speaker-test -twav -D hw:ep01 -c 2
    firefox http://localhost:1234
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


