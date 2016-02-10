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

#include <v8.h>
#include <node.h>
#include <map>
#include <list>
#include <string>
#include <cstring>
#include <vector>

#include "parsers.hpp"

#define ENCODED_BRACKETS "%5B%5D"
#define BRACKETS "[]"

enum parseOptions {
    kProtocol = 1,
    kAuth = 1 << 1,
    kHost = 1 << 2,
    kPort = 1 << 3,
    kQuery = 1 << 4,
    kFragment = 1 << 5,
    kPath = 1 << 6,
    kAll = kProtocol | kAuth | kHost | kPort | kQuery | kFragment | kPath
};

enum Engines {
    eUriParser = 1,
    eNgxParser
};

static v8::Persistent<v8::String> protocol_symbol = NODE_PSYMBOL("protocol");
static v8::Persistent<v8::String> auth_symbol = NODE_PSYMBOL("auth");
static v8::Persistent<v8::String> host_symbol = NODE_PSYMBOL("host");
static v8::Persistent<v8::String> port_symbol = NODE_PSYMBOL("port");
static v8::Persistent<v8::String> query_symbol = NODE_PSYMBOL("query");
static v8::Persistent<v8::String> query_arr_suffix = NODE_PSYMBOL("queryArraySuffix");
static v8::Persistent<v8::String> fragment_symbol = NODE_PSYMBOL("fragment");
static v8::Persistent<v8::String> path_symbol = NODE_PSYMBOL("path");
static v8::Persistent<v8::String> user_symbol = NODE_PSYMBOL("user");
static v8::Persistent<v8::String> password_symbol = NODE_PSYMBOL("password");

static v8::Handle<v8::Value> parse(const v8::Arguments& args){
    v8::HandleScope scope;
    parseOptions opts = kAll;
    Engines engine = eNgxParser;

    if (args.Length() == 0 || !args[0]->IsString()) {
        return v8::ThrowException(v8::Exception::TypeError(v8::String::New("First argument has to be string")));
    }

    if (args[1]->IsNumber()) {
        opts = static_cast<parseOptions>(args[1]->Int32Value());
    }

    v8::String::Utf8Value url(args[0]->ToString());

    if (url.length() == 0) {
        return v8::ThrowException(v8::Exception::TypeError(v8::String::New("String mustn't be empty")));
    }

    if (args[2]->IsNumber()) {
        engine = static_cast<Engines>(args[2]->Int32Value());
    }

    v8::PropertyAttribute attrib = (v8::PropertyAttribute) (v8::ReadOnly | v8::DontDelete);
    v8::Local<v8::Object> data = v8::Object::New();

    Url uri;
    Parser *parser;

    if (engine == eUriParser) {
        parser = new UriParser(*url);
    } else {
        parser = new NgxParser(*url);
    }

    if (parser->status != Parser::OK) {
        return v8::ThrowException(v8::Exception::Error(v8::String::New("Unable to parse given url")));
    }
    uri = parser->url;

    if (uri.scheme.start && (opts & kProtocol)) {
        // +1 becasue we need ":" in scheme/protocol
        data->Set(protocol_symbol, v8::String::New(uri.scheme.start, uri.scheme.len + 1), attrib);
    }

    if (uri.auth.start && (opts & kAuth)) {
        const char *delim = ":";
        char *authPtr, *authUser, *authPassword;

        char * auth = new char[uri.auth.len + 1];
        std::strncpy(auth, uri.auth.start, uri.auth.len);
        auth[uri.auth.len] = '\0';

        authUser = strtok_r(auth, delim, &authPtr);
        authPassword = strtok_r(NULL, delim, &authPtr);

        if (authUser != NULL && authPassword != NULL) {
            v8::Local<v8::Object> authData = v8::Object::New();
            authData->Set(user_symbol, v8::String::New(authUser), attrib);
            authData->Set(password_symbol, v8::String::New(authPassword), attrib);

            data->Set(auth_symbol, authData, attrib);
        }

        delete[] auth;
    }

    if (uri.host.start && (opts & kHost)) {
        data->Set(host_symbol, v8::String::New(uri.host.start, uri.host.len), attrib);
    }

    if (uri.port.start && (opts & kPort)) {
        data->Set(port_symbol, v8::String::New(uri.port.start, uri.port.len), attrib);
    }

    if (uri.query.start && (opts & kQuery)) {
        std::map<std::string, std::list<const char *> > paramsMap;
        std::vector<std::string> paramsOrder;
        char *query = new char[uri.query.len + 1];
        std::strncpy(query, uri.query.start, uri.query.len);
        query[uri.query.len] = '\0';

        const char *amp = "&", *sum = "=", *semicolon = ";", *delimeter = amp;
        char *queryParamPairPtr, *queryParam, *queryParamKey, *queryParamValue, *queryParamPtr;
        bool empty = true;
        v8::Local<v8::Object> qsSuffix = v8::Object::New();

        // find qs delimeter & or ;
        for (size_t i = 0; i < uri.query.len; ++i) {
            if (query[i] == *amp) {
                delimeter = amp;
                break;
            } else if (query[i] == *semicolon) {
                delimeter = semicolon;
                break;
            }
        }

        queryParam = strtok_r(query, delimeter, &queryParamPairPtr);

        v8::Local<v8::Object> queryData = v8::Object::New();
        bool arrayBrackets = false;
        while (queryParam) {
            if (*queryParam != *sum) {
                size_t len;
                empty = false;
                queryParamKey = strtok_r(queryParam, sum, &queryParamPtr);
                len = strlen(queryParamKey);
                if (len > (sizeof(ENCODED_BRACKETS) - 1) &&
                        !strncmp(queryParamKey + len - (sizeof(ENCODED_BRACKETS) - 1),
                                 ENCODED_BRACKETS,
                                 sizeof(ENCODED_BRACKETS) - 1)) {
                    arrayBrackets = true;
                    queryParamKey[len - (sizeof(ENCODED_BRACKETS) - 1)] = '\0';
                    qsSuffix->Set(v8::String::New(queryParamKey), v8::String::New(ENCODED_BRACKETS));
                } else if (len > (sizeof(BRACKETS) - 1) &&
                        !strncmp(queryParamKey + len - (sizeof(BRACKETS) - 1),
                                 BRACKETS,
                                 sizeof(BRACKETS) - 1)) {
                    arrayBrackets = true;
                    queryParamKey[len - (sizeof(BRACKETS) - 1)] = '\0';
                    qsSuffix->Set(v8::String::New(queryParamKey), v8::String::New(BRACKETS));
                }

                queryParamValue = strtok_r(NULL, sum, &queryParamPtr);
                if (paramsMap.find(queryParamKey) == paramsMap.end()) {
                    paramsOrder.push_back(queryParamKey);
                }
                paramsMap[queryParamKey].push_back(queryParamValue ? queryParamValue : "");
            }
            queryParam = strtok_r(NULL, delimeter, &queryParamPairPtr);
        }

        for (std::vector<std::string>::iterator it=paramsOrder.begin(); it!=paramsOrder.end(); ++it) {
            v8::Local<v8::String> key = v8::String::New(it->c_str());
            std::list<const char *> vals = paramsMap[*it];
            int arrSize = vals.size();
            if (arrSize > 1 || qsSuffix->Has(key)) {
                v8::Local<v8::Array> arrVal = v8::Array::New(arrSize);

                int i = 0;
                for (std::list<const char *>::iterator it2 = vals.begin(); it2 != vals.end(); ++it2) {
                    arrVal->Set(i, v8::String::New(*it2));
                    i++;
                }
                queryData->Set(key, arrVal);
            } else {
                queryData->Set(key, v8::String::New(vals.front()));
            }
        }

        //no need for empty object if the query string is going to be wrong
        if (!empty) {
            data->Set(query_symbol, queryData, attrib);
            if (arrayBrackets) {
                data->Set(query_arr_suffix, qsSuffix, attrib);
            }
        }

        delete[] query;
    }

    if (uri.fragment.start && (opts & kFragment)) {
        data->Set(fragment_symbol, v8::String::New(uri.fragment.start, uri.fragment.len), attrib);
    }

    if (opts & kPath) {
        if (uri.path.start) {
            data->Set(path_symbol, v8::String::New(uri.path.start, uri.path.len), attrib);
        } else {
            data->Set(path_symbol, v8::String::New("/"), attrib);
        }
    }

    delete parser;

    return scope.Close(data);
}


