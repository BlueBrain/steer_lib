/*-----------------------------------------------------------------------
    This file contains routines and data structures for socket
    communication.

    (C)Copyright 2002 The University of Manchester, United Kingdom,
    all rights reserved.

    This software is produced by the Supercomputing, Visualization &
    e-Science Group, Manchester Computing, the Victoria University of
    Manchester as part of the RealityGrid project.

    This software has been tested with care but is not guaranteed for
    any particular purpose. Neither the copyright holder, nor the
    University of Manchester offer any warranties or representations,
    nor do they accept any liabilities with respect to this software.

    This software must not be used for commercial gain without the
    written permission of the authors.
    
    This software must not be redistributed without the written
    permission of the authors.

    Permission is granted to modify this software, provided any
    modifications are made freely available to the original authors.
 
    Supercomputing, Visualization & e-Science Group
    Manchester Computing
    University of Manchester
    Manchester M13 9PL
    
    WWW:    http://www.sve.man.ac.uk  
    email:  sve@man.ac.uk
    Tel:    +44 161 275 6095
    Fax:    +44 161 275 6800    

    Initial version by:  A Porter and R Haines, 29.1.2004      0.1  

-----------------------------------------------------------------------*/

#if REG_SOCKET_SAMPLES

#include "ReG_Steer_Common.h"
#include "ReG_Steer_Appside_internal.h"
#include "ReG_Steer_Appside_Sockets.h"
#include <string.h>
#include <signal.h>
#include <sys/time.h>

#ifdef _AIX
#include <fcntl.h>
#endif

#ifndef REG_DEBUG
#define REG_DEBUG 0
#endif

/* Need access to these tables which are actually declared in 
   ReG_Steer_Appside_internal.h */
extern IOdef_table_type IOTypes_table;

extern Steerer_connection_table_type Steerer_connection;

/*--------------------------------------------------------------------*/

int socket_info_init(const int index) {

  char* pchar;
  char* ip_addr;
  int min = 21370;
  int max = 65535;

  if(pchar = getenv("GLOBUS_TCP_PORT_RANGE")) {
    if(sscanf(pchar, "%d,%d", &(min), &(max)) != 2) {
      min = 21370;
      max = 65535;
    }
  }

#if REG_DEBUG
  fprintf(stderr, "socket_info_init: port range %d - %d\n", min, max);
#endif

  /* set local TCP interface to use */
  if(Get_fully_qualified_hostname(&pchar, &ip_addr) == REG_SUCCESS) {
    strcpy(IOTypes_table.io_def[index].socket_info.tcp_interface, ip_addr);
  }
  else {
    sprintf(IOTypes_table.io_def[index].socket_info.tcp_interface, " ");
  }

#if REG_DEBUG
  fprintf(stderr, "socket_info_init: local tcp interface: %s\n", IOTypes_table.io_def[index].socket_info.tcp_interface);
#endif

  IOTypes_table.io_def[index].socket_info.min_port = min;
  IOTypes_table.io_def[index].socket_info.max_port = max;
  IOTypes_table.io_def[index].socket_info.listener_port = 0;
  IOTypes_table.io_def[index].socket_info.listener_handle = -1;
  IOTypes_table.io_def[index].socket_info.listener_status = REG_COMMS_STATUS_NULL;
  IOTypes_table.io_def[index].socket_info.comms_status = REG_COMMS_STATUS_NULL;   
  IOTypes_table.io_def[index].socket_info.connector_port = 0;
  IOTypes_table.io_def[index].socket_info.connector_handle = -1;
  sprintf(IOTypes_table.io_def[index].socket_info.connector_hostname, " ");

  return REG_SUCCESS;
}

/*--------------------------------------------------------------------*/

void socket_info_cleanup(const int index) {

}

/*--------------------------------------------------------------------*/

