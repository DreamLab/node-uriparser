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
static v8::Persistent<v8::String> fragment_symbol = NODE_PSYMBOL("fragment");
static v8::Persistent<v8::String> path_symbol = NODE_PSYMBOL("path");

static v8::Handle<v8::Value> parse(const v8::Arguments& args){
    v8::HandleScope scope;

    parseOptions opts = kAll;

    if (args.Length() == 0 || !args[0]->IsString()) {
        v8::ThrowException(v8::Exception::TypeError(v8::String::New("First argument has to be string")));
    }

    if (args[1]->IsNumber()) {
        opts = static_cast<parseOptions>(args[1]->Int32Value());
    }

    v8::String::Utf8Value url (args[0]->ToString());

    if (url.length() == 0) {
        v8::ThrowException(v8::Exception::TypeError(v8::String::New("String mustn't be empty")));
    }

    UriParserStateA state;
    UriUriA uri;

    state.uri = &uri;

    if (uriParseUriA(&state, *url) != URI_SUCCESS) {
        return scope.Close(v8::Boolean::New(false));
    }

    v8::PropertyAttribute attrib = (v8::PropertyAttribute) (v8::ReadOnly | v8::DontDelete);
    v8::Local<v8::Object> data = v8::Object::New();

    if (uri.scheme.first && opts & kProtocol) {
        // +1 here because we need : after protocol
        data->Set(protocol_symbol, v8::String::New(uri.scheme.first, strlen(uri.scheme.first) - strlen(uri.scheme.afterLast) + 1), attrib);
    }

    if (uri.userInfo.first && opts & kAuth) {
        char *auth = (char *) uri.userInfo.first;
        char *authPtr, *authUser, *authPassword;
        const char *delim = ":";
        auth[strlen(uri.userInfo.first) - strlen(uri.userInfo.afterLast)] = '\0';

        authUser = strtok_r(auth, delim, &authPtr);
        authPassword = strtok_r(NULL, delim, &authPtr);

        if (authUser != NULL && authPassword != NULL) {
            v8::Local<v8::Object> authData = v8::Object::New();
            authData->Set(v8::String::New("user"), v8::String::New(authUser)), attrib;
            authData->Set(v8::String::New("password"), v8::String::New(authPassword), attrib);

            data->Set(auth_symbol, authData, attrib);
        }
    }

    if (uri.hostText.first && opts & kHost) {
        int tmpLength = strlen(uri.hostText.first);

        data->Set(host_symbol, v8::String::New(uri.hostText.first, tmpLength - strlen(uri.hostText.afterLast)), attrib);
    }

    if (uri.portText.first && opts & kPort) {
        data->Set(port_symbol, v8::String::New(uri.portText.first, strlen(uri.portText.first) - strlen(uri.portText.afterLast)), attrib);
    }

    if (uri.query.first && opts & kQuery) {
        char *query = (char *) uri.query.first;
        const char *amp = "&", *sum = "=";
        char *queryParamPairPtr, *queryParam, *queryParamKey, *queryParamValue, *queryParamPtr;
        bool empty = true;
             
        query[strlen(uri.query.first) - strlen(uri.query.afterLast)] = '\0';
        queryParam = strtok_r(query, amp, &queryParamPairPtr);

        v8::Local<v8::Object> queryData = v8::Object::New();

        while (queryParam) {
            if (*queryParam != *sum) {
                empty = false;
                queryParamKey = strtok_r(queryParam, sum, &queryParamPtr);
                queryParamValue = strtok_r(NULL, sum, &queryParamPtr);
                queryData->Set(v8::String::New(queryParamKey), v8::String::New(queryParamValue ? queryParamValue : ""), attrib);
            }
            queryParam = strtok_r(NULL, amp, &queryParamPairPtr);
        }

        //no need for empty object if the query string is going to be wrong
        if (!empty) {
            data->Set(query_symbol, queryData, attrib);
        }
        //parsing the path will be easier
        query--;
        *query = '\0';
    }

    if (uri.fragment.first && opts & kFragment) {
        data->Set(fragment_symbol, v8::String::New(uri.fragment.first, strlen(uri.fragment.first) - strlen(uri.fragment.afterLast)), attrib);
    }

    if (uri.pathHead && uri.pathHead->text.first && opts & kPath) {
        UriPathSegmentA pathHead = *uri.pathHead;
        char *path = (char *)pathHead.text.first;
        int position = strlen(pathHead.text.first);
        int tmpPosition;

        while (pathHead.next) {
            pathHead = *pathHead.next;
        }

        tmpPosition = strlen(pathHead.text.afterLast);

        if ((position - tmpPosition) == 0) {
            path = (char *)"/";
        }

        if ((uri.absolutePath || uri.hostText.first) && strlen(path) >= 1) {
            if(*(path - 1)) {
                path--;
            }
        }

        data->Set(path_symbol, v8::String::New(path), attrib);
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