extern "C" void init (v8::Handle<v8::Object> target){
    v8::HandleScope scope;

    NODE_SET_METHOD(target, "parse", parse);

    // Old properties
    NODE_DEFINE_CONSTANT(target, kProtocol);
    NODE_DEFINE_CONSTANT(target, kAuth);
    NODE_DEFINE_CONSTANT(target, kHost);
    NODE_DEFINE_CONSTANT(target, kPort);
    NODE_DEFINE_CONSTANT(target, kQuery);
    NODE_DEFINE_CONSTANT(target, kFragment);
    NODE_DEFINE_CONSTANT(target, kPath);
    NODE_DEFINE_CONSTANT(target, kAll);

    v8::Handle<v8::Object> uri = v8::Object::New();
    uri->Set(v8::String::New("PROTOCOL"), v8::Integer::New(kProtocol));
    uri->Set(v8::String::New("AUTH"), v8::Integer::New(kAuth));
    uri->Set(v8::String::New("HOST"), v8::Integer::New(kHost));
    uri->Set(v8::String::New("PORT"), v8::Integer::New(kPort));
    uri->Set(v8::String::New("QUERY"), v8::Integer::New(kQuery));
    uri->Set(v8::String::New("FRAGMENT"), v8::Integer::New(kFragment));
    uri->Set(v8::String::New("ALL"), v8::Integer::New(kAll));
    target->Set(v8::String::New("Uri"), uri);

    v8::Handle<v8::Object> engines = v8::Object::New();
    engines->Set(v8::String::New("URIPARSER"), v8::Integer::New(eUriParser));
    engines->Set(v8::String::New("NGINX"), v8::Integer::New(eNgxParser));
    target->Set(v8::String::New("Engines"), engines);

}

NODE_MODULE(uriparser, init)
