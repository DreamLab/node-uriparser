/*
 * Copyright (C) 2011 by Jakub Lekstan <kuebzky@gmail.com>
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
#include <string.h>

static v8::Handle<v8::Value> parse(const v8::Arguments& args){
    v8::HandleScope scope;

    if (args.Length() == 0 || !args[0]->IsString()) {
        v8::ThrowException(v8::Exception::TypeError(v8::String::New("First argument has to be string")));
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
    v8::Local<v8::String> emptyString = v8::String::New("");
    v8::Local<v8::Array> emptyArray = v8::Array::New();
    v8::Local<v8::Object> emptyObject = v8::Object::New();

    int i, position, tmpPosition;
    int hostLastPosition = 0;

    if (uri.scheme.first) {
        data->Set(v8::String::New("protocol"), v8::String::New(uri.scheme.first, strlen(uri.scheme.first) - strlen(uri.scheme.afterLast)), attrib);
    } else {
        data->Set(v8::String::New("protocol"), emptyString, attrib);
    }

    if (uri.userInfo.first) {
        char *auth = (char*) uri.userInfo.first;
        const char *delim = ":";
        auth[strlen(uri.userInfo.first) - strlen(uri.userInfo.afterLast)] = '\0';

        v8::Local<v8::Object> authData = v8::Object::New();
        authData->Set(v8::String::New("user"), v8::String::New(strtok(auth, delim))), attrib;
        authData->Set(v8::String::New("password"), v8::String::New(strtok(NULL, delim)), attrib);

        data->Set(v8::String::New("auth"), authData, attrib);
    } else {
        data->Set(v8::String::New("auth"), emptyObject, attrib);
    }

    if (uri.hostText.first) {
        int tmpLength = strlen(uri.hostText.first);
        const char *tmp = strchr(uri.hostText.first, '/');
        hostLastPosition = tmpLength - ((tmp - uri.hostText.first) + 1);

        data->Set(v8::String::New("hostname"), v8::String::New(uri.hostText.first, tmpLength - strlen(uri.hostText.afterLast)), attrib);
    } else {
        data->Set(v8::String::New("hostname"), emptyString, attrib);
    }

/*    UriIp4 ip4 = *uri.hostData.ip4;
    fprintf(stderr, "HOSTDATA4: %s\n", ip4.data);
    UriIp6 ip6 = *uri.hostData.ip6;
    fprintf(stderr, "HOSTDATA6: %s\n", ip6.data);*/

    if (uri.portText.first) {
        data->Set(v8::String::New("port"), v8::String::New(uri.portText.first, strlen(uri.portText.first) - strlen(uri.portText.afterLast)), attrib);
    } else {
        data->Set(v8::String::New("port"), v8::Integer::New(80), attrib);
    }

    if (uri.query.first) {
        char *query = (char*) uri.query.first;
        query[strlen(uri.query.first) - strlen(uri.query.afterLast)] = '\0';
        const char *amp = "&", *sum = "=";
        char *queryParamPtr, *queryParam = strtok_r(query, amp, &queryParamPtr), *queryParamKey, *queryParamValue;

        v8::Local<v8::Object> queryData = v8::Object::New();

        while (queryParam) {
            queryParamKey = strtok(queryParam, sum);
            queryParamValue = strtok(NULL, sum);
            queryParam = strtok_r(NULL, amp, &queryParamPtr);

            queryData->Set(v8::String::New(queryParamKey), v8::String::New(queryParamValue), attrib);
        }

        data->Set(v8::String::New("query"), queryData, attrib);
    } else {
        data->Set(v8::String::New("query"), emptyObject, attrib);
    }

    if (uri.fragment.first) {
        data->Set(v8::String::New("fragment"), v8::String::New(uri.fragment.first, strlen(uri.fragment.first) - strlen(uri.fragment.afterLast)), attrib);
    } else {
        data->Set(v8::String::New("fragment"), emptyString, attrib);
    }

    if (uri.pathHead && uri.pathHead->text.first && hostLastPosition > 1) {
        UriPathSegmentA pathHead = *uri.pathHead;

        char *path = (char*) pathHead.text.first;

        position = strlen(pathHead.text.first);
        while (pathHead.next) {
            i++;
            pathHead = *pathHead.next;
        }

        tmpPosition = strlen(pathHead.text.afterLast);

        path[position - tmpPosition] = '\0';

        if (uri.absolutePath || uri.hostText.first) {
            path--;
        }

        data->Set(v8::String::New("pathname"), v8::String::New(path), attrib);
    } else {
        data->Set(v8::String::New("pathname"), v8::String::New("/"), attrib);
    }

/*    UriPathSegmentA pathHead = *uri.pathHead;

    if (pathHead.text.first) {
        i = 0;

        while (pathHead.next) {
            i++;
            pathHead = *pathHead.next;
        }

        pathHead = *uri.pathHead;
        i = 0;
        position = strlen(pathHead.text.afterLast);
        tmpPosition = position - 1;

        v8::Local<v8::Array> aPathHead = v8::Array::New(i + 1);

        aPathHead->Set(v8::Number::New(i), v8::String::New(pathHead.text.first, strlen(pathHead.text.first) - position));

        while (pathHead.next) {
            i++;
            pathHead = *pathHead.next;

            position = strlen(pathHead.text.afterLast);

            aPathHead->Set(v8::Number::New(i), v8::String::New(pathHead.text.first, tmpPosition - position));
            tmpPosition = position - 1;
        }

        data->Set(v8::String::New("pathHead"), aPathHead);
    } else {
        data->Set(v8::String::New("pathHead"), emptyArray);
    }*/

/*    UriPathSegmentA pathTail = *uri.pathTail;

    if (pathTail.text.first) {
        i = 0;

        while (pathTail.next) {
            i++;
            pathTail = *pathTail.next;
        }

        pathTail = *uri.pathTail;
        i = 0;
        position = strlen(pathTail.text.afterLast);
        tmpPosition = position - 1;

        v8::Local<v8::Array> aPathTail = v8::Array::New(i + 1);

        aPathTail->Set(v8::Number::New(i), v8::String::New(pathTail.text.first, strlen(pathTail.text.first) - position));

        while (pathTail.next) {
            i++;
            pathTail = *pathTail.next;

            position = strlen(pathTail.text.afterLast);

            aPathTail->Set(v8::Number::New(i), v8::String::New(pathTail.text.first, tmpPosition - position));
            tmpPosition = position - 1;
        }

        data->Set(v8::String::New("pathTail"), aPathTail);
    } else {
        data->Set(v8::String::New("pathTail"), emptyArray);
    }*/

//    data->Set(v8::String::New("absolutePath"), v8::Boolean::New(uri.absolutePath));
//    data->Set(v8::String::New("owner"), v8::Boolean::New(uri.owner));

    uriFreeUriMembersA(&uri);

    return scope.Close(data);
}

extern "C" void init (v8::Handle<v8::Object> target){
    v8::HandleScope scope;

    NODE_SET_METHOD(target, "parse", parse);
}