int create_listener(const int index) {

  char* pchar;
  char* ip_addr;

  int listener;
  int yes = 1;
  int i;
  struct sockaddr_in myAddr;

  /* create and register listener */
  listener = socket(AF_INET, SOCK_STREAM, 0);
  if(listener == REG_SOCKETS_ERROR) {
    perror("socket");
    IOTypes_table.io_def[index].socket_info.comms_status=REG_COMMS_STATUS_FAILURE;
    return REG_FAILURE;
  }
  IOTypes_table.io_def[index].socket_info.listener_handle = listener;

  /* Turn off the "Address already in use" error message */
  if(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == REG_SOCKETS_ERROR) {
    perror("setsockopt");
    return REG_FAILURE;
  }

  /* Get the hostname of the machine we're running on (so we can publish
   * the endpoint of this connection). If REG_IO_ADDRESS is set then
   * that's what we use.  If not, call Get_fully_qualified_hostname which 
   * itself uses  REG_TCP_INTERFACE if set. */
  if(pchar = getenv("REG_IO_ADDRESS")){
    strcpy(IOTypes_table.io_def[index].socket_info.listener_hostname, pchar);
  }
  else if(Get_fully_qualified_hostname(&pchar, &ip_addr) == REG_SUCCESS){
    strcpy(IOTypes_table.io_def[index].socket_info.listener_hostname, pchar);
  }
  else{
    fprintf(stderr, "Sockets_create_listener: WARNING: failed to get hostname\n");
    sprintf(IOTypes_table.io_def[index].socket_info.listener_hostname, "NOT_SET");
  }
  
  /* set up server address */
  myAddr.sin_family = AF_INET;
  if(strlen(IOTypes_table.io_def[index].socket_info.tcp_interface) == 1) {
    myAddr.sin_addr.s_addr = INADDR_ANY;
  }
  else {
    inet_aton(IOTypes_table.io_def[index].socket_info.tcp_interface, &(myAddr.sin_addr));
  }
  memset(&(myAddr.sin_zero), '\0', 8); /* zero the rest */

  /* Now bind listener so we can accept connections when they happen */
  i = IOTypes_table.io_def[index].socket_info.min_port;
  myAddr.sin_port = htons((short) i);

  while(bind(listener, (struct sockaddr*) &myAddr, sizeof(struct sockaddr)) == REG_SOCKETS_ERROR) {
    if(++i > IOTypes_table.io_def[index].socket_info.max_port) {
      perror("bind");
      close(listener);
      IOTypes_table.io_def[index].socket_info.comms_status=REG_COMMS_STATUS_FAILURE;
      return REG_FAILURE;
    }
    myAddr.sin_port = htons((short) i);
  }
  /* we're bound, so save the port number we're using */
  IOTypes_table.io_def[index].socket_info.listener_port = i;

  /* now we need to actually listen */
  if(listen(listener, 10) == REG_SOCKETS_ERROR) {
    perror("listen");
    close(listener);
    IOTypes_table.io_def[index].socket_info.comms_status = REG_COMMS_STATUS_FAILURE;
    IOTypes_table.io_def[index].socket_info.listener_status = REG_COMMS_STATUS_FAILURE;
    return REG_FAILURE;
  }

  /* we are listening! */
  IOTypes_table.io_def[index].socket_info.listener_status = REG_COMMS_STATUS_LISTENING;
  IOTypes_table.io_def[index].socket_info.comms_status = REG_COMMS_STATUS_LISTENING;

  return REG_SUCCESS;
}

/*--------------------------------------------------------------------*/

