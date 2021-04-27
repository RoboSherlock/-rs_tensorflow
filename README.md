# rs_tensorflow

You will need Robosherlock http://robosherlock.org/installation.html
Using this as the library to use tensorflow models: https://github.com/serizba/cppflow/tree/cppflow2

## Prerequisites
Install the TF C API globally: https://serizba.github.io/cppflow/installation.html#install-the-tf-c-api-globally

You can also install the TF C API in a custom directory:
1. https://www.tensorflow.org/install/lang_c download the linux CPU only (not tested with GPU support yet)
2. mkdir -p ~/libtensorflow2/ && tar -C ~/libtensorflow2/ -xzf (downloaded file)
3. sudo ldconfig
4. export LIBRARY_PATH=$LIBRARY_PATH:~/libtensorflow2/lib
5. export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/libtensorflow2/lib

Step 4 & 5 can be also done in the CMakeLists.txt

## Test usage after setup
$ rosrun robosherlock runAAE _ae:=tensor_low_level
