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
#include <list>
#include <string>
#include <cstring>
#include <vector>
#include <nan.h>
#include "parser.hpp"

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
    eRfcParser = 1,
    eNgxParser
};

#define URI_PSYMBOL(name) Nan::New<v8::String>(name).ToLocalChecked()

static Nan::Persistent<v8::String> protocol_symbol(URI_PSYMBOL("protocol"));
static Nan::Persistent<v8::String> auth_symbol(URI_PSYMBOL("auth"));
static Nan::Persistent<v8::String> host_symbol(URI_PSYMBOL("host"));
static Nan::Persistent<v8::String> port_symbol(URI_PSYMBOL("port"));
static Nan::Persistent<v8::String> query_symbol(URI_PSYMBOL("query"));
static Nan::Persistent<v8::String> queryArraySuffix_symbol(URI_PSYMBOL("queryArraySuffix"));
static Nan::Persistent<v8::String> fragment_symbol(URI_PSYMBOL("fragment"));
static Nan::Persistent<v8::String> path_symbol(URI_PSYMBOL("path"));
static Nan::Persistent<v8::String> user_symbol(URI_PSYMBOL("user"));
static Nan::Persistent<v8::String> password_symbol(URI_PSYMBOL("password"));


NAN_METHOD(parse) {
    parseOptions opts = kAll;
    Engines engine = eRfcParser;

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

    if (info[2]->IsNumber()) {
        engine = static_cast<Engines>(info[2]->Int32Value());
    }

    v8::PropertyAttribute attrib = (v8::PropertyAttribute) (v8::ReadOnly | v8::DontDelete);
    v8::Local<v8::Object> data = Nan::New<v8::Object>();

    Url uri;
    Parser *parser;

    if (engine == eNgxParser) {
        parser = new NgxParser(*url);
    } else {
        parser = new RfcParser(*url);
    }

    if (parser->status != Parser::OK) {
        Nan::ThrowError("Unable to parse given url");
        return;
    }
    uri = parser->url;

    if (uri.schema && (opts & kProtocol)) {
        data->ForceSet(Nan::New<v8::String>(protocol_symbol), Nan::New<v8::String>(std::strcat(uri.schema, ":")).ToLocalChecked(), attrib);
    }

    if (uri.auth && (opts & kAuth)) {
        const char *delim = ":";
        char *authPtr, *authUser, *authPassword;

        authUser = strtok_r(uri.auth, delim, &authPtr);
        authPassword = strtok_r(NULL, delim, &authPtr);

        if (authUser != NULL && authPassword != NULL) {
            v8::Local<v8::Object> authData = Nan::New<v8::Object>();
            authData->ForceSet(Nan::New(user_symbol), Nan::New<v8::String>(authUser).ToLocalChecked(), attrib);
            authData->ForceSet(Nan::New(password_symbol), Nan::New<v8::String>(authPassword).ToLocalChecked(), attrib);

            data->ForceSet(Nan::New(auth_symbol), authData, attrib);
        }
    }

    if (uri.host && (opts & kHost)) {
        data->ForceSet(Nan::New(host_symbol), Nan::New<v8::String>(uri.host).ToLocalChecked(), attrib);
    }

    if (uri.port && (opts & kPort)) {
        data->ForceSet(Nan::New(port_symbol), Nan::New<v8::String>(uri.port).ToLocalChecked(), attrib);
    }

    if (uri.query && (opts & kQuery)) {
        std::map<std::string, std::list<const char *> > paramsMap;
        std::vector<std::string> paramsOrder;
        char *query = uri.query;
        const char *amp = "&", *sum = "=";
        char *queryParamPairPtr, *queryParam, *queryParamKey, *queryParamValue, *queryParamPtr;
        bool empty = true;
        v8::Local<v8::Object> qsSuffix = Nan::New<v8::Object>();

        queryParam = strtok_r(query, amp, &queryParamPairPtr);

        v8::Local<v8::Object> queryData = Nan::New<v8::Object>();
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
                    qsSuffix->Set(Nan::New<v8::String>(queryParamKey).ToLocalChecked(), Nan::New<v8::String>(ENCODED_BRACKETS).ToLocalChecked());
                } else if (len > (sizeof(BRACKETS) - 1) &&
                        !strncmp(queryParamKey + len - (sizeof(BRACKETS) - 1),
                                 BRACKETS,
                                 sizeof(BRACKETS) - 1)) {
                    arrayBrackets = true;
                    queryParamKey[len - (sizeof(BRACKETS) - 1)] = '\0';
                    qsSuffix->Set(Nan::New<v8::String>(queryParamKey).ToLocalChecked(), Nan::New<v8::String>(BRACKETS).ToLocalChecked());
                }

                queryParamValue = strtok_r(NULL, sum, &queryParamPtr);
                if (paramsMap.find(queryParamKey) == paramsMap.end()) {
                    paramsOrder.push_back(queryParamKey);
                }
                paramsMap[queryParamKey].push_back(queryParamValue ? queryParamValue : "");
            }
            queryParam = strtok_r(NULL, amp, &queryParamPairPtr);
        }


        for (std::vector<std::string>::iterator it=paramsOrder.begin(); it!=paramsOrder.end(); ++it) {
            v8::Local<v8::String> key = Nan::New<v8::String>(it->c_str()).ToLocalChecked();
            std::list<const char *> vals = paramsMap[*it];
            int arrSize = vals.size();
            if (arrSize > 1 || qsSuffix->Has(key)) {
                v8::Local<v8::Array> arrVal = Nan::New<v8::Array>(arrSize);

                int i = 0;
                for (std::list<const char *>::iterator it2 = vals.begin(); it2 != vals.end(); ++it2) {
                    arrVal->Set(i, Nan::New<v8::String>(*it2).ToLocalChecked());
                    i++;
                }
                queryData->Set(key, arrVal);
            } else {
                queryData->Set(key, Nan::New<v8::String>(vals.front()).ToLocalChecked());
            }
        }

        //no need for empty object if the query string is going to be wrong
        if (!empty) {
            data->ForceSet(Nan::New(query_symbol), queryData, attrib);
            if (arrayBrackets) {
                data->ForceSet(Nan::New(queryArraySuffix_symbol), qsSuffix, attrib);
            }
        }
    }

    if (uri.fragment && (opts & kFragment)) {
        data->ForceSet(Nan::New(fragment_symbol), Nan::New<v8::String>(uri.fragment).ToLocalChecked(), attrib);
    }

    if (uri.path && (opts & kPath)) {
        data->ForceSet(Nan::New(path_symbol), Nan::New<v8::String>(uri.path).ToLocalChecked(), attrib);
    } else {
        data->ForceSet(Nan::New(path_symbol), Nan::New<v8::String>("/").ToLocalChecked(), attrib);
    }

    delete parser;

    info.GetReturnValue().Set(data);
}

void init (v8::Handle<v8::Object> target){


    Nan::SetMethod(target, "parse", parse);
    NODE_DEFINE_CONSTANT(target, kProtocol);
    NODE_DEFINE_CONSTANT(target, kAuth);
    NODE_DEFINE_CONSTANT(target, kHost);
    NODE_DEFINE_CONSTANT(target, kPort);
    NODE_DEFINE_CONSTANT(target, kQuery);
    NODE_DEFINE_CONSTANT(target, kFragment);
    NODE_DEFINE_CONSTANT(target, kPath);
    NODE_DEFINE_CONSTANT(target, kAll);

    NODE_DEFINE_CONSTANT(target, eNgxParser);
    NODE_DEFINE_CONSTANT(target, eRfcParser);

}

NODE_MODULE(uriparser, init)