int create_connector(const int index) {

  int i;
  int yes = 1;
  int connector;
  struct sockaddr_in myAddr;

  /* create connector*/
  connector = socket(AF_INET, SOCK_STREAM, 0);
  if(connector == REG_SOCKETS_ERROR) {
    /* problem! */
    perror("socket");
    IOTypes_table.io_def[index].socket_info.comms_status=REG_COMMS_STATUS_FAILURE;
    return REG_FAILURE;
  }
  /* all okay, so save connector handle */
  IOTypes_table.io_def[index].socket_info.connector_handle = connector;

  /* ...turn off the "Address already in use" error message... */
  if(setsockopt(connector, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == REG_SOCKETS_ERROR) {
    perror("setsockopt");
    return REG_FAILURE;
  }

  /* ...build local address struct... */
  myAddr.sin_family = AF_INET;
  if(strlen(IOTypes_table.io_def[index].socket_info.tcp_interface) == 1) {
    myAddr.sin_addr.s_addr = INADDR_ANY;
  }
  else {
    inet_aton(IOTypes_table.io_def[index].socket_info.tcp_interface, &(myAddr.sin_addr));
  }
  memset(&(myAddr.sin_zero), '\0', 8); /* zero the rest */

  /* ...and bind connector so we can punch out of firewalls... */
  i = IOTypes_table.io_def[index].socket_info.min_port;
  myAddr.sin_port = htons((short) i);

  while(bind(connector, (struct sockaddr*) &myAddr, sizeof(struct sockaddr)) == REG_SOCKETS_ERROR) {
    if(++i > IOTypes_table.io_def[index].socket_info.max_port) {
      perror("bind");
      close(connector);
      IOTypes_table.io_def[index].socket_info.comms_status=REG_COMMS_STATUS_FAILURE;
      return REG_FAILURE;
    }
    myAddr.sin_port = htons((short) i);
  }
  IOTypes_table.io_def[index].socket_info.comms_status=REG_COMMS_STATUS_WAITING_TO_CONNECT;

  /* might as well try to connect now... */
  connect_connector(index);

  return REG_SUCCESS;
}

/*--------------------------------------------------------------------*/

int connect_connector(const int index) {

  struct sockaddr_in theirAddr;
  int connector = IOTypes_table.io_def[index].socket_info.connector_handle;

  int return_status = REG_SUCCESS;

  /* get a remote address if we need to */
  if(IOTypes_table.io_def[index].socket_info.connector_port == 0) {
#if REG_SOAP_STEERING	  
    /* Go out into the world of grid services... */
    return_status = Get_data_source_address_soap(IOTypes_table.io_def[index].input_index, 
						 IOTypes_table.io_def[index].socket_info.connector_hostname,
						 &(IOTypes_table.io_def[index].socket_info.connector_port));
#else
    /* get hostname and port from environment variables */
    return_status = Get_data_source_address_file(IOTypes_table.io_def[index].input_index, 
						 IOTypes_table.io_def[index].socket_info.connector_hostname,
						 &(IOTypes_table.io_def[index].socket_info.connector_port));
#endif /* !REG_SOAP_STEERING */
  }

  if(return_status == REG_SUCCESS && 
     IOTypes_table.io_def[index].socket_info.connector_port != 0) {

    /* ...look up and then build remote address struct... */
    if(dns_lookup(IOTypes_table.io_def[index].socket_info.connector_hostname) == REG_FAILURE) {
      fprintf(stderr, "connect_connector: Could not resolve hostname <%s>\n", 
	      IOTypes_table.io_def[index].socket_info.connector_hostname);
      return REG_FAILURE;
    }

    theirAddr.sin_family = AF_INET;
    theirAddr.sin_port = htons(IOTypes_table.io_def[index].socket_info.connector_port);
    inet_aton(IOTypes_table.io_def[index].socket_info.connector_hostname, &(theirAddr.sin_addr));
    memset(&(theirAddr.sin_zero), '\0', 8); /* zero the rest */
    
    /* ...finally connect to the remote address! */
    if(connect(connector, (struct sockaddr*) &theirAddr, sizeof(struct sockaddr)) == REG_SOCKETS_ERROR) {
      perror("connect");
      IOTypes_table.io_def[index].socket_info.connector_port = 0;
      return REG_FAILURE;
    }
    IOTypes_table.io_def[index].socket_info.comms_status=REG_COMMS_STATUS_CONNECTED;
  }
  else {
    fprintf(stderr, "connect_connector: cannot get remote address\n");
  }

  return return_status;  
}

/*--------------------------------------------------------------------*/

void cleanup_listener_connection(const int index) {
  if(IOTypes_table.io_def[index].socket_info.listener_status == REG_COMMS_STATUS_LISTENING) {
    close_listener_handle(index);
  }

  if(IOTypes_table.io_def[index].socket_info.comms_status == REG_COMMS_STATUS_CONNECTED) { 
    close_connector_handle(index);
  }

  /* Flag that this listener is dead - used in Emit_IOType_defs */
  sprintf(IOTypes_table.io_def[index].socket_info.listener_hostname, "NOT_SET");
  IOTypes_table.io_def[index].socket_info.listener_port = 0;
}

/*--------------------------------------------------------------------*/

void cleanup_connector_connection(const int index) {
  if (IOTypes_table.io_def[index].socket_info.comms_status == REG_COMMS_STATUS_CONNECTED ||
      IOTypes_table.io_def[index].socket_info.comms_status == REG_COMMS_STATUS_WAITING_TO_CONNECT ){
    close_connector_handle(index);
  }
}

/*--------------------------------------------------------------------*/

void close_listener_handle(const int index) {
  if(close(IOTypes_table.io_def[index].socket_info.listener_handle) == REG_SOCKETS_ERROR) {
    perror("close");
    IOTypes_table.io_def[index].socket_info.listener_status = REG_COMMS_STATUS_FAILURE;
  }
  else {
#if REG_DEBUG
    fprintf(stderr, "close_listener_handle: close OK\n");
#endif
    IOTypes_table.io_def[index].socket_info.listener_status = REG_COMMS_STATUS_NULL;
  }
}

/*--------------------------------------------------------------------*/

void close_connector_handle(const int index) {
  if(close(IOTypes_table.io_def[index].socket_info.connector_handle) == REG_SOCKETS_ERROR) {
    perror("close");
    IOTypes_table.io_def[index].socket_info.comms_status = REG_COMMS_STATUS_FAILURE;
  }
  else {
#if REG_DEBUG
    fprintf(stderr, "close_connector_handle: close OK\n");
#endif
    IOTypes_table.io_def[index].socket_info.comms_status = REG_COMMS_STATUS_NULL;
  }
}

/*--------------------------------------------------------------------*/

void attempt_listener_connect(const int index) {
  socket_io_type *socket_info;
  socket_info = &(IOTypes_table.io_def[index].socket_info);

  if(socket_info->listener_status != REG_COMMS_STATUS_LISTENING) {
#if REG_DEBUG
    fprintf(stderr, "attempt_listener_connect:dealing with listener_status not LISTENING \n");
#endif
    if (socket_info->listener_status == REG_COMMS_STATUS_FAILURE ||
	socket_info->listener_status == REG_COMMS_STATUS_NULL ) {
      create_listener(index);
    }
  }

  /* only check comms_status if listener_status is now listening */
  if (socket_info->listener_status == REG_COMMS_STATUS_LISTENING) {
    if (socket_info->comms_status == REG_COMMS_STATUS_LISTENING) {
#if REG_DEBUG
      fprintf(stderr, "attempt_listener_connect: poll\n");
#endif
      poll(index);
    }

    if (socket_info->comms_status == REG_COMMS_STATUS_FAILURE ||
	socket_info->comms_status == REG_COMMS_STATUS_NULL ) {
      /* connection has broken - we're still listening so see if 
         anything to connect */
#if REG_DEBUG
      fprintf(stderr, "attempt_listener_connect: retry accept connect\n");
#endif
      retry_accept_connect(index);
    }
  }
}

/*--------------------------------------------------------------------*/

void retry_accept_connect(const int index) {
  socket_io_type *socket_info;
  socket_info = &(IOTypes_table.io_def[index].socket_info);

  /* if this is called, a write has failed */

  /* if we're here and  status is connected  we've lost a connection, 
     so first close socket */
  if (socket_info->comms_status==REG_COMMS_STATUS_CONNECTED) {
    close_connector_handle(index);
  }

  poll(index);
}

/*--------------------------------------------------------------------*/

void attempt_connector_connect(const int index) {
  socket_io_type *socket_info;
  socket_info = &(IOTypes_table.io_def[index].socket_info);

  if(socket_info->comms_status == REG_COMMS_STATUS_WAITING_TO_CONNECT) {
#if REG_DEBUG    
    fprintf(stderr, "attempt_connector_connect: poll\n");
#endif
    poll(index);
  }

  if(socket_info->comms_status == REG_COMMS_STATUS_FAILURE || 
     socket_info->comms_status == REG_COMMS_STATUS_NULL) {
    /* connection has broken - try to re-connect */
#if REG_DEBUG
    fprintf(stderr, "attempt_connector_connect: retry connect\n");
#endif
    retry_connect(index);
  }
}

/*--------------------------------------------------------------------*/

void retry_connect(const int index) {
  socket_io_type *socket_info;
  socket_info = &(IOTypes_table.io_def[index].socket_info);

  /* close the failed connector and retry to connect */
  if(socket_info->comms_status == REG_COMMS_STATUS_CONNECTED) {
    /* Reset connector port (to force us to go and look for it
       again in case it has changed) */
    socket_info->connector_port = 0;
    close_connector_handle(index);
  }

  if(create_connector(index) != REG_SUCCESS) {
#if REG_DEBUG
    fprintf(stderr, "retry_connect: failed to register connector for IOType\n");
#endif
    /* Set to FAILURE so create_connector is attempted again next time round */
    socket_info->comms_status=REG_COMMS_STATUS_FAILURE;
  }
}

/*--------------------------------------------------------------------*/

void poll(const int index) {

  struct timeval timeout;
  fd_set sockets;	/* the set of sockets to check */
  int fd_max;		/* the max socket number */

  int listener = IOTypes_table.io_def[index].socket_info.listener_handle;
  int connector = IOTypes_table.io_def[index].socket_info.connector_handle;
  int direction = IOTypes_table.io_def[index].direction;

  timeout.tv_sec  = 0;
  timeout.tv_usec = 0;

  /* just return if we have no handles */
  if((listener == -1) && (connector == -1)) return;

  /* clear the socket set and add required handles */
  FD_ZERO(&sockets);
  if(direction == REG_IO_OUT) {
    /* SERVER */
    if(IOTypes_table.io_def[index].socket_info.listener_status == REG_COMMS_STATUS_LISTENING) {
      FD_SET(listener, &sockets);
      fd_max = listener;

      /* poll using select() */
#if REG_DEBUG
      fprintf(stderr, "poll: polling for accept\n");
#endif
      if(select(fd_max + 1, &sockets, NULL, NULL, &timeout) == -1) {
	perror("select");
	return;
      }

      /* see if anything needs doing */
      if(FD_ISSET(listener, &sockets)) {
	/* new connection */
	struct sockaddr_in theirAddr;
	unsigned int addrlen = sizeof(theirAddr);
	int new_fd = accept(listener, (struct sockaddr*) &theirAddr, &addrlen);
	if(new_fd == REG_SOCKETS_ERROR) {
	  perror("accept");
	  return;
	}
	IOTypes_table.io_def[index].socket_info.connector_handle = new_fd;
	IOTypes_table.io_def[index].socket_info.comms_status=REG_COMMS_STATUS_CONNECTED;
      }
    }
  }
  else if(direction == REG_IO_IN) {
    /* CLIENT */
    if(IOTypes_table.io_def[index].socket_info.comms_status != REG_COMMS_STATUS_CONNECTED) {
      connect_connector(index);
    }
  }
}

/*--------------------------------------------------------------------*/

int dns_lookup(char* hostname) {
  struct hostent* host;

  if((host = gethostbyname(hostname)) == NULL) {
    herror("gethostbyname");
    return REG_FAILURE;
  }

#if REG_DEBUG
  fprintf(stderr, "DNS lookup: host: %s\n", host->h_name);
  fprintf(stderr, "            IP:   %s\n", inet_ntoa(*((struct in_addr*) host->h_addr)));
#endif

  /* This next bit must be done with a sprintf for AIX...
   * It can be done with a strcpy on Linux or IRIX...      */
  sprintf(hostname, "%s", inet_ntoa(*((struct in_addr*) host->h_addr)));
  return REG_SUCCESS;
}

/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*
 *                         EXTERNAL METHODS                           *
 *--------------------------------------------------------------------*/

int Initialize_IOType_transport_sockets(const int direction, const int index) {

  int return_status = REG_SUCCESS;
  static int call_count = 1;

  /* set up socket info stuff */
  if(socket_info_init(index) != REG_SUCCESS) {
#if REG_DEBUG
    fprintf(stderr, "Initialize_IOType_transport_sockets: failed to init socket info for IOType\n");
#endif
    return_status = REG_FAILURE;
  }
  else {
    if(direction == REG_IO_OUT) {

      /* Don't create socket yet if this flag is set */
      if(IOTypes_table.enable_on_registration == FALSE) return REG_SUCCESS;

      /* open socket and register callback function to listen for and
	 accept connections */
      if(create_listener(index) != REG_SUCCESS) {
#if REG_DEBUG
	fprintf(stderr, "Initialize_IOType_transport_sockets: failed to create listener for IOType\n");
#endif
	return_status = REG_FAILURE;
      }
#if REG_DEBUG
      else {
	fprintf(stderr, "Initialize_IOType_transport_sockets: Created listener on port %d, index %d, label %s\n", 
		IOTypes_table.io_def[index].socket_info.listener_port, 
		index, IOTypes_table.io_def[index].label );
      }
#endif
    }
    else if(direction == REG_IO_IN) {

      IOTypes_table.io_def[index].input_index = call_count;
      call_count++;
	
      /* Don't create socket yet if this flag is set */
      if(IOTypes_table.enable_on_registration == FALSE) {
	return REG_SUCCESS;
      }

      if(create_connector(index) != REG_SUCCESS) {
#if REG_DEBUG
	fprintf(stderr, "Initialize_IOType_transport_sockets: failed to register connector for IOType\n");
#endif
	return_status = REG_FAILURE;
      }
#if REG_DEBUG
      else {
	fprintf(stderr, "Initialize_IOType_transport_sockets: registered connector on port %d, hostname = %s, index %d, label %s\n", 
		IOTypes_table.io_def[index].socket_info.connector_port,
		IOTypes_table.io_def[index].socket_info.connector_hostname,
		index, IOTypes_table.io_def[index].label );
      }
#endif
      
    }
  }

  return return_status;
}

/*---------------------------------------------------*/

void Finalize_IOType_transport_sockets() {
  int index;

  for(index = 0; index < IOTypes_table.num_registered; index++) {
    if(IOTypes_table.io_def[index].direction == REG_IO_OUT) {
      /* close sockets */
      cleanup_listener_connection(index);
      socket_info_cleanup(index);
    }
    else if(IOTypes_table.io_def[index].direction == REG_IO_IN) {
      /* close sockets */
      cleanup_connector_connection(index);
      socket_info_cleanup(index);
    }
  }
}

/*---------------------------------------------------*/

int Disable_IOType_sockets(const int index) {
  /* check index is valid */
  if(index < 0 || index >= IOTypes_table.num_registered) {
    fprintf(stderr, "Disable_IOType_sockets: index out of range\n");
    return REG_FAILURE;
  }

  if(IOTypes_table.io_def[index].direction == REG_IO_OUT) {
    /* close sockets */
    cleanup_listener_connection(index);
  }
  else if(IOTypes_table.io_def[index].direction == REG_IO_IN) {
    /* close sockets */
    cleanup_connector_connection(index);
  }

  return REG_SUCCESS;
}

/*---------------------------------------------------*/

int Enable_IOType_sockets(int index) {
  /* check index is valid */
  if(index < 0 || index >= IOTypes_table.num_registered) return REG_FAILURE;

  if(IOTypes_table.io_def[index].direction == REG_IO_OUT) {
    /* open socket and register callback function to listen for and
       accept connections */
    if(create_listener(index) != REG_SUCCESS) {
#if REG_DEBUG
      fprintf(stderr, "Enable_IOType_sockets: failed to create listener for IOType\n");
#endif
      return REG_FAILURE;
    }
  }
  else if(IOTypes_table.io_def[index].direction == REG_IO_IN) {
    if (create_connector(index) != REG_SUCCESS) {
#if REG_DEBUG
      fprintf(stderr, "Enable_IOType_sockets: failed to register connector for IOType\n");
#endif
      return REG_FAILURE;
    }
  }

  return REG_SUCCESS;
}

/*---------------------------------------------------*/

int Get_communication_status_sockets(const int index) {
  if(IOTypes_table.io_def[index].socket_info.comms_status != REG_COMMS_STATUS_CONNECTED)
    return REG_FAILURE;
  
  return REG_SUCCESS;
}

/*---------------------------------------------------*/

int Write_sockets(const int index, const int size, void* buffer) {

  int bytes_left;
  int result;
  char* pchar;
  int connector = IOTypes_table.io_def[index].socket_info.connector_handle;

  if(size < 0) {
    fprintf(stderr, "Write_sockets: requested to write < 0 bytes!\n");
    return REG_FAILURE;
  }
  else if(size == 0) {
    fprintf(stderr, "Write_sockets: asked to send 0 bytes!\n");
    return REG_SUCCESS;
  }

  bytes_left = size;
  pchar = (char*) buffer;

#if REG_DEBUG
  fprintf(stderr, "Writing...\n");
#endif
  while(bytes_left > 0) {
#ifndef __linux
    result = send(connector, pchar, bytes_left, 0);
#else
    result = send(connector, pchar, bytes_left, MSG_NOSIGNAL);
#endif
    if(result == REG_SOCKETS_ERROR) {
      perror("send");
      return REG_FAILURE;
    }
    else {
      bytes_left -= result;
      pchar += result;
    }
  }

  if(bytes_left > 0) {
#if REG_DEBUG
    fprintf(stderr, "Write_sockets: timed-out trying to write data\n");
#endif
    return REG_TIMED_OUT;
  }

  return REG_SUCCESS;
}

/*---------------------------------------------------*/

int Write_non_blocking_sockets(const int index, const int size, void* buffer) {

  struct timeval timeout;
  int connector = IOTypes_table.io_def[index].socket_info.connector_handle;
  fd_set sock;

  timeout.tv_sec  = 0;
  timeout.tv_usec = 0;

  FD_ZERO(&sock);
  FD_SET(connector, &sock);

  if(select(connector + 1, NULL, &sock, NULL, &timeout) == -1) {
    perror("select");
    return REG_FAILURE;
  }

  /* are we free to write? */
  if(FD_ISSET(connector, &sock)) {
    return Write_sockets(index, size, buffer);
  }

  return REG_FAILURE;
}

/*---------------------------------------------------*/

int Emit_header_sockets(const int index) {

  char buffer[REG_PACKET_SIZE];
  int status;

  /* check if socket connection has been made */
  if(IOTypes_table.io_def[index].socket_info.comms_status != REG_COMMS_STATUS_CONNECTED) {
    attempt_listener_connect(index);
  }

  /* now are we connected? */
  if(IOTypes_table.io_def[index].socket_info.comms_status == REG_COMMS_STATUS_CONNECTED) {
#if REG_DEBUG
    fprintf(stderr, "Emit_header_sockets: socket status is connected, index = %d\n", index );
#endif

    /* send header */
    sprintf(buffer, REG_PACKET_FORMAT, REG_DATA_HEADER);
    buffer[REG_PACKET_SIZE - 1] = '\0';
#if REG_DEBUG
    fprintf(stderr, "Emit_header_sockets: Sending >>%s<<\n", buffer);
#endif
    status = Write_non_blocking_sockets(index, REG_PACKET_SIZE, (void*) buffer);

    if(status == REG_SUCCESS) {
#if REG_DEBUG
      fprintf(stderr, "Emit_header_sockets: Sent %d bytes\n", REG_PACKET_SIZE);
#endif
      return REG_SUCCESS;
    }
    else if(status == REG_FAILURE) {
#if REG_DEBUG
      fprintf(stderr, "Emit_header_sockets: Write_sockets failed - immediate retry connect\n");
#endif
      retry_accept_connect(index);

      if(IOTypes_table.io_def[index].socket_info.comms_status == REG_COMMS_STATUS_CONNECTED) {
#if REG_DEBUG
	fprintf(stderr, "Emit_header_sockets: Sending >>%s<<\n", buffer);
#endif    
	if(Write_sockets(index, REG_PACKET_SIZE, (void*) buffer) == REG_SUCCESS) {
	  return REG_SUCCESS;
	}
      }
    }
#if REG_DEBUG
    else{
      fprintf(stderr, "Emit_header_sockets: attempt to write to socket timed out\n");
    }
#endif
  }
#if REG_DEBUG
  else {
    fprintf(stderr, "Emit_header_sockets: socket not connected, index = %d\n", index );
  }
#endif

  return REG_FAILURE;
}
/*---------------------------------------------------*/

int Emit_data_sockets(const int index, const size_t num_bytes_to_send, void* pData) {
  if(Write_sockets(index, num_bytes_to_send, (void*) pData) != REG_SUCCESS) {
    fprintf(stderr, "Emit_data_sockets: error in send\n");
    return REG_FAILURE;
  }

#if REG_DEBUG
  fprintf(stderr, "Emit_data_sockets: sent %d bytes...\n", (int) num_bytes_to_send);
#endif
  
  return REG_SUCCESS;
}


/*---------------------------------------------------*/

int Consume_msg_header_sockets(int index, int* datatype, int* count, int* num_bytes, int* is_fortran_array) {

  int nbytes;
  char buffer[REG_PACKET_SIZE];
  socket_io_type  *sock_info;
  sock_info = &(IOTypes_table.io_def[index].socket_info);

  /* check socket connection has been made */
  if (sock_info->comms_status != REG_COMMS_STATUS_CONNECTED) return REG_FAILURE;

  /* Read header */
#if REG_DEBUG
  fprintf(stderr, "Consume_msg_header_sockets: calling recv...\n");
#endif

  /* Blocks until REG_PACKET_SIZE bytes received */
  if((nbytes = recv(sock_info->connector_handle, buffer, REG_PACKET_SIZE, MSG_WAITALL)) <= 0) {
    if(nbytes == 0) {
      /* closed connection */
      fprintf(stderr, "Consume_msg_header_sockets: hung up!\n");
    }
    else {
      /* error */
      perror("recv");
    }

    return REG_FAILURE;
  }

  /* if we're here, we've got data */
#if REG_DEBUG
  fprintf(stderr, "Consume_msg_header_sockets: read <%s> from socket\n", buffer);
#endif

  /* Check for end of data */
  if(!strncmp(buffer, REG_DATA_FOOTER, strlen(REG_DATA_FOOTER))) {
    return REG_EOD;
  }
  else if(strncmp(buffer, BEGIN_SLICE_HEADER, strlen(BEGIN_SLICE_HEADER))) {
    fprintf(stderr, "Consume_msg_header_sockets: incorrect header on slice\n");
    return REG_FAILURE;
  }

  /*--- Type of objects in message ---*/
  if((nbytes = recv(sock_info->connector_handle, buffer, REG_PACKET_SIZE, MSG_WAITALL)) <= 0) {
    if(nbytes == 0) {
      /* closed connection */
      fprintf(stderr, "Consume_msg_header_sockets: hung up!\n");
    }
    else {
      /* error */
      perror("recv");
    }

    return REG_FAILURE;
  }

#if REG_DEBUG
  fprintf(stderr, "Consume_msg_header_sockets: read <%s> from socket\n", buffer);
#endif

  if(!strstr(buffer, "<Data_type>")) {
    return REG_FAILURE;
  }

  sscanf(buffer, "<Data_type>%d</Data_type>", datatype);

  /*--- No. of objects in message ---*/
  if((nbytes = recv(sock_info->connector_handle, buffer, REG_PACKET_SIZE, MSG_WAITALL)) <= 0) {
    if(nbytes == 0) {
      /* closed connection */
      fprintf(stderr, "Consume_msg_header_sockets: hung up!\n");
    }
    else {
      /* error */
      perror("recv");
    }

    return REG_FAILURE;
  }

#if REG_DEBUG
  fprintf(stderr, "Consume_msg_header_sockets: read <%s> from socket\n", buffer);
#endif

  if(!strstr(buffer, "<Num_objects>")) {
    return REG_FAILURE;
  }

  if(sscanf(buffer, "<Num_objects>%d</Num_objects>", count) != 1){
    fprintf(stderr, "Consume_msg_header_sockets: failed to read Num_objects\n");
    return REG_FAILURE;
  }

  /*--- No. of bytes in message ---*/
  if((nbytes = recv(sock_info->connector_handle, buffer, REG_PACKET_SIZE, MSG_WAITALL)) <= 0) {
    if(nbytes == 0) {
      /* closed connection */
      fprintf(stderr, "Consume_msg_header_sockets: hung up!\n");
    }
    else {
      /* error */
      perror("recv");
    }

    return REG_FAILURE;
  }

#if REG_DEBUG
  fprintf(stderr, "Consume_msg_header_sockets: read >%s< from socket\n", buffer);
#endif

  if(!strstr(buffer, "<Num_bytes>")) {
    return REG_FAILURE;
  }

  if(sscanf(buffer, "<Num_bytes>%d</Num_bytes>", num_bytes) != 1) {
    fprintf(stderr, "Consume_msg_header_sockets: failed to read Num_bytes\n");
    return REG_FAILURE;
  }

  /*--- Array ordering in message ---*/
  if((nbytes = recv(sock_info->connector_handle, buffer, REG_PACKET_SIZE, MSG_WAITALL)) <= 0) {
    if(nbytes == 0) {
      /* closed connection */
      fprintf(stderr, "Consume_msg_header_sockets: hung up!\n");
    }
    else {
      /* error */
      perror("recv");
    }

    return REG_FAILURE;
  }

#if REG_DEBUG
  fprintf(stderr, "Consume_msg_header_socket: read >%s< from socket\n", buffer);
#endif

  if(!strstr(buffer, "<Array_order>")) {
    return REG_FAILURE;
  }

  if(strstr(buffer, "FORTRAN")){
    /* Array data is from Fortran */
    *is_fortran_array = TRUE;
  }
  else{
    /* Array data is not from Fortran */
    *is_fortran_array = FALSE;
  }

  /*--- End of header ---*/
  if((nbytes = recv(sock_info->connector_handle, buffer, REG_PACKET_SIZE, MSG_WAITALL)) <= 0) {
    if(nbytes == 0) {
      /* closed connection */
      fprintf(stderr, "Consume_msg_header_sockets: hung up!\n");
    }
    else {
      /* error */
      perror("recv");
    }

    return REG_FAILURE;
  }

#if REG_DEBUG
  fprintf(stderr, "Consume_msg_header_sockets: read <%s> from socket\n", buffer);
#endif

  if(strncmp(buffer, END_SLICE_HEADER, strlen(END_SLICE_HEADER))) {
    fprintf(stderr, "Consume_msg_header_sockets: failed to find end of header\n");
    return REG_FAILURE;
  }

  return REG_SUCCESS;
}

/*---------------------------------------------------*/

int Consume_start_data_check_sockets(const int index) {  
  char buffer[REG_PACKET_SIZE];
  char* pstart;
  int nbytes, nbytes1;
  int attempt_reconnect;

  socket_io_type  *sock_info;
  sock_info = &(IOTypes_table.io_def[index].socket_info);

  /* if not connected attempt to connect now */
  if(IOTypes_table.io_def[index].socket_info.comms_status != REG_COMMS_STATUS_CONNECTED) {
    attempt_connector_connect(index);
  }

  /* check if socket connection has been made */
  if(IOTypes_table.io_def[index].socket_info.comms_status != REG_COMMS_STATUS_CONNECTED) {
#if REG_DEBUG
    fprintf(stderr, "Consume_start_data_check_socket: socket is NOT connected, index = %d\n", index);
#endif
    return REG_FAILURE;
  }

#if REG_DEBUG
  fprintf(stderr, "Consume_start_data_check_socket: socket status is connected, index = %d\n", index);
#endif

  /* Drain socket until start tag found */
  attempt_reconnect = 1;
  memset(buffer, '\0', 1);

#if REG_DEBUG
  fprintf(stderr, "Consume_start_data_check_socket: searching for start tag\n");
#endif

  while(!(pstart = strstr(buffer, REG_DATA_HEADER))) {

#ifdef _AIX

    /* So it looks like AIX blocks by default... So make the socket
     * non-blocking as we can't control this with flags to recv() in AIX... */
    fcntl(sock_info->connector_handle, F_SETFL, fcntl(sock_info->connector_handle, F_GETFL)|O_NONBLOCK);

    nbytes = recv(sock_info->connector_handle, buffer, REG_PACKET_SIZE, 0);

    /* ...And turn off non-blocking again... */
    fcntl(sock_info->connector_handle, F_SETFL, fcntl(sock_info->connector_handle, F_GETFL)&~O_NONBLOCK);

    if(nbytes <= 0) {

#else

    if((nbytes = recv(sock_info->connector_handle, buffer, REG_PACKET_SIZE, MSG_DONTWAIT)) <= 0) {

#endif

      if(nbytes == 0) {
	/* closed connection */
	fprintf(stderr, "Consume_start_data_check_sockets: hung up!\n");
      }
      else if(errno == EAGAIN) {
	/* Call would have blocked because no data to read */
#if REG_DEBUG
	fprintf(stderr, "\n");
#endif
	/* Call was OK but there's no data to read... */
	return REG_FAILURE;
      }
      else {
	/* Some error occurred */
#if REG_DEBUG
	fprintf(stderr, "\n");
#endif
	perror("recv");
      }

      /* We're in the middle of a while loop here so don't keep trying
	 to reconnect ad infinitum */
      if(!attempt_reconnect) {
	return REG_FAILURE;
      }

#if REG_DEBUG
      fprintf(stderr, "\nConsume_start_data_check_sockets: recv failed - "
	      "try immediate reconnect for index %d\n", index);
#endif

      retry_connect(index);

      /* check if socket reconnection has been made and check for 
	 data if it has */
      if (IOTypes_table.io_def[index].socket_info.comms_status != REG_COMMS_STATUS_CONNECTED) {
	return REG_FAILURE;
      }
    
      attempt_reconnect = 0;
      memset(buffer, '\0', 1);
    }

#if REG_DEBUG
    fprintf(stderr, "!");
#endif
  } /* !while */

#if REG_DEBUG
  fprintf(stderr, "\n");
#endif

  if(nbytes > 0) {

#if REG_DEBUG
    fprintf(stderr, "Consume_start_data_check_sockets: read >>%s<< from socket\n", buffer);
#endif

    /* Need to make sure we've read the full packet marking the 
       beginning of the next data slice */
    nbytes1 = (int) (pstart - buffer) + (REG_PACKET_SIZE - nbytes);

    if(nbytes1 > 0) {

      if((nbytes = recv(sock_info->connector_handle, buffer, 
			nbytes1, 0)) <= 0) {
	if(nbytes == 0) {
	  /* closed connection */
	  fprintf(stderr, "Consume_start_data_check_sockets: hung up!\n");
	}
	else {
	  /* error */
	  perror("recv");
	}

	if(nbytes != nbytes1) {
	  fprintf(stderr, "Consume_start_data_check_sockets: failed to read remaining %d bytes of header\n", (int) nbytes1);
	  return REG_FAILURE;
	}
      }
    }

    IOTypes_table.io_def[index].buffer_bytes = REG_IO_BUFSIZE;
    IOTypes_table.io_def[index].buffer = (void*) malloc(REG_IO_BUFSIZE);
    if(!IOTypes_table.io_def[index].buffer) {
      IOTypes_table.io_def[index].buffer_bytes = 0;
      fprintf(stderr, "Consume_start_data_check_sockets: malloc of IO buffer failed\n");
      return REG_FAILURE;
    }

    return REG_SUCCESS;
  }
  return REG_FAILURE;
}

/*--------------------------------------------------------------*/

int Consume_data_read_sockets(const int index, const int datatype, const int num_bytes_to_read, void *pData) {
  int nbytes;

  socket_io_type  *sock_info;

#if REG_DEBUG

#ifdef USE_REG_TIMING
  double time0, time1;
#endif

  fprintf(stderr, "Consume_data_read_sockets: calling recv for %d bytes\n", (int) num_bytes_to_read);

#ifdef USE_REG_TIMING
  Get_current_time_seconds(&time0);
#endif

#endif /* REG_DEBUG */

  sock_info = &(IOTypes_table.io_def[index].socket_info);

  if(IOTypes_table.io_def[index].use_xdr || IOTypes_table.io_def[index].convert_array_order == TRUE) {
    nbytes = recv(sock_info->connector_handle, IOTypes_table.io_def[index].buffer, num_bytes_to_read, MSG_WAITALL);
  }
  else {
    nbytes = recv(sock_info->connector_handle, pData, num_bytes_to_read, MSG_WAITALL);
  }

#if REG_DEBUG
  fprintf(stderr, "Consume_data_read_sockets: recv read %d bytes\n", (int) nbytes);

#ifdef USE_REG_TIMING
  Get_current_time_seconds(&time1);
  fprintf(stderr, "                          in %.3f seconds\n", (float) (time1-time0));
#endif

  if(datatype == REG_CHAR) {
    fprintf(stderr, "Consume_data_read_sockets: got char data:\n>>%s<<\n", (char *) pData);
  }
#endif /* REG_DEBUG */

  if(nbytes <= 0) {
      if(nbytes == 0) {
	/* closed connection */
	fprintf(stderr, "Consume_data_read_sockets: hung up!\n");
      }
      else {
	/* error */
	perror("recv");
      }
      
      /* Reset use_xdr flag set as only valid on a per-slice basis */
      IOTypes_table.io_def[index].use_xdr = FALSE;

      return REG_FAILURE;
    }

  return REG_SUCCESS;
}

/*---------------------------------------------------*/

#ifndef __linux
void signal_handler_sockets(int a_signal) {

#if REG_DEBUG
  fprintf(stderr, "Caught SIGPIPE!\n");
#endif

  signal(SIGPIPE, signal_handler_sockets);

}
#endif

/*---------------------------------------------------*/

#endif