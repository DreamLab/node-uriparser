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

#include <map>
#include <cstring>
#include <vector>
#include <map>
#include <nan.h>
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

#define URI_LOCAL_STR(name) Nan::New<v8::String>(name).ToLocalChecked()

static Nan::Persistent<v8::String> protocol_symbol(URI_LOCAL_STR("protocol"));
static Nan::Persistent<v8::String> auth_symbol(URI_LOCAL_STR("auth"));
static Nan::Persistent<v8::String> host_symbol(URI_LOCAL_STR("host"));
static Nan::Persistent<v8::String> port_symbol(URI_LOCAL_STR("port"));
static Nan::Persistent<v8::String> query_symbol(URI_LOCAL_STR("query"));
static Nan::Persistent<v8::String> queryArraySuffix_symbol(URI_LOCAL_STR("queryArraySuffix"));
static Nan::Persistent<v8::String> querySeparator_symbol(URI_LOCAL_STR("querySeparator"));
static Nan::Persistent<v8::String> fragment_symbol(URI_LOCAL_STR("fragment"));
static Nan::Persistent<v8::String> path_symbol(URI_LOCAL_STR("path"));
static Nan::Persistent<v8::String> user_symbol(URI_LOCAL_STR("user"));
static Nan::Persistent<v8::String> search_symbol(URI_LOCAL_STR("search"));
static Nan::Persistent<v8::String> password_symbol(URI_LOCAL_STR("password"));


NAN_METHOD(parse) {
    parseOptions opts = kAll;

    if (info.Length() == 0 || !info[0]->IsString()) {
        return Nan::ThrowError("First argument has to be string");
    }

    if (info[1]->IsNumber()) {
        opts = static_cast<parseOptions>(info[1]->Int32Value());
    }

    Nan::Utf8String  url(info[0]->ToString());

    if (url.length() == 0) {
        Nan::ThrowTypeError("String mustn't be empty");
        return;
    }

    v8::PropertyAttribute attrib = (v8::PropertyAttribute) (v8::ReadOnly | v8::DontDelete);
    v8::Local<v8::Object> data = Nan::New<v8::Object>();

    Url uri;
    NgxParser *parser;
    parser = new NgxParser(*url);

    if (parser->status != Parser::OK) {
        Nan::ThrowError("Unable to parse given url");
        return;
    }
    uri = parser->url;

    if (uri.scheme.start && (opts & kProtocol)) {
        // +1 because we need ":" in scheme/protocol
        Nan::DefineOwnProperty(data, Nan::New<v8::String>(protocol_symbol), Nan::New<v8::String>(uri.scheme.start, uri.scheme.len + 1).ToLocalChecked(), attrib);
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
            v8::Local<v8::Object> authData = Nan::New<v8::Object>();
            Nan::DefineOwnProperty(authData, Nan::New(user_symbol), URI_LOCAL_STR(authUser), attrib);
            Nan::DefineOwnProperty(authData, Nan::New(password_symbol), URI_LOCAL_STR(authPassword), attrib);

            Nan::DefineOwnProperty(data, Nan::New(auth_symbol), authData, attrib);
        }

        delete[] auth;
    }

    if (uri.host.start && (opts & kHost)) {
        Nan::DefineOwnProperty(data, Nan::New(host_symbol), Nan::New<v8::String>(uri.host.start, uri.host.len).ToLocalChecked(), attrib);
    }

    if (uri.port.start && (opts & kPort)) {
        Nan::DefineOwnProperty(data, Nan::New(port_symbol), Nan::New<v8::String>(uri.port.start, uri.port.len).ToLocalChecked(), attrib);
    }

    if (uri.query.start && (opts & kQuery)) {
        std::map<std::string, std::vector<const char *> > paramsMap;
        std::vector<std::string> paramsOrder;
        paramsOrder.reserve(uri.query.len / 2);
        char *query = new char[uri.query.len + 2];
        std::strncpy(query, uri.query.start - 1, uri.query.len + 1);
        query[uri.query.len + 1] = '\0';

        const char *amp = "&", *sum = "=", *semicolon = ";", *separator = amp;
        char *queryParamPairPtr, *queryParam, *queryParamKey, *queryParamValue, *queryParamPtr;
        bool empty = true;
        v8::Local<v8::Object> qsSuffix = Nan::New<v8::Object>();
        Nan::DefineOwnProperty(data, Nan::New(search_symbol), Nan::New<v8::String>(query).ToLocalChecked(), attrib);
        query++;

        // find qs separator & or ;
        for (size_t i = 0; i < uri.query.len; ++i) {
            if (query[i] == *amp) {
                separator = amp;
                break;
            } else if (query[i] == *semicolon) {
                separator = semicolon;
                break;
            }
        }

        Nan::DefineOwnProperty(data, Nan::New(querySeparator_symbol), Nan::New<v8::String>(separator).ToLocalChecked(), attrib);
        queryParam = strtok_r(query, separator, &queryParamPairPtr);

        v8::Local<v8::Object> queryData = Nan::New<v8::Object>();
        bool arrayBrackets = false;
        while (queryParam) {
            if (*queryParam != *sum) {
                uint16_t len, queryLen;
                empty = false;
                queryLen = strlen(queryParam);
                queryParamKey = strtok_r(queryParam, sum, &queryParamPtr);
                len = strlen(queryParamKey);
                if (len > (sizeof(ENCODED_BRACKETS) - 1) &&
                        !strncmp(queryParamKey + len - (sizeof(ENCODED_BRACKETS) - 1),
                                 ENCODED_BRACKETS,
                                 sizeof(ENCODED_BRACKETS) - 1)) {
                    arrayBrackets = true;
                    queryParamKey[len - (sizeof(ENCODED_BRACKETS) - 1)] = '\0';
                    qsSuffix->Set(URI_LOCAL_STR(queryParamKey), URI_LOCAL_STR(ENCODED_BRACKETS));
                } else if (len > (sizeof(BRACKETS) - 1) &&
                        !strncmp(queryParamKey + len - (sizeof(BRACKETS) - 1),
                                 BRACKETS,
                                 sizeof(BRACKETS) - 1)) {
                    arrayBrackets = true;
                    queryParamKey[len - (sizeof(BRACKETS) - 1)] = '\0';
                    qsSuffix->Set(URI_LOCAL_STR(queryParamKey), URI_LOCAL_STR(BRACKETS));
                }

                queryParamValue = strtok_r(NULL, separator, &queryParamPtr);
                if (paramsMap.find(queryParamKey) == paramsMap.end()) {
                    paramsOrder.push_back(queryParamKey);
                }

                if (queryLen - len > 0) {
                    paramsMap[queryParamKey].push_back(queryParamValue ? queryParamValue: "");
                } else {
                    paramsMap[queryParamKey].push_back(NULL);
                }
            }
            queryParam = strtok_r(NULL, separator, &queryParamPairPtr);
        }


        for (std::vector<std::string>::iterator it=paramsOrder.begin(); it!=paramsOrder.end(); ++it) {
            v8::Local<v8::String> key = URI_LOCAL_STR(it->c_str());
            std::vector<const char *> vals = paramsMap[*it];
            const int arrSize = vals.size();
            if (arrSize > 1 || qsSuffix->Has(key)) {
                v8::Local<v8::Array> arrVal = Nan::New<v8::Array>(arrSize);

                int i = 0;
                for (std::vector<const char *>::iterator it2 = vals.begin(); it2 != vals.end(); ++it2) {
                    if (*it2 != NULL) {
                        arrVal->Set(i, URI_LOCAL_STR(*it2));
                    } else {
                        arrVal->Set(i, Nan::Null());
                    }
                    i++;
                }
                queryData->Set(key, arrVal);
            } else {
                const char * tmp = vals.front();
                if (tmp) {
                    queryData->Set(key, URI_LOCAL_STR(vals.front()));
                } else {
                    queryData->Set(key, Nan::Null());
                }
            }
        }

        //no need for empty object if the query string is going to be wrong
        if (!empty) {
            Nan::DefineOwnProperty(data, Nan::New(query_symbol), queryData, attrib);
            if (arrayBrackets) {
                Nan::DefineOwnProperty(data, Nan::New(queryArraySuffix_symbol), qsSuffix, attrib);
            }
        }

        query--;
        delete[] query;
    }

    if (uri.fragment.start && (opts & kFragment)) {
        Nan::DefineOwnProperty(data, Nan::New(fragment_symbol), Nan::New<v8::String>(uri.fragment.start, uri.fragment.len).ToLocalChecked(), attrib);
    }


    if (opts & kPath) {
        if (uri.path.start) {
            Nan::DefineOwnProperty(data, Nan::New(path_symbol), Nan::New<v8::String>(uri.path.start, uri.path.len).ToLocalChecked(), attrib);
        } else {
            Nan::DefineOwnProperty(data, Nan::New(path_symbol), URI_LOCAL_STR("/"), attrib);
        }
    }

    delete parser;

    info.GetReturnValue().Set(data);
}

