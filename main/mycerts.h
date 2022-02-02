#ifndef __MYCERTS__
#define __MYCERTS__

#include <Arduino.h>

//Data Topics   
    #define AWS_BASE_DATA_TOPIC "devices/data"

//Acitvation Topics
    //Device Setup Topics - Subscribe
    #define AWS_CERT_REQUEST_ACCEPT "$aws/certificates/create/json/accepted"
    #define AWS_CERT_REQUEST_REJECT "$aws/certificates/create/json/rejected"

    #define AWS_CERT_PROVISION_ACCEPT "$aws/provisioning-templates/Air_Quality/provision/json/accepted"
    #define AWS_CERT_PROVISION_REJECT "$aws/provisioning-templates/Air_Quality/provision/json/rejected"

    //Device Setup Topics - Publish
    #define AWS_CERT_REQUEST_CREATE "$aws/certificates/create/json"
    #define AWS_CERT_REQUEST_PROVISION "$aws/provisioning-templates/Air_Quality/provision/json"

//AWS Endpoint
    const char AWS_IOT_ENDPOINT[] = "a2q3xq7o4p30im-ats.iot.us-east-1.amazonaws.com";

//Max Retries
    int AWS_MAX_RECONNECT_TRIES = 10;

// Amazon's root CA.
    char AWS_ROOT_CA[] = "-----BEGIN CERTIFICATE-----\n" \
    "MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n" \
    "ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n" \
    "b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n" \
    "MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n" \
    "b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n" \
    "ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n" \
    "9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n" \
    "IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n" \
    "VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n" \
    "93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n" \
    "jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n" \
    "AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n" \
    "A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n" \
    "U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n" \
    "N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n" \
    "o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n" \
    "5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n" \
    "rqXRfboQnoZsG4q5WTP468SQvvG5\n" \
    "-----END CERTIFICATE-----\n";

//Init Cert
    char AWS_INIT_CERT[] =  "-----BEGIN CERTIFICATE-----\n"
    "MIIDVDCCAjwCCQDdQG25VAyAETANBgkqhkiG9w0BAQsFADAhMQswCQYDVQQGEwJQ\n"
    "SzESMBAGA1UECgwJQnJlYXRoZUlPMB4XDTIyMDEyOTA4Mzk0OFoXDTIzMDYxMzA4\n"
    "Mzk0OFowgbYxCzAJBgNVBAYTAlBLMQ8wDQYDVQQIDAZQdW5qYWIxDzANBgNVBAcM\n"
    "BkxhaG9yZTEUMBIGA1UECgwLQWlyIFF1YWxpdHkxSTBHBgNVBAMMQDY2Y2Y3MjQy\n"
    "MzcyNDg5NjAzYzY3ZWJmYjNmMGJlZTNmYTdiY2VkZTZiOTI0NDQ1MTAxNjhkMzE4\n"
    "OTUwNzRjNmIxJDAiBgkqhkiG9w0BCQEWFXN1cHBvcnRAYnJlYXRoZWlvLmNvbTCC\n"
    "ASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAME1nLxRJOR21zRUPGVhx6zy\n"
    "OOCjV23X0hFuATIZtWryRcnVm93AVv2U+lmcZWDLcCyolM1q1EPmHtd58Zk6uw97\n"
    "0P574UzhtgxV/JzeYsb3gAiEzxIA7PQfXxAHCMjEUrLrrqMfvk8ifwUgSy92ihKW\n"
    "QKH2Y70RiQTqb89jeIcIexY9MRJzENVlBM4H+p4YKIhi6E3IgQ7CZ1gTf28NwgUP\n"
    "5WOja6/F5DEThFehRoDwHJvXhPhf2F4nWNdFZ3a8c/GFi7v37egX+2wbNAJDESuj\n"
    "jKVkomONNdWUSHDjL/FlKPwZg+n7H1NrOImbEbMxzwAyJGsSJ0otrbJZJhgZwQsC\n"
    "AwEAATANBgkqhkiG9w0BAQsFAAOCAQEARiz4spseVS3lqVHh7getya3FakU6qPiG\n"
    "coQiPXt8Uk54gDbkot0+mtF4ki1G/0yQzI97arLZJ0oXusiIGmgYBgDhatumK2gR\n"
    "YWLCI1vNGlit6gDlUE9uiKOz99tWXvUTiz2I9VPvA1NRrPyGfnEqL63NjZ42JmOD\n"
    "0JLiXxXxF+ONzfWebeD5m0B5DXPVeg0b2GRu1VX4VAWX21T4DAT019B2o1+IJG2D\n"
    "Eq4OHnggM5tY3PZRQVPA5bDKXBEND18ZrumK9o29kvdqiORaaoQ5xjsZKRziadm9\n"
    "z7nvbxkpELlZjIjrnLqKhIPATnjkntoVajlHh093L95KHiAxDnWSng==\n"
    "-----END CERTIFICATE-----\n";

