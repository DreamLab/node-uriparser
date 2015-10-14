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
#include <Uri.h>
#include <map>
#include <list>
#include <string>
#include <vector>

#define THROW_IF_NULL(var) if (var.afterLast == NULL) { \
        return v8::ThrowException(v8::Exception::SyntaxError(v8::String::New("Bad string given"))); \
    }

#define ENCODED_BRACKETS "%5B%5D"

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

    if (args.Length() == 0 || !args[0]->IsString()) {
        return v8::ThrowException(v8::Exception::TypeError(v8::String::New("First argument has to be string")));
    }

    if (args[1]->IsNumber()) {
        opts = static_cast<parseOptions>(args[1]->Int32Value());
    }

    v8::String::AsciiValue url (args[0]->ToString());

    if (url.length() == 0) {
        return v8::ThrowException(v8::Exception::TypeError(v8::String::New("String mustn't be empty")));
    }

    UriParserStateA state;
    UriUriA uri;

    state.uri = &uri;

    if (uriParseUriA(&state, *url) != URI_SUCCESS) {
        return v8::ThrowException(v8::Exception::Error(v8::String::New("Unable to parse given url")));
    }

    v8::PropertyAttribute attrib = (v8::PropertyAttribute) (v8::ReadOnly | v8::DontDelete);
    v8::Local<v8::Object> data = v8::Object::New();

    if (uri.scheme.first && opts & kProtocol) {
        THROW_IF_NULL(uri.scheme)
        // +1 here because we need : after protocol
        data->Set(protocol_symbol, v8::String::New(uri.scheme.first, (uri.scheme.afterLast - uri.scheme.first) + 1), attrib);
    }

    if (uri.userInfo.first && opts & kAuth) {
        THROW_IF_NULL(uri.userInfo)
        char *auth = (char *) uri.userInfo.first;
        char *authPtr, *authUser, *authPassword;
        const char *delim = ":";
        auth[uri.userInfo.afterLast - uri.userInfo.first] = '\0';

        authUser = strtok_r(auth, delim, &authPtr);
        authPassword = strtok_r(NULL, delim, &authPtr);

        if (authUser != NULL && authPassword != NULL) {
            v8::Local<v8::Object> authData = v8::Object::New();
            authData->Set(user_symbol, v8::String::New(authUser), attrib);
            authData->Set(password_symbol, v8::String::New(authPassword), attrib);

            data->Set(auth_symbol, authData, attrib);
        }
    }

    if (uri.hostText.first && opts & kHost) {
        THROW_IF_NULL(uri.hostText)
        data->Set(host_symbol, v8::String::New(uri.hostText.first, uri.hostText.afterLast - uri.hostText.first), attrib);
    }

    if (uri.portText.first && opts & kPort) {
        THROW_IF_NULL(uri.portText)
        data->Set(port_symbol, v8::String::New(uri.portText.first, uri.portText.afterLast - uri.portText.first), attrib);
    }

    if (uri.query.first && opts & kQuery) {
        THROW_IF_NULL(uri.query)
        std::map<std::string, std::list<const char *> > paramsMap;
        std::vector<std::string> paramsOrder;
        char *query = (char *) uri.query.first;
        const char *amp = "&", *sum = "=";
        char *queryParamPairPtr, *queryParam, *queryParamKey, *queryParamValue, *queryParamPtr;
        bool empty = true;
        v8::Local<v8::Object> qsSuffix = v8::Object::New();

        query[uri.query.afterLast - uri.query.first] = '\0';
        queryParam = strtok_r(query, amp, &queryParamPairPtr);

        v8::Local<v8::Object> queryData = v8::Object::New();

        bool arrayEncodedBrackets = false;
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
                    arrayEncodedBrackets = true;
                    queryParamKey[len - (sizeof(ENCODED_BRACKETS) - 1)] = '\0';
                    qsSuffix->Set(v8::String::New(queryParamKey), v8::String::New(ENCODED_BRACKETS));
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
            v8::Local<v8::String> key = v8::String::New(it->c_str());
            std::list<const char *> vals = paramsMap[*it];
            int arrSize = vals.size();
            if (arrSize > 1) {
                v8::Local<v8::Array> arrVal = v8::Array::New(arrSize);

                int i = 0;
                for (std::list<const char *>::iterator it2 = vals.begin(); it2 != vals.end(); ++it2) {
                    arrVal->Set(i, v8::String::New(*it2));
                    i++;
                }
                queryData->Set(key, arrVal);
            } else {
                queryData->Set(key, v8::String::New((vals.front())));
            }
        }

        //no need for empty object if the query string is going to be wrong
        if (!empty) {
            data->Set(query_symbol, queryData, attrib);
            if (arrayEncodedBrackets) {
                data->Set(query_arr_suffix, qsSuffix, attrib);
            }
        }
        //parsing the path will be easier
        query--;
        *query = '\0';
    }

    if (uri.fragment.first && opts & kFragment) {
        THROW_IF_NULL(uri.fragment);
        data->Set(fragment_symbol, v8::String::New(uri.fragment.first, uri.fragment.afterLast - uri.fragment.first), attrib);
    }

    if (uri.pathHead && uri.pathHead->text.first && opts & kPath) {
        int len = 0;


        UriPathSegmentA *pathHead = uri.pathHead;
        do {
            THROW_IF_NULL(pathHead->text)
            if (pathHead->text.first == pathHead->text.afterLast) {
                len++;
            } else {
                //len +1 because we need to add slash
                len += pathHead->text.afterLast - pathHead->text.first + 1;
            }
            pathHead = pathHead->next;
        } while (pathHead);

        char *path, *p, *tmp;
        path = p = tmp = (char *) malloc(len + 1);
        path[len] = '\0';

        pathHead = uri.pathHead;
        do {
            //adding slash no matter what
            *path = '/';
            path++;
            //copying text only in case when path segment is not empty
            if (pathHead->text.first != pathHead->text.afterLast) {
                int len = pathHead->text.afterLast - pathHead->text.first;
                memcpy(path, pathHead->text.first, len);
                path += len;
            }
            pathHead = pathHead->next;
        } while (pathHead);

        //path is always prefixed with / in case of relative url we move pointer further
        if (!uri.absolutePath && !uri.hostText.first) {
            p++;
        }

        data->Set(path_symbol, v8::String::New(p), attrib);
        free(tmp);
    } else if (opts & kPath) {
        data->Set(path_symbol, v8::String::New("/"), attrib);
    }

    uriFreeUriMembersA(&uri);

    return scope.Close(data);
}

extern "C" void init (v8::Handle<v8::Object> target){
    v8::HandleScope scope;

    NODE_SET_METHOD(target, "parse", parse);
    NODE_DEFINE_CONSTANT(target, kProtocol);
    NODE_DEFINE_CONSTANT(target, kAuth);
    NODE_DEFINE_CONSTANT(target, kHost);
    NODE_DEFINE_CONSTANT(target, kPort);
    NODE_DEFINE_CONSTANT(target, kQuery);
    NODE_DEFINE_CONSTANT(target, kFragment);
    NODE_DEFINE_CONSTANT(target, kPath);
    NODE_DEFINE_CONSTANT(target, kAll);
}