void init (v8::Handle<v8::Object> target){

    // Old properties
    Nan::SetMethod(target, "parse", parse);
    NODE_DEFINE_CONSTANT(target, kProtocol);
    NODE_DEFINE_CONSTANT(target, kAuth);
    NODE_DEFINE_CONSTANT(target, kHost);
    NODE_DEFINE_CONSTANT(target, kPort);
    NODE_DEFINE_CONSTANT(target, kQuery);
    NODE_DEFINE_CONSTANT(target, kFragment);
    NODE_DEFINE_CONSTANT(target, kPath);
    NODE_DEFINE_CONSTANT(target, kAll);

    v8::Handle<v8::Object> uri = Nan::New<v8::Object>();
    uri->Set(URI_LOCAL_STR("PROTOCOL"), Nan::New<v8::Integer>(kProtocol));
    uri->Set(URI_LOCAL_STR("AUTH"), Nan::New<v8::Integer>(kAuth));
    uri->Set(URI_LOCAL_STR("HOST"), Nan::New<v8::Integer>(kHost));
    uri->Set(URI_LOCAL_STR("PORT"), Nan::New<v8::Integer>(kPort));
    uri->Set(URI_LOCAL_STR("QUERY"), Nan::New<v8::Integer>(kQuery));
    uri->Set(URI_LOCAL_STR("FRAGMENT"), Nan::New<v8::Integer>(kFragment));
    uri->Set(URI_LOCAL_STR("ALL"), Nan::New<v8::Integer>(kAll));
    target->Set(URI_LOCAL_STR("Uri"), uri);

}

NODE_MODULE(uriparser, init)
