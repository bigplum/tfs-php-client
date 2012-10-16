<?php
/**
*@param ns_ip: name server ip
*/

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
$tfsobj = new tfs_client($argv[1]);
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

exit(0);

  echo "\n------------stat case------------\n";
  $st=$tfsobj->stat($fd);
  echo "\nstat: $st[0], $st[1], $st[2], $st[3], $st[4], $st[5], $st[6]\n";
  $tfsobj->close($fd);
  echo "\n------------unlink case------------\n";
  $fd=$tfsobj->open($filename[1],".txt",2);
  $tfsobj->unlink($filename[1], ".txt", 0);
  $st=$tfsobj->stat($fd);
  echo "\nunlink stat: $st[0], $st[1], $st[2], $st[3], $st[4], $st[5], $st[6]\n";
  $tfsobj->close($fd);
?>
