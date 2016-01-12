/*
 * Copyright (C) DreamLab Onet.pl Sp. z o. o.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

extern "C" {
    #include <ngx_url_parser.h>
}
#include <Uri.h>

// base structure for url properties
typedef struct {
    const char * start;
    size_t len;
} Element;

typedef struct {
    Element scheme;
    Element host;
    Element path;
    Element query;
    Element fragment;
    Element port;
    Element auth;
} Url;


class Parser {
    public:
        Parser(const char * str): m_str(str) {
            url.scheme.start = NULL;
            url.host.start = NULL;
            url.path.start = NULL;
            url.query.start = NULL;
            url.fragment.start = NULL;
            url.port.start = NULL;
            url.auth.start = NULL;
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
        NgxParser(const char * str): Parser(str), url_meta() {
            if (ngx_url_parser_meta(&url_meta, str) == NGX_URL_OK) {
                status = Parser::OK;
            } else {
                status = Parser::ERROR;
                return;
            }

            url.scheme.start = url_meta.scheme_start;
            url.scheme.len = url_meta.scheme_end - url_meta.scheme_start;
            url.host.start = url_meta.host_start;
            url.host.len = url_meta.host_end - url_meta.host_start;
            url.path.start = url_meta.uri_start;
            url.path.len = url_meta.uri_end - url_meta.uri_start;
            url.query.start = url_meta.args_start;
            url.query.len = url_meta.args_end - url_meta.args_start;
            url.fragment.start = url_meta.fragment_start;
            url.fragment.len = url_meta.fragment_end - url_meta.fragment_start;
            url.port.start = url_meta.port_start;
            url.port.len = url_meta.port_end - url_meta.port_end;
            url.auth.start = url_meta.auth_start;
            url.auth.len  = url_meta.auth_end - url_meta.auth_start;
        }

        virtual ~NgxParser() {
        }

    protected:
        ngx_http_url_meta url_meta;
};

class UriParser: public Parser {
    public:
    UriParser(const char *str): Parser(str), m_uri(), m_state() {

        m_state.uri = &m_uri;
        if (uriParseUriA(&m_state, str) == URI_SUCCESS) {
            status = Parser::OK;
        } else {
            status = Parser::ERROR;
            return;
        }
        if (m_uri.scheme.first) {
            url.scheme.start = m_uri.scheme.first;
            url.scheme.len = calculateLen(m_uri.scheme);
        }

        if (m_uri.userInfo.first) {
            url.auth.start = m_uri.userInfo.first;
            url.auth.len = calculateLen(m_uri.userInfo);
        }

        if (m_uri.hostText.first) {
            url.host.start = m_uri.hostText.first;
            url.host.len = calculateLen(m_uri.hostText);
        }

        if (m_uri.portText.first) {
            url.port.start = m_uri.portText.first;
            url.port.len = calculateLen(m_uri.portText);
        }

        if (m_uri.query.first) {
            url.query.start = m_uri.query.first;
            url.query.len = calculateLen(m_uri.query);
        }

        if (m_uri.fragment.first) {
            url.fragment.start = m_uri.fragment.first;
            url.fragment.len = calculateLen(m_uri.fragment);
        }

        if (m_uri.pathHead && m_uri.pathHead->text.first) {
            int len = 0;

            UriPathSegmentA *pathHead = m_uri.pathHead;
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

            pathHead = m_uri.pathHead;
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
            if (!m_uri.absolutePath && !m_uri.hostText.first) {
                p++;
            }

            int pathLen = std::strlen(p);
            char * pathPtr = new char[pathLen + 1];
            std::strcpy(pathPtr, p);
            url.path.start = pathPtr;
            url.path.len = pathLen;
            delete[] tmp;

        } else {
            char * pathPtr = new char[2];
            pathPtr[0] = '/';
            pathPtr[1] = '\0';
            url.path.start = pathPtr;
            url.path.len = 1;
        }

    }

   virtual ~UriParser() {
        uriFreeUriMembersA(&m_uri);

        if (url.path.start) {
            delete[] url.path.start;
            url.path.start = NULL;
        }
    }

    private:
        UriUriA m_uri;
        UriParserStateA m_state;

        inline size_t calculateLen(UriTextRangeStructA &src) {
            return src.afterLast - src.first;
        }
};