//Init Key
    char AWS_INIT_KEY[] =  "-----BEGIN RSA PRIVATE KEY-----\n"
    "MIIEowIBAAKCAQEAwTWcvFEk5HbXNFQ8ZWHHrPI44KNXbdfSEW4BMhm1avJFydWb\n" \
    "3cBW/ZT6WZxlYMtwLKiUzWrUQ+Ye13nxmTq7D3vQ/nvhTOG2DFX8nN5ixveACITP\n" \
    "EgDs9B9fEAcIyMRSsuuuox++TyJ/BSBLL3aKEpZAofZjvRGJBOpvz2N4hwh7Fj0x\n" \
    "EnMQ1WUEzgf6nhgoiGLoTciBDsJnWBN/bw3CBQ/lY6Nrr8XkMROEV6FGgPAcm9eE\n" \
    "+F/YXidY10Vndrxz8YWLu/ft6Bf7bBs0AkMRK6OMpWSiY4011ZRIcOMv8WUo/BmD\n" \
    "6fsfU2s4iZsRszHPADIkaxInSi2tslkmGBnBCwIDAQABAoIBACBnzZIIShfHlvC/\n" \
    "8CFQ9Rycw81pmBBkUmd5+9lbVoJJgEsJ+MqIqA6AHj1Jbfd+J6JV5B9P+6onvqSF\n" \
    "X3BD2XwPWE5WmMM8WDxZb7Uf15FnkaHcJJ7waM3JNTiK9X4272fy/6d8e37qWkOR\n" \
    "1wpu51mWueBwYuojNsgMShSOqnh7OK82Pd0oNR59Ua96QBmjAo83kVXCtVY3wfE1\n" \
    "++ElIpFPuZjOLPx09UFYkvAw23LYe7BbBHN+afBWyAgzViUsK9Q1cJ7lIJr3/ab9\n" \
    "ytt1NkKSjgScAHU15f7WOWtXfLfVTjn/rwIrL7AR+yTa/Bu13IhlWaqW7bEiRQ38\n" \
    "sFZZl4ECgYEA9dlvtNzBzpLSGdbgxalq6THU509tCubZV7l/blTfLMVTYMEmfh6L\n" \
    "8D1Yuae/wYv9pDhxJmJNKKwUnUELPLD7R6Jtpzej7c7pmIq9h4+nOgskq4oitddv\n" \
    "fwoEl675ZDyH6E2UZlYz4/RpPruKrxml0Cu+knZmeEL4jKs+TGD//bcCgYEAyS/J\n" \
    "FOkpMbVJ4Js1ARGreQtrVgQ5qn1Pt9doKecbff7YNi7KTRY2L8ynUKGh5oA4j0ko\n" \
    "xdc3ow4Y32IqjhFYcH4EB9D0/XzY2iWqBUq8ODQKOx4BVxe0viM37dVE50wbV226\n" \
    "8BKb9otP98hU7W/EEj9ajjsVsROBX/89K/8pF00CgYBGSckvK2JZng5lhtKykHJZ\n" \
    "g368NzAUytVeyFEPiUJZOX1SD0bjDfayU8e3JKelPVHiDqUC9V3b//7QytTcoDjn\n" \
    "TSAOi9DZX6vXiesQA9kqth7g0FiGJrbg2ktf1o3Ew62TlAKWk5S4WptwbQwEbc/D\n" \
    "SY5tr14yiesnxJnGGoF11wKBgHhMzpxmX2+Ny8M2i5c8DaCvp1sj46eCjuyLLYoA\n" \
    "23uXq4maqPU3Vud2JIYeYEUIXsmvIWZLrsUdZNoORQv2az+42ogh9pyszq6C3wuT\n" \
    "CMFCvOtkocCGOFmJjaP1ZuI3rBG3V5S+S+w8mXC8pavkrQWMRIXP5sSfuTjB0r5A\n" \
    "MT3xAoGBAOtU36u3uLRXysppzcMZaG1GJF+ZR2cW2AdT2p4nDg1fjTc+CeEC4/7Z\n" \
    "ToxbRQK12dbgt86th5hyglhjRpMbtMcwxiEQ7Ax+ScnLJK0QIhsJmZgounQ4deU0\n" \
    "qo7pKLXC/XtemI+/Q5CBCJXQu5kDnt7/VX76N4DjT80OxeOPzs/l\n" \
    "-----END RSA PRIVATE KEY-----\n";

#endif