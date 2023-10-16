#pragma once

#include <string>
#include <vector>

#include <dns_sd.h>

class ZeroConf {
public:
    struct ServiceRecord {
        std::string Name;
        std::string Type;
        std::string Domain;
    };

    struct ResolvedServiceRecord {
        std::string FullName;
        std::string HostName;
        int Port;
    };

    ZeroConf(const std::string& serviceType, const std::string& domain = "", int interfaceIndex = 0);
    ~ZeroConf();

    const std::vector<ServiceRecord>& getServices(void);
    bool resolveService(const ServiceRecord& serviceRecord, ResolvedServiceRecord& result);

    bool registerService(const std::string& name, const std::string& host, int port);

protected:
    void handleEvents(void);

private:
    std::string _serviceType;
    std::string _domain;
    int _interfaceIndex;

    DNSServiceRef _dnsService;

    bool _requestInProgress;
    bool _registeredService;

    std::vector<ServiceRecord> _services;

    ResolvedServiceRecord _resolvedService;

    ServiceRecord _service;

    static void ServiceBrowseReply(DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex,
                                   DNSServiceErrorType errorCode, const char* serviceName, const char* regtype,
                                   const char* replyDomain, void* context);

    static void ServiceResolveReply(DNSServiceRef sdRef, DNSServiceFlags flags, uint32_t interfaceIndex,
                                    DNSServiceErrorType errorCode, const char* fullname, const char* hosttarget,
                                    uint16_t port, uint16_t txtLen, const unsigned char* txtRecord, void* context);

    static void ServiceRegisterReply(DNSServiceRef sdRef, DNSServiceFlags flags, DNSServiceErrorType errorCode,
                                     const char* name, const char* regtype, const char* domain, void* context);
};
