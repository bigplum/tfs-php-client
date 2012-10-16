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
5. excute run.sh to test the client

Usage
----------------
There are only 4 functions now....
    
    /**
    *typedef enum
    *{
    *  T_DEFAULT = 0,
    *  T_READ = 1,
    *  T_WRITE = 2,
    *  T_CREATE = 4,
    *  T_NEWBLK = 8,
    *  T_NOLEASE = 16,
    *  T_STAT = 32,
    *  T_LARGE = 64,
    *  T_UNLINK = 128,
    *  T_FORCE = 256
    *} OpenFlag;
    */

    $tfsobj = new tfs_client("localhost:8108");
    echo "\n-------------------\n";
    $data='XXXXXXXXXXXX';
    $data_len = strlen($data);
    echo "\n------------write data case------------\n";
    $fd=$tfsobj->open(NULL,"",2);
    echo "\n=============fd:$fd\n";
    $ret=$tfsobj->write($fd, $data, $data_len);
    echo "\n=============write ret:$ret\n";
    $filename=$tfsobj->close($fd);
    echo "\n=============close ret:$filename[0], $filename[1]\n";
    
    echo "\n------------read data case------------\n";
    $fd=$tfsobj->open($filename[1], "",1);
    $ary=$tfsobj->read($fd, 20);
    echo "\n=============read data: length: $ary[0], data: $ary[1]\n";
    $tfsobj->close($fd);
    
