/* soapStub.h
   Generated by gSOAP 2.2.3b from SGS.h
   Copyright (C) 2001-2003 Genivia inc.
   All Rights Reserved.
*/
#ifndef soapStub_H
#define soapStub_H
#ifdef __cplusplus
extern "C" {
#endif

/* Types With Custom (De)serializers: */

/* Enumerations */

/* Classes and Structs */

#ifndef _SOAP_tns__GetStatusResponse
#define _SOAP_tns__GetStatusResponse
struct tns__GetStatusResponse
{
	char *_result;
};
#endif

#ifndef _SOAP_tns__AppStartResponse
#define _SOAP_tns__AppStartResponse
struct tns__AppStartResponse
{
	char *_result;
};
#endif

#ifndef _SOAP_tns__StopResponse
#define _SOAP_tns__StopResponse
struct tns__StopResponse
{
	char *_result;
};
#endif

#ifndef _SOAP_tns__AppStopResponse
#define _SOAP_tns__AppStopResponse
struct tns__AppStopResponse
{
	char *_result;
};
#endif

#ifndef _SOAP_tns__DetachResponse
#define _SOAP_tns__DetachResponse
struct tns__DetachResponse
{
	char *_result;
};
#endif

#ifndef _SOAP_tns__setServiceDataResponse
#define _SOAP_tns__setServiceDataResponse
struct tns__setServiceDataResponse
{
	char *_result;
};
#endif

#ifndef _SOAP_tns__GetNthDataSourceResponse
#define _SOAP_tns__GetNthDataSourceResponse
struct tns__GetNthDataSourceResponse
{
	char *_result;
};
#endif

#ifndef _SOAP_tns__ResumeResponse
#define _SOAP_tns__ResumeResponse
struct tns__ResumeResponse
{
	char *_result;
};
#endif

#ifndef _SOAP_tns__AppDetachResponse
#define _SOAP_tns__AppDetachResponse
struct tns__AppDetachResponse
{
	char *_result;
};
#endif

#ifndef _SOAP_tns__PutControlResponse
#define _SOAP_tns__PutControlResponse
struct tns__PutControlResponse
{
	char *_result;
};
#endif

#ifndef _SOAP_tns__PutStatusResponse
#define _SOAP_tns__PutStatusResponse
struct tns__PutStatusResponse
{
	char *_result;
};
#endif

#ifndef _SOAP_tns__AttachResponse
#define _SOAP_tns__AttachResponse
struct tns__AttachResponse
{
	char *_result;
};
#endif

#ifndef _SOAP_tns__findServiceDataResponse
#define _SOAP_tns__findServiceDataResponse
struct tns__findServiceDataResponse
{
	char *_result;
};
#endif

#ifndef _SOAP_tns__PauseResponse
#define _SOAP_tns__PauseResponse
struct tns__PauseResponse
{
	char *_result;
};
#endif

#ifndef _SOAP_tns__GetNotificationsResponse
#define _SOAP_tns__GetNotificationsResponse
struct tns__GetNotificationsResponse
{
	char *_result;
};
#endif

#ifndef _SOAP_tns__DestroyResponse
#define _SOAP_tns__DestroyResponse
struct tns__DestroyResponse
{
  void *rubbish; /* ARPDBG */
};
#endif

#ifndef _SOAP_tns__AppRecordChkpointResponse
#define _SOAP_tns__AppRecordChkpointResponse
struct tns__AppRecordChkpointResponse
{
	char *_result;
};
#endif

#ifndef _SOAP_tns__GetControlResponse
#define _SOAP_tns__GetControlResponse
struct tns__GetControlResponse
{
	char *_result;
};
#endif

#ifndef _SOAP_tns__AppDetach
#define _SOAP_tns__AppDetach
struct tns__AppDetach
{
  void *rubbish; /* ARPDBG */
};
#endif

#ifndef _SOAP_tns__Attach
#define _SOAP_tns__Attach
struct tns__Attach
{
};
#endif

#ifndef _SOAP_tns__findServiceData
#define _SOAP_tns__findServiceData
struct tns__findServiceData
{
	char *input;
};
#endif

#ifndef _SOAP_tns__AppRecordChkpoint
#define _SOAP_tns__AppRecordChkpoint
struct tns__AppRecordChkpoint
{
	char *chk_USCORE_meta_USCORE_data;
	char *node_USCORE_meta_USCORE_data;
};
#endif

#ifndef _SOAP_tns__setServiceData
#define _SOAP_tns__setServiceData
struct tns__setServiceData
{
	char *input;
};
#endif

#ifndef _SOAP_tns__Detach
#define _SOAP_tns__Detach
struct tns__Detach
{
  void *rubbish; /* ARPDBG */
};
#endif

#ifndef _SOAP_tns__GetStatus
#define _SOAP_tns__GetStatus
struct tns__GetStatus
{
  void *rubbish; /* ARPDBG */
};
#endif

#ifndef _SOAP_tns__GetNthDataSource
#define _SOAP_tns__GetNthDataSource
struct tns__GetNthDataSource
{
	char *input;
};
#endif

#ifndef _SOAP_tns__AppStop
#define _SOAP_tns__AppStop
struct tns__AppStop
{
  void *rubbish; /* ARPDBG */
};
#endif

#ifndef _SOAP_tns__Resume
#define _SOAP_tns__Resume
struct tns__Resume
{
  void *rubbish; /* ARPDBG */
};
#endif

#ifndef _SOAP_tns__GetControl
#define _SOAP_tns__GetControl
struct tns__GetControl
{
  void *rubbish; /* ARPDBG */
};
#endif

#ifndef _SOAP_tns__Stop
#define _SOAP_tns__Stop
struct tns__Stop
{
  void *rubbish; /* ARPDBG */
};
#endif

#ifndef _SOAP_tns__GetNotifications
#define _SOAP_tns__GetNotifications
struct tns__GetNotifications
{
  void *rubbish; /* ARPDBG */
};
#endif

#ifndef _SOAP_tns__Pause
#define _SOAP_tns__Pause
struct tns__Pause
{
  void *rubbish; /* ARPDBG */
};
#endif

#ifndef _SOAP_tns__PutStatus
#define _SOAP_tns__PutStatus
struct tns__PutStatus
{
	char *input;
};
#endif

#ifndef _SOAP_tns__AppStart
#define _SOAP_tns__AppStart
struct tns__AppStart
{
  void *rubbish; /* ARPDBG */
};
#endif

#ifndef _SOAP_tns__Destroy
#define _SOAP_tns__Destroy
struct tns__Destroy
{
  void *rubbish; /* ARPDBG */
};
#endif

#ifndef _SOAP_tns__PutControl
#define _SOAP_tns__PutControl
struct tns__PutControl
{
	char *input;
};
#endif

#ifndef _SOAP_SOAP_ENV__Header
#define _SOAP_SOAP_ENV__Header
/* SOAP Header: */
struct SOAP_ENV__Header
{
	void *dummy;	/* transient */
};
#endif

#ifndef _SOAP_SOAP_ENV__Code
#define _SOAP_SOAP_ENV__Code
/* SOAP Fault Code: */
struct SOAP_ENV__Code
{
	char *SOAP_ENV__Value;
	char *SOAP_ENV__Node;
	char *SOAP_ENV__Role;
};
#endif

#ifndef _SOAP_SOAP_ENV__Fault
#define _SOAP_SOAP_ENV__Fault
/* SOAP Fault: */
struct SOAP_ENV__Fault
{
	char *faultcode;
	char *faultstring;
	char *faultactor;
	char *detail;
	struct SOAP_ENV__Code *SOAP_ENV__Code;
	char *SOAP_ENV__Reason;
	char *SOAP_ENV__Detail;
};
#endif

/* Typedefs */
typedef char *xsd__string;
typedef char *xsd__integer;
typedef char *xsdl__DestroyResponse;
typedef char *xsdl__GetControlResponse;
typedef char *xsdl__PutControlRequest;
typedef char *xsdl__DestroyRequest;
typedef char *xsdl__setServiceDataRequest;
typedef char *xsdl__GetNotificationsResponse;
typedef char *xsdl__GetStatusResponse;
typedef char *xsdl__GetNotificationsRequest;
typedef char *xsdl__PauseRequest;
typedef char *xsdl__AppStopRequest;
typedef char *xsdl__AppStartResponse;
typedef char *xsdl__AppStopResponse;
typedef char *xsdl__ResumeRequest;
typedef char *xsdl__findServiceDataResponse;
typedef char *xsdl__DetachResponse;
typedef char *xsdl__StopResponse;
typedef char *xsdl__AppRecordChkpointResponse;
typedef char *xsdl__setServiceDataResponse;
typedef char *xsdl__GetStatusRequest;
typedef char *xsdl__findServiceDataRequest;
typedef char *xsdl__PauseResponse;
typedef char *xsdl__PutControlResponse;
typedef char *xsdl__ResumeResponse;
typedef char *xsdl__GetControlRequest;
typedef char *xsdl__AppRecordChkpointRequest;
typedef char *xsdl__AttachResponse;
typedef char *xsdl__PutStatusResponse;
typedef char *xsdl__AppDetachResponse;
typedef char *xsdl__AppStartRequest;
typedef char *xsdl__GetNthDataSourceResponse;
typedef char *xsdl__AttachRequest;
typedef char *xsdl__GetNthDataSourceRequest;
typedef char *xsdl__AppDetachRequest;
typedef char *xsdl__DetachRequest;
typedef char *xsdl__StopRequest;
typedef char *xsdl__PutStatusRequest;

/* Variables */

/* Remote Methods */

SOAP_FMAC1 int SOAP_FMAC2 tns__AppDetach(struct soap*, struct tns__AppDetachResponse *);

SOAP_FMAC1 int SOAP_FMAC2 tns__Attach(struct soap*, struct tns__AttachResponse *);

SOAP_FMAC1 int SOAP_FMAC2 tns__findServiceData(struct soap*, char *, struct tns__findServiceDataResponse *);

SOAP_FMAC1 int SOAP_FMAC2 tns__AppRecordChkpoint(struct soap*, char *, char *, struct tns__AppRecordChkpointResponse *);

SOAP_FMAC1 int SOAP_FMAC2 tns__setServiceData(struct soap*, char *, struct tns__setServiceDataResponse *);

SOAP_FMAC1 int SOAP_FMAC2 tns__Detach(struct soap*, struct tns__DetachResponse *);

SOAP_FMAC1 int SOAP_FMAC2 tns__GetStatus(struct soap*, struct tns__GetStatusResponse *);

SOAP_FMAC1 int SOAP_FMAC2 tns__GetNthDataSource(struct soap*, char *, struct tns__GetNthDataSourceResponse *);

SOAP_FMAC1 int SOAP_FMAC2 tns__AppStop(struct soap*, struct tns__AppStopResponse *);

SOAP_FMAC1 int SOAP_FMAC2 tns__Resume(struct soap*, struct tns__ResumeResponse *);

SOAP_FMAC1 int SOAP_FMAC2 tns__GetControl(struct soap*, struct tns__GetControlResponse *);

SOAP_FMAC1 int SOAP_FMAC2 tns__Stop(struct soap*, struct tns__StopResponse *);

SOAP_FMAC1 int SOAP_FMAC2 tns__GetNotifications(struct soap*, struct tns__GetNotificationsResponse *);

SOAP_FMAC1 int SOAP_FMAC2 tns__Pause(struct soap*, struct tns__PauseResponse *);

SOAP_FMAC1 int SOAP_FMAC2 tns__PutStatus(struct soap*, char *, struct tns__PutStatusResponse *);

SOAP_FMAC1 int SOAP_FMAC2 tns__AppStart(struct soap*, struct tns__AppStartResponse *);

SOAP_FMAC1 int SOAP_FMAC2 tns__Destroy(struct soap*, struct tns__DestroyResponse *);

SOAP_FMAC1 int SOAP_FMAC2 tns__PutControl(struct soap*, char *, struct tns__PutControlResponse *);

/* Stubs */

SOAP_FMAC1 int SOAP_FMAC2 soap_call_tns__AppDetach(struct soap*, const char*, const char*, struct tns__AppDetachResponse *);

SOAP_FMAC1 int SOAP_FMAC2 soap_call_tns__Attach(struct soap*, const char*, const char*, struct tns__AttachResponse *);

SOAP_FMAC1 int SOAP_FMAC2 soap_call_tns__findServiceData(struct soap*, const char*, const char*, char *, struct tns__findServiceDataResponse *);

SOAP_FMAC1 int SOAP_FMAC2 soap_call_tns__AppRecordChkpoint(struct soap*, const char*, const char*, char *, char *, struct tns__AppRecordChkpointResponse *);

SOAP_FMAC1 int SOAP_FMAC2 soap_call_tns__setServiceData(struct soap*, const char*, const char*, char *, struct tns__setServiceDataResponse *);

SOAP_FMAC1 int SOAP_FMAC2 soap_call_tns__Detach(struct soap*, const char*, const char*, struct tns__DetachResponse *);

SOAP_FMAC1 int SOAP_FMAC2 soap_call_tns__GetStatus(struct soap*, const char*, const char*, struct tns__GetStatusResponse *);

SOAP_FMAC1 int SOAP_FMAC2 soap_call_tns__GetNthDataSource(struct soap*, const char*, const char*, char *, struct tns__GetNthDataSourceResponse *);

SOAP_FMAC1 int SOAP_FMAC2 soap_call_tns__AppStop(struct soap*, const char*, const char*, struct tns__AppStopResponse *);

SOAP_FMAC1 int SOAP_FMAC2 soap_call_tns__Resume(struct soap*, const char*, const char*, struct tns__ResumeResponse *);

SOAP_FMAC1 int SOAP_FMAC2 soap_call_tns__GetControl(struct soap*, const char*, const char*, struct tns__GetControlResponse *);

SOAP_FMAC1 int SOAP_FMAC2 soap_call_tns__Stop(struct soap*, const char*, const char*, struct tns__StopResponse *);

SOAP_FMAC1 int SOAP_FMAC2 soap_call_tns__GetNotifications(struct soap*, const char*, const char*, struct tns__GetNotificationsResponse *);

SOAP_FMAC1 int SOAP_FMAC2 soap_call_tns__Pause(struct soap*, const char*, const char*, struct tns__PauseResponse *);

SOAP_FMAC1 int SOAP_FMAC2 soap_call_tns__PutStatus(struct soap*, const char*, const char*, char *, struct tns__PutStatusResponse *);

SOAP_FMAC1 int SOAP_FMAC2 soap_call_tns__AppStart(struct soap*, const char*, const char*, struct tns__AppStartResponse *);

SOAP_FMAC1 int SOAP_FMAC2 soap_call_tns__Destroy(struct soap*, const char*, const char*, struct tns__DestroyResponse *);

SOAP_FMAC1 int SOAP_FMAC2 soap_call_tns__PutControl(struct soap*, const char*, const char*, char *, struct tns__PutControlResponse *);

/* Skeletons */

SOAP_FMAC1 int SOAP_FMAC2 soap_serve(struct soap*);

SOAP_FMAC1 int SOAP_FMAC2 soap_serve_tns__AppDetach(struct soap*);

SOAP_FMAC1 int SOAP_FMAC2 soap_serve_tns__Attach(struct soap*);

SOAP_FMAC1 int SOAP_FMAC2 soap_serve_tns__findServiceData(struct soap*);

SOAP_FMAC1 int SOAP_FMAC2 soap_serve_tns__AppRecordChkpoint(struct soap*);

SOAP_FMAC1 int SOAP_FMAC2 soap_serve_tns__setServiceData(struct soap*);

SOAP_FMAC1 int SOAP_FMAC2 soap_serve_tns__Detach(struct soap*);

SOAP_FMAC1 int SOAP_FMAC2 soap_serve_tns__GetStatus(struct soap*);

SOAP_FMAC1 int SOAP_FMAC2 soap_serve_tns__GetNthDataSource(struct soap*);

SOAP_FMAC1 int SOAP_FMAC2 soap_serve_tns__AppStop(struct soap*);

SOAP_FMAC1 int SOAP_FMAC2 soap_serve_tns__Resume(struct soap*);

SOAP_FMAC1 int SOAP_FMAC2 soap_serve_tns__GetControl(struct soap*);

SOAP_FMAC1 int SOAP_FMAC2 soap_serve_tns__Stop(struct soap*);

SOAP_FMAC1 int SOAP_FMAC2 soap_serve_tns__GetNotifications(struct soap*);

SOAP_FMAC1 int SOAP_FMAC2 soap_serve_tns__Pause(struct soap*);

SOAP_FMAC1 int SOAP_FMAC2 soap_serve_tns__PutStatus(struct soap*);

SOAP_FMAC1 int SOAP_FMAC2 soap_serve_tns__AppStart(struct soap*);

SOAP_FMAC1 int SOAP_FMAC2 soap_serve_tns__Destroy(struct soap*);

SOAP_FMAC1 int SOAP_FMAC2 soap_serve_tns__PutControl(struct soap*);
#ifdef __cplusplus
}
#endif
#endif

/* end of soapStub.h */
