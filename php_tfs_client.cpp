extern "C"
{
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
}

#include "php_tfs_client.h"
#include "../new_client/tfs_client_api.h"

using namespace std;
using namespace tfs::client;
using namespace tfs::common;

/* True global resources - no need for thread safety here */
static zend_class_entry *tfs_client_class_entry_ptr;
static TfsClient* gclient;

/* {{{ tfs_client_functions[]
 *
 * Every user visible function must have an entry in tfs_client_functions[].
 */
function_entry tfs_client_functions[] = {
    PHP_FE(confirm_tfs_client_compiled,	NULL)		/* For testing, remove later. */
    {NULL, NULL, NULL}	/* Must be the last line in tfs_client_functions[] */
};
/* }}} */

static zend_function_entry tfs_client_class_functions[] = {
    PHP_FE(tfs_client, NULL)
        PHP_FALIAS(write, tfs_client_write, NULL)
        PHP_FALIAS(open, tfs_client_open, NULL)
        PHP_FALIAS(close, tfs_client_close, NULL)
        PHP_FALIAS(read, tfs_client_read, NULL)
        {NULL, NULL, NULL}
};

/* {{{ tfs_client_module_entry
 */
zend_module_entry tfs_client_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    "tfs_client",
    tfs_client_functions,
    PHP_MINIT(tfs_client),
    PHP_MSHUTDOWN(tfs_client),
    NULL,
    NULL,
    PHP_MINFO(tfs_client),
#if ZEND_MODULE_API_NO >= 20010901
    "$", /* Replace with version number for your extension */
#endif
    STANDARD_MODULE_PROPERTIES
};
/* }}} */

extern "C"
{
#ifdef COMPILE_DL_TFS_CLIENT
    ZEND_GET_MODULE(tfs_client)
#endif
}

/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(tfs_client)
{
    /*** Defines TFS Class ***/
    static zend_class_entry tfs_client_class_entry;
    INIT_CLASS_ENTRY( tfs_client_class_entry, "tfs_client", tfs_client_class_functions );
    tfs_client_class_entry_ptr = zend_register_internal_class( &tfs_client_class_entry TSRMLS_CC);
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(tfs_client)
{
    if (NULL != gclient)
    {
        //delete gclient;
    }
    return SUCCESS;
}
/* }}} */


/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(tfs_client)
{
    php_info_print_table_start();
    php_info_print_table_row(2, "tfs_client support without rcs", "enabled");
    php_info_print_table_row(2, "Version", "2.0.0");
    php_info_print_table_end();
}
/* }}} */

/* {{{ $tfsclient = new tfs_client(const char* ns_ip)
 *
 * Constructor for tfs_client class
 */
PHP_FUNCTION(tfs_client)
{
    char* ns_ip  = NULL;
    char* level = NULL;
    long ns_ip_length = 0;
    long level_length = 0;
    int32_t ret = zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
            &ns_ip, &ns_ip_length, &level, &level_length);
    if (FAILURE == ret)
    {
        php_error(E_WARNING, "tfs_client: can't parse parameters");
    }
    if (SUCCESS == ret)
    {
        ret = NULL == ns_ip || ns_ip_length <= 0 ? FAILURE : SUCCESS;
        if (SUCCESS != ret)
        {
            php_error(E_WARNING, "tfs_client: parameters is invalid");
        }
    }
    if (SUCCESS == ret)
    {
        gclient = TfsClient::Instance();
        ret = gclient->initialize(ns_ip);
        if (TFS_SUCCESS != ret)
        {
            php_error(E_WARNING, "tfs_client: initialize failed, ret: %d", ret);
        }
        if (level && strcmp(level,"INFO") && strcmp(level, "DEBUG")
                 && strcmp(level,"ERROR"))
        {
            php_error(E_WARNING, "tfs_client: parameters is invalid");
        } else 
        {
	    gclient->set_log_level(level);
        }
        ret = ret == TFS_SUCCESS ? SUCCESS : FAILURE;
    }
    if (SUCCESS != ret)
        RETURN_NULL();
}
/* }}} */

/* {{{ int $tfs_client->open(const char* filename, const char* suffix, int mode, bool large, const char* local_key)
 *  open a file via tfs_client
 */
PHP_FUNCTION(tfs_client_open)
{
    char* filename = NULL;
    char* suffix   = NULL;
    long file_name_length = 0;
    long suffix_length = 0;
    long mode = -1;
    long fd = -1;

    int32_t ret = zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s!s!l",
            &filename, &file_name_length, &suffix, &suffix_length , &mode);
    if (FAILURE == ret)
    {
        php_error(E_WARNING, "tfs_client: can't parse parameters");
    }
    if (SUCCESS == ret)
    {
        ret = mode < 0 ?  FAILURE : SUCCESS;
        if (SUCCESS != ret)
        {
            php_error(E_WARNING, "tfs_client: parameters is invalid");
        }
    }
    if (SUCCESS == ret)
    {
        fd = gclient->open(filename, suffix, mode);
        ret = fd > 0 ? SUCCESS : FAILURE;
        if (SUCCESS != ret)
        {
            php_error(E_ERROR, "tfs_client: open file %s failed, ret: %ld", filename, fd);
        }
    }
    RETURN_LONG(fd);
}
/* }}} */

