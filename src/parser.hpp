extern "C" {
    #include <ngx_url_parser.h>
}
#include <Uri.h>

// base structure for url properties
typedef struct {
    char * schema = NULL;
    char * host = NULL;
    char * path = NULL;
    char * query = NULL;
    char * fragment = NULL;
    char * port = NULL;
    char * auth = NULL;
} Url;


class Parser {
    public:
        Parser(const char * str): m_str(str) {
        }

        virtual ~Parser() {
        }

        enum Status {
            ERROR = -1,
            OK,
        };

        Status status;
        Url url;
    protected:
        const char * m_str;
};

class NgxParser: public Parser {
    public:
        NgxParser(const char * str): Parser(str), ngx_url() {
            if (ngx_url_parser(&ngx_url, str) == NGX_URL_OK) {
                status = Parser::OK;
            } else {
                status = Parser::ERROR;
            }

            url.schema = ngx_url.schema;
            url.host = ngx_url.host;
            url.path = ngx_url.path;
            url.query = ngx_url.query;
            url.fragment = ngx_url.fragment;
            url.port = ngx_url.port;
            url.auth   = ngx_url.auth;
        }

        virtual ~NgxParser() {
            ngx_url_free(&ngx_url);
        }

    protected:
        ngx_http_url ngx_url;
};

class RfcParser: public Parser {
    public:
    RfcParser(const char *str): Parser(str) {
        UriParserStateA state;
        UriUriA uri;

        state.uri = &uri;
        if (uriParseUriA(&state, str) == URI_SUCCESS) {
            status = Parser::OK;
        } else {
            status = Parser::ERROR;
        }

        if (uri.scheme.first) {
            copyFromMeta(&url.schema, uri.scheme);
        }

        if (uri.userInfo.first) {
            copyFromMeta(&url.auth, uri.userInfo);
        }

        if (uri.hostText.first) {
            copyFromMeta(&url.host, uri.hostText);
        }

        if (uri.portText.first) {
            copyFromMeta(&url.port, uri.portText);
        }

        if (uri.query.first) {
            copyFromMeta(&url.query, uri.query);
        }

        if (uri.fragment.first) {
            copyFromMeta(&url.fragment, uri.fragment);
        }

        if (uri.pathHead && uri.pathHead->text.first) {
            int len = 0;

            UriPathSegmentA *pathHead = uri.pathHead;
            do {
                if (pathHead->text.first == pathHead->text.afterLast) {
                    len++;
                } else {
                    //len +1 because we need to add slash
                    len += pathHead->text.afterLast - pathHead->text.first + 1;
                }
                pathHead = pathHead->next;
            } while (pathHead);

            char *path, *p, *tmp;
            path = p = tmp = new char[len + 1];
            path[len] = '\0';

            pathHead = uri.pathHead;
            do {
                // adding slash no matter what
                *path = '/';
                path++;
                // copying text only in case when path segment is not empty
                if (pathHead->text.first != pathHead->text.afterLast) {
                    int localLen = pathHead->text.afterLast - pathHead->text.first;
                    memcpy(path, pathHead->text.first, localLen);
                    path += localLen;
                }
                pathHead = pathHead->next;
            } while (pathHead);

            // path is always prefixed with / in case of relative url we move pointer further
            if (!uri.absolutePath && !uri.hostText.first) {
                p++;
            }

            int pathLen = std::strlen(p);
            url.path = new char[pathLen];
            std::strcpy(url.path, p);
            delete[] tmp;

        } else {
            url.path = new char[2];
            url.path[0] = '/';
            url.path[1] = '\0';
        }

        uriFreeUriMembersA(&uri);
    }

   virtual ~RfcParser() {
        if (url.schema != NULL) {
            delete[] url.schema;
            url.schema = NULL;
        }

        if (url.host != NULL) {
            delete[] url.host;
            url.host = NULL;
        }

        if (url.path != NULL) {
            delete url.path;
            url.path = NULL;
        }

        if (url.query != NULL) {
            delete[] url.query;
            url.query= NULL;
        }

        if (url.fragment != NULL) {
            delete[] url.fragment;
            url.fragment = NULL;
        }

        if (url.port != NULL) {
            delete[] url.port;
            url.port = NULL;
        }

        if (url.auth != NULL) {
            delete[] url.auth;
            url.auth = NULL;
        }
    }

    private:
        void copyFromMeta(char ** dest, UriTextRangeStructA &src) {
            uint32_t len = src.afterLast - src.first + 1;
            *dest = new char[len];
            std::memcpy(*dest, src.first, len - 1);
            (*dest)[len - 1] = '\0';
        }
};
