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
            url.port.len = url_meta.port_end - url_meta.port_start;
            url.auth.start = url_meta.auth_start;
            url.auth.len  = url_meta.auth_end - url_meta.auth_start;
        }

        virtual ~NgxParser() {
        }

    protected:
        ngx_http_url_meta url_meta;
};

