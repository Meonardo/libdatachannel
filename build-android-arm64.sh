# use the latest cmake toolset
export PATH=/home/meonardo/cmake-3.30.0-linux-x86_64/bin:$PATH

export PKG_CONFIG_PATH=/home/meonardo/Android/gstreamer/1.22.2/cerbero/build/dist/android_arm64/lib/pkgconfig:$PKG_CONFIG_PATH

# remove old builds
rm -fr ./build

# get current directory
current_dir=$(pwd)
openssl_dir=/home/meonardo/Android/openssl
ndk_dir=/home/meonardo/Android/NDK/android-ndk-r25c
strip="$ndk_dir/toolchains/llvm/prebuilt/linux-x86_64/bin/llvm-strip"
tool_chain_file="$ndk_dir/build/cmake/android.toolchain.cmake"
android_abi="arm64-v8a"
android_platform="android-21"

# config
cmake -B build -DUSE_GNUTLS=0 -DUSE_NICE=0 -DCMAKE_BUILD_TYPE=Release -DOPENSSL_USE_STATIC_LIBS=1 -DOPENSSL_ROOT_DIR=/home/meonardo/Android/openssl -DCMAKE_TOOLCHAIN_FILE=$tool_chain_file -DANDROID_ABI=$android_abi -DANDROID_PLATFORM=$android_platform -DCMAKE_INSTALL_PREFIX=$current_dir/build/install

# preparations
cd ./build
mkdir ./install

# build & install
make -j20
make install

# strip symbols

# Path to the library
lib_path="${current_dir}/build/install/lib/libdatachannel.so"

# Check if the library exists
if [ -f "${lib_path}" ]; then
    # Strip symbols from the library
    ${strip} --strip-unneeded ${lib_path}
    echo "Stripped symbols from ${lib_path}"
else
    echo "Library not found at ${lib_path}"
fi

echo "Copying the library to the destination"
cd install
cp -r ./ /mnt/d/File/Android/MediaCoreAndroid/mediacore/src/main/cpp/deps/libdatachannel