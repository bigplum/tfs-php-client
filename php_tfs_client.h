#ifndef PHP_TFS_CLIENT_H
#define PHP_TFS_CLIENT_H

extern zend_module_entry tfs_client_module_entry;
#define phpext_tfs_client_ptr &tfs_client_module_entry

#ifdef PHP_WIN32
#define PHP_TFS_CLIENT_API __declspec(dllexport)
#else
#define PHP_TFS_CLIENT_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(tfs_client);
PHP_MSHUTDOWN_FUNCTION(tfs_client);
PHP_MINFO_FUNCTION(tfs_client);

PHP_FUNCTION(tfs_client);
PHP_FUNCTION(tfs_client_open);
PHP_FUNCTION(tfs_client_close);
PHP_FUNCTION(tfs_client_write);
PHP_FUNCTION(tfs_client_read);
PHP_FUNCTION(confirm_tfs_client_compiled);      /* For testing, remove later. */


#ifdef ZTS
#define TFS_CLIENT_G(v) TSRMG(tfs_client_globals_id, zend_tfs_client_globals *, v)
#else
#define TFS_CLIENT_G(v) (tfs_client_globals.v)
#endif

#endif	/* PHP_TFS_CLIENT_H */

