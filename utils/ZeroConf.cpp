#include "ZeroConf.hpp"

#include <string.h>  //strerror
#include <iostream>
#include <errno.h>

using namespace std;

ZeroConf::ZeroConf(const std::string& serviceType, const std::string& domain, int interfaceIndex) :
    _serviceType(serviceType),
    _domain(domain),
    _interfaceIndex(interfaceIndex),
    _requestInProgress(false),
    _registeredService(false) {}

ZeroConf::~ZeroConf() {
    if (_registeredService) {
        DNSServiceRefDeallocate(_dnsService);
    }
}

void ZeroConf::ServiceBrowseReply(DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex,
                                  DNSServiceErrorType errorCode, const char* serviceName, const char* regtype,
                                  const char* replyDomain, void* context) {
    ZeroConf* zc = (ZeroConf*)context;

    if (errorCode == kDNSServiceErr_NoError) {
        if (flags & kDNSServiceFlagsAdd) {
            //cout << "Added service: [" << serviceName << ":" << regtype << ":" << replyDomain << "]\n";
            ZeroConf::ServiceRecord newService;
            newService.Name = serviceName;
            newService.Type = regtype;
            newService.Domain = replyDomain;
            zc->_services.push_back(newService);
        } else {
            //cout << "Removed service: [" << serviceName << ":" << regtype << ":" << replyDomain << "]\n";
        }

        if (!(flags & kDNSServiceFlagsMoreComing)) {
            zc->_requestInProgress = false;
        }
    } else {
        cout << "ErrorCode: " << errorCode << "\n";
        zc->_requestInProgress = false;
    }
}

void ZeroConf::ServiceResolveReply(DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex,
                                   DNSServiceErrorType errorCode, const char* fullname, const char* hosttarget,
                                   uint16_t port, uint16_t txtLen, const unsigned char* txtRecord, void* context) {
    ZeroConf* zc = (ZeroConf*)context;

    if (errorCode == kDNSServiceErr_NoError) {
        zc->_resolvedService.FullName = fullname;
        zc->_resolvedService.HostName = hosttarget;
        zc->_resolvedService.Port = port;

        if (!(flags & kDNSServiceFlagsMoreComing)) {
            zc->_requestInProgress = false;
        }
    } else {
        cout << "ErrorCode: " << errorCode << "\n";
        zc->_requestInProgress = false;
    }
}

void ZeroConf::ServiceRegisterReply(DNSServiceRef sdRef, DNSServiceFlags flags, DNSServiceErrorType errorCode,
                                    const char* name, const char* regtype, const char* domain, void* context) {
    ZeroConf* zc = (ZeroConf*)context;

    if (errorCode == kDNSServiceErr_NoError) {
        if (flags & kDNSServiceFlagsAdd) {
            //cout << "Add: " << name << "," << regtype << "," << domain << "\n";
            zc->_service.Name = name;
            zc->_service.Type = regtype;
            zc->_service.Domain = domain;
            zc->_registeredService = true;
        }

        if (!(flags & kDNSServiceFlagsMoreComing)) {
            zc->_requestInProgress = false;
        }
    } else {
        cout << "ErrorCode: " << errorCode << "\n";
        zc->_requestInProgress = false;
    }
}

void ZeroConf::handleEvents() {
    int fd = DNSServiceRefSockFD(_dnsService);
    int maxFd = fd + 1;

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);

    struct timeval tv;
    tv.tv_sec = 10;
    tv.tv_usec = 0;

    int result = select(maxFd, &readfds, 0, 0, &tv);
    if (result > 0) {
        if (FD_ISSET(fd, &readfds)) {
            DNSServiceProcessResult(_dnsService);
        }
    } else if (result == 0) {
        //timed out without results
        _requestInProgress = false;
    } else {
        cout << "select() error: " << result << "\n";
        cout << "  errno: " << errno << ":" << strerror(errno) << "\n";
        _requestInProgress = false;
    }
}

const std::vector<ZeroConf::ServiceRecord>& ZeroConf::getServices(void) {
    _services.clear();

    DNSServiceErrorType result =
        DNSServiceBrowse(&_dnsService,
                         0,  //flags - ignored
                         _interfaceIndex, _serviceType.c_str(), _domain.c_str(), ServiceBrowseReply,
                         this);  //context

    if (result == kDNSServiceErr_NoError) {
        _requestInProgress = true;
        while (_requestInProgress) {
            handleEvents();
        }
        DNSServiceRefDeallocate(_dnsService);
    } else {
        cout << "ErrorCode: " << result << "\n";
    }

    return _services;
}

bool ZeroConf::resolveService(const ServiceRecord& serviceRecord, ResolvedServiceRecord& resultRecord) {
    DNSServiceErrorType result =
        DNSServiceResolve(&_dnsService,
                          0,  //flags
                          _interfaceIndex, serviceRecord.Name.c_str(), serviceRecord.Type.c_str(),
                          serviceRecord.Domain.c_str(), ServiceResolveReply, this);

    if (result == kDNSServiceErr_NoError) {
        _requestInProgress = true;
        while (_requestInProgress) {
            handleEvents();
        }
        DNSServiceRefDeallocate(_dnsService);
    } else {
        cout << "ErrorCode: " << result << "\n";
        return false;
    }

    resultRecord = _resolvedService;

    return true;
}

bool ZeroConf::registerService(const std::string& name, const std::string& host, int port) {
    DNSServiceErrorType result = DNSServiceRegister(&_dnsService,
                                                    0,  //flags
                                                    _interfaceIndex, name.c_str(), _serviceType.c_str(),
                                                    _domain.c_str(), host.c_str(), port, 0, 0,  //txtLen, txtRecord
                                                    ServiceRegisterReply, this);

    if (result == kDNSServiceErr_NoError) {
        _requestInProgress = true;
        while (_requestInProgress) {
            handleEvents();
        }
    } else {
        cout << "ErrorCode: " << result << "\n";
        return false;
    }

    return true;
}
