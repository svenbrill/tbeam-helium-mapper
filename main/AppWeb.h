#ifndef APPWEB_H_
#define APPWEB_H_

#include <WebServer.h>
#include <vector>

typedef std::function<void(WebServer& server)> THandlerFunction;

class AppWeb {
public:
    AppWeb();
    ~AppWeb();

    void begin();
    void end();
    void loop();
    void attachRequest(RequestHandler *handler);

private:
    WebServer *server;

private:
    static void getConfig(WebServer& server);
    static void postConfig(WebServer& server);
    static void getStatus(WebServer& server);
    static void postUpdate(WebServer& server);
    static void postUpdateUpload(WebServer& server);
};


class FunctionRequestHandler : public RequestHandler {
public:
    FunctionRequestHandler(
        THandlerFunction fn,
        THandlerFunction ufn,
        const Uri &uri, HTTPMethod method
    )
    : _fn(fn)
    , _ufn(ufn)
    , _uri(uri.clone())
    , _method(method)
    {
        _uri->initPathArgs(pathArgs);
    }

    ~FunctionRequestHandler() {
        delete _uri;
    }


    bool canHandle(HTTPMethod requestMethod, String requestUri) override  {
        if (_method != HTTP_ANY && _method != requestMethod)
            return false;

        return _uri->canHandle(requestUri, pathArgs);
    }

    bool canUpload(String requestUri) override  {
        if (!_ufn || !canHandle(HTTP_POST, requestUri))
            return false;

        return true;
    }

    bool handle(WebServer& server, HTTPMethod requestMethod, String requestUri) override {
        if (!canHandle(requestMethod, requestUri))
            return false;

        _fn(server);
        return true;
    }

    void upload(WebServer& server, String requestUri, HTTPUpload& upload) override {
        (void) upload;
        if (canUpload(requestUri))
            _ufn(server);
    }

protected:
    THandlerFunction _fn;
    THandlerFunction _ufn;
    Uri *_uri;
    HTTPMethod _method;
};


#endif