/* {{{ int $tfs_client->close(const int fd, char* ret_tfs_file_name, const ret_tfs_file_name_length)
 * close tfs file via tfs_client
 */
PHP_FUNCTION(tfs_client_close)
{
    long fd = -1;
    int32_t ret = zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l",
            &fd);
    if (FAILURE == ret)
    {
        php_error(E_WARNING, "tfs_client: can't parse parameters");
    }
    if (SUCCESS == ret)
    {
        ret = fd <= 0 ?  FAILURE : SUCCESS;
        if (SUCCESS != ret)
        {
            php_error(E_WARNING, "tfs_client: parameters is invalid");
        }
    }
    array_init(return_value);
    if (SUCCESS == ret)
    {
        char name[MAX_FILE_NAME_LEN];
        ret = gclient->close(fd, name, MAX_FILE_NAME_LEN);
        if (TFS_SUCCESS != ret)
        {
            php_error(E_ERROR, "tfs_client: close file failed, fd: %ld, ret: %d", fd, ret);
        }
        else
        {
            add_next_index_long(return_value, strlen(name));
            add_next_index_string(return_value, name, 1);
        }
    }
    if (SUCCESS != ret)
    {
        add_next_index_long(return_value, 0);
    }
}
/* }}} */

/* {{{ int $tfs_client->write(const int fd, const char* data, const int length)
 * write data to tfs via tfs_client
 */
PHP_FUNCTION(tfs_client_write)
{
    char* data = NULL;
    long data_length = 0;
    long length = -1;
    long fd = -1;
    long ret_length = -1;
    int32_t ret = zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lsl",
            &fd, &data, &data_length, &length);
    if (FAILURE == ret)
    {
        php_error(E_WARNING, "tfs_client: can't parse parameters");
    }
    if (SUCCESS == ret)
    {
        ret = fd <= 0 || NULL == data || length <= 0 ?  FAILURE : SUCCESS;
        if (SUCCESS != ret)
        {
            php_error(E_WARNING, "tfs_client: parameters is invalid");
        }
    }
    if (SUCCESS == ret)
    {
        ret_length = gclient->write(fd, data, length);
        ret = ret_length <= 0 ? FAILURE : SUCCESS;
        if (ret_length <= 0)
        {
            php_error(E_ERROR, "tfs_client: write data failed, fd: %ld, ret: %ld", fd, ret_length);
        }
    }
    RETURN_LONG(ret_length);
}
/* }}} */

/* {{{ int $tfs_client->read(const int fd);
 * read data form tfs
 */
PHP_FUNCTION(tfs_client_read)
{
    long fd = -1;
    long count = 0;
    int32_t ret = zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &fd, &count);
    if (FAILURE == ret)
    {
        php_error(E_WARNING, "tfs_client: can't parse parameters");
    }
    if (SUCCESS == ret)
    {
        ret = fd <= 0  || count <= 0 ?  FAILURE : SUCCESS;
        if (SUCCESS != ret)
        {
            php_error(E_WARNING, "tfs_client: parameters is invalid");
        }
    }
    long offset = -1;
    array_init(return_value);
    if (SUCCESS == ret)
    {
        char* data = (char*)emalloc(count);
        ret = NULL == data ? FAILURE : SUCCESS;
        if (SUCCESS == ret)
        {
            offset = gclient->read(fd, data, count);
            ret = offset > 0 ? SUCCESS : FAILURE;
            if (SUCCESS != ret)
            {
                php_error(E_WARNING, "read data failed, fd: %ld, count: %ld", fd, offset);
            }
            else
            {
                add_next_index_long(return_value, offset);
                add_next_index_stringl(return_value, data, offset, 1);
            }
            efree(data);
            data = NULL;
        }
    }
    if (SUCCESS != ret)
    {
        add_next_index_long(return_value, offset);
    }
}
/* }}} */


/* Remove the following function when you have succesfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_tfs_client_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_tfs_client_compiled)
{
    char *arg = NULL;
    int arg_len, len;
    char string[256];

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
        return;
    }

    len = sprintf(string, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "tfs_client", arg);
    RETURN_STRINGL(string, len, 1);
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and
   unfold functions in source code. See the corresponding marks just before
   function definition, where the functions purpose is also documented. Please
   follow this convention for the convenience of others editing your code.
 */
