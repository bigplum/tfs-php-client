PHP Client for TFS(taobao filesystem) without rcserver supporting
-----------------

The original php-tfs-client need rcserver/meta_server, but they are diffcult to
setup for less of documents. And also we don't need the features provided by
rcserver.

So I wrote the new php client by wraping the c++ API:[TfsClient](http://code.taobao.org/p/tfs/wiki/cclient/).

Install
-----------------
1. copy the *.cpp and *.h to tfs/src/phpclient
2. modify the phpize and php-config path in the tfs/src/phpclient/build.sh
3. run build.sh and make
4. copy modules/tfs_client.so to php lib directory and modify php.ini to add
   the module
